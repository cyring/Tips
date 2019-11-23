#include <conio.h>
#include <dos.h>


int	KIOcheckMouse( int *ButtonsInstalled )
{
	struct	REGPACK	cpu;

	cpu.r_ax = 0x0000;

	intr( 0x33, &cpu );

	if( cpu.r_ax != 0 )
		{
		*ButtonsInstalled = cpu.r_bx;

		return( 1 );
		}
	else
		{
		*ButtonsInstalled = 0;

		cprintf( "No mouse detected\r\n" );

		return( 0 );
		}
}


int	KIOmouseOpen( int XscreenClip, int YscreenClip )
{
	struct	REGPACK	cpu;

	cpu.r_ax = 0x0007;
	cpu.r_cx = 0;
	cpu.r_dx = XscreenClip;

	intr( 0x33, &cpu );

	cpu.r_ax = 0x0008;
	cpu.r_cx = 0;
	cpu.r_dx = YscreenClip;

	intr( 0x33, &cpu );

	cpu.r_ax = 0x000f;
	cpu.r_cx = 8;
	cpu.r_dx = 16;

	intr( 0x33, &cpu );

	KIOsetMouseXY( XscreenClip >> 1, YscreenClip >> 1 );

	return( 1 );
}


int KIOmouseClose( void )
{
	struct	REGPACK	cpu;

	cpu.r_ax = 0x0007;
	cpu.r_cx = 0;
	cpu.r_dx = 80;

	intr( 0x33, &cpu );

	cpu.r_ax = 0x0008;
	cpu.r_cx = 0;
	cpu.r_dx = 25;

	intr( 0x33, &cpu );

	return( 1 );
}


int	KIOshowMouse( void )
{
	struct	REGPACK	cpu;

	cpu.r_ax = 0x0001;

	intr( 0x33, &cpu );

	return( 1 );
}


int	KIOhideMouse( void )
{
	struct	REGPACK	cpu;

	cpu.r_ax = 0x0002;

	intr( 0x33, &cpu );

	return( 1 );
}


int	KIOsetMouseXY( int X, int Y )
{
	struct	REGPACK	cpu;

	cpu.r_ax = 0x0004;
	cpu.r_cx = X;
	cpu.r_dx = Y;

	intr( 0x33, &cpu );

	return( 1 );
}


int	KIOgetMouseXY( int *X, int *Y )
{
	struct	REGPACK	cpu;

	cpu.r_ax = 0x0003;

	intr( 0x33, &cpu );

	*X = cpu.r_cx;
	*Y = cpu.r_dx;

	return( 1 );
}


int	KIOmouseEvent( int ButtonMask, int *ButtonPushed, int *Pushes, int *X, int *Y )
{
	struct	REGPACK	cpu;

	cpu.r_ax = 0x0005;
	cpu.r_bx = ButtonMask;

	intr( 0x33, &cpu );

	*ButtonPushed	= cpu.r_ax;
	*Pushes 		= cpu.r_bx;
	*X				= cpu.r_cx;
	*Y				= cpu.r_dx;

	return( 1 );
}
