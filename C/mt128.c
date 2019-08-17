/*
	cc mt128.c -lpthread -o mt128
	./mt128 ; echo $?
*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <pthread.h>

#define MP 128

#define LOCKLESS " "
#define BUS_LOCK "lock "

#define CMPXCHG16B(_lock, _dest, _src)					\
({									\
	register unsigned char _ret = 0;				\
									\
	__asm__ volatile						\
	(								\
		"movq		%[losrc], %%rax"	"\n\t"		\
		"movq		%[hisrc], %%rdx"	"\n\t"		\
		"movq		%%rax, %%rbx"		"\n\t"		\
		"movq		%%rdx, %%rcx"		"\n\t"		\
	_lock"rex cmpxchg16b	%[dest]"		"\n\t"		\
		"setz		%[ret]" 				\
		: [dest] "=m" (_dest), [ret] "+r" (_ret)		\
		: [losrc] "m" (_src[0]), [hisrc] "m" (_src[1])		\
		: "cc", "memory", "%rax", "%rbx", "%rcx", "%rdx"	\
	);								\
									\
	_ret;								\
})

#define _BITSET_GPR(_lock, _base, _offset)				\
({									\
	__asm__ volatile						\
	(								\
	_lock	"btsq	%%rdx, %[base]" 				\
		: [base] "=m" (_base)					\
		: "d" (_offset)						\
		: "cc", "memory"					\
	);								\
})

#define _BITSET_IMM(_lock, _base, _imm8)				\
({									\
	__asm__ volatile						\
	(								\
	_lock	"btsq	%[imm8], %[base]"				\
		: [base] "=m" (_base)					\
		: [imm8] "i" (_imm8)					\
		: "cc", "memory"					\
	);								\
})

#define _BITCLR_GPR(_lock, _base, _offset)				\
({									\
	__asm__ volatile						\
	(								\
	_lock	"btrq	%%rdx,	%[base]"				\
		: [base] "=m" (_base)					\
		: "d" (_offset)						\
		: "cc", "memory"					\
	);								\
})

#define _BITCLR_IMM(_lock, _base, _imm8)				\
({									\
	__asm__ volatile						\
	(								\
	_lock	"btrq	%[imm8], %[base]"				\
		: [base] "=m" (_base)					\
		: [imm8] "i" (_imm8)					\
		: "cc", "memory"					\
	);								\
})

#define BITSET(_lock, _base, _offset)					\
(									\
	__builtin_constant_p(_offset) ? 				\
		_BITSET_IMM(_lock, _base, _offset)			\
	:	_BITSET_GPR(_lock, _base, _offset)			\
)

#define BITCLR(_lock, _base, _offset)					\
(									\
	__builtin_constant_p(_offset) ? 				\
		_BITCLR_IMM(_lock, _base, _offset)			\
	:	_BITCLR_GPR(_lock, _base, _offset)			\
)

#define BITSET128(_lock, _base, _offset)				\
({									\
	unsigned int Q = !(_offset / 64), R = _offset % 64;		\
	BITSET(BUS_LOCK, _base[Q], R);					\
})

#define BITCLR128(_lock, _base, _offset)				\
({									\
	unsigned int Q = !(_offset / 64), R = _offset % 64;		\
	BITCLR(BUS_LOCK, _base[Q], R);					\
})

typedef unsigned long long int	Bit128[2];

static Bit128 Room __attribute__ ((aligned (MP)));
static Bit128 Seed __attribute__ ((aligned (MP))) = {
	0xffffffffffffffffLLU, 0xffffffffffffffffLLU
};

typedef struct {
	unsigned int	bind;
	pthread_t	tid;
} ARG;

static void *CPU_Loop(void *arg)
{
	ARG		*Arg = (ARG *) arg;
	unsigned int	cpu  = Arg->bind;

	pthread_t tid = pthread_self();
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(cpu, &cpuset);
	if (pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset) == 0)
	{
		unsigned int loop;
		for (loop = cpu * 1000000; loop > 0; loop--);
	}

	BITSET128(BUS_LOCK, Room, cpu);

	return(NULL);
}

int main(int argc, char *argv[])
{
	unsigned count = 1000;

	ARG *Arg = calloc(MP, sizeof(ARG));
    if (Arg != NULL) {
		unsigned int cpu;
	do {
		Room[0] = 0x0LLU; Room[1] = 0x0LLU;

	    for (cpu = 0; cpu < MP; cpu++) {
		Arg[cpu].bind = cpu;
		pthread_create(&Arg[cpu].tid, NULL, CPU_Loop, &Arg[cpu]);
	    }

	    while(CMPXCHG16B(BUS_LOCK, Seed, Room) == 0);

	    for (cpu = 0; cpu < MP; cpu++) {
		if (Arg[cpu].tid)
			pthread_join(Arg[cpu].tid, NULL);
	    }
	} while (--count != 0);

	free(Arg);
    }
	return(count);
}

