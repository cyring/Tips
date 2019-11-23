#define	_MAJOR	"0"
#define	_MINOR	"11"
#define AutoDate "@(#) Call Simulator - "_MAJOR"."_MINOR" (C) Bouygues Telecom "__DATE__
static  char    version[] = AutoDate;
/*
*       (C) 1997-98, Bouygues Telecom
*
*       Projet          [ CallSim - Simulateur d'appels          ]
*       Direction       [ DCTSI-IPS                              ]
*       Auteurs         [ Courtiat Cyril,                        ]
*
*       Fichier         [ CallSim.c                           	 ]
*
*       Description :   Traitements specifiques API WIN 32
*
*/

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <afxres.h>
#include <windows.h>
#include <windows.h>
#include <winuser.h>
#include <process.h>
#include "CallGen.h"
#include "CallSim.h"
#include "CallWIN.h"

int		percentTimeout = 10;
int		percentBusy = 30;
int		percentConnect = 60;

CRITICAL_SECTION CriticalData = {0} , CriticalSessions = {0};
HWND			hmain = 0;
HANDLE			hinst = {0};
int				wScreen = 0, hScreen = 0;

int		WINAPI	WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
		WSADATA		WsaData = {0};
		int			ret = 0;

		InitializeCriticalSection( &CriticalData );
		InitializeCriticalSection( &CriticalSessions );
		ret = WSAStartup (0x0101, &WsaData);
		hinst = hInstance;
		ret = OpenMainWindow(lpCmdLine);
		WSACleanup();
		DeleteCriticalSection( &CriticalData );
		DeleteCriticalSection( &CriticalSessions );

		return(ret);
}

BOOL	CenterWindow( HWND hwnd )
{
		RECT    rect;
		int     w, h;
		int     xNew, yNew;
		HDC     hdc;

		GetWindowRect(hwnd, &rect);
		w = rect.right - rect.left;
		h = rect.bottom - rect.top;

		hdc = GetDC(hwnd);
		wScreen = GetDeviceCaps(hdc, HORZRES);
		hScreen = GetDeviceCaps(hdc, VERTRES);
		ReleaseDC(hwnd, hdc);

		xNew = wScreen/2 - w/2;
		yNew = hScreen/2 - h/2;

		return( SetWindowPos(hwnd, NULL, xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER) );
}

int		OpenMainWindow(LPSTR lpCmdLine)
{
		MSG		msg;
		HWND	hwnd;
		HACCEL	hShortcuts = {0};
		int		ret = 0;

		hwnd = GetFocus();

		hShortcuts = LoadAccelerators(hinst, MAKEINTRESOURCE(IDR_SHORTCUTS));

		hmain = CreateDialog(hinst, MAKEINTRESOURCE(ID_SIMULATOR), hwnd, MainProc);

		AddMessage( "Welcome to the Call Simulator. (C) 1999, Copyright Bouygues Telecom" );
		AppSTART();

		ShowWindow(hmain, SW_SHOW );

		while( GetMessage(&msg, hmain, 0, 0) ) {
			if( !TranslateAccelerator(hmain, hShortcuts, &msg) )
				TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		AppSTOP(0);

		return(TRUE);
}

BOOL APIENTRY DistribProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch(msg) {
        case WM_INITDIALOG:
			SetDlgItemInt(hdlg, IDC_TIMEOUT_INP, percentTimeout, FALSE);
			SetDlgItemInt(hdlg, IDC_BUSY_INP, percentBusy, FALSE);
			SetDlgItemInt(hdlg, IDC_CONNECT_INP, percentConnect, FALSE);
			return(TRUE);
		break;

        case WM_COMMAND:
			{
			WORD	wNotifyCode = HIWORD(wParam);
			WORD	wmId = LOWORD(wParam);
			HWND	hwndCtl = (HWND) lParam;
			BOOL	fTranslated = FALSE;
			int		Tpercent = 0;

            switch(wmId) {
                case IDCANCEL:
                    EndDialog( hdlg, FALSE );
				break;
				case IDOK:
					Tpercent = GetDlgItemInt(hdlg, IDC_TIMEOUT_INP, &fTranslated, FALSE);
					if(	&fTranslated ) percentTimeout = Tpercent;
					Tpercent = GetDlgItemInt(hdlg, IDC_BUSY_INP, &fTranslated, FALSE);
					if(	&fTranslated ) percentBusy = Tpercent;
					Tpercent = GetDlgItemInt(hdlg, IDC_CONNECT_INP, &fTranslated, FALSE);
					if(	&fTranslated ) percentConnect = Tpercent;
					EndDialog( hdlg, TRUE );
				break;
				}
			}
        break;

		case WM_VSCROLL:
			{
			int			nScrollCode = (int) LOWORD(wParam);
			short int	nPos = (short int) HIWORD(wParam);
			HWND		hSB = (HWND) lParam;
			BOOL		fTranslated = FALSE;
			int			Tpercent = 0;

			  if(hSB == GetDlgItem(hdlg,IDC_SB_TIMEOUT)) {
				Tpercent = GetDlgItemInt(hdlg, IDC_TIMEOUT_INP, &fTranslated, FALSE);
				if(	&fTranslated ) {
					switch(nScrollCode) {
						case SB_LINEUP: if(Tpercent < 100) Tpercent++ ; break;
						case SB_LINEDOWN:  if(Tpercent > 0) Tpercent-- ; break;
					}
					if( GetDlgItemInt(hdlg, IDC_CONNECT_INP, &fTranslated, FALSE) > 0 )
						SetDlgItemInt(hdlg, IDC_CONNECT_INP, 100 - (Tpercent + GetDlgItemInt(hdlg, IDC_BUSY_INP, &fTranslated, FALSE)), FALSE);
					else
						SetDlgItemInt(hdlg, IDC_BUSY_INP, 100 - Tpercent, FALSE);
					SetDlgItemInt(hdlg, IDC_TIMEOUT_INP, Tpercent, FALSE);
				}
			  }
			else
			  if(hSB == GetDlgItem(hdlg,IDC_SB_BUSY)) {
				Tpercent = GetDlgItemInt(hdlg, IDC_BUSY_INP, &fTranslated, FALSE);
				if(	&fTranslated ) {
					switch(nScrollCode) {
						case SB_LINEUP: if(Tpercent < 100) Tpercent++ ; break;
						case SB_LINEDOWN:  if(Tpercent > 0) Tpercent-- ; break;
					}
					if( GetDlgItemInt(hdlg, IDC_TIMEOUT_INP, &fTranslated, FALSE) > 0 )
						SetDlgItemInt(hdlg, IDC_TIMEOUT_INP, 100 - (Tpercent + GetDlgItemInt(hdlg, IDC_CONNECT_INP, &fTranslated, FALSE)), FALSE);
					else
						SetDlgItemInt(hdlg, IDC_CONNECT_INP, 100 - Tpercent, FALSE);
					SetDlgItemInt(hdlg, IDC_BUSY_INP, Tpercent, FALSE);
				}
			  }
			else
			  if(hSB == GetDlgItem(hdlg,IDC_SB_CONNECT)) {
				Tpercent = GetDlgItemInt(hdlg, IDC_CONNECT_INP, &fTranslated, FALSE);
				if(	&fTranslated ) {
					switch(nScrollCode) {
						case SB_LINEUP: if(Tpercent < 100) Tpercent++ ; break;
						case SB_LINEDOWN:  if(Tpercent > 0) Tpercent-- ; break;
					}
					if( GetDlgItemInt(hdlg, IDC_BUSY_INP, &fTranslated, FALSE) > 0 )
						SetDlgItemInt(hdlg, IDC_BUSY_INP, 100 - (Tpercent + GetDlgItemInt(hdlg, IDC_TIMEOUT_INP, &fTranslated, FALSE)), FALSE);
					else
						SetDlgItemInt(hdlg, IDC_TIMEOUT_INP, 100 - Tpercent, FALSE);
					SetDlgItemInt(hdlg, IDC_CONNECT_INP, Tpercent, FALSE);
				}
			  }
			}
		break;
    }

    return(FALSE);
}

BOOL APIENTRY MainProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch(msg) {
        case WM_INITDIALOG:
			CenterWindow(hdlg);
			return (TRUE);
		break;

		case WM_SIZE:
			{
			int fwSizeType = wParam;      // resizing flag
			int	nWidth = LOWORD(lParam);  // width of client area
			int	nHeight = HIWORD(lParam); // height of client area

			switch(fwSizeType)
				{
				case SIZE_MAXHIDE:
				case SIZE_MAXIMIZED:
				case SIZE_RESTORED:
					{
					}
				break;
				case SIZE_MINIMIZED:
					{
					}
				break;
				}
			return(TRUE);
			}
		break;

        case WM_DESTROY:
        break;

        case WM_COMMAND:
			{
			WORD	wNotifyCode = HIWORD(wParam);
			WORD	wmId = LOWORD(wParam);
			HWND	hwndCtl = (HWND) lParam;

            switch (wmId) {
				case ID_FILE_EXIT:
                case IDCANCEL:
                    PostQuitMessage(0);
				break;

				case ID_EDIT_COPY:
					SendMessage(GetDlgItem(hdlg,IDC_SIMLIST), WM_COPY, (WPARAM) 0, (LPARAM) 0 );
				break;

				case ID_EDIT_CLEAR:
					SendMessage(GetDlgItem(hdlg,IDC_SIMLIST), LB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0 );
				break;

				case ID_SETTINGS_DISTRIB:
					DialogBox(hinst, MAKEINTRESOURCE(ID_DISTRIB), hdlg, DistribProc);
				break;

				case ID_ABOUT:
					{
					char	mbText[] = "(C) Bouygues Telecom 1999\nAuthor[Cyril Courtiat]\nRelease["_MAJOR"."_MINOR"]";
					MSGBOXPARAMS mbParams = {sizeof(MSGBOXPARAMS),
											hdlg,
											hinst,
											mbText,
											"About Call Simulator",
											MB_OK|MB_USERICON,
											MAKEINTRESOURCE(IDI_CALLSIM),
											0,
											NULL,
											0x0400};


					MessageBoxIndirect(&mbParams);
					}
				break;
				}
			}
        break;
    }

    return(FALSE);
}

LRESULT	AddMessage(char *buffer)
{
		int		index;

		HWND	hSimList = GetDlgItem(hmain,IDC_SIMLIST);
		index = SendMessage(hSimList, LB_ADDSTRING, (WPARAM) 0, (LPARAM) buffer );
		return( SendMessage(hSimList, LB_SETTOPINDEX, (WPARAM) index, (LPARAM) 0 ) );
}

/*---------------------------------------------------------------------------*/

struct	dirent	dirEntry = {0};

DIR		*opendir(char *dirName)
{
		DIR	*pDIR = (DIR *) malloc(sizeof(DIR));
		pDIR->first = TRUE;
		pDIR->hFile = INVALID_HANDLE_VALUE;
		strcpy(pDIR->current, dirName);
		strcat(pDIR->current, "/*");
		return(pDIR);
}

void	closedir(DIR *pDIR)
{
		if(pDIR != NULL) free(pDIR);
}

struct	dirent	*readdir(DIR *pDIR)
{
		if( pDIR != NULL ) {
			WIN32_FIND_DATA  FindFileData = {0};

			if( pDIR->first == TRUE ) {
				pDIR->first = FALSE;
				if( (pDIR->hFile = FindFirstFile( pDIR->current, &FindFileData)) != INVALID_HANDLE_VALUE) {
					strcpy(dirEntry.d_name, FindFileData.cFileName);
					return(&dirEntry);
				}
			}
			else {
				if( FindNextFile( pDIR->hFile, &FindFileData) == FALSE ) {
					FindClose(pDIR->hFile);
					pDIR->hFile = INVALID_HANDLE_VALUE;
				}
				else {
					strcpy(dirEntry.d_name, FindFileData.cFileName);
					return(&dirEntry);
				}
			}
		}
		return(NULL);
}

/*---------------------------------------------------------------------------*/

typedef struct	{
		int	session;
		int	random;
}	WCallArgs;

_Sessions	*Sessions[TABLESIZE] = {NULL};
int			Sequence = 0;
int			WCalls = 0, prevWCalls = 0;

DWORD	WCall(LPVOID Targ)
{
		WCallArgs	*WCargs = (WCallArgs *) Targ;
		BOOL		fTimeout = FALSE;
		char		data[8];

		EnterCriticalSection( &CriticalData );
			{
			WCalls++ ;

			if( Sequence < 0xFFFF )
				Sequence++ ;
			else
				Sequence = 0;
			}
		LeaveCriticalSection( &CriticalData );

		sprintf( data, "%d", GetCurrentThreadId() );
		cgSetSessionKey( WCargs->session, "$chan", data ); 
		sprintf( data, "%d", Sequence );
		cgSetSessionKey( WCargs->session, "$uref", data );

		  if( (WCargs->random > 0 ) && (WCargs->random <= percentTimeout) ) {
			cgSetSessionKey( WCargs->session, "$strin", "?");
			cgSetSessionKey( WCargs->session, "$key", "TIME");
			AllEvent();
		}
		else
		  if( (WCargs->random > percentTimeout) && (WCargs->random <= (percentBusy+percentTimeout)) ) {
			cgSetSessionKey( WCargs->session, "$strin", "?");
			cgSetSessionKey( WCargs->session, "$key", "BUSY");
			BusyEvent();
		}
		else 
		  if( (WCargs->random > (percentBusy+percentTimeout)) && (WCargs->random <= 100) ) {
			cgSetSessionKey( WCargs->session, "$strin", "?");
			cgSetSessionKey( WCargs->session, "$key", "NEXT");
			NextEvent();

			while( !fTimeout ) {
				if( !strcmp( IsTimeout(), "TIMEOUT+EVT0" ) )
					fTimeout = TRUE;
				else
					Sleep(500);
			}
		}

		cgDiscCall(WCargs->session);
		cgDeleteSession(WCargs->session);

		EnterCriticalSection( &CriticalData );
			{
			WCalls-- ;
			}
		LeaveCriticalSection( &CriticalData );

		free(WCargs);

		return(0);
}

void	WClock(void *Targ)
{
		void	( *Func ) ( void * ) = Targ;
		char	sCalls[] = "Call Simulator [000]";
		srand( (unsigned)time( NULL ) );
		while(TRUE) {
			Sleep(1000);
			Func(NULL);
			if( WCalls != prevWCalls ) {
				sprintf(sCalls, "Call Simulator [%03d]", WCalls);
				SetWindowText(hmain, sCalls);
				prevWCalls = WCalls;
			}
		}

}

void	cgClockFunc(void *FuncAddr )
{
		_beginthread( WClock, 0, FuncAddr);
}

int		cgLookUpSession(DWORD CallID)
{
		int	i = -1, r = -1;

		EnterCriticalSection( &CriticalSessions );
			{
			for( i = 0; i < TABLESIZE; i++ )
				if( (Sessions[i] != NULL) && (Sessions[i]->CallID == CallID) )
					break;
			}
		LeaveCriticalSection( &CriticalSessions );
		r = (i == TABLESIZE) ? -1 : i;
		return(r);
}

int		cgCreateSession()
{
		int	i = -1, r = -1;

		EnterCriticalSection( &CriticalSessions );
			{
			for( i = 0; i < TABLESIZE; i++ )
				if( Sessions[i] == NULL ) {
					Sessions[i] = (_Sessions *) malloc(sizeof(_Sessions));
					Sessions[i]->CallID = (DWORD) -1;
					Sessions[i]->Keys = 0;
					break;
				}
			}
		LeaveCriticalSection( &CriticalSessions );
		r = (i == TABLESIZE) ? -1 : i;
		return(r);
}

int		cgSetSessionKey(int session, char *key, char *val)
{
		int	s = ( session == -1 ) ? cgLookUpSession(GetCurrentThreadId()) : session;

		if( (s != -1) && (Sessions[s]->Keys < 16) ) {
			strcpy( Sessions[s]->Key[Sessions[s]->Keys].var, key);
			strcpy( Sessions[s]->Key[Sessions[s]->Keys].val, val);
			Sessions[s]->Keys++ ;
			return(0);
		}
		else
			return(-1);
}

char	*cgGetSessionKey(int session, char *key)
{
		int	s = ( session == -1 ) ? cgLookUpSession(GetCurrentThreadId()) : session;
		int	i = 0;

		for( i = 0; (s != -1) && (i < Sessions[s]->Keys); i++ )
			if( !strcmp( Sessions[s]->Key[i].var, key ) )
				return( Sessions[s]->Key[i].val );
		return(NULL);
}

int		cgDeleteSession(int session)
{
		int	r = -1;

		EnterCriticalSection( &CriticalSessions );
			{
			if( Sessions[session] != NULL ) {
				free( Sessions[session] );
				Sessions[session] = NULL;
				r = 0;
				}
			else
				r = -1;
			}
		LeaveCriticalSection( &CriticalSessions );
		return(r);
}

int		cgMakeCall(int session)
{
		WCallArgs	*Targ = (WCallArgs *) malloc(sizeof(WCallArgs));
		int			ret = -1;

		Targ->session = session;
		Targ->random = (100 * rand()) / RAND_MAX;

		EnterCriticalSection( &CriticalSessions );
			{
			if(CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) WCall, (LPVOID) Targ, 0, &Sessions[session]->CallID) == NULL) {
				free(Targ);
				ret = -1;
			}
			else
				ret = Sessions[session]->CallID;
			}
		LeaveCriticalSection( &CriticalSessions );

		return(ret);
}

int		cgDiscCall(int session)
{
		UserDISC(0);

		return(0);
}
