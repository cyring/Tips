/*
 * zencli.c by CyrIng
 *
 * Copyright (C) 2020 CYRIL INGENIERIE
 * Licenses: GPL2
 *
 * - Build Instructions -
 *	cc zencli.c -o zencli
 */

#define _GNU_SOURCE
#include <sys/io.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define PCI_CONFIG_ADDRESS(bus, dev, fn, reg) \
	(0x80000000 | (bus << 16) | (dev << 11) | (fn << 8) | (reg & ~3))

#define RDPCI(_data, _reg)						\
({									\
	__asm__ volatile						\
	(								\
		"movl	%1,	%%eax"	"\n\t"				\
		"movl	$0xcf8,	%%edx"	"\n\t"				\
		"outl	%%eax,	%%dx"	"\n\t"				\
		"movl	$0xcfc,	%%edx"	"\n\t"				\
		"inl	%%dx,	%%eax"	"\n\t"				\
		"movl	%%eax,	%0"					\
		: "=m"	(_data)						\
		: "ir"	(_reg)						\
		: "%rax", "%rdx", "memory"				\
	);								\
})

#define WRPCI(_data, _reg)						\
({									\
	__asm__ volatile						\
	(								\
		"movl	%1,	%%eax"	"\n\t"				\
		"movl	$0xcf8,	%%edx"	"\n\t"				\
		"outl	%%eax,	%%dx"	"\n\t"				\
		"movl	%0,	%%eax"	"\n\t"				\
		"movl	$0xcfc,	%%edx"	"\n\t"				\
		"outl	%%eax,	%%dx"					\
		:							\
		: "irm" (_data),					\
		  "ir"	(_reg)						\
		: "%rax", "%rdx", "memory"				\
	);								\
})

#define SMU_AMD_INDEX_REGISTER_F17H	PCI_CONFIG_ADDRESS(0, 0, 0, 0x60)
#define SMU_AMD_DATA_REGISTER_F17H	PCI_CONFIG_ADDRESS(0, 0, 0, 0x64)

#define AMD_FCH_READ16(_data, _reg)					\
({									\
	__asm__ volatile						\
	(								\
		"movl	%1	,	%%eax"		"\n\t"		\
		"movl	$0xcd6	,	%%edx"		"\n\t"		\
		"outl	%%eax	,	%%dx"		"\n\t"		\
		"movl	$0xcd7	,	%%edx"		"\n\t"		\
		"inw	%%dx	,	%%ax"		"\n\t"		\
		"movw	%%ax	,	%0"				\
		: "=m"	(_data) 					\
		: "ir"	(_reg)						\
		: "%rax", "%rdx", "memory"				\
	);								\
})

#define AMD_FCH_WRITE16(_data, _reg)					\
({									\
	__asm__ volatile						\
	(								\
		"movl	%1	,	%%eax"		"\n\t"		\
		"movl	$0xcd6	,	%%edx"		"\n\t"		\
		"outl	%%eax	,	%%dx"		"\n\t"		\
		"movw	%0	,	%%ax" 		"\n\t"		\
		"movl	$0xcd7	,	%%edx"		"\n\t"		\
		"outw	%%ax	,	%%dx"		"\n\t"		\
		:							\
		: "im"	(_data),					\
		  "ir"	(_reg)						\
		: "%rax", "%rdx", "memory"				\
	);								\
})

union DATA {
	unsigned short	word;
	unsigned int	dword;
};

void SMU_Read(union DATA *data, unsigned int addr)
{
	WRPCI(addr, SMU_AMD_INDEX_REGISTER_F17H);
	RDPCI(data->dword, SMU_AMD_DATA_REGISTER_F17H);
}

void FCH_Read(union DATA *data, unsigned int addr)
{
	AMD_FCH_READ16(data->word, addr);
}

enum IC {
	SMU,
	FCH,
	LAST
};

char *component[LAST] = { [SMU] = "smu", [FCH] = "fch" };

void (*IC_Read[LAST])(union DATA*, unsigned int) = {
	[SMU] = SMU_Read,
	[FCH] = FCH_Read
};

int main(int argc, char *argv[])
{
	int rc;
    if (argc != 3) {
	rc = 1;
USAGE:
	printf("\nUsage: %s <component> <addr>\n\n", argv[0]);
    } else {
	enum IC ic;
	for (ic = SMU; ic < LAST; ic++)
	{
	    if (!strncmp(component[ic], argv[1], strlen(component[ic]))) {
		break;
	    }
	}
	if (ic == LAST) {
		rc = 2;
		printf("\nError: List of components:");
		for (ic = SMU; ic < LAST; ic++) {
			printf(" %s", component[ic]);
		}
		goto USAGE;
	} else {
		unsigned int addr = 0x0;
	    if (sscanf(argv[2], "0x%x", &addr) != 1)
	    {
		rc = 3;
		printf("\nError: Invalid Hexadecimal Address\n" \
			"\tExpected  0x1a2b3c4d\n");
		goto USAGE;
	    } else {
		uid_t uid = geteuid();
		if (uid != 0) {
			rc = 4;
			printf("\nError: Missing root privileges\n");
			goto USAGE;
		} else if (!iopl(3)) {
			union DATA data = { .dword = 0 };

			IC_Read[ic](&data, addr);

			printf("0x%x (%u)\n", data.dword, data.dword);

			iopl(0);
			rc = 0;
		} else {
			rc = 5;
			printf("\nIOPL Not Permitted\n\n");
		}
	    }
	}
    }
	return (rc);
}

