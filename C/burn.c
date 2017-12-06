#include <stdio.h>
#include <stdlib.h>

static inline unsigned long long *xchg_64(void *ptr, unsigned long long x)
{
	__asm__ __volatile__
	(
		"xchgq %0,%1"
		:"=r"((unsigned long long) x)
		:"m" (*(volatile long long *) ptr), "0" (x)
		:"memory"
	);
	return(x);
}

int main(int argc, char *argv[])
{
	unsigned long long cx = 1LLU;
	unsigned long long P = (argc == 2) ? atol(argv[1]) : 1000000000LLU;

	printf("xchg(%llu, %llu)\n", P, cx);
	while (cx > 0) {
		cx = xchg_64(&P, (P - 1LLU));
	};
	printf("xchg(%llu, %llu)\n", P, cx);
}
