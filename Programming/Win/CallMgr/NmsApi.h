/*
*       (C) 1997,98,99 Bouygues Telecom
*
*       Projet          [ NMS Layer									   ]
*       Direction       [ DCTSI/IPS/IPA                                ]
*       Auteurs         [ Courtiat Cyril                               ]
*
*       Fichier         [ NmsApi.h                                	   ]
*
*       Description :   Interface simplifiée de l'API NMS
*
*/

/*---------------------------------------------------------------------------*/

void	nmsWaitForEvent( CTAHD ctahd, CTA_EVENT *eventp );
void	nmsWaitForSpecificEvent(CTAHD ctahd,
								DWORD desired_event, CTA_EVENT *eventp );
void	nmsInitialize(CTA_SERVICE_NAME servicelist[], unsigned numservices);
void	nmsOpenPort( unsigned    userid,
					 char       *contextname,
					 CTA_SERVICE_DESC services[],
					 unsigned    numservices,
					 CTAQUEUEHD *ctaqueuehd,
					 CTAHD      *ctahd) ;
void	nmsOpenAnotherPort(	unsigned          userid,
							char             *contextname,
							CTA_SERVICE_DESC  services[],
							unsigned          numservices,
							CTAQUEUEHD        ctaqueuehd,
							CTAHD            *ctahd );
void	nmsShutdown( CTAHD ctahd );

void	nmsStartProtocol( CTAHD            ctahd,
						  char            *protocol,
						  WORD            *protparmsp,
						  ADI_START_PARMS *stparmsp );
void	nmsStopProtocol( CTAHD ctahd );
void	nmsRejectCall( CTAHD ctahd, unsigned method );
int		nmsPlaceCall( CTAHD ctahd, char *digits, ADI_PLACECALL_PARMS *parmsp );
int		nmsHangUp( CTAHD ctahd );


typedef struct
{
    CTAQUEUEHD  ctaqueuehd;       /* CT Access queue handle                 */
    CTAHD       ctahd;            /* CT Access context handle               */
    unsigned    maxredial;        /* maximum retries on that number         */
    unsigned    lowlevelflag;     /* flag to get low-level events           */
} NMSCONTEXT;
