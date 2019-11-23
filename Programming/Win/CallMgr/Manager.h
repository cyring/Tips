/*
*       (C) 1997,98,99 Bouygues Telecom
*
*       Projet          [ Call Manager								   ]
*       Direction       [ DCTSI/IPS/IPA                                ]
*       Auteurs         [ Courtiat Cyril                               ]
*
*       Fichier         [ Manager.h                                	   ]
*
*       Description :   Gestionnaire de communication
*
*/

/*---------------------------------------------------------------------------*/

#define	MAX_PORTS		240

typedef	struct
{
	NMSCONTEXT	*ctxHandle;
	unsigned	ag_board;
	unsigned	mvip_stream;
	unsigned	mvip_slot;
} TRUNK;
