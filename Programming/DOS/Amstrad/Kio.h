#define	LEFTBUTTON				0
#define	RIGHTBUTTON				1
#define	LEFTBUTTONPRESSED		1
#define	RIGHTBUTTONPRESSED		2
#define	BOTHBUTTONSPRESSED		3
#define	CENTERBUTTONPRESSED		4

int	KIOcheckMouse( int	*ButtonsInstalled );
int	KIOmouseOpen( int XscreenClip, int YscreenClip );
int KIOmouseClose( void );
int	KIOshowMouse( void );
int	KIOhideMouse( void );
int	KIOsetMouseXY( int X, int Y );
int	KIOgetMouseXY( int *X, int *Y );
int	KIOmouseEvent( int ButtonMask, int *ButtonPushed, int *Pushes, int *X, int *Y );
