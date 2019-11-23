#include <dos.h>
#include <mem.h>

#define	BYTE	unsigned char

#define	COUNTOUT	30000

int		WPORT=0, RPORT=0, SPORT=0;

#define	READY	outportb( SPORT, inportb(SPORT) & 0xfe );
#define	BUSY	outportb( SPORT, inportb(SPORT) | 0x01 );

BYTE	recvHalfByte( BYTE *value )
	{
	BYTE	flag = 0, recv = 0, ret = 0;
	int		countOut = 0;

	while( ( (flag = inportb( RPORT ) & 0x80) == 0x80 )
		&& ( countOut < COUNTOUT ) )
		{countOut++; }

	if( countOut == COUNTOUT )	ret = 2;
	else
		{
		recv = (inportb( RPORT ) & 0x78) >> 3;

		outportb( WPORT, recv | 0x10 );

		outportb( SPORT, inportb(SPORT) & 0xf5 );

		countOut = 0;
		while( ( (flag = inportb( SPORT ) & 0x0a ) == 0 )
			&& ( countOut < COUNTOUT ) )
			{countOut++; }

		outportb( WPORT, 0x00 );

		if( countOut == COUNTOUT )	ret = 2;
		else
			{
			if( flag & 0x08 )
				{
				*value = recv;
				ret = 1;
				}
			else
				ret =  0;
			}
		}

	return( ret );
	}

BYTE	recvByte( BYTE *value )
	{
	BYTE	loHalf, hiHalf, ret = 0;

	if( (inportb(RPORT) & 0x80) != 0x80)
		{
		if( ( recvHalfByte( &loHalf ) == 1 )
		&&  ( recvHalfByte( &hiHalf ) == 1 ) )
			{
			*value = loHalf + (hiHalf << 4);
			ret = 1;
			}
		else
			ret = 0 ;
		}
	else
		ret = 2;

	return( ret );
	}


BYTE	sendHalfByte( BYTE value )
	{
	BYTE	flag = 0, comp = 0, ret = 0;
	int		countOut = 0;

	outportb( SPORT, inportb(SPORT) & 0xf5 );
	outportb( WPORT, value | 0x10 );

	while( ( (flag = (inportb( RPORT ) & 0x80)) == 0x80 )
		&& ( countOut < COUNTOUT ) )
		{countOut++; }

	outportb( WPORT, 0x00 );

	if( countOut == COUNTOUT )	ret = 2;
	else
		{
		comp = (inportb( RPORT ) & 0x78) >> 3;

		if( comp == value )
			{
			outportb( SPORT, inportb(SPORT) & 0xf5 | 0x08 );
			ret = 1;
			}
		else
			{
			outportb( SPORT, inportb(SPORT) & 0xf5 | 0x02 );
			ret = 0;
			}

		countOut = 0;
		while( ( (flag = (inportb( RPORT ) & 0x80)) != 0x80 )
			&& ( countOut < COUNTOUT ) )
			{countOut++; }

		if( countOut == COUNTOUT )	ret = 2;
		}

	return( ret );
	}

BYTE	sendByte( BYTE value)
	{
	BYTE	loHalf = 0, hiHalf = 0, ret = 0;

	if( (inportb( SPORT ) & 0x01) == 0x01 )
		{
		loHalf = value & 0x0f;
		hiHalf = (value & 0xf0) >> 4;

		if( ( sendHalfByte( loHalf ) == 1 )
		 && ( sendHalfByte( hiHalf ) == 1 ) )
			ret = 1;
		else
			ret = 0;
		}
	else
		ret = 2;

	return( ret );
	}

typedef	struct
	{
	BYTE	flag;
	int		count;
	int		start;
	BYTE	buffer[1024];
	}
		RING;

RING	recvRING = {0}, sendRING = {0};

BYTE	WriteRing( RING *ring, BYTE value )
	{
	if( !ring->flag )
		{
		int		index = 0;

		ring->flag = 1;

		index = ( ring->start + ring->count ) % 1024;

		ring->buffer[index] = value;

		if( ring->count >= 1024 )
			{
			ring->start++;

			if( ring->start >= 1024 )
				ring->start = 1024;
			}
		else
			ring->count++ ;

		ring->flag = 0;

		return( 1 );
		}
	}

BYTE	TestRing( RING *ring, BYTE *value )
	{
	if( ring->count != 0 )
		{
		*value = ring->buffer[ring->start];

		return( 1 );
		}
	else
		return( 0 );
	}

BYTE	ReadRing( RING *ring, BYTE *value )
	{
	if( !ring->flag && ( ring->count != 0 ) )
		{
		ring->flag = 1;

		*value = ring->buffer[ring->start];

		ring->start++ ;

		if( ring->start >= 1024 )
			ring->start = 0;

		ring->count-- ;

		ring->flag = 0;

		return( 1 );
		}
	else
		return( 0 );
	}

void	interrupt	Link( void )
	{
	BYTE	value = 0;

	READY

	while( recvByte( &value ) == 1 )
		WriteRing( &recvRING, value );

	BUSY

	while( TestRing( &sendRING, &value ) && (sendByte( value ) == 1) )
		ReadRing( &sendRING, &value);
	}

BYTE	Recv( BYTE *recvString, int *length )
	{
	int		index = 0;
	BYTE	value = 0;

	if( TestRing( &recvRING, &value) )
		{
		while( ReadRing( &recvRING, &value ) )
			{
			recvString[index] = value;
			index++ ;
			}

		*length = index;

		return( 1 );
		}
	else
		return( 0 );
	}

void	Send( BYTE *sendString, int length )
	{
	int		index = 0;

	while( index < length )
		if( WriteRing( &sendRING, sendString[index] ) )
			index++ ;
	}

void	Sync( void )
	{
	BYTE	flag = 0;

	WPORT = peek( 0x0040, 0x0008 );
	RPORT = WPORT + 1;
	SPORT = RPORT + 1;

	outportb( WPORT, 0x00 );
	outportb( SPORT, 0x00 );

	do
		{
		outportb( WPORT, 0x0f );
		}
	while( (flag = inportb(RPORT) & 0xf8 ) != 0xf8 );

	delay( 10 );

	outportb( WPORT, 0x00 );

	setvect( 0x1c, Link );
	}
