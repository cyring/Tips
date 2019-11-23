/*
*       (C) 1997-98, Bouygues Telecom
*
*       Projet          [ CallSim - Simulateur d'appels          ]
*       Direction       [ DCTSI-IPS                              ]
*       Auteurs         [ Courtiat Cyril,                        ]
*
*       Fichier         [ CallWIN.h                           	 ]
*
*       Description :   Traitements specifiques API WIN 32
*
*/

/*------------------------------ Extended Functions -------------------------*/
#include <windows.h>

#define	TID				unsigned long

typedef struct	_DIR {
		BOOL			first;
		HANDLE			hFile;
		char			current[256];
}	DIR;

struct	dirent {
		char			d_name[256];
};

#define	SOCKETNULL		0
#define	SOCKETDESC		SOCKET
#define	SOCKETSTRUCT	SOCKADDR_IN
#define	PORTDEF			u_short

int				OpenMainWindow(LPSTR lpCmdLine);
BOOL APIENTRY	MainProc( HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT			AddMessage(char *buffer);

typedef	struct	 {
		char	var[32];
		char	val[32];
		} _Key;

typedef	struct	 {
		DWORD		CallID;
		int			Keys;
		_Key		Key[16];
		} _Sessions;

DIR		*opendir(char *dirName);
void	closedir(DIR *pDIR);
struct	dirent	*readdir(DIR *pDIR);
void	cgClockFunc(void *FuncAddr );
int		cgCreateSession();
int		cgSetSessionKey(int session, char *key, char *val);
char	*cgGetSessionKey(int session, char *key);
int		cgDeleteSession(int session);
int		cgMakeCall(int session);
int		cgDiscCall(int session);
