#define	_MAJOR	"0"
#define	_MINOR	"01"
#define AutoDate "@(#) Call Windows - "_MAJOR"."_MINOR" (C) Bouygues Telecom "__DATE__
static  char    version[] = AutoDate;
/*
*       (C) 1997,98,99 Bouygues Telecom
*
*       Projet          [ CallWIN - Générateur d'appels          ]
*       Direction       [ DCTSI-IPS                              ]
*       Auteurs         [ Courtiat Cyril,                        ]
*
*       Fichier         [ CallWIN.c                           	 ]
*
*       Description :   Traitements specifiques Call Manager & WIN32 API
*
*/

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <afxres.h>
#include <windows.h>
#include <winuser.h>
#include <process.h>
#include "CallGen.h"
#include "CallWIN.h"

#include "CallMgr.h"


CRITICAL_SECTION CriticalData = {0} , CriticalSessions = {0};
_Sessions		*Sessions[TABLESIZE] = {NULL};
int				Sequence = 0;
int				WCalls = 0;



void	AddMessage(char *buffer)
{
		char	foutput[256];

		sprintf( foutput, "%s\n", buffer );
		printf( foutput );
		fflush(stdout);
}

int		main(void)
{
		WSADATA		WsaData = {0};
		int			ret = 0;

		cmInitializeCallManager();

		InitializeCriticalSection( &CriticalData );
		InitializeCriticalSection( &CriticalSessions );
		ret = WSAStartup (0x0101, &WsaData);

		AddMessage( "Welcome to the Call Simulator. (C) 1998, Copyright Bouygues Telecom" );

		AppSTART();

		getchar();

		AppSTOP(0);

		WSACleanup();
		DeleteCriticalSection( &CriticalData );
		DeleteCriticalSection( &CriticalSessions );

		cmTerminateCallManager();

		return(ret);
}


/*----------------------------------- UNIX style functions ----------------------------------------*/

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

/*----------------------------------- CallGen abstract layer ----------------------------------------*/

typedef struct	{
		int		session;
		int		portHandle;
		char	dialstring[64];
		int		callStatus;
}	WCallArgs;

DWORD	WCall(LPVOID Targ)
{
		WCallArgs	*WCargs = (WCallArgs *) Targ;
		char		datastring[16] = {0};
		int			portHandle = -1;
		BOOL		fTimeout = FALSE;
		int			ret = FAILURE;

		WCargs->callStatus = cmMakeCall(WCargs->dialstring, &WCargs->portHandle);

		if( WCargs->portHandle != -1 ) {
			printf(	"Placing Call to %s on resource %d\n", WCargs->dialstring, WCargs->portHandle );

			EnterCriticalSection( &CriticalData );
				{
				WCalls++ ;

				if( Sequence < 0xFFFF )
					Sequence++ ;
				else
					Sequence = 0;
				}
			LeaveCriticalSection( &CriticalData );

			sprintf( datastring, "%d", WCargs->portHandle );
			cgSetSessionKey( WCargs->session, "$chan", datastring ); 
			sprintf( datastring, "%d", Sequence );
			cgSetSessionKey( WCargs->session, "$uref", datastring );

			switch( WCargs->callStatus ) {
				case TIMEOUT :
					cgSetSessionKey( WCargs->session, "$strin", "?");
					cgSetSessionKey( WCargs->session, "$key", "TIME");
					AllEvent();
				break;
				case BUSY :
					cgSetSessionKey( WCargs->session, "$strin", "?");
					cgSetSessionKey( WCargs->session, "$key", "BUSY");
					BusyEvent();
				break;
				case SUCCESS :
					cgSetSessionKey( WCargs->session, "$strin", "?");
					cgSetSessionKey( WCargs->session, "$key", "NEXT");
					NextEvent();

					while( !fTimeout ) {
						if( !strcmp( IsTimeout(), "TIMEOUT+EVT0" ) )
							fTimeout = TRUE;
						else
							Sleep(500);
					}
				break;
			}

			cgDiscCall(WCargs->session);

			cmReleaseCall( WCargs->portHandle );

			EnterCriticalSection( &CriticalData );
				{
				WCalls-- ;
				}
			LeaveCriticalSection( &CriticalData );
		}
		cgDeleteSession(WCargs->session);

		free(WCargs);

		return(0);
}

void	WClock(void *Targ)
{
		void	( *Func ) ( void * ) = Targ;

		srand( (unsigned)time( NULL ) );
		while(TRUE) {
			Sleep(1000);
			Func(NULL);
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
		int	ret = -1;

		EnterCriticalSection( &CriticalSessions );
			{
			if( Sessions[session] != NULL ) {
				free( Sessions[session] );
				Sessions[session] = NULL;
				ret = 0;
				}
			else
				ret = -1;
			}
		LeaveCriticalSection( &CriticalSessions );
		return(ret);
}

int		cgMakeCall(int session)
{
		WCallArgs	*Targ = (WCallArgs *) malloc(sizeof(WCallArgs));

		Targ->session = session;

		strcpy( Targ->dialstring, cgGetSessionKey(Targ->session, "$VDN"));

		CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) WCall, (LPVOID) Targ, 0, &Sessions[session]->CallID);

		return( Sessions[session]->CallID );
}

int		cgDiscCall(int session)
{
		UserDISC(0);

		return(0);
}
