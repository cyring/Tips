/*
 * zencli.c by CyrIng
 *
 * Copyright (C) 2020-2021 CYRIL INGENIERIE
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

#define _BITVAL_GPR(_lock,_base, _offset)				\
({									\
	Bit64 _tmp __attribute__ ((aligned (8))) = _base;		\
	volatile unsigned char _ret;					\
									\
	__asm__ volatile						\
	(								\
	_lock	"btcq	%%rdx, %[tmp]"		"\n\t"			\
		"setc	%[ret]" 					\
		: [ret] "+m" (_ret)					\
		: [tmp] "m" (_tmp),					\
		  "d" (_offset) 					\
		: "cc", "memory"					\
	);								\
	_ret;								\
})

#define _BITVAL_IMM(_lock, _base, _imm8)				\
({									\
	Bit64 _tmp __attribute__ ((aligned (8))) = _base;		\
	volatile unsigned char _ret;					\
									\
	__asm__ volatile						\
	(								\
	_lock	"btcq	%[imm8], %[tmp]"	"\n\t"			\
		"setc	%[ret]" 					\
		: [ret] "+m" (_ret)					\
		: [tmp] "m"  (_tmp),					\
		  [imm8] "i" (_imm8)					\
		: "cc", "memory"					\
	);								\
	_ret;								\
})

#define _BIT_TEST_GPR(_base, _offset)					\
({									\
	volatile unsigned char _ret;					\
									\
	__asm__ volatile						\
	(								\
		"btq	%%rdx, %[base]" 	"\n\t"			\
		"setc	%[ret]" 					\
		: [ret] "+m" (_ret)					\
		: [base] "m" (_base),					\
		  "d" ( _offset )					\
		: "cc", "memory"					\
	);								\
	_ret;								\
})

#define _BIT_TEST_IMM(_base, _imm8)					\
({									\
	volatile unsigned char _ret;					\
									\
	__asm__ volatile						\
	(								\
		"btq	%[imm8], %[base]"	"\n\t"			\
		"setc	%[ret]" 					\
		: [ret] "+m" (_ret)					\
		: [base] "m" (_base),					\
		  [imm8] "i" (_imm8)					\
		: "cc", "memory"					\
	);								\
	_ret;								\
})

#define BITVAL_2xPARAM(_base, _offset)					\
(									\
	__builtin_constant_p(_offset) ? 				\
		_BIT_TEST_IMM(_base, _offset)				\
	:	_BIT_TEST_GPR(_base, _offset)				\
)

#define BITVAL_3xPARAM(_lock, _base, _offset)				\
(									\
	__builtin_constant_p(_offset) ? 				\
		_BITVAL_IMM(_lock, _base, _offset)			\
	:	_BITVAL_GPR(_lock, _base, _offset)			\
)

#define BITVAL_DISPATCH(_1,_2,_3,BITVAL_CURSOR, ...)			\
	BITVAL_CURSOR

#define BITVAL(...)							\
	BITVAL_DISPATCH( __VA_ARGS__ ,	BITVAL_3xPARAM ,		\
					BITVAL_2xPARAM ,		\
					NULL)( __VA_ARGS__ )

#define PCI_CONFIG_ADDRESS(bus, dev, fn, reg)				\
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

#define SMU_AMD_INDEX_REGISTER_F15H	PCI_CONFIG_ADDRESS(0, 0, 0, 0xb8)
#define SMU_AMD_DATA_REGISTER_F15H	PCI_CONFIG_ADDRESS(0, 0, 0, 0xbc)

#define SMU_AMD_INDEX_REGISTER_F17H	PCI_CONFIG_ADDRESS(0, 0, 0, 0x60)
#define SMU_AMD_DATA_REGISTER_F17H	PCI_CONFIG_ADDRESS(0, 0, 0, 0x64)
/* F17h PCI alternates addr: { 0xc4 , 0xc8 } - or - { 0xb4 , 0xb8 }	*/
#define SMU_AMD_INDEX_REGISTER_ALT_F17H	PCI_CONFIG_ADDRESS(0, 0, 0, 0xc4)
#define SMU_AMD_DATA_REGISTER_ALT_F17H	PCI_CONFIG_ADDRESS(0, 0, 0, 0xc8)

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

#define AMD_BIOS_READ16(_data, _reg)					\
({									\
	__asm__ volatile						\
	(								\
		"movl	%1	,	%%eax"		"\n\t"		\
		"movl	$0xcd4	,	%%edx"		"\n\t"		\
		"outl	%%eax	,	%%dx"		"\n\t"		\
		"movl	$0xcd5	,	%%edx"		"\n\t"		\
		"inw	%%dx	,	%%ax"		"\n\t"		\
		"movw	%%ax	,	%0"				\
		: "=m"	(_data) 					\
		: "ir"	(_reg)						\
		: "%rax", "%rdx", "memory"				\
	);								\
})

#define AMD_BIOS_WRITE16(_data, _reg)					\
({									\
	__asm__ volatile						\
	(								\
		"movl	%1	,	%%eax"		"\n\t"		\
		"movl	$0xcd4	,	%%edx"		"\n\t"		\
		"outl	%%eax	,	%%dx"		"\n\t"		\
		"movw	%0	,	%%ax" 		"\n\t"		\
		"movl	$0xcd5	,	%%edx"		"\n\t"		\
		"outw	%%ax	,	%%dx"		"\n\t"		\
		:							\
		: "im"	(_data),					\
		  "ir"	(_reg)						\
		: "%rax", "%rdx", "memory"				\
	);								\
})

#define AMD_PM2_READ16(_data, _reg)					\
({									\
	__asm__ volatile						\
	(								\
		"movl	%1	,	%%eax"		"\n\t"		\
		"movl	$0xcd0	,	%%edx"		"\n\t"		\
		"outl	%%eax	,	%%dx"		"\n\t"		\
		"movl	$0xcd1	,	%%edx"		"\n\t"		\
		"inw	%%dx	,	%%ax"		"\n\t"		\
		"movw	%%ax	,	%0"				\
		: "=m"	(_data) 					\
		: "ir"	(_reg)						\
		: "%rax", "%rdx", "memory"				\
	);								\
})

#define AMD_PM2_WRITE16(_data, _reg)					\
({									\
	__asm__ volatile						\
	(								\
		"movl	%1	,	%%eax"		"\n\t"		\
		"movl	$0xcd0	,	%%edx"		"\n\t"		\
		"outl	%%eax	,	%%dx"		"\n\t"		\
		"movw	%0	,	%%ax" 		"\n\t"		\
		"movl	$0xcd1	,	%%edx"		"\n\t"		\
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

void OTH_Read(union DATA *data, unsigned int addr)
{
	WRPCI(addr, SMU_AMD_INDEX_REGISTER_F15H);
	RDPCI(data->dword, SMU_AMD_DATA_REGISTER_F15H);
}

void PCI_Read(union DATA *data, unsigned int addr)
{
	RDPCI(data->dword, addr);
}

void PCI_Write(union DATA *data, unsigned int addr)
{
	WRPCI(addr, data->dword);
}

void SMU_Read(union DATA *data, unsigned int addr)
{
	WRPCI(addr, SMU_AMD_INDEX_REGISTER_F17H);
	RDPCI(data->dword, SMU_AMD_DATA_REGISTER_F17H);
}

void SMU_Write(union DATA *data, unsigned int addr)
{
	WRPCI(addr, SMU_AMD_INDEX_REGISTER_F17H);
	WRPCI(SMU_AMD_DATA_REGISTER_F17H, data->dword);
}

void FCH_Read(union DATA *data, unsigned int addr)
{
	AMD_FCH_READ16(data->word, addr);
}

void FCH_WRITE(union DATA *data, unsigned int addr)
{
	AMD_FCH_WRITE16(data->word, addr);
}

void BIOS_Read(union DATA *data, unsigned int addr)
{
	AMD_BIOS_READ16(data->word, addr);
}

void BIOS_Write(union DATA *data, unsigned int addr)
{
	AMD_BIOS_WRITE16(data->word, addr);
}

void PM2_Read(union DATA *data, unsigned int addr)
{
	AMD_PM2_READ16(data->word, addr);
}

void PM2_Write(union DATA *data, unsigned int addr)
{
	AMD_PM2_WRITE16(data->word, addr);
}

/* BEGIN
 * Source: taken from the <Ryzen SMU> project
 *	@ https://gitlab.com/leogx9r/ryzen_smu
*/
enum SMU_RC {
	SMU_OK	= 0x01
};

#define CMD	/*	0x3b10524	*/	0x3b10530
#define RSP	/*	0x3b10570	*/	0x3b1057c
#define ARG	/*	0x3b10a40	*/	0x3b109c4

void ZEN2_Read(union DATA *data, unsigned int addr)
{
	union DATA local = {.dword = 0};
	unsigned int tries = 30000;
	do {
		SMU_Read(&local, RSP);
		if (local.dword != 0) {
			break;
		}
	} while (tries-- != 0);

	if (tries > 0) {
		local.dword = 0;
		SMU_Write(&local, RSP);

		local.dword = 0;
		for (unsigned int idx = 0; idx < 6; idx++) {
			SMU_Write(&local, ARG + (idx * 4));
		}

		local.dword = addr;
		SMU_Write(&local, CMD);

		local.dword = 0;
		tries = 30000;
		do {
			SMU_Read(&local, RSP);
			if (local.dword != 0) {
				break;
			}
		} while (tries-- != 0);

		if (tries > 0) {
		    if (local.dword == SMU_OK) {
			for (unsigned int idx = 0; idx < 6; idx++) {
				SMU_Read(data + (idx *4), ARG + (idx * 4));
			}
		    }
		}
	}
}

#define ZEN3_Read	ZEN2_Read
/* <Ryzen SMU>: END */

#define MAX_CHANNELS	8
#define SMU_AMD_UMC_BASE_CHA_F17H( _bar, _cha )	( _bar + (_cha << 20) )

void UMC_Read(union DATA *data, unsigned int _addr)
{
	unsigned int addr = _addr == 0x0 ? 0x00050000 : _addr;
	unsigned int UMC_BAR[MAX_CHANNELS] = { 0,0,0,0,0,0,0,0 };
	unsigned short ChannelCount = 0, cha, chip, sec;

	printf("\nData Fabric: scanning UMC @ BAR[0x%08x] : ", addr);
    for (cha = 0; cha < MAX_CHANNELS; cha++)
    {
	union DATA SdpCtrl = {.dword = 0};

	SMU_Read(&SdpCtrl, SMU_AMD_UMC_BASE_CHA_F17H(addr, cha) + 0x104);

	if ((SdpCtrl.dword != 0xffffffff) && (BITVAL(SdpCtrl.dword, 31)))
	{
		UMC_BAR[ChannelCount++] = SMU_AMD_UMC_BASE_CHA_F17H(addr, cha);
	}
	printf("%u ", cha);
    }
	printf("for %u Channels\n\n", ChannelCount);

    for (cha = 0; cha < ChannelCount; cha++)
    {
	unsigned long long DIMM_Size = 0;

	const unsigned int CHIP_BAR[2][2] = {
	[0] =	{
		[0] = UMC_BAR[cha] + 0x0,
		[1] = UMC_BAR[cha] + 0x20
		},
	[1] =	{
		[0] = UMC_BAR[cha] + 0x10,
		[1] = UMC_BAR[cha] + 0x28
		}
	};
	for (chip = 0; chip < 4; chip++)
	{
	    for (sec = 0; sec < 2; sec++)
	    {
		union DATA ChipReg, MaskReg;
		unsigned int addr[2], state, rank = 0;

		addr[1] = CHIP_BAR[sec][1] + 4 * (chip >> 1);

		SMU_Read(&MaskReg, addr[1]);

		if ((rank == 0) && (MaskReg.dword != 0)) {
			rank = BITVAL(MaskReg.dword, 9) ? 1 : 2;
		}
		if (rank == 2) {
			addr[0] = CHIP_BAR[sec][0] + 4 * chip;
		} else {
			addr[0] = CHIP_BAR[sec][0] + 4 * (chip - (chip > 2));
		}
		SMU_Read(&ChipReg, addr[0]);

		state = BITVAL(ChipReg.dword, 0);

		printf("CHA[%u] CHIP[%u:%u] @ 0x%08x[0x%08x] %sable, Rank=%u\n",
			cha, chip, sec, addr[0], ChipReg.dword,
			state ? "En":"Dis", rank);

		if (state)
		{
			unsigned int chipSize;

			__asm__ volatile
			(
			"DECODER:"				"\n\t"
				"xorl	%%edx, %%edx"		"\n\t"
				"bsrl	%[base], %%ecx" 	"\n\t"
				"jz	1f"			"\n\t"
				"incl	%%edx"			"\n\t"
				"shll	%%ecx, %%edx"	 	"\n\t"
				"negl	%%edx"			"\n\t"
				"notl	%%edx"			"\n\t"
				"andl	$0xfffffffe, %%edx"	"\n\t"
				"shrl	$2, %%edx"		"\n\t"
				"incl	%%edx"			"\n\t"
			"1:"					"\n\t"
				"movl	%%edx, %[dest]"
				: [dest] "=m" (chipSize)
				: [base] "m"  (MaskReg.dword)
				: "cc", "memory", "%ecx", "%edx"
			);

			DIMM_Size += chipSize;

		printf( "CHA[%u] MASK[%u:%u] @ 0x%08x[0x%08x] ChipSize[%u]\n",
			cha, chip, sec, addr[1], MaskReg.dword, chipSize );
		} else {
		printf( "CHA[%u] MASK[%u:%u] @ 0x%08x[0x%08x]\n",
			cha, chip, sec, addr[1], MaskReg.dword );
		}
	    }
	}
	printf( "\nDIMM Size[%llu KB] [%llu MB]\n\n",
		DIMM_Size, (DIMM_Size >> 10) );
    }
}

const char *BIN[0x10] =	{
	"0000",
	"0001",
	"0010",
	"0011",

	"0100",
	"0101",
	"0110",
	"0111",

	"1000",
	"1001",
	"1010",
	"1011",

	"1100",
	"1101",
	"1110",
	"1111",
};

void Convert2Binary(unsigned long long value, char *pBinStr)
{
	unsigned int I, H = 0xf;
	for (I = 1; I <= 16; I++)
	{
		const unsigned int B =H<<2, nibble = value & 0xf;

		pBinStr[B  ] = BIN[nibble][0];
		pBinStr[B+1] = BIN[nibble][1];
		pBinStr[B+2] = BIN[nibble][2];
		pBinStr[B+3] = BIN[nibble][3];

		H--;
		value = value >> 4;
	}
}

void PrettyBin(char *pBinStr)
{
	unsigned short bit;
	for (bit = 63; bit > 0; bit--) {
		if (bit % 4 == 0) {
			printf("%02hu", bit);
		} else {
			printf(" ");
		}
	}
	printf("00\n ");
	for (bit = 0; bit < 64; bit++) {
		printf("%c", pBinStr[bit]);
		if (bit < 63 && bit % 4 == 3) {
			printf(" ");
		}
	}
}

enum OP {
	READ,
	WRITE,
	OPS
};

enum IC {
	SMU,
	FCH,
	UMC,
	PCI,
	BIOS,
	PM2,
	ZEN1,
	ZEN12,
	ZEN2,
	ZEN3,
	OTH,
	LAST
};

char *component[LAST] = {
	[SMU] = "smu" , [FCH] = "fch", [UMC] = "umc", [PCI] = "pci",
	[BIOS] = "bios", [PM2] = "pm2",
	[ZEN1] = "zen1", [ZEN12] = "zen12", [ZEN2] = "zen2", [ZEN3] = "zen3",
	[OTH] = "oth" 
};

void (*IC_Func[LAST][OPS])(union DATA*, unsigned int) = {
	[SMU]	= { SMU_Read, SMU_Write },
	[FCH]	= { FCH_Read, FCH_WRITE },
	[UMC]	= { UMC_Read, NULL	},
	[PCI]	= { PCI_Read, PCI_Write },
	[BIOS]	= { BIOS_Read,BIOS_Write},
	[PM2]	= { PM2_Read, PM2_Write },
	[ZEN1]	= { NULL,	NULL	},
	[ZEN12] = { NULL,	NULL	},
	[ZEN2]	= { ZEN2_Read,	NULL	},
	[ZEN3]	= { ZEN3_Read,	NULL	},
	[OTH]	= { OTH_Read, NULL	}
};

void Help_Argument(void)
{
	enum IC ic;
	for (ic = SMU; ic < LAST; ic++) {
		printf(" %s", component[ic]);
	}
}

void Help_Usage(int rc, char *ctx)
{
	switch (rc) {
	case 6:
		printf("Sorry: %s Operation Unimplemented\n", ctx);
		break;
	case 5:
		printf("Prerequisite: IOPL Not Permitted\n\n");
		break;
	case 4:
		printf("Prerequisite: Missing root privileges\n");
		break;
	case 3:
		printf("Syntax: Invalid Hexadecimal Address\n"	\
			"\tExpected  <addr> like 0x1a2b3c4d\n"	\
			"\tOr"					\
			"\t  <addr> like bus:0x1-dev:0x2-fn:0x3-reg:0xff\n");
		break;
	case 2:
		printf("Syntax: '%s' Undefined component\n", ctx);
		break;
	case 1:
	default:
		printf( "Usage: %s <component> [ <addr> ]\n"	\
			"Where: <component> is one of {\n\t", ctx );
		Help_Argument();
		printf("\n}\n");
		break;
	}
}

int main(int argc, char *argv[])
{
	int rc = 0;
    if (argc < 2) {
	rc = 1;
	Help_Usage(rc, argv[0]);
    }
    else
    {
	enum IC ic;
	for (ic = SMU; ic < LAST; ic++)
	{
		const size_t nc = strlen(component[ic]);
	    if ((strlen(argv[1]) == nc) && !strncmp(component[ic], argv[1], nc))
	    {
		break;
	    }
	}
	if (ic == LAST) {
		rc = 2;
		Help_Usage(rc, argv[1]);
	}
	else
	{
		union DATA data = { .dword = 0 };
		unsigned int addr = 0x0;
		char tr = 0;
	    if (argc > 3)
	    {
		if (1 != sscanf(argv[3], "0x%x%c", &data.dword, &tr)) {
			rc = 3;
			Help_Usage(rc, argv[0]);
		}
	    }
	    if (argc > 2) {
	      if (1 != sscanf(argv[2], "0x%x%c", &addr, &tr))
	      {
		unsigned char _bus, _dev, _fn, _reg;
		if (4 != sscanf(argv[2], "bus:0x%x-dev:0x%x-fn:0x%x-reg:0x%x%c",
				&_bus, &_dev, &_fn, &_reg, &tr))
		{
			rc = 3;
			Help_Usage(rc, argv[0]);
		} else {
			addr = PCI_CONFIG_ADDRESS(_bus, _dev, _fn, _reg);
		}
	      }
	    }
	    if (rc == 0)
	    {
		uid_t uid = geteuid();
		if (uid != 0) {
			rc = 4;
			Help_Usage(rc, argv[0]);
		}
		else if (!iopl(3))
		{
		    if (rc == 0)
		    {
			const enum OP op = (argc > 3) ? WRITE : READ;
			char *what[OPS] = { "READ" , "WRITE" };

			if (IC_Func[ic][op] != NULL)
			{
				char binStr[64];

			  if (ic == UMC)
			  {
				IC_Func[ic][op](&data, addr);
			  }
			else
				if((ic == ZEN1)
				|| (ic == ZEN12)
				|| (ic == ZEN2)
				|| (ic == ZEN3))
			  {
				union DATA out[6];
				unsigned int idx;

				IC_Func[ic][op](out, addr);


				printf("[0x%08x] %s(%s) = 0x%08x (%u)\n", addr,
					what[op], component[ic]);

			    for (idx = 0; idx < 6; idx++)
			    {
				printf("\n0x%08x (%u)\n",
					out[idx].dword, out[idx].dword);

				Convert2Binary(out[idx].dword, binStr);
				PrettyBin(binStr);

				printf("\n");
			    }
			  }
			else if (argc > 2)
			  {
				IC_Func[ic][op](&data, addr);

				printf("[0x%08x] %s(%s) = 0x%08x (%u)\n", addr,
					what[op], component[ic],
					data.dword, data.dword);

				Convert2Binary(data.dword, binStr);
				PrettyBin(binStr);

				printf("\n");
			  } else {
				rc = 1;
				Help_Usage(rc, argv[0]);
			  }
			} else {
				rc = 6;
				Help_Usage(rc, what[op]);
			}
		    }
			iopl(0);
		} else {
			rc = 5;
			Help_Usage(rc, argv[0]);
		}
	    }
	}
    }
	return (rc);
}

