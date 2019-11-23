#include "KIO.h"
#include "OBJ.h"
#include "EVENT.h"


int	EVENTquery( int *what, int *objHandle )
{
	int ButtonPushed;
	int Pushes;
	int Xevent;
	int Yevent;
	int	rc;

	rc = KIOmouseEvent( LEFTBUTTON, &ButtonPushed, &Pushes, &Xevent, &Yevent );

	if( Pushes > 0 )
		{
		rc = OBJcollision( Xevent, Yevent, objHandle );

		if( Pushes == 1 )
			*what = LEFTSINGLECLICK;
		else
			*what = LEFTDOUBLECLICK;

		return( rc );
		}
	else
		{
		rc = KIOmouseEvent( RIGHTBUTTON, &ButtonPushed, &Pushes, &Xevent, &Yevent );

		if( Pushes > 0 )
			{
			rc = OBJcollision( Xevent, Yevent, objHandle );

			if( Pushes == 1 )
				*what = RIGHTSINGLECLICK;
			else
				*what = RIGHTDOUBLECLICK;

			return( rc );
			}
		else
			{
			*what	= NOEVENT;

			return( 0 );
			}
		}
}
