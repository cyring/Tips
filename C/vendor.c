#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VENDOR_INTEL	"GenuineIntel"
#define VENDOR_AMD	"AuthenticAMD"
#define VENDOR_HYGON	"HygonGenuine"
#define VENDOR_KVM	"TCGTGTCGCGTC"
#define VENDOR_VBOX	"VBoxVBoxVBox"
#define VENDOR_KBOX	"KVMKM"
#define VENDOR_VMWARE	"VMwawarereVM"

#define CRC_INTEL	0x75a2ba39
#define CRC_AMD 	0x3485bbd3
#define CRC_HYGON	0x18044630
#define CRC_KVM 	0x0e8c8561
#define CRC_VBOX	0x5091f045
#define CRC_KBOX	0x02b76f04
#define CRC_VMWARE	0x2a974552

typedef struct
{
	union
	{
	    struct SIGNATURE
	    {
		unsigned int
		Stepping	:  4-0,
		Model		:  8-4,
		Family		: 12-8,
		ProcType	: 14-12,
		Unused1 	: 16-14,
		ExtModel	: 20-16,
		ExtFamily	: 28-20,
		Unused2 	: 32-28;
	    } EAX;
		unsigned int Signature;
	};
	struct CPUID_0x00000001_EBX
	{
		unsigned int
		Brand_ID	:  8-0,
		CLFSH_Size	: 16-8,
		Max_SMT_ID	: 24-16,
		Init_APIC_ID	: 32-24;
	} EBX;
	struct
	{
		unsigned int
		SSE3	:  1-0,  /* AMD Family 0Fh			*/
		PCLMULDQ:  2-1,
		DTES64	:  3-2,
		MONITOR :  4-3,
		DS_CPL	:  5-4,
		VMX	:  6-5,
		SMX	:  7-6,
		EIST	:  8-7,
		TM2	:  9-8,
		SSSE3	: 10-9,  /* AMD Family 0Fh			*/
		CNXT_ID : 11-10,
		SDBG	: 12-11, /* IA32_DEBUG_INTERFACE MSR support	*/
		FMA	: 13-12,
		CMPXCHG16:14-13,
		xTPR	: 15-14,
		PDCM	: 16-15,
		Unused1 : 17-16,
		PCID	: 18-17,
		DCA	: 19-18,
		SSE41	: 20-19,
		SSE42	: 21-20,
		x2APIC	: 22-21,
		MOVBE	: 23-22,
		POPCNT	: 24-23,
		TSCDEAD : 25-24,
		AES	: 26-25,
		XSAVE	: 27-26,
		OSXSAVE : 28-27,
		AVX	: 29-28,
		F16C	: 30-29,
		RDRAND	: 31-30,
		Hyperv	: 32-31;
	} ECX;
	struct
	{	/* Most common x86					*/
		unsigned int
		FPU	:  1-0,
		VME	:  2-1,
		DE	:  3-2,
		PSE	:  4-3,
		TSC	:  5-4,
		MSR	:  6-5,
		PAE	:  7-6,
		MCE	:  8-7,
		CMPXCHG8:  9-8,
		APIC	: 10-9,
		Unused1 : 11-10,
		SEP	: 12-11,
		MTRR	: 13-12,
		PGE	: 14-13,
		MCA	: 15-14,
		CMOV	: 16-15,
		PAT	: 17-16,
		PSE36	: 18-17,
		PSN	: 19-18, /* Intel Processor Serial Number	*/
		CLFLUSH : 20-19,
		Unused2 : 21-20,
		DS_PEBS : 22-21,
		ACPI	: 23-22,
		MMX	: 24-23,
		FXSR	: 25-24, /* FXSAVE and FXRSTOR instructions.	*/
		SSE	: 26-25,
		SSE2	: 27-26,
		SS	: 28-27, /* Intel				*/
		HTT	: 29-28,
		TM1	: 30-29, /* Intel				*/
		Unused3 : 31-30,
		PBE	: 32-31; /* Intel				*/
	} EDX;
} CPUID_0x00000001;

void FeaturesFromCPUID(CPUID_0x00000001 *pFeatures)
{
	__asm__ volatile
	(
		"movq	$0x1,  %%rax	\n\t"
		"xorq	%%rbx, %%rbx	\n\t"
		"xorq	%%rcx, %%rcx	\n\t"
		"xorq	%%rdx, %%rdx	\n\t"
		"cpuid			\n\t"
		"mov	%%eax, %0	\n\t"
		"mov	%%ebx, %1	\n\t"
		"mov	%%ecx, %2	\n\t"
		"mov	%%edx, %3"
		: "=r" (pFeatures->EAX),
		  "=r" (pFeatures->EBX),
		  "=r" (pFeatures->ECX),
		  "=r" (pFeatures->EDX)
		:
		: "%rax", "%rbx", "%rcx", "%rdx"
	);
}

unsigned int VendorFromCPUID( char *pString,	unsigned long leaf,
						unsigned long subLeaf )
{
	unsigned int eax = 0x0, ebx = 0x0, ecx = 0x0, edx = 0x0; /*DWORD Only!*/

	__asm__ volatile
	(
		"movq	%4, %%rax	\n\t"
		"movq	%5, %%rcx	\n\t"
		"xorq	%%rbx, %%rbx	\n\t"
		"xorq	%%rdx, %%rdx	\n\t"
		"cpuid			\n\t"
		"mov	%%eax, %0	\n\t"
		"mov	%%ebx, %1	\n\t"
		"mov	%%ecx, %2	\n\t"
		"mov	%%edx, %3"
		: "=r" (eax),
		  "=r" (ebx),
		  "=r" (ecx),
		  "=r" (edx)
		: "ir" (leaf),
		  "ir" (subLeaf)
		: "%rax", "%rbx", "%rcx", "%rdx"
	);
	pString[ 0] = ebx;
	pString[ 1] = (ebx >> 8);
	pString[ 2] = (ebx >> 16);
	pString[ 3] = (ebx >> 24);
	pString[ 4] = edx;
	pString[ 5] = (edx >> 8);
	pString[ 6] = (edx >> 16);
	pString[ 7] = (edx >> 24);
	pString[ 8] = ecx;
	pString[ 9] = (ecx >> 8);
	pString[10] = (ecx >> 16);
	pString[11] = (ecx >> 24);
	pString[12] = '\0';

	return (eax);
}

int main(int argc, char *argv[])
{
    if (argc > 2) {
	char *VendorID = calloc(12, 1);

	if (VendorID != NULL)
	{
		unsigned long leaf, subLeaf;
		unsigned int largestStdFunc = 0;

	    if ((1 == sscanf(argv[1], "%lx", &leaf))
	     && (1 == sscanf(argv[1], "%lx", &subLeaf)))
	    {
		CPUID_0x00000001 Features;

		struct {
			char		*ID;
			size_t		Len;
			unsigned int	CRC;
		} Table[] = {
		{ VENDOR_INTEL	, __builtin_strlen(VENDOR_INTEL), CRC_INTEL  },
		{ VENDOR_AMD	, __builtin_strlen(VENDOR_AMD)	, CRC_AMD    },
		{ VENDOR_HYGON	, __builtin_strlen(VENDOR_HYGON), CRC_HYGON  },
		{ VENDOR_KVM	, __builtin_strlen(VENDOR_KVM)	, CRC_KVM    },
		{ VENDOR_VBOX	, __builtin_strlen(VENDOR_VBOX) , CRC_VBOX   },
		{ VENDOR_KBOX	, __builtin_strlen(VENDOR_KBOX) , CRC_KBOX   },
		{ VENDOR_VMWARE , __builtin_strlen(VENDOR_VMWARE),CRC_VMWARE}};

		unsigned int VendorCRC, idx;
		const unsigned int Dim = sizeof(Table) / sizeof(Table[0]);

		FeaturesFromCPUID( &Features );

		largestStdFunc = VendorFromCPUID(VendorID, leaf, subLeaf);

		for (idx = 0; idx < Dim; idx++)
		{
			if (!strncmp(VendorID, Table[idx].ID, Table[idx].Len)) {
				break;
			}
		}
		if (idx < Dim) {
			printf( "Hypervisor[%d]"	\
				" [%s] (CRC:0x%x)"	\
				" (Last leaf=0x%x)\n",
				Features.ECX.Hyperv,
				Table[idx].ID, Table[idx].CRC,
				largestStdFunc );
		}
	    }
		free(VendorID);
		return (0);
	}
    } else {
		printf( "Usage: %s < EAX > < ECX >\n"	\
			"where: registers as hexa-value '0x'\n", argv[0]);
    }
	return (1);
}

