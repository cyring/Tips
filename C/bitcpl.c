#include <stdio.h>
#include <stdlib.h>

#define BITCPL(_src)					\
({							\
	unsigned long long _dest;			\
	asm volatile					\
	(						\
		"mov	%[src], %[dest]"	"\n\t"	\
		"negq	%[dest]"			\
		: [dest] "=m" (_dest)			\
		: [src] "ir" (_src)			\
		: "memory"				\
	);						\
	_dest;						\
})

int main(int argc, char *argv[])
{
	unsigned long long m, r;
	unsigned long long s;

	if (argc > 1) {
		s = atol(argv[1]);
		r = ~s + 1;
		m = BITCPL(s);
		printf("s=%x(%u)\no=%llx(%llu)\nr=%llx(%llu)\n",s,s,m,m,r,r);
	}
}
