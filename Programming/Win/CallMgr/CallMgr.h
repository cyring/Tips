/*
*       (C) 1997,98,99 Bouygues Telecom
*
*       Projet          [ Call Manager								   ]
*       Direction       [ DCTSI/IPS/IPA                                ]
*       Auteurs         [ Courtiat Cyril                               ]
*
*       Fichier         [ CallMgr.h                                	   ]
*
*       Description :   Gestionnaire de communication
*
*/

/*---------------------------------------------------------------------------*/

int		cmInitializeCallManager(void);
void	cmTerminateCallManager(void);
int		cmMakeCall(char *dialstring, int *portHandle);
int		cmReleaseCall(int portHandle);

#ifndef SUCCESS
  #define SUCCESS		0
#endif
#define FAILURE			1
#define DISCONNECT		2
#define BUSY			3
#define TIMEOUT			4
