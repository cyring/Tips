#define PCI_CONFIG_ADDRESS(bus, dev, fn, reg) \
	(0x80000000 | (bus << 16) | (dev << 11) | (fn << 8) | (reg & ~3))

#define RDPCI(_data, _reg)						\
({									\
	asm volatile							\
	(								\
		"xorq	%%rax,	%%rax"	"\n\t"				\
		"xorq	%%rdx,	%%rdx"	"\n\t"				\
		"movl	%1,	%%eax"	"\n\t"				\
		"movl	$0xcf8,	%%edx"	"\n\t"				\
		"outl	%%eax,	%%dx"	"\n\t"				\
		"movl	$0xcfc,	%%edx"	"\n\t"				\
		"inl	%%dx,	%%eax"	"\n\t"				\
		"movl	%%eax,	%0"					\
		: "=m"	(_data)						\
		: "i"	(_reg)						\
		: "%rax", "%rdx", "memory"				\
	);								\
})

#define WRPCI(_data, _reg)						\
({									\
	asm volatile							\
	(								\
		"xorq	%%rax,	%%rax"	"\n\t"				\
		"xorq	%%rdx,	%%rdx"	"\n\t"				\
		"movl	%1,	%%eax"	"\n\t"				\
		"movl	$0xcf8,	%%edx"	"\n\t"				\
		"outl	%%eax,	%%dx"	"\n\t"				\
		"movl	%0,	%%eax"	"\n\t"				\
		"movl	$0xcfc,	%%edx"	"\n\t"				\
		"outl	%%eax,	%%dx"					\
		:							\
		: "m"	(_data),					\
		  "i"	(_reg)						\
		: "%rax", "%rdx", "memory"				\
	);								\
})

int main(int argc, char *argv[])
{
	unsigned int Thermtrip=0;

	RDPCI(Thermtrip, PCI_CONFIG_ADDRESS(0, 24, 3, 0xe4));
	WRPCI(Thermtrip, PCI_CONFIG_ADDRESS(0, 24, 3, 0xe4));
	// Read PCI configuration register
	asm volatile
	(
		"outl	%0, %1"
		:
		: "a"	(PCI_CONFIG_ADDRESS(0, 24, 3, 0xe4)),
		  "dN"	((unsigned short) 0xcf8)
	);
	asm volatile
	(
		"inl	%1, %0"
		: "=a"	(Thermtrip)
		: "dN"	((unsigned short) 0xcfc)
	);
	// Write PCI
	asm volatile
	(
		"outl	%0, %1"
		:
		: "a"	(PCI_CONFIG_ADDRESS(0, 24, 3, 0xe4)),
		  "dN"	((unsigned short) 0xcf8)
	);
	asm volatile
	(
		"outl	%0, %1"
		:
		: "a"	(Thermtrip),
		  "dN"	((unsigned short) 0xcfc)
	);
}
