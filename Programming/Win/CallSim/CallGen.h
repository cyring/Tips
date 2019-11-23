/*
*       (C) 1997-98, Bouygues Telecom
*
*       Projet          [ CallGen - Generateur d'appels          ]
*       Direction       [ DCTSI-IPS                              ]
*       Auteurs         [ Courtiat Cyril,                        ]
*
*       Fichier         [ CallGen.h                           	 ]
*
*       Description :   Dialogue Socket avec les processus clients Windows NT
*
*/

/*---------------------------------------------------------------------------*/

#define	TABLESIZE	240
#define	UNKNOWN_STATUS	0
#define	OFFLINE_STATUS	1
#define	ONLINE_STATUS	2
#define	BUSY_STATUS	3
#define	DIALING_STATUS	4
#define	FREEING_STATUS	5

struct	Call_t
	{
	int	channel;
	int	sequence;
	int	userID;
	char	din[32];
	int	status;
	int	count;
	int	duration;
	int	loop;
	};

typedef struct	{
	int	HHH;
	int	MM;
	int	SS;
	} TIMER_T;

#define	CALLGENCFGFILE	"CallGen.cfg"
#define CONFIGDIRECTORY	"./configFiles"
#define	MSGDIRECTORY	"./messages"
#define	DEFAULTCONFIG	".default"
#define	EMPTYMESSAGE	"---none---"
#define	MAXSERVICES	3
#define	SOCKETSIZE	80
#define	CLIENTS		5
#define	STDLOGNAME	"./traces.log"
#define	QUOTE		(char) 34

#define	FMTINFO_SAVE	"S%.12s"		/* <SAVE><FILE> */
#define	FMTINFO_DEL	"D%.12s"		/* <DELETE><FILE> */
#define	FMTINFO_ERR	"E%c%03d"	/* <ERROR><CODE><PHONEID> */
#define	ERRCODE_CTX	'C'		/* "CAN NOT CREATE CALL CONTEXT" */
#define	ERRCODE_INUSE	'U'		/* "CAN NOT DELETE <FILE>. CONFIGURATION IN USE" */


/*---------------------------- Prototypes BEGIN -----------------------------*/

void			fstderr(char *format, ...);
int				sendInformation(int client, char *format, ...);
int				sendEngineStatus(int client);
int				sendConnections(int client, int counters);
int				sendTiming(int client, TIMER_T *pTIMER);
int				sendDebugStatus(int client);
int				sendConfig(int client);
void			AppSTOP( int sigNum );
#ifndef WIN32
#define			SocketClose		close
void			AppSTART();
#else
#define			SocketClose		closesocket
int				AppSTART();
#endif

/*---------------------------- Prototypes  END  -----------------------------*/
