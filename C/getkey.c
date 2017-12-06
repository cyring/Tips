#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <termios.h>
#include <unistd.h>

typedef union
{
	unsigned long long key[2];
	unsigned char code[16];
} SCANKEY;

int GetKey(SCANKEY *scan)
{
	struct pollfd fds = {.fd = STDIN_FILENO, .events = POLLIN};
	int jk = 0, rp = 0, rz = 0;

	if ((rp = poll(&fds, 1, 0)) > 0)
	{
		size_t lc = 0, tc = 0;
//		for(jk=0; jk < 2; jk++)
		{
			lc = fread(&scan->key[jk], 1, 16, stdin);
			tc = lc;
		}
		for (rz = tc; rz < 16; rz++)
			scan->code[rz] = 0;
	}
	return(rp);
}

int main(int argc, char *argv[])
{
	struct termios oldt, newt;

	tcgetattr ( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	newt.c_cc[VTIME] = 0;
	newt.c_cc[VMIN] = 0;
	tcsetattr ( STDIN_FILENO, TCSANOW, &newt );

	int xmouse = 0;
	if ((argc == 2) && (argv[1][0] == '-') && (argv[1][1] == 'x'))
		xmouse = 1;

	if (xmouse)
		printf("ANSI:\033[?1000h\n");

	int rp = 0;
	SCANKEY scan = {.key = 0};
	while(scan.key[0] != 0x1b)
	{
		usleep(250000);

	    if ((rp = GetKey(&scan)) > 0)
		printf(	"Poll(\033[1;35;49m%d\033[1;39;49m)"	\
			"\033[1;31;49m[\033[1;39;49m%x:"	\
			"%d,%d,%d,%d,%d,%d,%d,%d\033[1;31;49m]\033[1;39;49m\n",
			rp,
			scan.key[0],
			scan.code[ 8],
			scan.code[ 9],
			scan.code[10],
			scan.code[11],
			scan.code[12],
			scan.code[13],
			scan.code[14],
			scan.code[15]);
	}
	tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );

	if (xmouse)
		printf("\033[?1000l\033[0m");	fflush(stdout);
}
