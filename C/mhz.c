/*
 * mhz.c by CyrIng
 *
 * Copyright (C) 2012-2020 CYRIL INGENIERIE
 * Licenses: GPL2
 */

/* Usage: mhz <Core#>
 *
 * Output: Print the [over]clock frequency in MHz of the core number in arg,
 *	where arg is greater or equal 0 and less than the max logical core.
 *	If no arg then print frequencies of all logical cores.
 *
 * Prerequisites: Two kernel modules:	modprobe -a msr dmi-sysfs
 *
 * Return: rc = 0 if OK, -1 if invalid core number.
 *
 * Build: cc mhz.c -o mhz
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

struct FEATURES /* CPUID Leaves. */
{
	struct
	{
		struct
		{
		unsigned
			Stepping        :  4-0,
			Model           :  8-4,
			Family          : 12-8,
			ProcType        : 14-12,
			Unused1         : 16-14,
			ExtModel        : 20-16,
			ExtFamily       : 28-20,
			Unused2         : 32-28;
		} EAX;
		struct
		{
		unsigned
			Brand_ID        :  8-0,
			CLFSH_Size      : 16-8,
			MaxThread       : 24-16,
			APIC_ID         : 32-24;
		} EBX;
		struct
		{
		unsigned
			SSE3    :  1-0,
			PCLMULDQ:  2-1,
			DTES64  :  3-2,
			MONITOR :  4-3,
			DS_CPL  :  5-4,
			VMX     :  6-5,
			SMX     :  7-6,
			EIST    :  8-7,
			TM2     :  9-8,
			SSSE3   : 10-9,
			CNXT_ID : 11-10,
			Unused1 : 12-11,
			FMA     : 13-12,
			CX16    : 14-13,
			xTPR    : 15-14,
			PDCM    : 16-15,
			Unused2 : 17-16,
			PCID    : 18-17,
			DCA     : 19-18,
			SSE41   : 20-19,
			SSE42   : 21-20,
			x2APIC  : 22-21,
			MOVBE   : 23-22,
			POPCNT  : 24-23,
			TSCDEAD : 25-24,
			AES     : 26-25,
			XSAVE   : 27-26,
			OSXSAVE : 28-27,
			AVX     : 29-28,
			F16C    : 30-29,
			RDRAND  : 31-30,
			Unused3 : 32-31;
		} ECX;
		struct
		{
		unsigned
			FPU     :  1-0,
			VME     :  2-1,
			DE      :  3-2,
			PSE     :  4-3,
			TSC     :  5-4,
			MSR     :  6-5,
			PAE     :  7-6,
			MCE     :  8-7,
			CX8     :  9-8,
			APIC    : 10-9,
			Unused1 : 11-10,
			SEP     : 12-11,
			MTRR    : 13-12,
			PGE     : 14-13,
			MCA     : 15-14,
			CMOV    : 16-15,
			PAT     : 17-16,
			PSE36   : 18-17,
			PSN     : 19-18,
			CLFSH   : 20-19,
			Unused2 : 21-20,
			DS      : 22-21,
			ACPI    : 23-22,
			MMX     : 24-23,
			FXSR    : 25-24,
			SSE     : 26-25,
			SSE2    : 27-26,
			SS      : 28-27,
			HTT     : 29-28,
			TM1     : 30-29,
			Unused3 : 31-30,
			PBE     : 32-31;
		} EDX;
	} Std;
	unsigned        ThreadCount;
	unsigned        LargestExtFunc;
	struct
	{
		struct
		{
			unsigned
			LAHFSAHF:  1-0,
			Unused1 : 32-1;
		} ECX;
		struct
		{
			unsigned
			Unused1 : 11-0,
			SYSCALL : 12-11,
			Unused2 : 20-12,
			XD_Bit  : 21-20,
			Unused3 : 26-21,
			PG_1GB  : 27-26,
			RDTSCP  : 28-27,
			Unused4 : 29-28,
			IA64    : 30-29,
			Unused5 : 32-30;
		} EDX;
	} Ext;
	char		BrandString[48+1];
};

/*
 * Intel® 64 and IA-32 Architectures Software Developer’s Manual
 * Vol. 3C § 34-1 - Table 34-2. IA-32 Architectural MSRs
*/

#define	IA32_PERF_STATUS	0x198

/*
 * System Management BIOS (SMBIOS) Reference Specification
 * Version: 2.7.1 § 7.5 - Table 20 - Processor Information (Type 4) Structure
*/

#define	SMBIOS_PROCINFO_STRUCTURE	4
#define	SMBIOS_PROCINFO_INSTANCE	0
#define	SMBIOS_PROCINFO_EXTCLK		0x12


int	Read_MSR(int cpu, off_t offset, unsigned long long *msr)
{
	char	pathname[1 + strlen("/dev/cpu/999/msr")];
	int	fd, rc = -1;

	sprintf(pathname, "/dev/cpu/%d/msr", cpu);
	if( (fd = open(pathname, O_RDONLY)) != -1 ) {
		ssize_t retval = pread(fd, msr, sizeof(*msr), offset);
		close(fd);
		rc = (retval != sizeof *msr) ? -1 : 0;
	}
	return (rc);
}

int	Read_SMBIOS(int structure, int instance, off_t offset,
		void *buf, size_t nbyte)
{
	char	pathname[1 +strlen("/sys/firmware/dmi/entries/999-99/raw")];
	int	fd, rc = -1;

	sprintf(pathname, "/sys/firmware/dmi/entries/%d-%d/raw",
		structure, instance);
	if( (fd = open(pathname, O_RDONLY)) != -1 ) {
		ssize_t retval = pread(fd, buf, nbyte, offset);
		close(fd);
		rc = (retval != nbyte) ? -1 : 0;
	}
	return (rc);
}

int	Get_Ratio(int cpu)
{
	unsigned long long msr = 0;

	if ( Read_MSR(cpu, IA32_PERF_STATUS, &msr) != -1) {
		return ((int) msr);
	} else {
		return (0);
	}
}

int	External_Clock()
{
	int	clock = 0;

	if( Read_SMBIOS(SMBIOS_PROCINFO_STRUCTURE, SMBIOS_PROCINFO_INSTANCE,
			SMBIOS_PROCINFO_EXTCLK, &clock, 1) != -1)
	{
		return (clock);
	} else {
		return (0);
	}
}

void	CPUID(struct FEATURES *features)
{
	__asm__ volatile
	(
		"movq	$0x1, %%rax	\n\t"
		"cpuid"
		: "=a"	(features->Std.EAX),
		  "=b"	(features->Std.EBX),
		  "=c"	(features->Std.ECX),
		  "=d"	(features->Std.EDX)
	);
	__asm__ volatile
	(
		"movq	$0x4, %%rax	\n\t"
		"xorq	%%rcx, %%rcx	\n\t"
		"cpuid			\n\t"
		"shr	$26, %%rax	\n\t"
		"and	$0x3f, %%rax	\n\t"
		"add	$1, %%rax"
		: "=a"	(features->ThreadCount)
	);
	__asm__ volatile
	(
		"movq	$0x80000000, %%rax	\n\t"
		"cpuid"
		: "=a"	(features->LargestExtFunc)
	);
	if (features->LargestExtFunc >= 0x80000008)
	{
		__asm__ volatile
		(
			"movq	$0x80000001, %%rax	\n\t"
			"cpuid"
			: "=c"	(features->Ext.ECX),
			  "=d"	(features->Ext.EDX)
		);

		struct
		{
			struct
			{
				unsigned char Chr[4];
			} EAX, EBX, ECX, EDX;
		} Brand;
		int ix, jx, px = 0;
		for (ix = 0; ix < 3; ix++)
		{
			__asm__ volatile
			(
				"cpuid;"
				: "=a"  (Brand.EAX),
				  "=b"  (Brand.EBX),
				  "=c"  (Brand.ECX),
				  "=d"  (Brand.EDX)
				: "a"   (0x80000002 + ix)
			);
			for (jx = 0; jx < 4; jx++, px++) {
				features->BrandString[px] = Brand.EAX.Chr[jx];
			}
			for (jx = 0; jx < 4; jx++, px++) {
				features->BrandString[px] = Brand.EBX.Chr[jx];
			}
			for (jx = 0; jx < 4; jx++, px++) {
				features->BrandString[px] = Brand.ECX.Chr[jx];
			}
			for (jx = 0; jx < 4; jx++, px++) {
				features->BrandString[px] = Brand.EDX.Chr[jx];
			}
		}
	}
}

int main(int argc, char *argv[])
{
	int	cpu, clock, rc = 0;
	struct	FEATURES features = {0};

	CPUID( &features );

	clock = External_Clock();

    if(argc > 1)
    {
	const char *help = "--help";
	if (!strncmp(argv[1], help, strlen(help))) {
		printf("Usage %s < Core# >\n", argv[0]);
	} else if (((cpu = atoi(argv[1])) >= 0)&&(cpu < features.ThreadCount)) {
		printf("%d\n", Get_Ratio(cpu) * clock);
	} else { rc = -1; }
    } else {
	printf("\n\"%s\"\t[%X%X_%X%X stepping %d]\n\n",
		features.BrandString,
		features.Std.EAX.ExtFamily,
		features.Std.EAX.Family,
		features.Std.EAX.ExtModel,
		features.Std.EAX.Model,
		features.Std.EAX.Stepping);

	for (cpu = 0; cpu < features.ThreadCount; cpu++) {
		printf( "[%3d] %4d%c", cpu, Get_Ratio(cpu) * clock,
			!((1 + cpu) % 4) ? '\n' : '\t');
	}
    }
	return (rc);
}

