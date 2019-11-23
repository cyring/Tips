#define AutoDate "@(#) Call Generator - 4.0.0 (C) Bouygues Telecom "__DATE__
static  char    version[] = AutoDate;
/*
*       (C) 1997,98,99 Bouygues Telecom
*
*       Projet          [ CallGen - Generateur d'appels          ]
*       Direction       [ DCTSI-IPS                              ]
*       Auteurs         [ Courtiat Cyril,                        ]
*
*       Fichier         [ CallGen.c                           	 ]
*
*       Description :   Dialogue Socket avec les processus clients Windows NT
*
*/

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#ifdef WIN32
#include <ctype.h>
#include <winsock.h>
#include <process.h>
#include "CallWIN.h"
#else
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include "CallMG2.h"
#endif
#include "CallGen.h"

const	char *pSTATUS[] = { "UNKNOWN_STATUS", "OFFLINE_STATUS", "ONLINE_STATUS",
							"BUSY_STATUS",    "DIALING_STATUS", "FREEING_STATUS" };

struct	Call_t	Call[TABLESIZE] = { -1, -1, -1, "", UNKNOWN_STATUS, 0, 0, 0 };

char	currentConfigFile[12] = {0};
char	currentMsgFile[12] = {0};
int		physicalGroup = 15;
char	VDN[16] = {0};
int		CHANNELS = 0;
int		MAX_LOOP = -1;
int		CALL_DURATION = 60;
int		DIALING_DELAY = 10;
int		BUSY_DELAY    = 9;
int		FREEING_DELAY = 3;
FILE	*stdlog = NULL;		/* Standard trace debug log file */
short	Go = 0, Granted = 1, debug = 0;
int		CNX = 0, prevCNX = 0;
TIMER_T	TIMER = { 0, 0, 0 };

SOCKETDESC	listenSocket = SOCKETNULL, serviceSocket[CLIENTS] = {SOCKETNULL,SOCKETNULL,SOCKETNULL,SOCKETNULL,SOCKETNULL};
SOCKETSTRUCT sockaddrIn = {0};
int			port = 0;	/* Reserved in the NIS map */
char		userName[CLIENTS][56] = {"","","","",""};

TID			acceptTID, serviceTID;

/*------------------------- Prototypes BEGIN ------------------------- */
int	sendInformation(int client, char *format, ...);
/*------------------------- Prototypes  END  ------------------------- */

void	TimeStamp(FILE *ffile)	/* Horodatage de la trace */
{
	time_t	t = (time_t)0;
	struct	tm *tm = NULL;

	if( ffile != NULL ) {
		t = time(NULL);
		tm = localtime(&t);
		fprintf(ffile, "%c%02d/%02d-%02d:%02d:%02d%c,",
				QUOTE,
				tm->tm_mday, tm->tm_mon+1,
				tm->tm_hour, tm->tm_min, tm->tm_sec,
				QUOTE);
	}
}

void	fstdlog(char *format, ...)
{
	va_list ap;
	
	if( (stdlog == NULL) || !debug ) return;

	va_start(ap, format);
	TimeStamp(stdlog);
	vfprintf(stdlog, format, ap);
	va_end(ap);
	fflush(stdlog);
}

void	fstderr(char *format, ...)
{
	va_list ap;
	char	buffer[256];

#ifndef WIN32
	va_start(ap, format);
	TimeStamp(stderr);
	vsprintf(buffer, format, ap);
	strcat(buffer, "\n");
	fprintf(stderr, buffer);
	va_end(ap);
	fflush(stderr);
#else
	va_start(ap, format);
	vsprintf(buffer, format, ap);
	AddMessage(buffer);
	va_end(ap);
#endif
}

int		SocketRead(SOCKETDESC socketdesc, char *data, int len)
{
	int     rval = 0, more = 0;
	
	for( more = len; more != 0; more -= rval) {
#ifndef	WIN32
	    rval = read( socketdesc, data+(len - more), more);
#else
		rval = recv( socketdesc, data+(len - more), more, 0);
#endif
	    if( rval < 1 )      return(rval);
	}

	return(len);
}

int     SocketSend(SOCKETDESC socketdesc, char *data)
{
#ifndef	WIN32
		return( send( socketdesc, data, SOCKETSIZE, 0 ) );
#else
		fd_set	sockSet;
		struct	timeval timeout = {3, 0};	// 3 sec + 0 µsec
		int		ret = 0;

		FD_ZERO(&sockSet);
		FD_SET(socketdesc, &sockSet);

		if( (ret = select( 0, NULL, &sockSet, NULL, &timeout )) > 0 )
			return( send( socketdesc, data, SOCKETSIZE, 0 ) );
		else
			return(ret);
#endif
}


void	strace(int i)
{
	int		client = -1;
	int		len = -1;
	char	buffer[SOCKETSIZE + 1] = {0};
	
	sprintf( buffer, "S00%05d%05d%05d%02d%05d%s",
		i, Call[i].channel, Call[i].sequence, Call[i].status, Call[i].duration, Call[i].din);

	for( client = 0; client < CLIENTS; client++ )		
	    if( serviceSocket[client] != SOCKETNULL ) {
#ifndef WIN32
		  if((len = SocketSend( serviceSocket[client], buffer )) == -1 )
#else
		  if((len = SocketSend( serviceSocket[client], buffer )) == SOCKET_ERROR )
#endif
		    fstderr("strace() : send to client socket %d failed",client);
	}
}

void	ftrace(int i)
{
	fstdlog("%05d,%c%-10s%c,%c%-16s%c,%02d\n",
	Call[i].sequence,QUOTE,Call[i].din,QUOTE,QUOTE,VDN,QUOTE,Call[i].status);
	
	strace(i);
}

int	OpenConfigFile(char *filename)
{
	char	pathname[256] = {0};
	FILE	*file = NULL;
	int	i = 0;
	
	sprintf( pathname, "%s/%s", CONFIGDIRECTORY, filename );
	
	if( (file = fopen( pathname, "r" )) != NULL) {
		Go = 0;
		strcpy( currentConfigFile, filename );
		
		fscanf( file, "VDN=%s\nMESSAGE=%s\nCHANNELS=%d\nCALL_DURATION=%d\nDIALING_DELAY=%d\nBUSY_DELAY=%d\nFREEING_DELAY=%d\nLOOP=%d\n",
			VDN, currentMsgFile, &CHANNELS, &CALL_DURATION,
			&DIALING_DELAY, &BUSY_DELAY, &FREEING_DELAY, &MAX_LOOP );
		
		if( (CHANNELS > 0) && (CHANNELS <= TABLESIZE) ) {
		    for( i = 0; i < CHANNELS; i++ )
			{
			Call[i].sequence = -1;
			Call[i].userID = -1;
			Call[i].status = OFFLINE_STATUS;
			Call[i].count = 0;
			fscanf( file, "%s %05d", Call[i].din, &(Call[i].duration) );
			}
		    }
		else
		    fstderr("Warning: Invalid records number in configuration file <%s>", currentConfigFile);

		fclose(file);
	    	}
	else	{
		fstderr("Error: Configuration file <%s> not found.", pathname);
		return(-1);
		}

	return(0);
}

void	SaveConfigFile(char *filename)
{
	if( filename != NULL ) strcpy(currentConfigFile, filename );
	if( strlen(currentConfigFile) > 0)
	    {
	    char	pathname[256] = {0};
	    FILE	*file = NULL;

	    sprintf( pathname, "%s/%s", CONFIGDIRECTORY, currentConfigFile );
	    if((file = fopen( pathname, "w" )) != NULL)
		{
		int	client;
		int	i = -1;

		fprintf( file, "VDN=%s\nMESSAGE=%s\nCHANNELS=%d\nCALL_DURATION=%d\nDIALING_DELAY=%d\nBUSY_DELAY=%d\nFREEING_DELAY=%d\nLOOP=%d\n",
	    		VDN, currentMsgFile, CHANNELS, CALL_DURATION,
			DIALING_DELAY, BUSY_DELAY, FREEING_DELAY, MAX_LOOP );

		for( i = 0; i < CHANNELS; i++ )
		    fprintf( file, "%s %05d\n", Call[i].din, Call[i].duration );

		fclose(file);

		for( client = 0; client < CLIENTS; client++ )
		    if( serviceSocket[client] != SOCKETNULL )
			sendInformation(client, FMTINFO_SAVE, currentConfigFile);
		}
	    else fstderr("Error: Can not open Configuration <%s> for writing.",currentConfigFile);
	    }
	else fstderr("Error: Can not save Configuration <%s>. Empty file !",currentConfigFile);
}

int	DeleteConfigFile(char *filename)
{
	if( strcmp(currentConfigFile, filename) )
	    {
	    int		client;
	    char	pathname[256] = {0};

	    sprintf( pathname, "%s/%s", CONFIGDIRECTORY, filename );
	    unlink( pathname );

	    for( client = 0; client < CLIENTS; client++ )
	    	if( serviceSocket[client] != SOCKETNULL )
	    	    sendInformation(client, FMTINFO_DEL, filename);

	    return(0);
	    }
	  return(-1);
}


void	Command(int socClient, char *cmd )
{
	     if( !strncmp( cmd, "user", 4 ))
	     	{
	     	strncpy( userName[socClient], &cmd[4], 56 );
	     	}
	else if( !strncmp( cmd, "who", 3 ))
		{
		char	buffer[SOCKETSIZE + 1] = {0};
		int	len = -1;
		int	client;

		for( client = 0; client < CLIENTS; client++ )
	    	    if( serviceSocket[client] != SOCKETNULL )	{
			sprintf( buffer, "W00%s(%d)%c", userName[client], client, '\0' );
			len = SocketSend( serviceSocket[socClient], buffer );
			}
		}
	else if( !strncmp( cmd, "start", 5 ))
		{
		int	i, client;
		
		for(i = 0; i < CHANNELS; i++ )
			Call[i].loop = MAX_LOOP;
		TIMER.HHH = 0; TIMER.MM = 0; TIMER.SS = 0;
		Go = 1;
		for( client = 0; client < CLIENTS; client++ )		
	    		if( serviceSocket[client] != SOCKETNULL )
	    			sendEngineStatus(client);
		}
	else if( !strncmp( cmd, "stop", 4 ))
		{
		int	client;
		
		Go = 0;
		for( client = 0; client < CLIENTS; client++ )		
	    		if( serviceSocket[client] != SOCKETNULL )
	    			sendEngineStatus(client);
		}
	else if( !strncmp( cmd, "debug", 5 ))
		{
		int	client;
		
		debug = !debug;
		for( client = 0; client < CLIENTS; client++ )		
	    		if( serviceSocket[client] != SOCKETNULL )
	    			sendDebugStatus(client);
		}
	else if( !strncmp( cmd, "dump", 4 ))
		{
		int	i = -1;
	
		for( i = 0; i < CHANNELS; i++ )
			strace(i);
		}
	else if( !strncmp( cmd, "msisdn", 6))
		{
		char	data[12];
		int	i, s;
		 
		memmove(data, &cmd[6], 5); data[5] = '\0';
		if( ((i = atoi(data)) >= 0) && (i < CHANNELS) ) {
			memmove(data, &cmd[11], 5); data[5] = '\0';
			Call[i].duration = atoi(data);
			memmove(data, &cmd[16], 2); data[2] = '\0';
			s = atoi(data);
			memmove(Call[i].din, &cmd[18], s);
			Call[i].din[s] = '\0';
			strace(i);
		}
		else
			fstderr( "Command(): Invalid record number received [%d]", i );
		}
	else if( !strncmp( cmd, "config", 6 ))
		{
		char	data[5 + 1];
		int	client = 0;
		int	value = 0;
		int	xCHANNELS = 0;
		
		Go = 0;
		
		memmove(data, &cmd[6], 5);
		data[5] = '\0';
		xCHANNELS = atoi(data);

		memmove(data, &cmd[11], 5);
		data[5] = '\0';
		if( (value = atoi(data)) > 0)
			CALL_DURATION = value;

		memmove(data, &cmd[16], 5);
		data[5] = '\0';
		if( (value = atoi(data)) > 0)
			DIALING_DELAY = value;
		
		memmove(data, &cmd[21], 5);
		data[5] = '\0';
		if( (value = atoi(data)) > 0)
			BUSY_DELAY = value;

		memmove(data, &cmd[26], 5);
		data[5] = '\0';
		if( (value = atoi(data)) > 0)
			FREEING_DELAY = value;

		memmove(data, &cmd[31], 5);
		data[5] = '\0';
		if( ((value = atoi(data)) > 0) || (value == -1) )
			MAX_LOOP = value;

		strncpy(VDN, &cmd[36], 16);
		
		strncpy(currentMsgFile, &cmd[37+strlen(VDN)], 12);

		if( (xCHANNELS > 0) && (xCHANNELS <= TABLESIZE) ) {
			if( xCHANNELS > CHANNELS ) {
				int	s = 0;
				int	i = 0;
				char	f[32] ={0};

				s = strlen(Call[CHANNELS-1].din);
				sprintf(f, "%%0%dd", s);

				for(i = CHANNELS; i < xCHANNELS; i++ ) {
					Call[i].channel	  = -1;
					Call[i].sequence  = -1;
					Call[i].userID	  = -1;
					sprintf(Call[i].din, f, atoi(Call[i-1].din) + 1);
					Call[i].status	  = UNKNOWN_STATUS;
					Call[i].count	  = 0;
					Call[i].duration  = CALL_DURATION;
				}
			}
			CHANNELS = xCHANNELS;
		}

		for( client = 0; client < CLIENTS; client++ )		
	    		if( serviceSocket[client] != SOCKETNULL ) {
	    			sendConfig(client);
	    			sendEngineStatus(client);
	    		}
		}
	else if( !strncmp( cmd, "files", 5 ))
		{
		DIR		*pDIR = NULL;
		struct dirent	*dp = NULL;
		char		buffer[SOCKETSIZE + 1] = {0};
		int		len = -1;
		
		if( cmd[5] == 'C' )
			pDIR = opendir(CONFIGDIRECTORY);
		else	{
			pDIR = opendir(MSGDIRECTORY);
			sprintf( buffer, "F00%s%c", EMPTYMESSAGE, '\0' );
			len = SocketSend( serviceSocket[socClient], buffer );
			}

       		for(dp = readdir(pDIR); dp != NULL; dp = readdir(pDIR))
       		    if(dp->d_name[0] != '.')
       			{
			sprintf( buffer, "F00%s%c", dp->d_name, '\0' );
			len = SocketSend( serviceSocket[socClient], buffer );
			}
		closedir(pDIR);
		}
	else if( !strncmp( cmd, "new", 3 ))
		{
		int	client = 0;
		if( !OpenConfigFile(DEFAULTCONFIG) )
		    for( client = 0; client < CLIENTS; client++ )		
	    		if( serviceSocket[client] != SOCKETNULL ) {
	    			sendConfig(client);
	    			sendEngineStatus(client);
	    		}
		}
	else if( !strncmp( cmd, "open", 4 ))
		{
		int	client = 0;
		
		if( !OpenConfigFile( &(cmd[4]) ) )
		    for( client = 0; client < CLIENTS; client++ )		
	    		if( serviceSocket[client] != SOCKETNULL ) {
	    			sendConfig(client);
	    			sendEngineStatus(client);
	    		}
		}
	else if( !strncmp( cmd, "save", 4 ))
		{
	    	SaveConfigFile(NULL);
		}
	else if( !strncmp( cmd, "copied", 6 ))
		{
		SaveConfigFile( &(cmd[6]) );
		}
	else if( !strncmp( cmd, "delete", 6 ))
		{
		if( DeleteConfigFile( &(cmd[6]) ) == -1 )
		    sendInformation( socClient, FMTINFO_ERR, ERRCODE_INUSE);
		}
	else
		{
		fstderr("Command() : Unknown input [%s]\n", cmd );
		}
}


void	CallClock()
{
	if( Granted )
	  {
	  int	I = -1, testID = -1, Loops = CHANNELS;

	  Granted = 0;
	  CNX = 0;
	  for( I = 0; I < CHANNELS; I++ )
	    {
	    switch( Call[I].status )
		{
		case ONLINE_STATUS:
		    if( !Go )
			Call[I].count = Call[I].duration;
		    else {
			Call[I].count++ ;
			CNX++ ;
			}
		break;

		case UNKNOWN_STATUS:
		case OFFLINE_STATUS:
		  if( Go && (Call[I].userID == -1) ) {
		    if( (MAX_LOOP == -1) || ((MAX_LOOP>0) && (Call[I].loop>0))) {
		      if(-1 != (testID = cgCreateSession()) )
				{
				char	record[8] = {0}, physgrp[8] = {0}, dialdelay[8] = {0};
				int	ret = 0;
			
				sprintf( record, "%d", I );
				sprintf( physgrp, "%d", physicalGroup );
				sprintf( dialdelay, "%d", DIALING_DELAY );

				cgSetSessionKey(testID, "$RECORD", record);
				cgSetSessionKey(testID, "$VDN", VDN);
				cgSetSessionKey(testID, "$MSISDN", Call[I].din);
				cgSetSessionKey(testID, "$PHYSGRP", physgrp);
				cgSetSessionKey(testID, "$DIALDELAY", dialdelay);
				cgSetSessionKey(testID, "$MESSAGE",currentMsgFile);

				Call[I].userID   = testID;
				Call[I].status   = DIALING_STATUS;
				ftrace(I);
				ret = cgMakeCall(Call[I].userID);
				}
		      else
				{
				int	client;

				fstderr("Warning: Can not Create Call context with phone #%d", I );
				Call[I].status = BUSY_STATUS;
				Call[I].count = 0;
				ftrace(I);

				for( client = 0; client < CLIENTS; client++ )
					if( serviceSocket[client] != SOCKETNULL )
						sendInformation(client, FMTINFO_ERR, ERRCODE_CTX, I);
				}
		    }
		  else if( MAX_LOOP > 0 ) Loops-- ;
		  }
		break;

		case DIALING_STATUS:
		    if( Go )
		    	{
		    	Call[I].count++ ;
		    	if( Call[I].count > DIALING_DELAY )
			    {
			    Call[I].status   = UNKNOWN_STATUS;
			    ftrace(I);
			    
			    /* if(Call[I].userID  != -1) {fstderr( "AT: ID<%d> request disc\n", Call[I].userID); cgDiscCall(Call[I].userID); }else fstderr("WARNING: No ID set for disc\n"); */
			    
			    Call[I].channel  = -1;
			    Call[I].sequence = -1;
			    Call[I].userID   = -1;
			    Call[I].count    = 0;
			    }
			}
		    else
			{
			Call[I].status   = UNKNOWN_STATUS;
			ftrace(I);

			/* if(Call[I].userID  != -1) cgDiscCall(Call[I].userID); else fstderr("WARNING: No ID set for disc\n"); */

			Call[I].channel  = -1;
			Call[I].sequence = -1;
			Call[I].userID   = -1;
			Call[I].count    = 0;
			}
		break;

		case BUSY_STATUS:
		    if( Go )
		    	{
		    	Call[I].count++ ;
		    	if( Call[I].count > BUSY_DELAY )
		    	    {
		    	    Call[I].status = UNKNOWN_STATUS;
			    ftrace(I);
			    Call[I].channel  = -1;
			    Call[I].sequence = -1;
			    Call[I].userID   = -1;
		    	    Call[I].count    = 0;
		    	    }
		    	}
		    else
			{
			Call[I].status   = UNKNOWN_STATUS;
			ftrace(I);
			Call[I].channel  = -1;
			Call[I].sequence = -1;
			Call[I].userID   = -1;
			Call[I].count    = 0;
			}
		break;

		case FREEING_STATUS:
		    if( Go )
		    	{
		    	Call[I].count++ ;
		    	if( Call[I].count > FREEING_DELAY )
		    	    {
		    	    Call[I].status = OFFLINE_STATUS;
			    ftrace(I);
			    Call[I].channel  = -1;
			    Call[I].sequence = -1;
			    Call[I].userID   = -1;
		    	    Call[I].count    = 0;
		    	    }
		    	}
		    else
			{
			Call[I].status   = OFFLINE_STATUS;
			ftrace(I);
			Call[I].channel  = -1;
			Call[I].sequence = -1;
			Call[I].userID   = -1;
			Call[I].count    = 0;
			}
		break;
		}
	  } /* End For */

	  if( Go ) {
	  	if( (MAX_LOOP != -1) && !Loops ) {
	  		int client;
	  		Go = 0;
			for( client = 0; client < CLIENTS; client++ )		
	    			if( serviceSocket[client] != SOCKETNULL )
	    				sendEngineStatus(client);
	    	}

	  	if( !(TIMER.SS % 5) || !Go) {
	  		int client;
			for( client = 0; client < CLIENTS; client++ )		
	    			if( serviceSocket[client] != SOCKETNULL )
	    				sendTiming(client, &TIMER);
	  	}

		TIMER.SS++ ;
		if( TIMER.SS > 59 ) { TIMER.MM++; TIMER.SS = 0; }
		if( TIMER.MM > 59 ) { TIMER.HHH++; TIMER.MM = 0; }
	  }
	  if( (CNX != prevCNX) ) {
	  	int client;
		for( client = 0; client < CLIENTS; client++ )		
	    		if( serviceSocket[client] != SOCKETNULL )
	    			sendConnections(client, CNX);
	  }
	  prevCNX = CNX;

	  Granted = 1;
	  }
	else
	  fstderr( "CallClock() : Over Loop\n" );
}

void	UserDISC(int dcnx)
{
	char	*pRecord = cgGetSessionKey(-1,"$RECORD");
	
	if( pRecord != NULL )
		{
		int	I = atoi(pRecord);
		
		if( (I >= 0) && (I < CHANNELS ) && (Call[I].status != BUSY_STATUS) )
			{
			if(Call[I].status == ONLINE_STATUS)
				Call[I].loop-- ;

			Call[I].status   = FREEING_STATUS;
			Call[I].count    = 0;
			ftrace(I);
			}
		}
}

char	*DialStatus(int status)
{
	char	event[16] = {0};
	char	*pRecord = cgGetSessionKey(-1,"$RECORD");
	int		I = atoi(pRecord);

	if( (I >= 0) && (I < CHANNELS ) ) {
		Call[I].channel  = atoi(cgGetSessionKey(-1,"$chan"));
		Call[I].sequence = atoi(cgGetSessionKey(-1,"$uref"));
		Call[I].status	 = status;
		Call[I].count	 = 0;
		ftrace(I);
	}
	
	sprintf(event, "%s+%s", cgGetSessionKey(-1,"$strin"), cgGetSessionKey(-1,"$key") );
	return(event);
}

char	*NextEvent()
{
	return( DialStatus(ONLINE_STATUS) );
}

char	*BusyEvent()
{
	return( DialStatus(BUSY_STATUS) );
}

char	*AllEvent()
{
	return( DialStatus(UNKNOWN_STATUS) );
}

int	sendConfig(int client)
{
	char	buffer[SOCKETSIZE + 1] = {0};
	int	len = 0;
	
	sprintf( buffer, "O%02d%05d%05d%05d%05d%05d%05d%s%c%s%c%s%c", 30 + strlen(VDN) + strlen(currentConfigFile) + 2,
		CHANNELS, CALL_DURATION, DIALING_DELAY, BUSY_DELAY, FREEING_DELAY,
		MAX_LOOP, VDN,'\0',currentConfigFile,'\0',currentMsgFile,'\0');
	len = SocketSend( serviceSocket[client], buffer );
	
	return(len);
}

int	sendEngineStatus(int client)
{
	char	buffer[SOCKETSIZE + 1] = {0};
	int	len = 0;
	
	sprintf( buffer, "G%02d%1d", 1, Go);
	len = SocketSend( serviceSocket[client], buffer );
	
	return(len);
}

int	sendConnections(int client, int counters)
{
	char	buffer[SOCKETSIZE + 1] = {0};
	int	len = 0;
	
	sprintf( buffer, "C%02d%03d", 3, counters);
	len = SocketSend( serviceSocket[client], buffer );
	
	return(len);
}

int	sendTiming(int client, TIMER_T *pTIMER)
{
	char	buffer[SOCKETSIZE + 1] = {0};
	int	len = 0;
	
	sprintf( buffer, "T%02d%03d:%02d:%02d", 9, pTIMER->HHH, pTIMER->MM, pTIMER->SS);
	len = SocketSend( serviceSocket[client], buffer );
	
	return(len);
}

int	sendDebugStatus(int client)
{
	char	buffer[SOCKETSIZE + 1] = {0};
	int	len = 0;
	
	sprintf( buffer, "D%02d%1d", 1, debug);
	len = SocketSend( serviceSocket[client], buffer );
	
	return(len);
}

int	sendInformation(int client, char *format, ...)
{
	char	buffer[SOCKETSIZE + 1] = {0};
	char	info[SOCKETSIZE - 8] = {0};
	int		len = 0;
	
	va_list ap;
	va_start(ap, format);
	vsprintf(info, format, ap);
	va_end(ap);

	sprintf( buffer, "I%02d%s%c", strlen(info)+1, info, '\0');
	len = SocketSend( serviceSocket[client], buffer );
	
	return(len);
}

void	serviceThread(long client)
{
	char	buffer[SOCKETSIZE + 1] = {0}, data[SOCKETSIZE + 1] = {0};
	char	code = '\0', _size[8] = {0};
	int		size = 0;
	int		len = 0;
	short	bye = 0;
	
	fstderr("serviceThread(%d) : Welcome Client Socket <%d>", client,serviceSocket[client]);
	
	sendConfig((int) client);
	sendEngineStatus((int) client);
	sendConnections((int) client, CNX);
	sendDebugStatus((int) client);
	sendTiming(client, &TIMER);

	while( !bye )
		{
#ifndef WIN32
		if( (len = SocketRead(serviceSocket[client], buffer, SOCKETSIZE)) == -1) {
#else
		if( ((len = SocketRead(serviceSocket[client], buffer, SOCKETSIZE)) == SOCKET_ERROR) || (len == 0) ) {
#endif
			fstderr("serviceThread(%d) : Can not read socket <%d>. Thread exiting.",client,serviceSocket[client]);
			shutdown( serviceSocket[client], 2 );
			SocketClose( serviceSocket[client] );
	  		bye = 1;
		}
		else
			if(len > 0 ) {
				code = buffer[0];
				memmove(_size, &buffer[1], 2 );
				size = atoi(_size);

				switch( code ) {
					case 'C' :
						memmove(data, &buffer[3], size);
						Command((int) client, data);
					break;
					case 'X' :
						shutdown( serviceSocket[client], 2 );
						SocketClose( serviceSocket[client] );
						bye = 1;
					break;
				}
			}
			else
#ifndef WIN32
				bye = 1;
#else
				;
#endif
		}
	fstderr("serviceThread(%d) : Bye Bye Socket <%d>.", client, serviceSocket[client]);
	serviceSocket[client] = SOCKETNULL;
	userName[client][0] = '\0';
}

#ifndef WIN32
void	acceptSocket(long  dummy)
#else
void    acceptSocket(void *dummy)
#endif
{
	int	client = -1;
	short	bye = 0;
	
	while( !bye )
	  {
	  for( client = 0; client < CLIENTS; client++ )
	  	if( serviceSocket[client] == SOCKETNULL )
	  		break;
	  
	  if( client < CLIENTS ) {
#ifndef WIN32
	    if( -1 == (serviceSocket[client] = accept(listenSocket, NULL, NULL))) {
#else
	    if( INVALID_SOCKET == (serviceSocket[client] = accept(listenSocket, NULL, NULL))) {
#endif
	      fstderr("acceptSocket() : Can not accept any socket connection.");
	      /*
	      SocketClose( listenSocket );
	      bye = 1;
	      */
#ifndef WIN32
	      sleep(1);
#else
		  Sleep(1000);
#endif
	      }
	    else {
#ifndef WIN32
	    	 if( pthread_create( &serviceTID,
				     pthread_attr_default,
				     (pthread_startroutine_t) serviceThread,
				     (pthread_addr_t) client) != -1)
		     pthread_detach( &serviceTID );
#else
		  if( (serviceTID = _beginthread( serviceThread, 0, (void *) client)) == -1)
			fstderr( "Can not thread serviceThread(%d) function.", client );
#endif
	    	 }
	    }
	  else
#ifndef WIN32
		sleep(1);
#else
		Sleep(1000);
#endif
	  }
	  
	listenSocket = SOCKETNULL;
	fstderr("acceptSocket() : Bye Bye Socket.");
}

int	ReadCallGenConfig(char *filename)
{
	FILE	*file = NULL;
	char	serviceName[256] = {0};
	int		serviceRead = 0;
	struct	servent *servdata = NULL;
	int		physGrpRead = 0;
	int		ret = 0;
	
	if( (file = fopen( filename, "r" )) != NULL) {
	    if(fscanf(file, "SERVICENO=%s\nPHYSICALGROUP=%d\n", serviceName, &physGrpRead) == 2)
		{
		  if( (servdata = getservbyname( serviceName, "tcp" ) ) != NULL )
			port = ntohs(servdata->s_port);
		else
		  if( ((serviceRead = atoi(serviceName)) > 0) )
		    	port = serviceRead;
		else {
		    fstderr("Error: <%s> - Unknown service SERVICENO=%s", filename, serviceName);
		    ret = -2;
		    }

		if( (physGrpRead >= 0) && (physGrpRead <= 99) )
		    physicalGroup = physGrpRead;
		else {
		    fstderr("Error: <%s> - Invalid value PHYSICALGROUP=%d", filename, physGrpRead);
		    ret = -3;
		    }
		}
	    else
	    	{
	    	fstderr("Error: <%s> - Missing key words <SERVICENO=str|val><nl><PHYSICALGROUP=val>", filename);
	    	ret = -1;
	    	}
	    fclose(file);
	    }
	else
	    {
	    fstderr("Warning: CallGen Configuration file <%s> not found.", filename);
	    ret = -1;
	    }

	return(ret);
}

void	AppSTOP( int sigNum )
{
	int	client = -1;

	for( client = 0; client < CLIENTS; client++ )	
	    if( serviceSocket[client] != SOCKETNULL ) {
		    shutdown( serviceSocket[client], 2);
		    SocketClose(serviceSocket[client]);
	    }
	    
	if( listenSocket != SOCKETNULL )
		SocketClose( listenSocket );
		
	fclose(stdlog);
	exit(0);
}

#ifndef WIN32
void    AppSTART()
#else
int		AppSTART()
#endif
{		
	int	bufferSize = SOCKETSIZE + 1;
	int	i = -1, ret = 0;

	if( (ret = ReadCallGenConfig( CALLGENCFGFILE )) != 0) {
		fstderr( "System Exit." );
		exit(ret);
	}
	fstderr( "CallGen TCP service port number <%d>", port );

	for( i = 0; i < TABLESIZE; i++ ) {
		Call[i].channel   = -1;
		Call[i].sequence  = -1;
		Call[i].userID	  = -1;
		strcpy(Call[i].din, "");
		Call[i].status	  = UNKNOWN_STATUS;
		Call[i].count	  = 0;
		Call[i].duration  = 0;
		Call[i].loop	  = 0;
	}

	cgClockFunc( CallClock );

#ifndef WIN32
	signal( SIGTERM, AppSTOP );
#endif

#ifndef WIN32
	if( (listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fstderr( "Can not create socket." );
		fstderr( "System Exit." );
		exit(1);
#else
	if( (listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		fstderr( "Can not create socket." );
		return(1);
#endif
	}

	sockaddrIn.sin_family = AF_INET;
	sockaddrIn.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddrIn.sin_port = htons(port);

#ifndef WIN32
	if( bind(listenSocket, (struct sockaddr *) &sockaddrIn, sizeof(struct sockaddr_in)) == -1) {
		SocketClose(listenSocket);
		fstderr( "Can not bind socket." );
		fstderr( "System Exit." );
		exit(4);
#else
	if( bind(listenSocket, (PSOCKADDR) &sockaddrIn, sizeof(sockaddrIn)) == SOCKET_ERROR) {
		SocketClose(listenSocket);
		fstderr( "Can not bind socket." );
		return(4);
#endif
	}

#ifndef WIN32
	if( listen(listenSocket, CLIENTS) == -1 ) {
		SocketClose(listenSocket);
		fstderr( "Can not listen socket." );
		fstderr( "System Exit." );
		exit(5);
#else
	if( listen(listenSocket, CLIENTS) == SOCKET_ERROR ) {
		SocketClose(listenSocket);
		fstderr( "Can not listen socket." );
		return(5);
#endif
	}

#ifndef WIN32
	if( pthread_create( &acceptTID,
			    pthread_attr_default,
			    (pthread_startroutine_t) acceptSocket,
			    (pthread_addr_t) 0) == -1) {
		SocketClose(listenSocket);
		fstderr( "Can not thread acceptSocket() function." );
		fstderr( "System Exit." );
		exit(6);
	}
	else
		pthread_detach(&acceptTID);
#else
	if( (acceptTID = _beginthread( acceptSocket, 0,	NULL)) == -1) {
		SocketClose(listenSocket);
		fstderr( "Can not thread acceptSocket() function." );
		return(6);
	}
#endif

	OpenConfigFile(DEFAULTCONFIG);
	stdlog = fopen( STDLOGNAME, "a+" );
	fstderr( "System Ready." );
}

/*
char	*ProcessInput()
{
	//fstderr( "ProcessInput() : $INPUT <%s>\n", WvGet("$INPUT") );
	
	return("OK+EVT0");
}
*/
char	*IsTimeout()
{
	char	*pRecord = cgGetSessionKey(-1,"$RECORD" );
	int		I  = atoi( pRecord );

	if( (Call[I].count >= Call[I].duration) || !Go )
		return( "TIMEOUT+EVT0" );
	else {
		if( !strcmp(currentMsgFile, EMPTYMESSAGE) )
			return( "JUMP+EVT0" );
		else
			return( "PLAY+EVT0" );
	}
}
