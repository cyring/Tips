#define	_MAJOR	"0"
#define	_MINOR	"01"
#define AutoDate "@(#) Call Manager - "_MAJOR"."_MINOR" (C) Bouygues Telecom "__DATE__
static  char    version[] = AutoDate;
/*
*       (C) 1997,98,99 Bouygues Telecom
*
*       Projet          [ Call Manager - Gestionnaire de communication ]
*       Direction       [ DCTSI/IPS/IPA                                ]
*       Auteurs         [ Courtiat Cyril                               ]
*
*       Fichier         [ CallMgr.c                                	   ]
*
*       Description :   Interface simplifiée de l'API NMS
*
*/

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

/*----------------------------- CT Access Includes -------------------------*/
#include "ctadef.h"
#include "adidef.h"
#include "vcedef.h"
#include "swidef.h"

#include "NmsApi.h"
#include "Manager.h"
#include "CallMgr.h"

TRUNK	*Trunk = NULL;
int		Ports = 0;
char	Protocol[16]    = "wnk0";

void	CreateTruncMapping(char *inifile)
{
		char	port[16];
		char	key[5];
		int		i = 0;

		GetPrivateProfileString( "NMS", "PROTOCOL", "wnk0", Protocol, 16, inifile );

		if( ((Ports = GetPrivateProfileInt( "TRUNK", "PORTS", 0, inifile )) > 0 )
			&& (Ports <= MAX_PORTS) )
			{
			Trunk = (TRUNK *) malloc( sizeof(TRUNK) * Ports );

			for( i = 0; i < Ports; i++ ) {
				sprintf( key, "%d", i);
				if( GetPrivateProfileString( "TRUNK", key, "0:0:0", port, 16, inifile) > 0 ) {
					sscanf(port, "%d:%d:%d",	&Trunk[i].ag_board,
												&Trunk[i].mvip_stream,
												&Trunk[i].mvip_slot );
					Trunk[i].ctxHandle = NULL;
				}
			}
		}
}

int		cmInitializeCallManager(void)
{
		int		ret = 0;

		/* Initialize CT Access */
		CTA_SERVICE_NAME servicelist[] =     /* for ctaInitialize */
		{   { "ADI", "ADIMGR" },
			{ "SWI", "SWIMGR" },
			{ "VCE", "VCEMGR" }
		};
		CTA_SERVICE_DESC services[] =        /* for ctaOpenServices */
		{   { {"ADI", "ADIMGR"}, { 0 }, { 0 }, { 0 } },
			{ {"SWI", "SWIMGR"}, { 0 }, { 0 }, { 0 } },
			{ {"VCE", "VCEMGR"}, { 0 }, { 0 }, { 0 } }
		};

		CreateTruncMapping("CALLMGR.INI");
		nmsInitialize( servicelist, sizeof(servicelist)/sizeof(servicelist[0]) );

		return(ret);
}

void	cmTerminateCallManager(void)
{
		free(Trunk);
}

int		cmLookupPortHandle(void)
{
		int		i = 0;

		for( i = 0; i < Ports; i++ )
			if( Trunk[i].ctxHandle == NULL ) {
				NMSCONTEXT *nmsContext = (NMSCONTEXT *) malloc( sizeof(NMSCONTEXT) );

				nmsContext->ctaqueuehd = NULL_CTAQUEUEHD;	/* CT Access queue handle   */
				nmsContext->ctahd = NULL_CTAHD;				/* CT Access context handle */
				nmsContext->maxredial = 1;					/* maxredial                */
				nmsContext->lowlevelflag = 0;				/* lowlevelflag             */

				Trunk[i].ctxHandle = nmsContext;

				break;
			}
		return( (i == Ports) ? -1 : i );
}

int		cmMakeCall(char *dialstring, int *portHandle)
{
		char	datastring[16] = {0};
		int		port = -1;
		int		ret = FAILURE;

		if( (port = cmLookupPortHandle()) != -1 )
			{
			CTA_SERVICE_NAME servicelist[] =     /* for ctaInitialize */
				{   { "ADI", "ADIMGR" },
					{ "SWI", "SWIMGR" },
					{ "VCE", "VCEMGR" }
				};
			CTA_SERVICE_DESC services[] =        /* for ctaOpenServices */
				{   { {"ADI", "ADIMGR"}, { 0 }, { 0 }, { 0 } },
					{ {"SWI", "SWIMGR"}, { 0 }, { 0 }, { 0 } },
					{ {"VCE", "VCEMGR"}, { 0 }, { 0 }, { 0 } }
				};

			/* Fill in ADI service (index 0) MVIP address information */
			services[0].mvipaddr.board    = Trunk[port].ag_board;
			services[0].mvipaddr.stream   = Trunk[port].mvip_stream;
			services[0].mvipaddr.timeslot = Trunk[port].mvip_slot;
			services[0].mvipaddr.mode     = ADI_FULL_DUPLEX;

			sprintf( datastring, "CALLMGR%d", GetCurrentThreadId() );
			nmsOpenPort(0, datastring, services,
						sizeof(services)/sizeof(services[0]),
						&Trunk[port].ctxHandle->ctaqueuehd, &Trunk[port].ctxHandle->ctahd );

			nmsStartProtocol( Trunk[port].ctxHandle->ctahd, Protocol, NULL, NULL );

			ret = nmsPlaceCall( Trunk[port].ctxHandle->ctahd, dialstring, NULL );

			*portHandle = port;
			}
		else
			*portHandle = -1;

		return( ret );
}

int		cmReleaseCall(int portHandle)
{
		int	ret = FAILURE;

		if( Trunk[portHandle].ctxHandle != NULL ) {
			nmsHangUp( Trunk[portHandle].ctxHandle->ctahd );

			nmsStopProtocol( Trunk[portHandle].ctxHandle->ctahd );
			nmsShutdown( Trunk[portHandle].ctxHandle->ctahd );

			free(Trunk[portHandle].ctxHandle);
			Trunk[portHandle].ctxHandle = NULL;

			ret = SUCCESS;
		}
		return(ret);
}

/*--------------------------------------------------------------------------*/

void	nmsWaitForEvent( CTAHD ctahd, CTA_EVENT *eventp )
{
		CTAQUEUEHD	ctaqueuehd;
		DWORD		ctaret ;

		ctaGetQueueHandle(ctahd, &ctaqueuehd);

		if( (ctaret = ctaWaitEvent( ctaqueuehd, eventp, CTA_WAIT_FOREVER)) != SUCCESS)
			printf( "\007ctaWaitEvent returned %x\n", ctaret);
}

void	nmsWaitForSpecificEvent(CTAHD ctahd,
								DWORD desired_event, CTA_EVENT *eventp )
{
		for(;;)
		{
        nmsWaitForEvent(ctahd, eventp);

        if( ctahd == eventp->ctahd && eventp->id == desired_event )
            break;
		}
}

void	nmsInitialize(CTA_SERVICE_NAME servicelist[], unsigned numservices)
{
		CTA_INIT_PARMS		initparms = { 0 };
		CTA_ERROR_HANDLER	hdlr;
		DWORD				ret;

		/* Initialize size of init parms structure */
		initparms.size = sizeof(CTA_INIT_PARMS);
		initparms.ctacompatlevel = CTA_COMPATLEVEL;

		/* If daemon running then initialize tracing
		 * and system global default parameters.
		 */
		initparms.traceflags = CTA_TRACE_ENABLE;
		initparms.parmflags  = CTA_PARM_MGMT_SHARED;

		/* Set error handler to NULL and remember old handler */
		ctaSetErrorHandler( NULL, &hdlr );

		if( (ret = ctaInitialize(NULL, 0, &initparms) ) != SUCCESS)
			{
			initparms.traceflags = 0; /* clear trace flags */
			initparms.parmflags  = 0; /* use process global default parms */

			ctaSetErrorHandler( hdlr, NULL ); /* restore error handler */

			ctaInitialize( servicelist, numservices, &initparms );
			}
		else
			{
			ctaSetErrorHandler( hdlr, NULL );  /* restore error handler */
			}
}

void	nmsOpenPort(
		unsigned    userid,                  /* for ctaCreateContext */
		char       *contextname,             /* for ctaCreateContext */
		CTA_SERVICE_DESC services[],         /* for ctaOpenServices */
		unsigned    numservices,             /* number of "services" */
		CTAQUEUEHD *ctaqueuehd,              /* returned CT Access queue handle */
		CTAHD      *ctahd)                   /* returned CT Access context hd   */
{
		/* Open the CT Access application queue, attaching all defined
		 * service managers.
		 */
		ctaCreateQueue( NULL, 0, ctaqueuehd );

		/* Create a context and opens services on new queue */
		nmsOpenAnotherPort( userid,
							contextname,
							services,
							numservices,
							*ctaqueuehd,
							ctahd );
}

void	nmsOpenAnotherPort(
		unsigned          userid,        /* for ctaCreateContext                  */
		char             *contextname,   /* for ctaCreateContext                  */
		CTA_SERVICE_DESC  services[],    /* for ctaOpenServices                   */
		unsigned          numservices,   /* number of "services"                  */
		CTAQUEUEHD        ctaqueuehd,    /* CT Access queue handle                */
		CTAHD            *ctahd )        /* returned CT Access context hd         */
{
		CTA_EVENT	event = { 0 };

		/* Create a CT Access context */
		if( SUCCESS != ctaCreateContext( ctaqueuehd, userid, contextname, ctahd ) )
			{
			/* This routine should NOT receive an invalid queue handle */
			exit( 1 );
			}

		/* Open services */
		ctaOpenServices( *ctahd, services, numservices );

		/* Wait for services to be opened asynchronously */
		nmsWaitForSpecificEvent( *ctahd,
								  CTAEVN_OPEN_SERVICES_DONE, &event );

		if( event.value != CTA_REASON_FINISHED )
			{
			//nmsShowError( "Open services failed", event.value );
			exit( 1 );
			}
}

void	nmsShutdown( CTAHD ctahd )
{
		CTA_EVENT event;
		CTAQUEUEHD ctaqueuehd;

		ctaGetQueueHandle( ctahd, &ctaqueuehd );

		ctaDestroyContext( ctahd );    /* this will close services automatically */

		nmsWaitForSpecificEvent(ctahd,
								CTAEVN_DESTROY_CONTEXT_DONE,
								&event );

		if( event.value != CTA_REASON_FINISHED )
			{
			//nmsShowError( "Destroying the CT Access context failed", event.value );
			exit( 1 );
			}

		ctaDestroyQueue( ctaqueuehd );
}

void	nmsStartProtocol(
		CTAHD            ctahd,
		char            *protocol,       /* TCP to run (must be avail on board)   */
		WORD            *protparmsp,     /* optional protocol-specific parms      */
		ADI_START_PARMS *stparmsp )      /* optional parms for adiStartProtocol   */
{
		CTA_EVENT event;
		DWORD     ret ;

		ret = adiStartProtocol( ctahd, protocol, protparmsp, stparmsp );

    /* If the application is linked with ADI.LIB (libadi.so) instead of
     *  ADIMGR.LIB (libadimgr.so), then ADIERR_INVALID_QUEUEID will be returned;
     *  the error handler, which was registered with CT-ACCESS, will not be
     *  called.
     */
		if (ret == ADIERR_INVALID_QUEUEID)
		{
			exit( 1 );
		}

		nmsWaitForSpecificEvent( ctahd, ADIEVN_STARTPROTOCOL_DONE, &event );
		if( event.value != CTA_REASON_FINISHED )
			{
			//nmsShowError( "Start protocol failed", event.value );
			exit( 1 );
			}
}

void	nmsStopProtocol( CTAHD ctahd )
{
		CTA_EVENT event;

		adiStopProtocol( ctahd );

		nmsWaitForSpecificEvent( ctahd, ADIEVN_STOPPROTOCOL_DONE, &event );
		if( event.value != CTA_REASON_FINISHED )
			{
			//nmsShowError( "Stop protocol failed", event.value );
			exit( 1 );
			}
}

void	nmsRejectCall( CTAHD ctahd, unsigned method )
{
		CTA_EVENT event;

		/* This function doesn't support custom audio. */
		//assert( method != ADI_REJ_USER_AUDIO );

		adiRejectCall( ctahd, method );

		for (;;)
		{
		nmsWaitForEvent(ctahd, &event);

		switch( event.id )
			{
			case ADIEVN_REJECTING_CALL:
			break;

			case ADIEVN_CALL_DISCONNECTED:
				//( "Caller hung up." );
			return;

			default:
				//nmsReportUnexpectedEvent( &event );
			break;
			}
		}
}

int		nmsPlaceCall( CTAHD ctahd, char *digits, ADI_PLACECALL_PARMS *parmsp )
{
		CTA_EVENT event;

		adiPlaceCall( ctahd, digits, parmsp );

		for (;;)
		{
        nmsWaitForEvent( ctahd, &event );

        switch( event.id )
			{
			case ADIEVN_SEIZURE_DETECTED:    /* "low-level" events */
			case ADIEVN_CALL_PROCEEDING:
			case ADIEVN_REMOTE_ALERTING:
			case ADIEVN_REMOTE_ANSWERED:
			case ADIEVN_PROTOCOL_ERROR:      /* false seizure case */
			break;

			case ADIEVN_PLACING_CALL:        /* Glare resolved */
			break;

			case ADIEVN_INCOMING_CALL:
				nmsRejectCall( ctahd, ADI_REJ_PLAY_REORDER );
			return DISCONNECT;

			case ADIEVN_CALL_CONNECTED:
            return SUCCESS;

			case ADIEVN_CALL_DISCONNECTED:
				if(event.value == ADI_DIS_BUSY_DETECTED)
					return BUSY;
            return DISCONNECT;

			case ADIEVN_OUT_OF_SERVICE:
            break;

			case ADIEVN_IN_SERVICE:          /* back in service after O.O.S. */
            return FAILURE;

			default:
            break;
        }
    }
	return FAILURE;
}

int		nmsHangUp( CTAHD ctahd )
{
		CTA_EVENT event;

		adiReleaseCall( ctahd );

		for (;;)
		{
        nmsWaitForEvent( ctahd, &event );

			switch( event.id )
			{
			case ADIEVN_COLLECTION_DONE:
			case VCEEVN_PLAY_DONE:
			case VCEEVN_RECORD_DONE:
			case ADIEVN_DIGIT_END:
            break;   /* swallow */

			case ADIEVN_CALL_DISCONNECTED:
				//AddMessage( "Other party hung up." );
            break;

			case ADIEVN_CALL_RELEASED:
				//AddMessage( "Call done." );
            return DISCONNECT;

			default:
				//nmsReportUnexpectedEvent( &event );
            break;
			}
		}

		return FAILURE;
}
