// Linux sources: arch/x86/lib/delay.c

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HZ (sysconf(_SC_CLK_TCK))

#define CPU_PRESENT (sysconf(_SC_NPROCESSORS_CONF))
#define CPU_ONLINE (sysconf(_SC_NPROCESSORS_ONLN))

unsigned long loops_per_jiffy = (1 << 12);

static void delay_loop(unsigned long loops)
{
	asm volatile(
		"	test %0,%0	\n"
		"	jz 3f		\n"
		"	jmp 1f		\n"

		".align 16		\n"
		"1:	jmp 2f		\n"

		".align 16		\n"
		"2:	dec %0		\n"
		"	jnz 2b		\n"
		"3:	dec %0		\n"

		: /* we don't need output */
		:"a" (loops)
	);
}

static __always_inline unsigned long long rdtsc_ordered(void) 
{
	unsigned long long v64;
	asm volatile (
	"lfence"		"\n\t"
	"rdtsc"			"\n\t"
	"shlq	$32,	%%rdx"	"\n\t"
	"orq	%%rdx,	%%rax"	"\n\t"
	"movq	%%rax,	%0"
	: "=m" (v64)
	:
	: "%rax","%rcx","%rdx","memory");
	return(v64);
}

static void delay_tsc(unsigned long __loops)
{
	unsigned long long bclock, now, loops = __loops;
	// No check: preemption, migration
	bclock = rdtsc_ordered();
	for (;;) {
		now = rdtsc_ordered();
		if ((now - bclock) >= loops)
			break;
	}
}

static void (*delay_fn)(unsigned long) = delay_tsc;

void __delay(unsigned long loops)
{
	delay_fn(loops);
}

void __const_udelay(unsigned long xloops)
{
	unsigned long lpj = loops_per_jiffy;
	int d0;

	xloops *= 4;
	asm("mull %%edx"
		:"=d" (xloops), "=&a" (d0)
		:"1" (xloops), "0" (lpj * (HZ / 4)));

	__delay(++xloops);
}

void __udelay(unsigned long usecs)
{
	__const_udelay(usecs * 0x000010c7); /* 2**32 / 1000000 (rounded up) */
}

void __ndelay(unsigned long nsecs)
{
	__const_udelay(nsecs * 0x00005); /* 2**32 / 1000000000 (rounded up) */
}

#define MAX_UDELAY_MS	5

#define mdelay(n) (\
	(__builtin_constant_p(n) && (n)<=MAX_UDELAY_MS) ? __udelay((n)*1000) : \
	({unsigned long __ms=(n); while (__ms--) __udelay(1000);}))


int main(int argc, char* argv[])
{
	int rc = -1;
	if (argc == 4) {
		int cpu = atoi(argv[2]);

		if ((cpu < CPU_PRESENT) && (cpu >= 0)) {
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET(cpu, &cpuset);

			if (sched_setaffinity(0,sizeof(cpu_set_t),&cpuset) == 0)
			{
				unsigned long ksecs = atol(argv[3]);

				switch (argv[1][0]) {
				case 'm':
					mdelay(ksecs);

					rc = 0;
					break;
				case 'u':
					__udelay(ksecs);

					rc = 0;
					break;
				case 'n':
					__ndelay(ksecs);

					rc = 0;
					break;
				}
			}
		}
	}
	if (rc)
		printf( "Usage: %s <m|u|n> <cpu> <delay>\n"	\
			"Info : HZ [%ld] CPU[%ld/%ld]\n",
			argv[0], HZ, CPU_ONLINE, CPU_PRESENT);

	return(rc);
}

