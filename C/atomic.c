/*	atomic.c							*/

typedef unsigned long long int	Bit256[4];
typedef unsigned long long int	Bit64;
typedef unsigned int		Bit32;

#define LOCKLESS " "
#define BUS_LOCK "lock "

#define BITCMP_CC(_lock, _opl, _opr)					\
({									\
	volatile unsigned char _ret;					\
									\
	__asm__ volatile						\
	(								\
		"xorq		%%rsi, %%rsi"		"\n\t"		\
		"xorq		%%rdi, %%rdi"		"\n\t"		\
		"movq		0+%[opr], %%rax"	"\n\t"		\
		"movq		8+%[opr], %%rdx"	"\n\t"		\
		"movq		0+%[opl], %%rbx"	"\n\t"		\
		"movq		8+%[opl], %%rcx"	"\n\t"		\
	_lock	"rex cmpxchg16b 0+%[opl]"		"\n\t"		\
		"pushfq"				"\n\t"		\
		"movq		16+%[opr], %%rax"	"\n\t"		\
		"movq		24+%[opr], %%rdx"	"\n\t"		\
		"movq		16+%[opl], %%rbx"	"\n\t"		\
		"movq		24+%[opl], %%rcx"	"\n\t"		\
	_lock	"rex cmpxchg16b 16+%[opl]"		"\n\t"		\
		"setz		%%sil"			"\n\t"		\
		"popq		%%rdi"			"\n\t"		\
		"andq		$0x40, %%rdi"		"\n\t"		\
		"shrq		$6, %%rdi"		"\n\t"		\
		"andq		%%rsi, %%rdi"		"\n\t"		\
		"mov		%%dil, %[ret]"				\
		: [ret] "+m" (_ret),					\
		  [opl] "=m" (_opl)					\
		: [opr]  "m" (_opr)					\
		: "cc", "memory",					\
		  "%rax", "%rbx", "%rcx", "%rdx", "%rdi", "%rsi"	\
	);								\
									\
	_ret;								\
})

static Bit256 roomSeed __attribute__ ((aligned (64))) = {
0x1010101010101010, 0x0000111100001111, 0x1110001111100011, 0x1001100011100111
};
static Bit256 roomCore __attribute__ ((aligned (64))) = {
0x1010101010101010, 0x0000111100001111, 0x1110001111100011, 0x1001100011100111
};

int main(int argc, char *argv[])
{
	unsigned char ret = BITCMP_CC(BUS_LOCK, roomCore, roomSeed);

	return((int) ret);
}
