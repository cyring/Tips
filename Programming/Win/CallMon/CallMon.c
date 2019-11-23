#define	_MAJOR	"3"
#define	_MINOR	"39"
#define AutoDate "@(#) Call Monitor - "_MAJOR"."_MINOR" (C) Bouygues Telecom "__DATE__
static  char    version[] = AutoDate;
/*
*       (C) 1997-98, Bouygues Telecom
*
*       Projet          [ CallMon - Moniteur du Generateur d'appels	]
*       Direction       [ DCTSI-IPS-IPA								]
*       Auteurs         [ Courtiat Cyril,							]
*
*       Fichier         [ CallMon.c                           		]
*
*       Description :   Traitements de dialogue Socket avec le processus serveur.
*
*
*/

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <afxres.h>
#include <windows.h>
#include <windowsX.h>
#include <ctype.h>
#include <winsock.h>
#include <process.h>
#include <winuser.h>
#include "CallMon.h"


#define	MAXSERVICES		3
#define	SOCKETSIZE		80
#define	TABLESIZE		240
#define	MAXHOSTLIST		8
#define	DEFAULTCONFIG	".default"

#define	ERRCODE_CTX		'C'		//"Can not make call with Phone <%d>"
#define	ERRCODE_INUSE	'U'		//"Can not delete <file>. Configuration in use"

#define	UI_SELECT_OFF	0x00000000
#define	UI_SELECT_ON	0x00000001

#define	WS_PHONE		WS_BORDER|!WS_CAPTION|WS_CHILD|!WS_TABSTOP|SS_BITMAP|SS_CENTERIMAGE|SS_REALSIZEIMAGE
#define	WS_NUMBER		WS_BORDER|!WS_CAPTION|WS_CHILD|WS_TABSTOP|WS_GROUP|BS_PUSHBUTTON
#define	WS_RECORD		WS_BORDER|!WS_CAPTION|WS_CHILD|!WS_TABSTOP|WS_GROUP|SS_CENTER
#define	WS_CHANNEL		WS_BORDER|!WS_CAPTION|WS_CHILD|!WS_TABSTOP|WS_GROUP|SS_CENTER

int 			SocketRead(SOCKET socketdesc, char *data, int len);
int				SocketSend(SOCKET socketdesc, char *data);
void			TcpWatcher(void *arg);
void			UdpWatcher(void *arg);
int				OpenMainWindow(LPSTR lpCmdLine);
void			EnableSTART(void);
void			EnableSTOP(void);
void			EnableDEBUG(void);
void			DisableDEBUG(void);
void			EnableCONNECT(void);
void			EnableDISCONNECT(void);
void			DisplayCounters(char *pCounter);
void			UpdateTimeOnLine(char *pTIMER);
BOOL			CenterWindow(HWND hwnd);
BOOL			OpenAboutBox(HWND hParent);
BOOL APIENTRY	AboutProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL			OpenFileSelect(HWND hParent, char *title, char *filename);
BOOL			OpenMsgSelect(HWND hParent, char *title, char *filename);
BOOL APIENTRY	FilesProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL			OpenTCPIPsettings(HWND hParent);
BOOL APIENTRY	TCPIPproc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL			OpenHostList(HWND hParent);
BOOL APIENTRY	HostListProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL			OpenPropertyWindow(int record, HWND hParent);
BOOL			OpenMultiPropertiesWindow(HWND hParent);
BOOL			OpenOptionsWindow(HWND hParent);
BOOL			OpenSelectWindow(HWND hParent);
BOOL APIENTRY	MainProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL			SetBitmapButton( HWND hwnd, HBITMAP hbitmap );
BOOL APIENTRY	StatusProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL APIENTRY	CountersProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL APIENTRY	PropertyProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL APIENTRY	MultiPropertiesProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL APIENTRY	OptionsProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL APIENTRY	SelectionProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL			EmergencyExit(HWND hParent);
BOOL APIENTRY	EmergencyProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam );


HWND	hmain = 0, hstatus = 0, hcounters;
HWND	hPhone[TABLESIZE] = {0};
HWND	hNumber[TABLESIZE] = {0};
HWND	hRecord[TABLESIZE] = {0};
HWND	hChannel[TABLESIZE] = {0};
HWND	hFiles = 0;
HWND	hAbout = 0;
HANDLE	hinst = {0};
HICON	hCallMonOn = {0};
HICON	hCallMonOff = {0};
HBITMAP	hOnLine = {0};
HBITMAP	hOffLine = {0};
HBITMAP	hBusy = {0};
HBITMAP	hDialing = {0};
HFONT	hTinyFont = {0}, hSmallFont = {0}, hNormalFont = {0};
int		digitID[3][7] ={{IDC_BAR11,IDC_BAR10,IDC_BAR13,IDC_BAR12,IDC_BAR16,IDC_BAR15,IDC_BAR14},
						{IDC_BAR21,IDC_BAR20,IDC_BAR23,IDC_BAR22,IDC_BAR26,IDC_BAR25,IDC_BAR24},
						{IDC_BAR31,IDC_BAR30,IDC_BAR33,IDC_BAR32,IDC_BAR36,IDC_BAR35,IDC_BAR34}};
char	digitMASK[11][7] = {{1, 1, 1, 1, 1, 0, 1},	//0
							{0, 0, 1, 1, 0, 0, 0},	//1
							{0, 1, 1, 0, 1, 1, 1},	//2
							{0, 0, 1, 1, 1, 1, 1},	//3
							{1, 0, 1, 1, 0, 1, 0},	//4
							{1, 0, 0, 1, 1, 1, 1},	//5
							{1, 1, 0, 1, 1, 1, 1},	//6
							{0, 0, 1, 1, 1, 0, 0},	//7
							{1, 1, 1, 1, 1, 1, 1},	//8
							{1, 0, 1, 1, 1, 1, 1},	//9
							{0, 0, 0, 0, 0, 1, 0}};	//-
int		CALL_DURATION = 0;
int		DIALING_DELAY = 0;
int		BUSY_DELAY    = 0;
int		FREEING_DELAY = 0;
int		MAX_LOOP	  = 0;
int		channels = 0;
char	vdn[16] = {0};
char	currentConfigFile[12] = {0};
char	currentMsgFile[12] = {0};
char	host[32] = "";
char	hostlist[MAXHOSTLIST][32] = {0};
char	nicknames[MAXSERVICES][16] = { "Challenger", "Discovery", "Atlantis"  };	// Nicknames of service port numbers
u_short	services[MAXSERVICES] = { 10010, 16560, 16561 }; // Reserved in the NIS map
int		portindex = 0;	// Default services port index.
SOCKET	listenSocket = {0}, TcpSocket = {0}, UdpSocket = {0};
BOOL	Go = FALSE;
BOOL	fConnected = FALSE;
int		wScreen = 0, hScreen = 0;
int		iconsPerLine = 0, iconsPerCol = 0, iconsShift = 0;
int		scrollMinPos = 0, scrollMaxPos = 0;
char	timerString[] = "000:00:00";
BOOL	LeftButtonState = FALSE;
BOOL	RightButtonState = TRUE;


int		WINAPI	WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
		WSADATA		WsaData = {0};
		int			ret = 0;

		ret = WSAStartup (0x0101, &WsaData);
		hinst = hInstance;
		ret = OpenMainWindow(lpCmdLine);
		WSACleanup();

		return(ret);
}


void	winprintf(char *format, ...)
{
		va_list ap;
		char	buffer[256];
		int		index;

		va_start(ap, format);
		vsprintf(buffer, format, ap);
		index = SendMessage(GetDlgItem(hmain,IDC_INFO),LB_ADDSTRING, (WPARAM) 0, (LPARAM) buffer );
		SendMessage(GetDlgItem(hmain,IDC_INFO),LB_SETTOPINDEX, (WPARAM) index, (LPARAM) 0 );
		va_end(ap);
}


int		ConnectToServer(void)
{
		IN_ADDR			RemoteIpAddress = {0};
		SOCKADDR_IN		remoteAddr = {0};
		int				bufferSize = SOCKETSIZE + 1;
		struct hostent	*hostSt = NULL;
		int				ret = 0;

		if( strlen(host) == 0 )
			return(1);

		if( (RemoteIpAddress.s_addr = inet_addr(host)) == INADDR_NONE ) {
			if( (hostSt = gethostbyname( host )) != NULL )
				memcpy( &RemoteIpAddress.s_addr, hostSt->h_addr, hostSt->h_length);
			else 
				return(2);
		}

		if( (TcpSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
			winprintf("TCP socket() failed: %ld", WSAGetLastError ());
			return(3);
		}

		remoteAddr.sin_family = AF_INET;
		remoteAddr.sin_port = htons( services[portindex] );
		remoteAddr.sin_addr = RemoteIpAddress;

		if( (ret = connect( TcpSocket, (PSOCKADDR) &remoteAddr, sizeof(remoteAddr))) == SOCKET_ERROR) {
			winprintf("connect(host<%s>, port<%d>) failed: %ld", host, services[portindex], WSAGetLastError ());
			closesocket( TcpSocket);
			return(6);
		}

		UdpSocket = socket(AF_INET, SOCK_DGRAM, 0);

		if( bind( UdpSocket, (PSOCKADDR) &remoteAddr, sizeof(remoteAddr) ) == SOCKET_ERROR ) {
			winprintf(	"Can not bind(UDP:%d) to (%s:%d).",
					UdpSocket, inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port) );
			return(7);
		}

		return(0);
}

int		DisconnectFromServer(void)
{
		char	buffer[SOCKETSIZE + 1] = {0};
		int		len = 0;
		int		ret = 0;

		sprintf(buffer, "%c%02d", 'X', 0 );
		len = SocketSend(TcpSocket, buffer);

		ret = shutdown( TcpSocket, 2 );
		ret = shutdown( UdpSocket, 2 );

		if( (closesocket( TcpSocket) == SOCKET_ERROR)
		 || (closesocket( UdpSocket) == SOCKET_ERROR) )
		{
			winprintf("closesocket(TCP|UDP) failed: %ld", WSAGetLastError ());
			return(1);
		}

		return(0);
}

int 	SocketRead(SOCKET socketdesc, char *data, int len)
{
		int	rval = 0, more = 0;
	
		for( more = len; more != 0; more -= rval) {
			rval = recv( socketdesc, data+(len - more), more, 0);
			if( rval < 1 )	return(rval);
		}

		return(len);
}

int		SocketSend(SOCKET socketdesc, char *data)
{
		fd_set	sockSet;
		struct	timeval timeout = {3, 0};
		int		ret = 0;

		FD_ZERO(&sockSet);
		FD_SET(socketdesc, &sockSet);

		if( (ret = select( 0, NULL, &sockSet, NULL, &timeout )) > 0 )
			return( send(socketdesc, data, SOCKETSIZE, 0) );
		else
			return(ret);
}

void	UdpWatcher(void *arg)
{
		char	data[64] = {0};
		char	buffer[SOCKETSIZE + 1] = {0};
		int		len = 0;
		int		record = -1;
		char	channel[6];
		int		sequence = -1;
		char	din[10 + 1] = {0};
		int		status = 0;
		int		duration = 0;
		short	bye = 0;
		HWND	hdlg = (HWND) arg;

		while( !bye ) {
			if( (len = recvfrom( UdpSocket,	buffer,	SOCKETSIZE,	0, NULL, NULL )) > 0 ) {
			  switch( buffer[0] ) {

			  case 'S' :
				memmove(data, &buffer[3], 5);
				data[5] = '\0';
				record = atoi(data);
				sprintf(data, "% 5d", record );
				SetWindowText(hRecord[record], data);

				memmove(channel, &buffer[8], 5);
				channel[5] = '\0';

				memmove(data, &buffer[13], 5);
				data[5] = '\0';
				sequence = atoi(data);

				memmove(data, &buffer[18], 2);
				data[2] = '\0';
				status = atoi(data);

				memmove(data, &buffer[20], 5);
				data[5] = '\0';
				duration = atoi(data);

				strncpy(din, &buffer[25], 10);

				if( record < channels ) {
				  switch(status) {
				    case FREEING_STATUS:
						SendMessage(hPhone[record], STM_SETIMAGE,
									(WPARAM)IMAGE_BITMAP,(LPARAM) hOffLine);
						SetWindowText(hChannel[record], "FREE" );
					break;
					case OFFLINE_STATUS:
						SendMessage(hPhone[record], STM_SETIMAGE,
									(WPARAM)IMAGE_BITMAP,(LPARAM) hOffLine);
						SetWindowText(hChannel[record], "DISC" );
					break;
					case UNKNOWN_STATUS:
						SendMessage(hPhone[record], STM_SETIMAGE,
									(WPARAM)IMAGE_BITMAP,(LPARAM) hOffLine);
						SetWindowText(hChannel[record], "NULL" );
					break;
					case DIALING_STATUS:
						SendMessage(hPhone[record], STM_SETIMAGE,
									(WPARAM) IMAGE_BITMAP, (LPARAM) hDialing);
						SetWindowText(hChannel[record], "DIAL" );
					break;
					case BUSY_STATUS:
						SendMessage(hPhone[record], STM_SETIMAGE,
									(WPARAM) IMAGE_BITMAP, (LPARAM) hBusy);
						SetWindowText(hChannel[record], "BUSY" );
					break;
					case ONLINE_STATUS:
						SendMessage(hPhone[record], STM_SETIMAGE,
									(WPARAM) IMAGE_BITMAP, (LPARAM) hOnLine);
						SetWindowText(hChannel[record], channel );
					break;
					default:
						winprintf("TcpWatcher() : Unknown status %d", status);
					break;
				  }
				SetWindowText(hNumber[record], din);
				SetWindowLong(hNumber[record], GWL_USERDATA, duration);
				}
			  break;

			  case 'C':
				  DisplayCounters(&buffer[3]);
			  break;

			  case 'T':
				  UpdateTimeOnLine(&buffer[3]);
			  break;
			  }
			}
			else
				bye = 1;
		}
}

void	TcpWatcher(void *arg)
{
		char	data[64] = {0};
		char	buffer[SOCKETSIZE + 1] = {0};
		int		len = sizeof(data);	// maximum size of buffer to contain User Name
		short	bye = 0;
		HWND	hdlg = (HWND) arg;

		GetUserName( data, &len );
		sprintf(buffer, "%c%02d%4s%s%c", 'C', 5 + len, "user", data, '\0' );
		len = SocketSend(TcpSocket, buffer);

		sprintf( data, "%s-%d", host, services[portindex] );
		SetWindowText( GetDlgItem(hmain,IDC_HOSTNAME), data );

		EnableCONNECT();

		fConnected = TRUE;
		while( !bye ) {
			if( (len = SocketRead( TcpSocket, buffer, SOCKETSIZE )) > 0 ) {
			  switch( buffer[0] ) {

			  case 'O' :
				{
				int		oldVal = channels;

				memmove(data, &buffer[3], 5);
				data[5] = '\0';
				channels = atoi(data);

				memmove(data, &buffer[8], 5 );
				data[5] = '\0';
				CALL_DURATION = atoi(data);
				
				memmove(data, &buffer[13], 5 );
				data[5] = '\0';
				DIALING_DELAY = atoi(data);
				
				memmove(data, &buffer[18], 5 );
				data[5] = '\0';
				BUSY_DELAY = atoi(data);
				
				memmove(data, &buffer[23], 5 );
				data[5] = '\0';
				FREEING_DELAY = atoi(data);

				memmove(data, &buffer[28], 5 );
				data[5] = '\0';
				MAX_LOOP = atoi(data);

				strncpy(vdn, &buffer[33], 16);

				strncpy(currentConfigFile, &buffer[34+strlen(vdn)], 12);

				strncpy(currentMsgFile, &buffer[35+strlen(vdn)+strlen(currentConfigFile)], 12);

				sprintf( data, "Call Monitor[%s] - %s", nicknames[portindex], currentConfigFile);
				SetWindowText( hmain, data );

				if( oldVal != channels )
					PostMessage(hstatus, WP_UPDATE, channels, oldVal );
				else {
					sprintf(buffer, "%c%02d%s", 'C', 4, "dump\0" );
					len = SocketSend(TcpSocket, buffer);
					}


				winprintf("CONFIGURATION <%s> loaded", currentConfigFile);
				}
			  break;

			  case 'G' :
					switch( buffer[3] ) {
						case '0' :
							Go = FALSE;
							EnableSTART();
							SetClassLong( hmain, GCL_HICON, (LONG) hCallMonOff );
						break;
						case '1' :
							Go = TRUE;
							EnableSTOP();
							SetClassLong( hmain, GCL_HICON, (LONG) hCallMonOn );
						break;
					}
			  break;

			  case 'D' :
					switch( buffer[3] ) {
						case '0' :
							DisableDEBUG();
						break;
						case '1' :
							EnableDEBUG();
						break;
					}
					winprintf( "DEBUG mode %s", (buffer[3] == '1') ? "activated" : "deactivated" );
			  break;

			  case 'F' :
				SendMessage(GetDlgItem(hFiles,IDC_SELECT), LB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPCTSTR) &buffer[3] );
			  break;

			  case 'W':
				SendMessage(GetDlgItem(hAbout,IDC_WHO), LB_ADDSTRING, (WPARAM) 0, (LPARAM) (LPCTSTR) &buffer[3] );
			  break;

			  case 'I':
				  switch(buffer[3]) {
					case 'S': {
					  winprintf( "CONFIGURATION <%s> saved", &buffer[4] );
					  sprintf( data, "Call Monitor[%s] - %s", nicknames[portindex], &buffer[4]);
					  SetWindowText( hmain, data );
					  }
					break;
				    case 'D':
					  winprintf( "CONFIGURATION <%s> deleted", &buffer[4] );
					break;
					case 'E':
						switch(buffer[4]) {
						  case ERRCODE_CTX:
							  winprintf( "ERROR: Can not make call with phone <%s>", &buffer[5] );
						  break;
						  case ERRCODE_INUSE:
							  winprintf( "ERROR: Can not delete <%s>. Configuration in use.", currentConfigFile);
						  break;
						}
					break;
				  }
			  break;
			  }
			}
			else {
				if( len == 0 )	EmergencyExit(hmain);
				bye = 1;
			}
		}
		fConnected = FALSE;

		EnableDISCONNECT();
		SetWindowText( GetDlgItem(hmain,IDC_HOSTNAME), "DISCONNECTED" );
		SetWindowText( hmain, "Call Monitor" );
		DisplayCounters("---");
		UpdateTimeOnLine("000:00:00");
		SetClassLong( hmain, GCL_HICON, (LONG) hCallMonOff );
}

void	EnableSTART(void)
{
		HWND	hMenu = GetMenu(hmain);

		EnableWindow( GetDlgItem(hmain,IDSTART), TRUE );
		EnableWindow( GetDlgItem(hmain,IDSTOP), FALSE );
		EnableMenuItem( hMenu,ID_ACTION_START, MF_ENABLED );
		EnableMenuItem( hMenu,ID_ACTION_STOP, MF_GRAYED );
}

void	EnableSTOP(void)
{
		HWND	hMenu = GetMenu(hmain);

		EnableWindow( GetDlgItem(hmain,IDSTART), FALSE );
		EnableWindow( GetDlgItem(hmain,IDSTOP), TRUE );
		EnableMenuItem( hMenu,ID_ACTION_START, MF_GRAYED );
		EnableMenuItem( hMenu,ID_ACTION_STOP, MF_ENABLED );
}

void	EnableDEBUG(void)
{
		HWND	hMenu = GetMenu(hmain);

		CheckMenuItem( hMenu, ID_OPTION_DEBUG, MF_BYCOMMAND|MF_CHECKED );
}

void	DisableDEBUG(void)
{
		HWND	hMenu = GetMenu(hmain);

		CheckMenuItem( hMenu, ID_OPTION_DEBUG, MF_BYCOMMAND|MF_UNCHECKED );
}

void	EnableCONNECT(void)
{
		HWND	hMenu = GetMenu(hmain);

		EnableMenuItem( hMenu,ID_ACTION_CONNECT, MF_GRAYED );
		EnableMenuItem( hMenu,ID_ACTION_DISCONNECT, MF_ENABLED );
		EnableMenuItem( hMenu,ID_FILE_NEW, MF_ENABLED );
		EnableMenuItem( hMenu,ID_FILE_OPEN, MF_ENABLED );
		EnableMenuItem( hMenu,ID_FILE_SAVE, MF_ENABLED );
		EnableMenuItem( hMenu,ID_FILE_SAVEAS, MF_ENABLED );
		EnableMenuItem( hMenu,ID_FILE_DELETE, MF_ENABLED );
		EnableMenuItem( hMenu,ID_EDIT_SELECT_ALL, MF_ENABLED );
		EnableMenuItem( hMenu,ID_EDIT_SELECT_FROMTO, MF_ENABLED );
		EnableMenuItem( hMenu,ID_EDIT_SELECT_CLEAR, MF_ENABLED );
		EnableMenuItem( hMenu,ID_EDIT_PROPERTIES, MF_ENABLED );
		EnableMenuItem( hMenu,ID_OPTION_TELEPHONY, MF_ENABLED );
		EnableMenuItem( hMenu,ID_OPTION_DEBUG, MF_ENABLED );
		EnableMenuItem( hMenu,ID_OPTION_TCPIP, MF_GRAYED );
}

void	EnableDISCONNECT(void)
{
		HWND	hMenu = GetMenu(hmain);

		EnableMenuItem( hMenu,ID_ACTION_CONNECT, MF_ENABLED );
		EnableMenuItem( hMenu,ID_ACTION_DISCONNECT, MF_GRAYED );
		EnableMenuItem( hMenu,ID_FILE_NEW, MF_GRAYED );
		EnableMenuItem( hMenu,ID_FILE_OPEN, MF_GRAYED );
		EnableMenuItem( hMenu,ID_FILE_SAVE, MF_GRAYED );
		EnableMenuItem( hMenu,ID_FILE_SAVEAS, MF_GRAYED );
		EnableMenuItem( hMenu,ID_FILE_DELETE, MF_GRAYED );
		EnableWindow( GetDlgItem(hmain,IDSTART), FALSE );
		EnableWindow( GetDlgItem(hmain,IDSTOP), FALSE );
		EnableMenuItem( hMenu,ID_EDIT_SELECT_ALL, MF_GRAYED );
		EnableMenuItem( hMenu,ID_EDIT_SELECT_FROMTO, MF_GRAYED );
		EnableMenuItem( hMenu,ID_EDIT_SELECT_CLEAR, MF_GRAYED );
		EnableMenuItem( hMenu,ID_EDIT_PROPERTIES, MF_GRAYED );
		EnableMenuItem( hMenu,ID_ACTION_START, MF_GRAYED );
		EnableMenuItem( hMenu,ID_ACTION_STOP, MF_GRAYED );
		EnableMenuItem( hMenu,ID_OPTION_TELEPHONY, MF_GRAYED );
		EnableMenuItem( hMenu,ID_OPTION_DEBUG, MF_GRAYED );
		EnableMenuItem( hMenu,ID_OPTION_TCPIP, MF_ENABLED );
}


void	DisplayCounters(char *pCounter)
{
		int		i, j, v;

		for(i = 0; i < 3; i++) {
			v = (pCounter[i] != '-') ? (int) pCounter[i] - (int) '0' : 10;
			for(j = 0; j < 7; j++ )
				if(digitMASK[v][j] == 1)
					ShowWindow( GetDlgItem(hcounters, digitID[i][j]), SW_SHOW );
				else
					ShowWindow( GetDlgItem(hcounters, digitID[i][j]), SW_HIDE );
		}
		if(IsIconic(hmain) && fConnected) {
			char	data[64];
			sprintf( data, "Call Monitor[%s] - %s[%03d]", nicknames[portindex], currentConfigFile, atoi(pCounter));
			SetWindowText( hmain, data );
		}

}

void	UpdateTimeOnLine(char *pTIMER)
{
		HWND	hMenu = GetMenu(hmain);

		strcpy(timerString, pTIMER);
		ModifyMenu(hMenu, ID_TIMING, MF_BYCOMMAND|MF_OWNERDRAW|MF_DISABLED, ID_TIMING, NULL);
		DrawMenuBar(hmain);
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

void	SizeCountersWin(int X, int Y, int CX, int CY)
{
		#define	ds	4
		#define	cs	3
		int		dw = 0,	dh = 0, x = 0, i = 0, j = 0;
		RECT	rect = {0};

		SetWindowPos(hcounters,NULL,X,Y,CX,CY,SWP_NOZORDER|SWP_NOCOPYBITS);
		GetClientRect(hcounters, &rect );
		dw = ((rect.right-rect.left) - 2*cs - 6*ds) / 3;
		dh = ((rect.bottom-rect.top) - 3*ds) / 2;

		for(j = 0; j < 3; j++) {
			for(i = 0; i < 2; i++) {
				x = j*(dw + cs + 2*ds);
				SetWindowPos(GetDlgItem(hcounters,digitID[j][i]),NULL,
							 x, i*dh+(i+1)*ds, ds, dh,
							 SWP_NOZORDER|SWP_NOCOPYBITS);
				x += (dw + ds);
				SetWindowPos(GetDlgItem(hcounters,digitID[j][i+2]),NULL,
							 x, i*dh+(i+1)*ds, ds, dh,
							 SWP_NOZORDER|SWP_NOCOPYBITS);
			}
			for(i = 0; i < 3;i++ ) {
				x = j*(dw + cs) + (2*j+1)*ds;
				SetWindowPos(GetDlgItem(hcounters,digitID[j][i+4]),NULL,
							 x, i*(dh+ds), dw, ds,
							 SWP_NOZORDER|SWP_NOCOPYBITS);
			}
		}

				
}

#define	LEFTSPACE		4
#define	RIGHTSPACE		80
#define	DELTASPACE		8
#define	INTERSPACE		12
#define	STATICSPACE		2
#define	TOPSPACE		4
#define	BOTSPACE		52
#define	MARGINSPACE		6

#define	SCROLLDIV		4
#define	SCROLLSHIFT		16

#define	ICONHSPACE		80
#define	ICONVSPACE		SCROLLSHIFT * SCROLLDIV
#define	LEFTSTART		8
#define	TOPSTART		2
#define	RIGHTSTOP		(16 + ICONHSPACE * 2)
#define	LEFTPHONE		38
#define	TOPPHONE		0
#define	LEFTRECORD		2
#define	TOPRECORD		2
#define	LEFTCHANNEL		2
#define	TOPCHANNEL		18
#define	LEFTNUMBER		0
#define	TOPNUMBER		34

#define	WIDTHPHONE		32
#define	HEIGHTPHONE		32
#define	WIDTHNUMBER		72
#define	HEIGHTNUMBER	24
#define	WIDTHRECORD		30
#define	HEIGHTRECORD	12
#define	WIDTHCHANNEL	30
#define	HEIGHTCHANNEL	12

void	StatusScroll( void )
{
		RECT		rect = {0};
		int			i = 0, x = 0, y = 0, w = 0, h = 0;
		short int	bPos = 0;
		UINT		flags = SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE|SWP_SHOWWINDOW;

		GetWindowRect(hstatus, &rect);
		w = rect.right - rect.left;
		h = rect.bottom - rect.top;
		iconsPerCol  = 0;
		iconsPerLine = 0;
		for( i = 0, x = LEFTSTART, y = TOPSTART + iconsShift; i < channels; i++ ) {
			SetWindowPos(hPhone[i] ,  NULL, x + LEFTPHONE ,  y + TOPPHONE,   0, 0, flags);
			SetWindowPos(hRecord[i],  NULL, x + LEFTRECORD,  y + TOPRECORD,  0, 0, flags);
			SetWindowPos(hChannel[i], NULL, x + LEFTCHANNEL, y + TOPCHANNEL, 0, 0, flags);
			SetWindowPos(hNumber[i],  NULL, x + LEFTNUMBER,  y + TOPNUMBER,  0, 0, flags);
			if( x >= (w - RIGHTSTOP) ) {
				x = LEFTSTART;
				y += ICONVSPACE;
				if(!iconsPerLine) iconsPerLine = i;
				iconsPerCol++ ;
			}
			else
				x += ICONHSPACE;
			}
		if( iconsPerCol > 0 ) {
			bPos = (abs(iconsShift) * scrollMaxPos) / (iconsPerCol * ICONVSPACE);
			SetScrollPos( hstatus, SB_VERT, bPos, TRUE );
		}
}

void	SizeWindow( HWND hwnd, int width, int height )
{
		RECT    rect = {0};
		int		wa = 0, ha = 0, wb = 0;

		SetWindowPos(hstatus,NULL,LEFTSPACE,TOPSPACE,width-DELTASPACE,height-BOTSPACE,SWP_NOZORDER|SWP_NOCOPYBITS);
		
		GetClientRect(GetDlgItem(hwnd,IDSTART), &rect );
		wa = rect.right;
		ha = rect.bottom;
		SetWindowPos(GetDlgItem(hwnd,IDSTART),NULL,LEFTSPACE,height-BOTSPACE+MARGINSPACE,0,0,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE);
		SetWindowPos(GetDlgItem(hwnd,IDSTOP),NULL,wa+INTERSPACE,height-BOTSPACE+MARGINSPACE,0,0,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE);
		GetClientRect(GetDlgItem(hwnd,IDSTOP), &rect );
		wb = rect.right;
		SetWindowPos(GetDlgItem(hwnd,IDC_HOSTNAME),NULL,LEFTSPACE,height-BOTSPACE+MARGINSPACE+ha+STATICSPACE,wa+INTERSPACE+wb-LEFTSPACE,16,SWP_NOZORDER|SWP_NOCOPYBITS);
		SetWindowPos(GetDlgItem(hwnd,IDC_INFO),NULL,LEFTSPACE+wa+INTERSPACE+wb,height-BOTSPACE+MARGINSPACE,width-wa-INTERSPACE-wb-RIGHTSPACE,BOTSPACE-MARGINSPACE-STATICSPACE,SWP_NOZORDER|SWP_NOCOPYBITS);

		SizeCountersWin(width-RIGHTSPACE+DELTASPACE, height-BOTSPACE+MARGINSPACE, RIGHTSPACE-DELTASPACE-LEFTSPACE, BOTSPACE-MARGINSPACE-STATICSPACE);
}

void	DestroyPhonesWindow(int instances)
{
		int		i = 0;

		for( i = 0; i < instances; i++ ) {
			if( hPhone[i] )  DestroyWindow(hPhone[i]);
			if( hNumber[i] ) DestroyWindow(hNumber[i]);
			if( hRecord[i] ) DestroyWindow(hRecord[i]);
			if( hChannel[i] ) DestroyWindow(hChannel[i]);
			}
}

void	CreatePhonesWindow(int instances)
{
		char	rescname[16];
		int		i;

		memset( hPhone, 0, sizeof(HWND) * TABLESIZE );
		memset( hNumber, 0, sizeof(HWND) * TABLESIZE );
		memset( hRecord, 0, sizeof(HWND) * TABLESIZE );
		memset( hChannel, 0, sizeof(HWND) * TABLESIZE );

		for( i = 0; i < instances; i++ ) {
			sprintf( rescname, "#%d", MAKEINTRESOURCE(IDB_OFFLINE) );
			hPhone[i] = CreateWindow( "STATIC",
							  rescname,
							  WS_PHONE,
							  0,
							  0,
							  WIDTHPHONE,
							  HEIGHTPHONE,
							  hstatus,
							  NULL,
							  hinst,
							  NULL);

			hNumber[i] = CreateWindow( "BUTTON",
							  "",
							  WS_NUMBER,
							  0,
							  0,
							  WIDTHNUMBER,
							  HEIGHTNUMBER,
							  hstatus,
							  (HWND) (i+3000),
							  hinst,
							  NULL);

			SendMessage(hNumber[i], WM_SETFONT, (WPARAM) hNormalFont, (LPARAM) TRUE);

			hRecord[i] = CreateWindow( "STATIC",
							  "",
							  WS_RECORD,
							  0,
							  0,
							  WIDTHRECORD,
							  HEIGHTRECORD,
							  hstatus,
							  NULL,
							  hinst,
							  NULL);

			SendMessage(hRecord[i], WM_SETFONT, (WPARAM) hSmallFont, (LPARAM) TRUE);

			hChannel[i] = CreateWindow( "STATIC",
							  "",
							  WS_CHANNEL,
							  0,
							  0,
							  WIDTHCHANNEL,
							  HEIGHTCHANNEL,
							  hstatus,
							  NULL,
							  hinst,
							  NULL);

			SendMessage(hChannel[i], WM_SETFONT, (WPARAM) hTinyFont, (LPARAM) TRUE);
		}
}

int		OpenMainWindow(LPSTR lpCmdLine)
{
		MSG		msg;
		HWND	hwnd;
		HDC		hdc;
		HGDIOBJ	hPrevFont = NULL;
		RECT    rect;
		char	rescname[16];
		char	registry[32];
		HKEY	hKey;
		DWORD	regType = 0;
		int		i, ret = 0;
		HACCEL	hShortcuts = {0};

		hwnd = GetFocus();

		hShortcuts = LoadAccelerators(hinst, MAKEINTRESOURCE(IDR_SHORTCUTS));

		sprintf( rescname, "#%d", MAKEINTRESOURCE(CALLMON_ON) );
		hCallMonOn = LoadIcon( hinst, rescname );
		sprintf( rescname, "#%d", MAKEINTRESOURCE(CALLMON_OFF) );
		hCallMonOff = LoadIcon( hinst, rescname );
		sprintf( rescname, "#%d", MAKEINTRESOURCE(IDB_ONLINE) );
		hOnLine = LoadBitmap( hinst, rescname );
		sprintf( rescname, "#%d", MAKEINTRESOURCE(IDB_OFFLINE) );
		hOffLine = LoadBitmap( hinst, rescname );
		sprintf( rescname, "#%d", MAKEINTRESOURCE(IDB_BUSY) );
		hBusy = LoadBitmap( hinst, rescname );
		sprintf( rescname, "#%d", MAKEINTRESOURCE(IDB_DIALING) );
		hDialing = LoadBitmap( hinst, rescname );

		hdc = GetDC(NULL);
		hTinyFont = CreateFont(-MulDiv(6, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0,
								FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_DONTCARE,
								"Small Fonts" );
		hSmallFont = CreateFont(-MulDiv(7, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0,
								FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_DONTCARE,
								"Small Fonts" );
		hNormalFont = CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0,
								FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_DONTCARE,
								NULL );
		ReleaseDC(NULL, hdc);

		hmain = CreateDialog(hinst, MAKEINTRESOURCE(ID_CALLVIEW), hwnd, MainProc);

		hstatus = CreateDialog(hinst, MAKEINTRESOURCE(ID_STATUS), hmain, StatusProc);

		hcounters = CreateDialog(hinst, MAKEINTRESOURCE(ID_COUNTERS), hmain, CountersProc);

		if( RegCreateKey(HKEY_LOCAL_MACHINE, "Software\\BOUYGTEL\\ACDIVR\\CallMon", &hKey ) == ERROR_SUCCESS ) {
			for( i = 0; i < MAXHOSTLIST; i++ ) {
				int	keySize = 32;
				sprintf( registry, "host%1d", i );
				RegQueryValueEx(hKey, registry, 0, &regType, hostlist[i], &keySize );
				}
			for( i = 0; i < MAXSERVICES; i++ ) {
				int	keySize = 32;
				if(RegQueryValueEx(hKey, nicknames[i], 0, &regType, registry, &keySize ) == ERROR_SUCCESS)
					services[i] = atoi(registry);
				}
			RegCloseKey(hKey);
			}

		winprintf( "Welcome to the Call Monitor. The Call Generator client. (C) 1998-99, Copyright Bouygues Telecom" );
		winprintf( "System Ready." );

		GetClientRect(hmain, &rect);
		SizeWindow(hmain, rect.right - rect.left, rect.bottom - rect.top);

		ShowWindow(hmain, SW_SHOW );
		ShowWindow(hstatus, SW_SHOW );
		DisplayCounters("---");
		ShowWindow(hcounters, SW_SHOW );

		while( GetMessage(&msg, hmain, 0, 0) ) {
			if( !TranslateAccelerator(hmain, hShortcuts, &msg) )
				TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if( RegCreateKey(HKEY_LOCAL_MACHINE, "Software\\BOUYGTEL\\ACDIVR\\CallMon", &hKey ) == ERROR_SUCCESS ) {
			for( i = 0; i < MAXHOSTLIST; i++ ) {
				sprintf( registry, "host%1d", i );
				RegSetValueEx(hKey, registry, 0, REG_SZ, hostlist[i], strlen(hostlist[i])+1 );
				}
			for( i = 0; i < MAXSERVICES; i++ ) {
				int	keySize = 32;
				sprintf( registry, "%d", services[i]);
				RegSetValueEx(hKey, nicknames[i], 0, REG_SZ, registry, strlen(registry)+1 );
				}
			RegCloseKey(hKey);
			}

		return(TRUE);
}



VOID WINAPI MenuMeasureItem(HWND hwnd, LPMEASUREITEMSTRUCT lpmis)
{
	if( lpmis->itemID = ID_TIMING) {
	    HDC		hdc = GetDC(hwnd);
		HMENU	hMenu = GetMenu(hwnd);
		SIZE	size = {0};

		GetTextExtentPoint32(hdc, timerString, 9, &size);

		lpmis->itemWidth = size.cx - LOWORD(GetMenuCheckMarkDimensions());
		lpmis->itemHeight = size.cy;

		ReleaseDC(hwnd, hdc);
	}
} 

VOID WINAPI MenuDrawItem(HWND hwnd, LPDRAWITEMSTRUCT lpdis)
{
	if( lpdis->itemID = ID_TIMING) {
		HMENU	 hMenu = GetMenu(hwnd);
		COLORREF clrPrevText, clrPrevBkgnd;
		int		 x = 0, y = 0, w = 0;
		RECT	 rect = {0};

		clrPrevText = SetTextColor(lpdis->hDC, RGB(64,64,196));
		clrPrevBkgnd = SetBkColor(lpdis->hDC, GetSysColor(COLOR_MENU));
		GetClientRect(hmain, &rect);
		// Determine where to draw. Globule did it !
		w = lpdis->rcItem.right - lpdis->rcItem.left;
		lpdis->rcItem.left  = rect.right - w;
		lpdis->rcItem.right = rect.right;
		x = lpdis->rcItem.left;
		y = lpdis->rcItem.top;
		ExtTextOut(lpdis->hDC, x, y, ETO_OPAQUE, &lpdis->rcItem, timerString, 9, NULL);
	}
}


BOOL APIENTRY MainProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	char	buffer[SOCKETSIZE + 1] = {0};
	int		len = 0;

    switch(msg) {
        case WM_INITDIALOG:
			{
			CenterWindow(hdlg);
			ModifyMenu(GetMenu(hdlg),ID_TIMING,MF_BYCOMMAND|MF_OWNERDRAW|MF_DISABLED,ID_TIMING,NULL);
			SetClassLong( hdlg, GCL_HICON, (LONG) hCallMonOff );
			return (TRUE);
			}
		break;

		case WM_LBUTTONUP:
			LeftButtonState = TRUE;
		break;

		case WM_LBUTTONDOWN:
			LeftButtonState = FALSE;
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
					SizeWindow(hdlg, nWidth, nHeight);
					if( fConnected )
						{
						char	data[64];
						sprintf( data, "Call Monitor[%s] - %s", nicknames[portindex], currentConfigFile);
						SetWindowText( hmain, data );
						StatusScroll();
						}
					}
				break;
				case SIZE_MINIMIZED:
					if( fConnected )
						{
						char	data[64];

						if( Go )
							sprintf( data, "Call Monitor[%s] - %s[---]", nicknames[portindex], currentConfigFile);
						else
							sprintf( data, "Call Monitor[%s] - %s[000]", nicknames[portindex], currentConfigFile);
						SetWindowText( hmain, data );
						StatusScroll();
						}
				break;
				}
			return(TRUE);
			}
		break;

        case WM_MEASUREITEM: 
            MenuMeasureItem(hdlg, (LPMEASUREITEMSTRUCT) lParam); 
            return TRUE;
		break;

		case WM_DRAWITEM:
            MenuDrawItem(hdlg, (LPDRAWITEMSTRUCT) lParam); 
            return TRUE; 
		break;

		case WM_CTLCOLORSTATIC:
			{
			HDC			hDC = (HDC) wParam;
			HWND		hCtl = (HWND) lParam;
			HBRUSH		hBrush = {0};

			switch( GetDlgCtrlID(hCtl) ) {
				case IDC_HOSTNAME:
					SetTextColor(hDC, RGB(8,16,196));
					SetBkColor(hDC,RGB(192,192,192));
					hBrush = CreateSolidBrush(RGB(192,192,192));
					return((BOOL)hBrush);
				break;
				}
			}
		break;

        case WM_DESTROY:
			DisconnectFromServer();
			DestroyWindow(hdlg);
			DeleteObject(hOnLine);
			DeleteObject(hOffLine);
			DeleteObject(hBusy);
			DeleteObject(hDialing);
			DeleteObject(hCallMonOn);
			DeleteObject(hCallMonOff);
			DeleteObject(hTinyFont);
			DeleteObject(hSmallFont);
			DeleteObject(hNormalFont);
        break;

        case WM_COMMAND:
			{
			WORD	wNotifyCode = HIWORD(wParam);
			WORD	wmId = LOWORD(wParam);
			HWND	hwndCtl = (HWND) lParam;


            switch (wmId) {
				case ID_FILE_NEW:
					sprintf(buffer, "%c%02d%s", 'C', 3, "new\0" );
					len = SocketSend(TcpSocket, buffer);
				break;

				case ID_FILE_OPEN:
					{
					char	filename[12+1] = {0};

					if( OpenFileSelect(hdlg, "Open Configuration File", filename) ) {
						sprintf(buffer, "%c%02d%4s%s%c", 'C', 5 + strlen(filename), "open", filename, '\0' );
						len = SocketSend(TcpSocket, buffer);
						}
					}
				break;

				case ID_FILE_SAVE:
					{
					BOOL	ok = TRUE;

					if( !strcmp(currentConfigFile, DEFAULTCONFIG) )
						if( MessageBox(hdlg, "This will update the default configuration file. Do you want to continue ?", "Warning", MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON2) != IDYES )
							ok = FALSE;
					if( ok ) {
						sprintf(buffer, "%c%02d%s", 'C', 4, "save\0" );
						len = SocketSend(TcpSocket, buffer);
						}
					}
				break;

				case ID_FILE_SAVEAS:
					{
					char	filename[12+1] = {0};
					BOOL	ok = TRUE;

					if( OpenFileSelect(hdlg, "Save Configuration File as", filename) ) {
						if( !strcmp(filename, DEFAULTCONFIG) )
							if( MessageBox(hdlg, "This will update the default configuration file. Do you want to continue ?", "Warning", MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON2) != IDYES )
								ok = FALSE;

						if( ok ) {
							sprintf(buffer, "%c%02d%6s%s%c", 'C', 7 + strlen(filename), "copied", filename, '\0' );
							len = SocketSend(TcpSocket, buffer);
							}
						}
					}
				break;

				case ID_FILE_DELETE:
					{
					char	filename[12+1] = {0};

					if( OpenFileSelect(hdlg, "Delete Configuration File", filename) ) {
						sprintf(buffer, "%c%02d%6s%s%c", 'C', 7 + strlen(filename), "delete", filename, '\0' );
						len = SocketSend(TcpSocket, buffer);
						}
					}
				break;

				case ID_FILE_EXIT:
                case IDCANCEL:
					DisconnectFromServer();
                    PostQuitMessage(0);
				break;

				case ID_EDIT_SELECT_ALL:
					{
					int record = 0;

					for( record = 0; record < channels; record++ )
						EnableWindow( hNumber[record], FALSE );
					}
				break;

				case ID_EDIT_SELECT_FROMTO:
					OpenSelectWindow(hdlg);
				break;

				case ID_EDIT_SELECT_CLEAR:
					{
					int record = 0;

					for( record = 0; record < channels; record++ )
						if(!IsWindowEnabled( hNumber[record] ) )
							EnableWindow( hNumber[record], TRUE );
					}
				break;

				case ID_EDIT_PROPERTIES:
					OpenMultiPropertiesWindow(hdlg);
				break;

				case ID_OPTION_TELEPHONY:
					OpenOptionsWindow(hdlg);
				break;

				case ID_OPTION_DEBUG:
					sprintf(buffer, "%c%02d%s", 'C', 5, "debug\0" );
					len = SocketSend(TcpSocket, buffer);
				break;

				case ID_OPTION_TCPIP:
					OpenTCPIPsettings(hdlg);
				break;

				case ID_ABOUT:
					OpenAboutBox(hdlg);
				break;

				case ID_ACTION_START:
                case IDSTART:
					sprintf(buffer, "%c%02d%s", 'C', 5, "start\0" );
					len = SocketSend(TcpSocket, buffer);
                return(TRUE);

				case ID_ACTION_STOP:
                case IDSTOP:
					sprintf(buffer, "%c%02d%s", 'C', 4, "stop\0" );
					len = SocketSend(TcpSocket, buffer);
                return(TRUE);

				case ID_ACTION_CONNECT:
					OpenHostList(hdlg);
				break;

				case ID_ACTION_DISCONNECT:
					DisconnectFromServer();
					DestroyPhonesWindow(channels);
					CALL_DURATION = 0;
					DIALING_DELAY = 0;
					BUSY_DELAY    = 0;
					FREEING_DELAY = 0;
					MAX_LOOP	  = 0;
					channels = 0;
					vdn[0] = '\0';
				break;
				}
			}
        break;
    }

    return(FALSE);
}

	
BOOL APIENTRY StatusProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	int		i = 0;

    switch(msg) {
        case WM_INITDIALOG:
			GetScrollRange(hdlg, SB_VERT, &scrollMinPos, &scrollMaxPos);
			return (TRUE);
		break;

        case WM_DESTROY:
			DestroyWindow(hdlg);
        break;

		case WM_LBUTTONDOWN:
			if( fConnected )
				{
				RECT	rect;
				POINT	point;

				GetWindowRect( hstatus, &rect );
				point.x = (LONG) LOWORD(lParam) + rect.left;
				point.y = (LONG) HIWORD(lParam) + rect.top;

				for( i = 0; i < channels; i++ ) {
					GetWindowRect( hPhone[i], &rect );
					if( PtInRect( &rect, point ) == TRUE )
						if( IsWindowEnabled( hNumber[i] ) ) {
							EnableWindow( hNumber[i], FALSE );
							}
						else {
							EnableWindow( hNumber[i], TRUE );
							}
					}
				}
		break;

        case WM_COMMAND:
			{
			int	wNotifyCode, wmId;

			wNotifyCode = HIWORD(wParam);
			wmId = LOWORD(wParam);

			switch(wNotifyCode) {
				case BN_CLICKED :
					i = wmId - 3000;
					if( (i >= 0) && (i < channels) ) {
						OpenPropertyWindow( i, hdlg );
						return(TRUE);
					}
				break;
				}
			}
        break;

		case WM_VSCROLL:
		  if( fConnected )
			{
			RECT		rect = {0};
			int			nScrollCode = (int) LOWORD(wParam); // scroll bar value
			short int	nPos = (short int) HIWORD(wParam);  // scroll box position
			short int	bPos = 0, dPos = 0;
			int			vectorScroll = 0;
			BOOL		fUpdate = FALSE;

			switch(nScrollCode)
				{
				//case SB_ENDSCROLL:		//Ends scroll.
				case SB_TOP:				//Scrolls to the upper left.
				case SB_PAGEUP:				//Scrolls one page up.
					if( iconsPerCol > 0 ) {
						vectorScroll = abs(iconsShift);
						iconsShift = 0;
						bPos = scrollMinPos;
						fUpdate = TRUE;
					}
				break;

				case SB_LINEUP:				//Scrolls one line up.
					if( (iconsPerCol > 0 ) && ( iconsShift < 0 ) ) {
						iconsShift += SCROLLSHIFT;
						vectorScroll = +SCROLLSHIFT;
						bPos = (abs(iconsShift) * scrollMaxPos) / (iconsPerCol * ICONVSPACE);
						fUpdate = TRUE;
					}
					else
						MessageBeep( MB_ICONASTERISK );
				break;

				//case SB_THUMBPOSITION:	//Scrolls to the absolute position. Use nPos parameter.
				case SB_THUMBTRACK:		//Drags scroll box to the specified position. Use nPos parameter.
					if( iconsPerCol > 0 ) {
						if( nPos == 0 ) {
							vectorScroll = abs(iconsShift);
							iconsShift = 0;
							bPos = scrollMinPos;
							fUpdate = TRUE;
						}
						else {
							dPos = GetScrollPos(hdlg, SB_VERT) - nPos;
							vectorScroll = ((( dPos * iconsPerCol * ICONVSPACE) / scrollMaxPos) );
							vectorScroll /= SCROLLSHIFT;
							vectorScroll *= SCROLLSHIFT;
							if( vectorScroll || !nPos ) {
								iconsShift += vectorScroll;
								bPos = (abs(iconsShift) * scrollMaxPos) / (iconsPerCol * ICONVSPACE);
								fUpdate = TRUE;
							}
						}
					}
				break;

				case SB_BOTTOM:				//Scrolls to the lower right.
				case SB_PAGEDOWN:			//Scrolls one page down.
					if( iconsPerCol > 0 ) {
						vectorScroll = -((iconsPerCol * ICONVSPACE) - abs(iconsShift));
						iconsShift = -(iconsPerCol * ICONVSPACE);
						bPos = scrollMaxPos;
						fUpdate = TRUE;
					}
				break;

				case SB_LINEDOWN:			//Scrolls one line down.
					if( (iconsPerCol > 0 ) && ( abs(iconsShift) < (iconsPerCol * ICONVSPACE) )	) {
						iconsShift -= SCROLLSHIFT;
						vectorScroll = -SCROLLSHIFT;
						bPos = (abs(iconsShift) * scrollMaxPos) / (iconsPerCol * ICONVSPACE);
						fUpdate = TRUE;
					}
					else
						MessageBeep( MB_ICONASTERISK );
				break;
				}

			if( fUpdate )
				{
				SetScrollPos( hdlg, SB_VERT, bPos, TRUE );
				ScrollWindow( hdlg, 0, vectorScroll, NULL, NULL );
				}
			return(FALSE);
			}
		break;

		case WP_UPDATE:
			{
			char	buffer[SOCKETSIZE + 1] = {0};
			int		len = 0;
			RECT    rect = {0};
			int		newVal = (int) wParam;
			int		oldVal = (int) lParam;

			DestroyPhonesWindow(oldVal);
			iconsShift = 0;
			SetScrollPos( hdlg, SB_VERT, scrollMinPos, TRUE );
			CreatePhonesWindow(newVal);
			GetClientRect( hmain, &rect );
			SizeWindow( hmain, rect.right - rect.left, rect.bottom - rect.top);
			StatusScroll();

			sprintf(buffer, "%c%02d%s", 'C', 4, "dump\0" );
			len = SocketSend(TcpSocket, buffer);
			}
		break;
    }

    return(FALSE);
}
	
BOOL APIENTRY CountersProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	int		wmId = 0, i = 0;

    switch(msg) {
        case WM_INITDIALOG:
			return (TRUE);
		break;

        case WM_DESTROY:
			DestroyWindow(hdlg);
        break;
    }

    return(FALSE);
}


BOOL	OpenAboutBox(HWND hParent)
{
		return(DialogBoxParam(hinst, MAKEINTRESOURCE(ID_BOUYGUES), hParent, AboutProc, (LPARAM) hParent));
}

BOOL APIENTRY AboutProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	char	buffer[SOCKETSIZE + 1] = {0};
	int		len = 0;

    switch(msg) {
        case WM_INITDIALOG:
			{
			char	text[128];
			RECT	rect = {0};
			int		xParent, yParent, xChild, yChild, x, y;
			HWND	hwndParent = (HWND) lParam;

			hAbout = hdlg;
			sprintf(text,"(C) Bouygues Telecom 1999%c%c%c%cService[DCTSI-IPS-IPA]%c%cAuthor[Cyril Courtiat]%c%cRelease[%s.%s]",
					13,10, 13,10, 13,10, 13,10, _MAJOR, _MINOR);
			SetWindowText(GetDlgItem(hdlg,IDC_COPYRIGHT), text);
			GetWindowRect(hwndParent, &rect);
			xParent = (rect.right  + rect.left) / 2;
			yParent = (rect.bottom + rect.top) / 2;
			GetWindowRect(hdlg, &rect);
			xChild = rect.right  - rect.left;
			yChild = rect.bottom - rect.top;
			x = xParent - (xChild / 2);
			y = yParent - (yChild / 2);
			x = (x + xChild  > wScreen) ? wScreen - xChild : (x < 0) ? 0 :x;
			y = (y + yChild > hScreen) ? hScreen - yChild : (y < 0) ? 0 : y;
			SetWindowPos(hdlg, NULL, x, y, 0, 0, SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE);

			sprintf(buffer, "%c%02d%3s%c", 'C', 4, "who", '\0' );
			len = SocketSend(TcpSocket, buffer);

			return(TRUE);
			}
        break;

        case WM_COMMAND:
			{
			int		wmId = LOWORD(wParam);

            switch(wmId) {
				case IDOK:
						EndDialog( hdlg, TRUE );
				break;
				}
			}
        break;
    }

    return(FALSE);
}



struct	SelectParam_t
{
	HWND	hParent;
	char	*title;
	char	*filename;
	char	request;
};

BOOL	OpenFileSelect(HWND hParent, char *title, char *filename)
{
		struct	SelectParam_t	sp;

		sp.hParent	= hParent;
		sp.title	= title;
		sp.filename = filename;
		sp.request	= 'C';

		return(DialogBoxParam(hinst, MAKEINTRESOURCE(ID_FILES), sp.hParent, FilesProc, (LPARAM) &sp));
}

BOOL	OpenMsgSelect(HWND hParent, char *title, char *filename)
{
		struct	SelectParam_t	sp;

		sp.hParent	= hParent;
		sp.title	= title;
		sp.filename = filename;
		sp.request	= 'M';

		return(DialogBoxParam(hinst, MAKEINTRESOURCE(ID_FILES), sp.hParent, FilesProc, (LPARAM) &sp));
}

BOOL APIENTRY FilesProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	char	buffer[SOCKETSIZE + 1] = {0};
	int		len = 0;

    switch(msg) {
        case WM_INITDIALOG:
			{
			RECT	rect = {0};
			int		xParent, yParent, xChild, yChild, x, y;
			struct	SelectParam_t *sp = (struct SelectParam_t *) lParam;

			hFiles = hdlg;
			SetWindowLong(hdlg, GWL_USERDATA, (LONG) sp);
			SetWindowText(hdlg, sp->title);

			GetWindowRect(sp->hParent, &rect);
			xParent = (rect.right  + rect.left) / 2;
			yParent = (rect.bottom + rect.top) / 2;
			GetWindowRect(hdlg, &rect);
			xChild = rect.right  - rect.left;
			yChild = rect.bottom - rect.top;
			x = xParent - (xChild / 2);
			y = yParent - (yChild / 2);
			x = (x + xChild > wScreen) ? wScreen - xChild : (x < 0) ? 0 : x;
			y = (y + yChild > hScreen) ? hScreen - yChild : (y < 0) ? 0 : y;
			SetWindowPos(hdlg, NULL, x, y, 0, 0, SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE);

			if( sp->request == 'M' ) EnableWindow( GetDlgItem(hdlg,IDC_FILENAME), FALSE );

			sprintf(buffer, "%c%02d%5s%c\0", 'C', 6, "files", sp->request );
			len = SocketSend(TcpSocket, buffer);

			return(TRUE);
			}
        break;

        case WM_COMMAND:
			{
			struct	SelectParam_t *sp = NULL;
			int		whId = HIWORD(wParam);
			int		wmId = LOWORD(wParam);

			switch(whId) {
				case LBN_DBLCLK:
					{
					HWND	hwndListBox = (HWND) lParam;
					int		curSel = 0;

					if( ((curSel = SendMessage(hwndListBox, LB_GETCURSEL, (WPARAM)0, (LPARAM)0)) != LB_ERR)
					 && ((sp = (struct SelectParam_t *) GetWindowLong(hdlg,GWL_USERDATA) ) != NULL) ) {
						SendMessage(hwndListBox, LB_GETTEXT, (WPARAM) curSel, (LPARAM)(LPCTSTR) sp->filename);
						EndDialog( hdlg, TRUE );
						}
					}
				break;

				case LBN_SELCHANGE:
					{
					HWND	hwndListBox = (HWND) lParam;
					int		curSel = 0;
					char	filename[12+1] = {0};

					if( (curSel = SendMessage(hwndListBox, LB_GETCURSEL, (WPARAM)0, (LPARAM)0)) != LB_ERR) {
						SendMessage(hwndListBox, LB_GETTEXT, (WPARAM) curSel, (LPARAM)(LPCTSTR) filename);
						SetWindowText(GetDlgItem(hdlg,IDC_FILENAME), filename );
						}
					}
				break;
				}

            switch(wmId) {
				case IDOK: {
					int	strsize = 0;

					if( ((sp = (struct SelectParam_t *) GetWindowLong(hdlg,GWL_USERDATA) ) != NULL)
					 && ((strsize = GetWindowText(GetDlgItem(hdlg,IDC_FILENAME), sp->filename, 12)) > 0 ) ) {
						sp->filename[strsize] = '\0';
						EndDialog( hdlg, TRUE );
					}
					else
						EndDialog( hdlg, FALSE );
					}
				break;

                case IDCANCEL:
                    EndDialog( hdlg, FALSE );
                break;
				}
			}
        break;
    }

    return(FALSE);
}


BOOL	OpenTCPIPsettings(HWND hParent)
{
		return(DialogBoxParam(hinst, MAKEINTRESOURCE(ID_TCPIP), hParent, TCPIPproc, (LPARAM) hParent));
}

BOOL APIENTRY TCPIPproc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	const	int	CtrlID[3] = {IDC_PORT1, IDC_PORT2, IDC_PORT3};

    switch(msg) {
        case WM_INITDIALOG:
			{
			char	data[32] = {0};
			int		i = 0;
			RECT	rect = {0};
			int		xParent, yParent, xChild, yChild, x, y;
			HWND	hwndParent = (HWND) lParam;

			GetWindowRect(hwndParent, &rect);
			xParent = (rect.right  + rect.left) / 2;
			yParent = (rect.bottom + rect.top) / 2;
			GetWindowRect(hdlg, &rect);
			xChild = rect.right  - rect.left;
			yChild = rect.bottom - rect.top;
			x = xParent - (xChild / 2);
			y = yParent - (yChild / 2);
			x = (x + xChild  > wScreen) ? wScreen - xChild : (x < 0) ? 0 :x;
			y = (y + yChild > hScreen) ? hScreen - yChild : (y < 0) ? 0 : y;
			SetWindowPos(hdlg, NULL, x, y, 0, 0, SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE);

			for( i = 0; i < MAXSERVICES; i++ ) {
				SendMessage(GetDlgItem(hdlg,IDC_NICKNAMES), LB_INSERTSTRING, (WPARAM) i, (LPARAM) nicknames[i] );
				sprintf( data, "%d", services[i]),
				SetWindowText(GetDlgItem(hdlg,CtrlID[i]), data );
				}
			}
			return(TRUE);
        break;

        case WM_COMMAND:
			{
			int		wmId = LOWORD(wParam);
			char	data[32] = {0};
			int		i = 0;

            switch(wmId) {
				case IDOK:
					for( i = 0; i < MAXSERVICES; i++ ) {
						GetWindowText(GetDlgItem(hdlg, CtrlID[i]), data, 32);
						services[i] = atoi(data);
					}
					EndDialog( hdlg, TRUE );
				break;

                case IDCANCEL:
                    EndDialog( hdlg, FALSE );
                break;
				}
			}
	}
    return(FALSE);
}

BOOL	OpenHostList(HWND hParent)
{
		if( DialogBoxParam(hinst, MAKEINTRESOURCE(ID_HOSTLIST), hParent, HostListProc, (LPARAM) hParent)
		&& !ConnectToServer() )
			{
			_beginthread( TcpWatcher, 0, hmain );
			_beginthread( UdpWatcher, 0, hmain );
			return(TRUE);
			}
		return(FALSE);
}

BOOL APIENTRY HostListProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	int		i = 0;

    switch(msg) {
        case WM_INITDIALOG:
			{
			char	portString[80] = {0};
			RECT	rect = {0};
			int		xParent, yParent, xChild, yChild, x, y;
			HWND	hwndParent = (HWND) lParam;

			SetWindowText(GetDlgItem(hdlg,IDC_HOSTINPUT), host);

			SetWindowText(GetDlgItem(hdlg,IDC_SERVICES), nicknames[portindex] );
			for( i = 0; i < MAXSERVICES; i++ )
				SendMessage(GetDlgItem(hdlg,IDC_SERVICES), CB_INSERTSTRING, (WPARAM) i, (LPARAM) nicknames[i] );

			for( i = 0; i < MAXHOSTLIST; i++ )
				SendMessage(GetDlgItem(hdlg,IDC_SERVERS), LB_ADDSTRING, (WPARAM) 0, (LPARAM) hostlist[i] );

			GetWindowRect(hwndParent, &rect);
			xParent = (rect.right  + rect.left) / 2;
			yParent = (rect.bottom + rect.top) / 2;
			GetWindowRect(hdlg, &rect);
			xChild = rect.right  - rect.left;
			yChild = rect.bottom - rect.top;
			x = xParent - (xChild / 2);
			y = yParent - (yChild / 2);
			x = (x + xChild > wScreen) ? wScreen - xChild : (x < 0) ? 0 : x;
			y = (y + yChild > hScreen) ? hScreen - yChild : (y < 0) ? 0 : y;
			SetWindowPos(hdlg, NULL, x, y, 0, 0, SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE);

			return(TRUE);
			}
        break;

        case WM_COMMAND:
			{
			char	nickname[16] = {0};
			char	hostString[32] = {0};
			int		whId = HIWORD(wParam);
			int		wmId = LOWORD(wParam);
			HWND	hwndListBox = (HWND) lParam;

			if( ((whId == LBN_SELCHANGE) || (whId == LBN_DBLCLK)) && (hwndListBox == GetDlgItem(hdlg,IDC_SERVERS)) )
				{
				int		curSel = 0;

				if( (curSel = SendMessage(hwndListBox, LB_GETCURSEL, (WPARAM)0, (LPARAM)0)) != LB_ERR) {
					SendMessage(hwndListBox, LB_GETTEXT, (WPARAM) curSel, (LPARAM)(LPCTSTR) hostString);
					SetWindowText(GetDlgItem(hdlg,IDC_HOSTINPUT), hostString );
					}

				wmId = (whId == LBN_DBLCLK) ? IDOK : 0;
				}
            switch(wmId) {
				case IDOK:
					if( GetWindowText(GetDlgItem(hdlg,IDC_HOSTINPUT),hostString, 32) > 0) {
						strcpy(host, hostString);
						for( i = (MAXHOSTLIST - 1); i > 0 ; i-- )
							strcpy(hostlist[i], hostlist[i-1]);
						strcpy(hostlist[0], host);
						}
					if( GetWindowText(GetDlgItem(hdlg,IDC_SERVICES), nickname, 80 ) > 0) {
							struct	servent *servdata = NULL;

							for( i = 0; i < MAXSERVICES; i++ )
								if( !stricmp(nickname, nicknames[i]) )
									break;
							if( i < MAXSERVICES ) portindex = i; else portindex = 0;
/*
							if( (servdata = getservbyname( services[i], "tcp" )) != NULL)
								port = ntohs(servdata->s_port);
							else
								port = (u_short) atoi(portString);
*/
					}
					EndDialog( hdlg, TRUE );
				break;

                case IDCANCEL:
                    EndDialog( hdlg, FALSE );
                break;
				}
			}
        break;
    }

    return(FALSE);
}


struct	PropertyParam_t
{
	HWND	hParent;
	int		record;
};

void	LoadPropertyWindow(void *arg)
{
		struct	PropertyParam_t *pp = (struct PropertyParam_t *) arg;

		DialogBoxParam(hinst, MAKEINTRESOURCE(ID_PROPERTY), pp->hParent, PropertyProc, (LPARAM) pp);
		free(pp);
}

BOOL	OpenPropertyWindow(int record, HWND hParent)
{
		struct	PropertyParam_t	*pp;

		pp = (struct PropertyParam_t *) malloc(sizeof(struct PropertyParam_t));
		pp->record = record;
		pp->hParent = hParent;
		if( _beginthread( LoadPropertyWindow, 0, pp ) == -1 ) {
			free(pp);
			return(FALSE);
			}
		else
			return(TRUE);
}

BOOL APIENTRY PropertyProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	char	buffer[SOCKETSIZE + 1] = {0};
	char	record[5 + 1], din[10 + 1] = {0}, duration[5 + 1];
	int		len = 0;
	int		wmId = 0;

    switch(msg) {
        case WM_INITDIALOG:
			{
			RECT	rect = {0};
			LONG	w = 0, h = 0;
			int		x = 0, y = 0;
			struct	PropertyParam_t *pp = (struct PropertyParam_t *) lParam;

			sprintf( record, "%05d", pp->record);
			SendMessage(GetDlgItem(hdlg,IDC_RECORD), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) record);
			SendMessage(hNumber[pp->record], WM_GETTEXT, 10 + 1, (LPARAM) (LPCTSTR) din);
			SendMessage(GetDlgItem(hdlg,IDC_MSISDN), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) din);
			sprintf( duration, "%05d", GetWindowLong(hNumber[pp->record],GWL_USERDATA) );
			SendMessage(GetDlgItem(hdlg,IDC_TIMEOUT), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) duration);
			GetWindowRect(hdlg, &rect);
			w = rect.right - rect.left;
			h = rect.bottom - rect.top;
			GetWindowRect(hNumber[pp->record], &rect);
			x = (rect.left + w > wScreen) ? wScreen - w : (rect.left < 0) ? 0 : rect.left;
			y = (rect.top + h > hScreen)  ? hScreen - h : (rect.top  < 0) ? 0 : rect.top;
			SetWindowPos(hdlg,NULL,x,y,0,0,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE|SWP_SHOWWINDOW);

			return (TRUE);
			}
        break;

        case WM_COMMAND:
			wmId = LOWORD(wParam);
            switch(wmId) {
				case IDOK:
					SendMessage(GetDlgItem(hdlg,IDC_RECORD), WM_GETTEXT, 5 + 1, (LPARAM) (LPCTSTR) record);
					SendMessage(GetDlgItem(hdlg,IDC_MSISDN), WM_GETTEXT, 10 + 1, (LPARAM) (LPCTSTR) din);
					SendMessage(GetDlgItem(hdlg,IDC_TIMEOUT), WM_GETTEXT, 5 + 1, (LPARAM) (LPCTSTR) duration);
					if( strlen(din) > 0 ) {
						sprintf(buffer, "%c%02d%6s%5s%5s%02d%s",
										'C', 18 + strlen(din), "msisdn", record, duration, strlen(din), din);
						len = SocketSend(TcpSocket, buffer);
					}
					EndDialog( hdlg, TRUE );
				break;

                case IDCANCEL:
                    EndDialog( hdlg, FALSE );
                break;
			}
        break;
    }

    return(FALSE);
}


void	LoadMultiPropertiesWindow(void *arg)
{
		HWND	hParent = (HWND) arg;

		DialogBoxParam(hinst, MAKEINTRESOURCE(ID_PROPERTIES), hParent, MultiPropertiesProc, (LPARAM) hParent);
}

BOOL	OpenMultiPropertiesWindow(HWND hParent)
{
		if( _beginthread( LoadMultiPropertiesWindow, 0, (void *) hParent ) == -1 )
			return(FALSE);
		else
			return(TRUE);
}

BOOL APIENTRY MultiPropertiesProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	char	buffer[SOCKETSIZE + 1] = {0};
	char	din[10 + 1] = {0}, duration[5 + 1];
	int		len = 0;
	int		wmId = 0;

    switch(msg) {
        case WM_INITDIALOG:
			{
			RECT	rect = {0};
			int		xParent, yParent, xChild, yChild, x, y;
			HWND	hwndParent = (HWND) lParam;

			GetWindowRect(hwndParent, &rect);
			xParent = (rect.right  + rect.left) / 2;
			yParent = (rect.bottom + rect.top) / 2;
			GetWindowRect(hdlg, &rect);
			xChild = rect.right  - rect.left;
			yChild = rect.bottom - rect.top;
			x = xParent - (xChild / 2);
			y = yParent - (yChild / 2);
			x = (x + xChild > wScreen) ? wScreen - xChild : (x < 0) ? 0 : x;
			y = (y + yChild > hScreen) ? hScreen - yChild : (y < 0) ? 0 : y;
			SetWindowPos(hdlg,NULL,x,y,0,0,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE|SWP_SHOWWINDOW);

			return (TRUE);
			}
        break;

        case WM_COMMAND:
			{
			int		wNotifyCode, wmId;
			wNotifyCode = HIWORD(wParam);
			wmId = LOWORD(wParam);

            switch(wmId) {
				case IDOK:
					{
					short	test = 0;

					test |= (SendMessage(GetDlgItem(hdlg, IDM_MSISDNLOCK),BM_GETCHECK, 0,0) == BST_CHECKED)  ? 2:0;
					test |= (SendMessage(GetDlgItem(hdlg, IDM_TIMEOUTLOCK),BM_GETCHECK, 0,0) == BST_CHECKED) ? 1:0;

					switch(test)
						{
						case 1: 
							{
							int		record;
							SendMessage(GetDlgItem(hdlg,IDM_TIMEOUT), WM_GETTEXT, 5 + 1, (LPARAM) (LPCTSTR) duration);
							if( strlen(duration) > 0 )
								for( record = 0; record < channels; record++ )
									if(!IsWindowEnabled( hNumber[record] ) ) {
										SendMessage(hNumber[record], WM_GETTEXT, 10 + 1, (LPARAM) (LPCTSTR) din);
										sprintf(buffer, "%c%02d%6s%5d%5s%02d%s",
												'C', 18 + strlen(din), "msisdn", record, duration, strlen(din), din);
										if( (len = SocketSend(TcpSocket, buffer)) > 0 )
											EnableWindow( hNumber[record], TRUE );
									}
							}
						break;
						case 2:
							{
							int		record;
							SendMessage(GetDlgItem(hdlg,IDM_MSISDN), WM_GETTEXT, 10 + 1, (LPARAM) (LPCTSTR) din);
							if( strlen(din) > 0 )
								for( record = 0; record < channels; record++ )
									if(!IsWindowEnabled( hNumber[record] ) ) {
										sprintf( duration, "%05d", GetWindowLong(hNumber[record], GWL_USERDATA) );
										sprintf(buffer, "%c%02d%6s%5d%5s%02d%s",
												'C', 18 + strlen(din), "msisdn", record, duration, strlen(din), din);
										if( (len = SocketSend(TcpSocket, buffer)) > 0 )
											EnableWindow( hNumber[record], TRUE );
									}
							}
						break;
						case 3:
							{
							int		record;

							SendMessage(GetDlgItem(hdlg,IDM_MSISDN), WM_GETTEXT, 10 + 1, (LPARAM) (LPCTSTR) din);
							SendMessage(GetDlgItem(hdlg,IDM_TIMEOUT), WM_GETTEXT, 5 + 1, (LPARAM) (LPCTSTR) duration);
							if( (strlen(duration) > 0) && (strlen(din) > 0) )
								for( record = 0; record < channels; record++ )
									if(!IsWindowEnabled( hNumber[record] ) ) {
										sprintf(buffer, "%c%02d%6s%5d%5s%02d%s",
												'C', 18 + strlen(din), "msisdn", record, duration, strlen(din), din);
										if( (len = SocketSend(TcpSocket, buffer) ) > 0 )
											EnableWindow( hNumber[record], TRUE );
									}
							}
						break;
						}
					EndDialog( hdlg, TRUE );
					}					
				break;

                case IDCANCEL:
                    EndDialog( hdlg, FALSE );
                break;

				case IDM_MSISDNLOCK:
					{
					HWND	hInputField = GetDlgItem(hdlg, IDM_MSISDN);
					HWND	hCheckBox = GetDlgItem(hdlg, IDM_MSISDNLOCK);
					EnableWindow(hInputField, (SendMessage(hCheckBox,BM_GETCHECK, 0,0) == BST_CHECKED));
					}
				break;

				case IDM_TIMEOUTLOCK:
					{
					HWND	hInputField = GetDlgItem(hdlg, IDM_TIMEOUT);
					HWND	hCheckBox = GetDlgItem(hdlg, IDM_TIMEOUTLOCK);
					EnableWindow(hInputField, (SendMessage(hCheckBox,BM_GETCHECK, 0,0) == BST_CHECKED));
					}
				break;
				}
			}
        break;
    }

    return(FALSE);
}



void	LoadOptionsWindow(void *arg)
{
		HWND	hParent = (HWND) arg;

		DialogBoxParam(hinst, MAKEINTRESOURCE(ID_PARAMETERS), hParent, OptionsProc, (LPARAM) hParent );
}

BOOL	OpenOptionsWindow(HWND hParent)
{
		if( _beginthread( LoadOptionsWindow, 0, hParent ) == -1 )
			return(FALSE);
		else
			return(TRUE);
}

struct	ctrlHelp_st {
		int		ctrlID;
		char	*info;
		}
	buttonHelp[] = {
	/*0*/	IDOK,"Submit new options to the Call Generator.",
	/*1*/	IDCANCEL,"Cancel operation",
	/*2*/	IDC_VDN, "Input up to 10 digits",
	/*3*/	IDC_CALLS, "Input a number greater or equal to 1 and less or equal to 240",
	/*4*/	IDC_DURATION, "Input a number of seconds greater or equal to 1",
	/*5*/	IDC_MESSAGE, "Vocal message to play while the outgoing calls.",
	/*6*/	IDC_DIALING,"Input a number of seconds greater or equal to 1. A minimum of 3 seconds is recommended.",
	/*7*/	IDC_BUSY,"Input a number of seconds greater or equal to 1. A minimum of 3 seconds is recommended.",
	/*8*/	IDC_FREEING,"Input a number of seconds greater or equal to 1",
	/*9*/	IDBROWSE,"Click Browse to display a list of available messages in the server.",
	/*10*/	IDC_LOOP,"Input a value greater than 0 or -1 for illimited loop."
			},
	staticHelp[] = {
	/*0*/	IDC_WARNING, "Hit any text to get extended information.",
	/*1*/	IDC_VECTOR, "PBX or Network number. Each phone dials this number in the Call Generator.",
	/*2*/	IDC_MAXCALLS, "Total number of phones to process in the Call Generator.",
	/*3*/	IDC_CALLDURATION, "Default calls duration. This value will only be set to new phones.",
	/*4*/	IDC_LOOPTEXT, "Number of loops each phone dials the VDN.",
	/*5*/	IDC_DIALINGDELAY, "Delay before calls in dialing process time out.",
	/*6*/	IDC_BUSYDELAY,"Idle time before recycling busy calls.",
	/*7*/	IDC_FREEINGDELAY,"Idle time before recycling disconnected calls.",
			};

BOOL APIENTRY OptionsProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	char	buffer[SOCKETSIZE + 1] = {0};
	char	data[12 + 1] = {0};
	int		len = 0;
	int		wmId;

    switch(msg) {
        case WM_INITDIALOG:
			{
			RECT	rect = {0};
			int		xParent, yParent, xChild, yChild, x, y;
			HWND	hwndParent = (HWND) lParam;

			SetWindowText(GetDlgItem(hdlg,IDC_WARNING), staticHelp[0].info );
			SetWindowLong(hdlg, GWL_USERDATA, (LONG) GetDlgItem(hdlg,IDC_WARNING));
			SendMessage(GetDlgItem(hdlg,IDC_VDN), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) vdn);
			sprintf( data, "%03d", channels );
			SendMessage(GetDlgItem(hdlg,IDC_CALLS), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) data);
			sprintf( data, "%03d", CALL_DURATION );
			SendMessage(GetDlgItem(hdlg,IDC_DURATION), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) data);
			sprintf( data, "%03d", DIALING_DELAY );
			SendMessage(GetDlgItem(hdlg,IDC_DIALING), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) data);
			sprintf( data, "%03d", BUSY_DELAY );
			SendMessage(GetDlgItem(hdlg,IDC_BUSY), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) data);
			sprintf( data, "%03d", FREEING_DELAY );
			SendMessage(GetDlgItem(hdlg,IDC_FREEING), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) data);
			sprintf( data, "%d", MAX_LOOP );
			SendMessage(GetDlgItem(hdlg,IDC_LOOP), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) data);
			SendMessage(GetDlgItem(hdlg,IDC_MESSAGE), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) currentMsgFile);

			GetWindowRect(hwndParent, &rect);
			xParent = (rect.right  + rect.left) / 2;
			yParent = (rect.bottom + rect.top) / 2;
			GetWindowRect(hdlg, &rect);
			xChild = rect.right  - rect.left;
			yChild = rect.bottom - rect.top;
			x = xParent - (xChild / 2);
			y = yParent - (yChild / 2);
			x = (x + xChild > wScreen) ? wScreen - xChild : (x < 0) ? 0 : x;
			y = (y + yChild > hScreen) ? hScreen - yChild : (y < 0) ? 0 : y;
			SetWindowPos(hdlg, NULL, x, y, 0,0,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE|SWP_SHOWWINDOW);

			return(TRUE);
			}
        break;

		case WM_SETCURSOR:
			{
			HWND	hTest = (HWND) wParam;		// handle of window with cursor
			//WORD	nHittest = LOWORD(lParam);	// hit-test code
			//WORD	wMouseMsg = HIWORD(lParam);	// mouse-message identifier
			short	i;

			for( i = 0; i <= 10; i++ ) {
				if( hTest == GetDlgItem(hdlg, buttonHelp[i].ctrlID) )
					if( (HWND) GetWindowLong(hdlg, GWL_USERDATA) != hTest ) {
						SetWindowText(GetDlgItem(hdlg,IDC_WARNING), buttonHelp[i].info );
						SetWindowLong(hdlg, GWL_USERDATA, (LONG) hTest);
					}
				}
			}
		break;

		case WM_NCHITTEST:
			{
			HWND	hStatic;
			RECT	rect;
			POINT	point;
			short	i;

			point.x = (LONG) LOWORD(lParam);
			point.y = (LONG) HIWORD(lParam);

			for( i = 0; i <= 7; i++ ) {
				hStatic = GetDlgItem(hdlg, staticHelp[i].ctrlID);
				GetWindowRect( hStatic, &rect );
				if( PtInRect( &rect, point ) == TRUE )
					if( (HWND) GetWindowLong(hdlg, GWL_USERDATA) != hStatic ) {
						SetWindowText(GetDlgItem(hdlg,IDC_WARNING), staticHelp[i].info );
						SetWindowLong(hdlg, GWL_USERDATA, (LONG) hStatic);
					}
				}
			}
		break;

        case WM_COMMAND:
			wmId = LOWORD(wParam);
            switch(wmId) {
				case IDOK:
					{
					char	newVdn[16] = {0};
					int		newChannels = 0, newDuration = 0, newDialing = 0,
							newBusy = 0, newFreeing = 0, newLoop = 0;

					SendMessage(GetDlgItem(hdlg,IDC_VDN), WM_GETTEXT, 16 + 1, (LPARAM) (LPCTSTR) newVdn);
					SendMessage(GetDlgItem(hdlg,IDC_CALLS), WM_GETTEXT, 5 + 1, (LPARAM) (LPCTSTR) data);
					newChannels = (atoi(data) > TABLESIZE) ? TABLESIZE : atoi(data);
					SendMessage(GetDlgItem(hdlg,IDC_DURATION), WM_GETTEXT, 5 + 1, (LPARAM) (LPCTSTR) data);
					newDuration = atoi(data);
					SendMessage(GetDlgItem(hdlg,IDC_DIALING), WM_GETTEXT, 5 + 1, (LPARAM) (LPCTSTR) data);
					newDialing = atoi(data);
					SendMessage(GetDlgItem(hdlg,IDC_BUSY), WM_GETTEXT, 5 + 1, (LPARAM) (LPCTSTR) data);
					newBusy = atoi(data);
					SendMessage(GetDlgItem(hdlg,IDC_FREEING), WM_GETTEXT, 5 + 1, (LPARAM) (LPCTSTR) data);
					newFreeing = atoi(data);
					SendMessage(GetDlgItem(hdlg,IDC_LOOP), WM_GETTEXT, 5 + 1, (LPARAM) (LPCTSTR) data);
					newLoop = atoi(data);
					SendMessage(GetDlgItem(hdlg,IDC_MESSAGE), WM_GETTEXT, 12, (LPARAM) (LPCTSTR) data);
					
					if( (strlen(newVdn) > 0) && (newChannels > 0) && (newDuration > 0)
					 && (newDialing > 0) && (newBusy > 0) && (newFreeing > 0)
					 && ((newLoop == -1) || (newLoop > 0)) )
						{
						sprintf(buffer, "%c%02d%6s%05d%05d%05d%05d%05d%05d%s%c%s%c", 'C', 38 + strlen(newVdn) + strlen(data),
							"config", newChannels, newDuration, newDialing, newBusy, newFreeing, newLoop, newVdn, '\0', data, '\0' );
						len = SocketSend(TcpSocket, buffer);
						}
                    EndDialog( hdlg, TRUE );
					}
				break;

                case IDCANCEL:
                    EndDialog( hdlg, FALSE );
                break;

				case IDBROWSE:
					{
					char	messageFile[12] = {0};

					if( OpenMsgSelect(hdlg, "Select Vocal Message", messageFile) )
						SendMessage(GetDlgItem(hdlg,IDC_MESSAGE), WM_SETTEXT, 0, (LPARAM) (LPCTSTR) messageFile);
					}
				break;
			}
        break;
    }

    return(FALSE);
}


void	LoadSelectWindow(void *arg)
{
		HWND	hParent = (HWND) arg;

		DialogBoxParam(hinst, MAKEINTRESOURCE(ID_SELECT), hParent, SelectionProc, (LPARAM) hParent);
}

BOOL	OpenSelectWindow(HWND hParent)
{
		if( _beginthread( LoadSelectWindow, 0, (void *) hParent ) == -1 )
			return(FALSE);
		else
			return(TRUE);
}

BOOL APIENTRY SelectionProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch(msg) {
        case WM_INITDIALOG:
			{
			char	text[] = "To     [00000]";
			RECT	rect = {0};
			int		xParent, yParent, xChild, yChild, x, y;
			HWND	hwndParent = (HWND) lParam;

			sprintf( text, "To     [%05d]", channels - 1 );
			SendMessage(GetDlgItem(hdlg,IDC_SELECT_TO), WM_SETTEXT, 5 + 1, (LPARAM) (LPCTSTR) text);
			GetWindowRect(hwndParent, &rect);
			xParent = (rect.right  + rect.left) / 2;
			yParent = (rect.bottom + rect.top) / 2;
			GetWindowRect(hdlg, &rect);
			xChild = rect.right  - rect.left;
			yChild = rect.bottom - rect.top;
			x = xParent - (xChild / 2);
			y = yParent - (yChild / 2);
			x = (x + xChild > wScreen) ? wScreen - xChild : (x < 0) ? 0 : x;
			y = (y + yChild > hScreen) ? hScreen - yChild : (y < 0) ? 0 : y;
			SetWindowPos(hdlg,NULL,x,y,0,0,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE|SWP_SHOWWINDOW);

			return (TRUE);
			}
        break;

        case WM_COMMAND:
			{
			int		wNotifyCode, wmId;
			wNotifyCode = HIWORD(wParam);
			wmId = LOWORD(wParam);

            switch(wmId) {
				case IDOK:
					{
					int		From = -1, To = -1, record = 0;
					char	data[12 + 1] = {0};

					SendMessage(GetDlgItem(hdlg,IDC_EDIT_FROM), WM_GETTEXT, 5 + 1, (LPARAM) (LPCTSTR) data);
					From = atoi(data);
					SendMessage(GetDlgItem(hdlg,IDC_EDIT_TO), WM_GETTEXT, 5 + 1, (LPARAM) (LPCTSTR) data);
					To = atoi(data);

					if( (From >= 0) && (To < channels) )
						for( record = From; record <= To; record++ )
							EnableWindow( hNumber[record], FALSE );

					EndDialog( hdlg, TRUE );
					}					
				break;

                case IDCANCEL:
                    EndDialog( hdlg, FALSE );
                break;
				}
			}
        break;
    }

    return(FALSE);
}




void	LoadEmergencyWindow(void *arg)
{
		HWND	hParent = (HWND) arg;

		DialogBoxParam(hinst, MAKEINTRESOURCE(ID_EMERGENCY), hParent, EmergencyProc, (LPARAM) hParent);
		PostMessage(hmain, WM_QUIT, (WPARAM) 9, (LPARAM) NULL);
}

BOOL	EmergencyExit(HWND hParent)
{
		if( _beginthread( LoadEmergencyWindow, 0, (void *) hParent ) == -1 )
			return(FALSE);
		else
			return(TRUE);
}

BOOL APIENTRY EmergencyProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch(msg) {
        case WM_INITDIALOG:
			{
			RECT	rect = {0};
			int		xParent, yParent, xChild, yChild, x, y;
			HWND	hwndParent = (HWND) lParam;

			GetWindowRect(hwndParent, &rect);
			xParent = (rect.right  + rect.left) / 2;
			yParent = (rect.bottom + rect.top) / 2;
			GetWindowRect(hdlg, &rect);
			xChild = rect.right  - rect.left;
			yChild = rect.bottom - rect.top;
			x = xParent - (xChild / 2);
			y = yParent - (yChild / 2);
			x = (x + xChild > wScreen) ? wScreen - xChild : (x < 0) ? 0 : x;
			y = (y + yChild > hScreen) ? hScreen - yChild : (y < 0) ? 0 : y;
			SetWindowPos(hdlg,NULL,x,y,0,0,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE|SWP_SHOWWINDOW);

			SetTimer( hdlg, 1, 5000, NULL );
 			return(TRUE);
			}
        break;
		case WM_TIMER:
			EndDialog( hdlg, TRUE );
		break;
	}

    return(FALSE);
}