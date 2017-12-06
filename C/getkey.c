#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <termios.h>
#include <unistd.h>

#define TIMESPEC(nsec)							\
({									\
	struct timespec tsec = {					\
		.tv_sec  = (time_t) 0,					\
		.tv_nsec = nsec						\
	};								\
	tsec;								\
})

typedef union {
	unsigned long long key;
	unsigned char code[8];
} SCANKEY;

int GetKey(SCANKEY *scan, struct timespec *tsec)
{
	struct pollfd fds = {.fd = STDIN_FILENO, .events = POLLIN};
	int rp = 0, rz = 0;

	if ((rp = ppoll(&fds, 1, tsec, NULL)) > 0)
		if (fds.revents == POLLIN) {
			size_t lc = fread(&scan->key, 1, 8, stdin);
			for (rz = lc; rz < 8; rz++)
				scan->code[rz] = 0;
		}
	return(rp);
}

int main(int argc, char *argv[])
{
	struct timespec wait = TIMESPEC(250000000LLU);
	struct termios oldt, newt;
	SCANKEY scan = {.key = 0};
	int xmouse = 0, rp = 0;

	tcgetattr ( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	newt.c_cc[VTIME] = 0;
	newt.c_cc[VMIN] = 0;
	tcsetattr ( STDIN_FILENO, TCSANOW, &newt );

	if ((argc == 2) && (argv[1][0] == '-') && (argv[1][1] == 'x'))
		xmouse = 1;

	if (xmouse)
		printf("ANSI:\033[?1000h\n");

	while(scan.key != 0x1b)
	{
		if ((rp = GetKey(&scan, &wait)) > 0)
			printf(	"Poll(\033[1;35;49m%d\033[1;39;49m)"	\
				"\033[1;31;49m[\033[1;39;49m%llx"	\
				"\033[1;31;49m]\033[1;39;49m\n",
				rp, scan.key);
	}
	tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );

	if (xmouse)
		printf("\033[?1000l\033[0m");	fflush(stdout);
}
