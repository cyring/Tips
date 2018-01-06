/*
	cc -Wall hasbug_cpu_insecure.c -o hasbug_cpu_insecure
*/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef unsigned long bool;

int printk(const char *fmt, ...)
{
	va_list args;
	int r;

	va_start(args, fmt);
	r = printf(fmt, args);
	va_end(args);

	return r;
}

// Source=/include/linux/stddef.h
enum {
	false	= 0,
	true	= 1
};

// Source=/include/uapi/asm-generic/errno-base.h
#define	EPERM		 1	/* Operation not permitted */
#define	ENOENT		 2	/* No such file or directory */
#define	ESRCH		 3	/* No such process */
#define	EINTR		 4	/* Interrupted system call */
#define	EIO		 5	/* I/O error */
#define	ENXIO		 6	/* No such device or address */
#define	E2BIG		 7	/* Argument list too long */
#define	ENOEXEC		 8	/* Exec format error */
#define	EBADF		 9	/* Bad file number */
#define	ECHILD		10	/* No child processes */
#define	EAGAIN		11	/* Try again */
#define	ENOMEM		12	/* Out of memory */
#define	EACCES		13	/* Permission denied */
#define	EFAULT		14	/* Bad address */
#define	ENOTBLK		15	/* Block device required */
#define	EBUSY		16	/* Device or resource busy */
#define	EEXIST		17	/* File exists */
#define	EXDEV		18	/* Cross-device link */
#define	ENODEV		19	/* No such device */
#define	ENOTDIR		20	/* Not a directory */
#define	EISDIR		21	/* Is a directory */
#define	EINVAL		22	/* Invalid argument */
#define	ENFILE		23	/* File table overflow */
#define	EMFILE		24	/* Too many open files */
#define	ENOTTY		25	/* Not a typewriter */
#define	ETXTBSY		26	/* Text file busy */
#define	EFBIG		27	/* File too large */
#define	ENOSPC		28	/* No space left on device */
#define	ESPIPE		29	/* Illegal seek */
#define	EROFS		30	/* Read-only file system */
#define	EMLINK		31	/* Too many links */
#define	EPIPE		32	/* Broken pipe */
#define	EDOM		33	/* Math argument out of domain of func */
#define	ERANGE		34	/* Math result not representable */

// Source=/include/linux/compiler.h
# define likely(x)	__builtin_expect(!!(x), 1)
# define unlikely(x)	__builtin_expect(!!(x), 0)

// Source=/tools/include/asm/bug.h
#define __WARN_printf(arg...)	do { fprintf(stderr, arg); } while (0)

#define WARN(condition, format...) ({		\
	int __ret_warn_on = !!(condition);	\
	if (unlikely(__ret_warn_on))		\
		__WARN_printf(format);		\
	unlikely(__ret_warn_on);		\
})

#define WARN_ON(condition) ({					\
	int __ret_warn_on = !!(condition);			\
	if (unlikely(__ret_warn_on))				\
		__WARN_printf("assertion failed at %s:%d\n",	\
				__FILE__, __LINE__);		\
	unlikely(__ret_warn_on);				\
})

// Source=/include/linux/kern_levels.h
#define KERN_SOH	"\001"		/* ASCII Start Of Header */
#define KERN_SOH_ASCII	'\001'

#define KERN_EMERG	KERN_SOH "0"	/* system is unusable */
#define KERN_ALERT	KERN_SOH "1"	/* action must be taken immediately */
#define KERN_CRIT	KERN_SOH "2"	/* critical conditions */
#define KERN_ERR	KERN_SOH "3"	/* error conditions */
#define KERN_WARNING	KERN_SOH "4"	/* warning conditions */
#define KERN_NOTICE	KERN_SOH "5"	/* normal but significant condition */
#define KERN_INFO	KERN_SOH "6"	/* informational */
#define KERN_DEBUG	KERN_SOH "7"	/* debug-level messages */

#define KERN_DEFAULT	KERN_SOH "d"	/* the default kernel loglevel */

// Source=/arch/x86/mm/pti.c
#define pr_fmt(fmt)     "Kernel/User page tables isolation: " fmt

// Source=/include/linux/printk.h
#define pr_err_once(fmt, ...)					\
	printk/*_once*/(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__)

#define pr_emerg(fmt, ...) \
	printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__)
#define pr_alert(fmt, ...) \
	printk(KERN_ALERT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_crit(fmt, ...) \
	printk(KERN_CRIT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_err(fmt, ...) \
	printk(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warning(fmt, ...) \
	printk(KERN_WARNING pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warn pr_warning
#define pr_notice(fmt, ...) \
	printk(KERN_NOTICE pr_fmt(fmt), ##__VA_ARGS__)
#define pr_info(fmt, ...) \
	printk(KERN_INFO pr_fmt(fmt), ##__VA_ARGS__)

// Source=/arch/x86/include/asm/cpufeatures.h
/* Intel-defined CPU features, CPUID level 0x00000001 (EDX), word 0 */
#define X86_FEATURE_FPU			( 0*32+ 0) /* Onboard FPU */
#define X86_FEATURE_VME			( 0*32+ 1) /* Virtual Mode Extensions */
#define X86_FEATURE_DE			( 0*32+ 2) /* Debugging Extensions */
#define X86_FEATURE_PSE			( 0*32+ 3) /* Page Size Extensions */
#define X86_FEATURE_TSC			( 0*32+ 4) /* Time Stamp Counter */
#define X86_FEATURE_MSR			( 0*32+ 5) /* Model-Specific Registers */
#define X86_FEATURE_PAE			( 0*32+ 6) /* Physical Address Extensions */
#define X86_FEATURE_MCE			( 0*32+ 7) /* Machine Check Exception */
#define X86_FEATURE_CX8			( 0*32+ 8) /* CMPXCHG8 instruction */
#define X86_FEATURE_APIC		( 0*32+ 9) /* Onboard APIC */
#define X86_FEATURE_SEP			( 0*32+11) /* SYSENTER/SYSEXIT */
#define X86_FEATURE_MTRR		( 0*32+12) /* Memory Type Range Registers */
#define X86_FEATURE_PGE			( 0*32+13) /* Page Global Enable */
#define X86_FEATURE_MCA			( 0*32+14) /* Machine Check Architecture */
#define X86_FEATURE_CMOV		( 0*32+15) /* CMOV instructions (plus FCMOVcc, FCOMI with FPU) */
#define X86_FEATURE_PAT			( 0*32+16) /* Page Attribute Table */
#define X86_FEATURE_PSE36		( 0*32+17) /* 36-bit PSEs */
#define X86_FEATURE_PN			( 0*32+18) /* Processor serial number */
#define X86_FEATURE_CLFLUSH		( 0*32+19) /* CLFLUSH instruction */
#define X86_FEATURE_DS			( 0*32+21) /* "dts" Debug Store */
#define X86_FEATURE_ACPI		( 0*32+22) /* ACPI via MSR */
#define X86_FEATURE_MMX			( 0*32+23) /* Multimedia Extensions */
#define X86_FEATURE_FXSR		( 0*32+24) /* FXSAVE/FXRSTOR, CR4.OSFXSR */
#define X86_FEATURE_XMM			( 0*32+25) /* "sse" */
#define X86_FEATURE_XMM2		( 0*32+26) /* "sse2" */
#define X86_FEATURE_SELFSNOOP		( 0*32+27) /* "ss" CPU self snoop */
#define X86_FEATURE_HT			( 0*32+28) /* Hyper-Threading */
#define X86_FEATURE_ACC			( 0*32+29) /* "tm" Automatic clock control */
#define X86_FEATURE_IA64		( 0*32+30) /* IA-64 processor */
#define X86_FEATURE_PBE			( 0*32+31) /* Pending Break Enable */

/* AMD-defined CPU features, CPUID level 0x80000001, word 1 */
/* Don't duplicate feature flags which are redundant with Intel! */
#define X86_FEATURE_SYSCALL		( 1*32+11) /* SYSCALL/SYSRET */
#define X86_FEATURE_MP			( 1*32+19) /* MP Capable */
#define X86_FEATURE_NX			( 1*32+20) /* Execute Disable */
#define X86_FEATURE_MMXEXT		( 1*32+22) /* AMD MMX extensions */
#define X86_FEATURE_FXSR_OPT		( 1*32+25) /* FXSAVE/FXRSTOR optimizations */
#define X86_FEATURE_GBPAGES		( 1*32+26) /* "pdpe1gb" GB pages */
#define X86_FEATURE_RDTSCP		( 1*32+27) /* RDTSCP */
#define X86_FEATURE_LM			( 1*32+29) /* Long Mode (x86-64, 64-bit support) */
#define X86_FEATURE_3DNOWEXT		( 1*32+30) /* AMD 3DNow extensions */
#define X86_FEATURE_3DNOW		( 1*32+31) /* 3DNow */
/* Transmeta-defined CPU features, CPUID level 0x80860001, word 2 */
#define X86_FEATURE_RECOVERY		( 2*32+ 0) /* CPU in recovery mode */
#define X86_FEATURE_LONGRUN		( 2*32+ 1) /* Longrun power control */
#define X86_FEATURE_LRTI		( 2*32+ 3) /* LongRun table interface */

/* Other features, Linux-defined mapping, word 3 */
/* This range is used for feature bits which conflict or are synthesized */
#define X86_FEATURE_CXMMX		( 3*32+ 0) /* Cyrix MMX extensions */
#define X86_FEATURE_K6_MTRR		( 3*32+ 1) /* AMD K6 nonstandard MTRRs */
#define X86_FEATURE_CYRIX_ARR		( 3*32+ 2) /* Cyrix ARRs (= MTRRs) */
#define X86_FEATURE_CENTAUR_MCR		( 3*32+ 3) /* Centaur MCRs (= MTRRs) */

/* CPU types for specific tunings: */
#define X86_FEATURE_K8			( 3*32+ 4) /* "" Opteron, Athlon64 */
#define X86_FEATURE_K7			( 3*32+ 5) /* "" Athlon */
#define X86_FEATURE_P3			( 3*32+ 6) /* "" P3 */
#define X86_FEATURE_P4			( 3*32+ 7) /* "" P4 */
#define X86_FEATURE_CONSTANT_TSC	( 3*32+ 8) /* TSC ticks at a constant rate */
#define X86_FEATURE_UP			( 3*32+ 9) /* SMP kernel running on UP */
#define X86_FEATURE_ART			( 3*32+10) /* Always running timer (ART) */
#define X86_FEATURE_ARCH_PERFMON	( 3*32+11) /* Intel Architectural PerfMon */
#define X86_FEATURE_PEBS		( 3*32+12) /* Precise-Event Based Sampling */
#define X86_FEATURE_BTS			( 3*32+13) /* Branch Trace Store */
#define X86_FEATURE_SYSCALL32		( 3*32+14) /* "" syscall in IA32 userspace */
#define X86_FEATURE_SYSENTER32		( 3*32+15) /* "" sysenter in IA32 userspace */
#define X86_FEATURE_REP_GOOD		( 3*32+16) /* REP microcode works well */
#define X86_FEATURE_MFENCE_RDTSC	( 3*32+17) /* "" MFENCE synchronizes RDTSC */
#define X86_FEATURE_LFENCE_RDTSC	( 3*32+18) /* "" LFENCE synchronizes RDTSC */
#define X86_FEATURE_ACC_POWER		( 3*32+19) /* AMD Accumulated Power Mechanism */
#define X86_FEATURE_NOPL		( 3*32+20) /* The NOPL (0F 1F) instructions */
#define X86_FEATURE_ALWAYS		( 3*32+21) /* "" Always-present feature */
#define X86_FEATURE_XTOPOLOGY		( 3*32+22) /* CPU topology enum extensions */
#define X86_FEATURE_TSC_RELIABLE	( 3*32+23) /* TSC is known to be reliable */
#define X86_FEATURE_NONSTOP_TSC		( 3*32+24) /* TSC does not stop in C states */
#define X86_FEATURE_CPUID		( 3*32+25) /* CPU has CPUID instruction itself */
#define X86_FEATURE_EXTD_APICID		( 3*32+26) /* Extended APICID (8 bits) */
#define X86_FEATURE_AMD_DCM		( 3*32+27) /* AMD multi-node processor */
#define X86_FEATURE_APERFMPERF		( 3*32+28) /* P-State hardware coordination feedback capability (APERF/MPERF MSRs) */
#define X86_FEATURE_NONSTOP_TSC_S3	( 3*32+30) /* TSC doesn't stop in S3 state */
#define X86_FEATURE_TSC_KNOWN_FREQ	( 3*32+31) /* TSC has known frequency */

/* Intel-defined CPU features, CPUID level 0x00000001 (ECX), word 4 */
#define X86_FEATURE_XMM3		( 4*32+ 0) /* "pni" SSE-3 */
#define X86_FEATURE_PCLMULQDQ		( 4*32+ 1) /* PCLMULQDQ instruction */
#define X86_FEATURE_DTES64		( 4*32+ 2) /* 64-bit Debug Store */
#define X86_FEATURE_MWAIT		( 4*32+ 3) /* "monitor" MONITOR/MWAIT support */
#define X86_FEATURE_DSCPL		( 4*32+ 4) /* "ds_cpl" CPL-qualified (filtered) Debug Store */
#define X86_FEATURE_VMX			( 4*32+ 5) /* Hardware virtualization */
#define X86_FEATURE_SMX			( 4*32+ 6) /* Safer Mode eXtensions */
#define X86_FEATURE_EST			( 4*32+ 7) /* Enhanced SpeedStep */
#define X86_FEATURE_TM2			( 4*32+ 8) /* Thermal Monitor 2 */
#define X86_FEATURE_SSSE3		( 4*32+ 9) /* Supplemental SSE-3 */
#define X86_FEATURE_CID			( 4*32+10) /* Context ID */
#define X86_FEATURE_SDBG		( 4*32+11) /* Silicon Debug */
#define X86_FEATURE_FMA			( 4*32+12) /* Fused multiply-add */
#define X86_FEATURE_CX16		( 4*32+13) /* CMPXCHG16B instruction */
#define X86_FEATURE_XTPR		( 4*32+14) /* Send Task Priority Messages */
#define X86_FEATURE_PDCM		( 4*32+15) /* Perf/Debug Capabilities MSR */
#define X86_FEATURE_PCID		( 4*32+17) /* Process Context Identifiers */
#define X86_FEATURE_DCA			( 4*32+18) /* Direct Cache Access */
#define X86_FEATURE_XMM4_1		( 4*32+19) /* "sse4_1" SSE-4.1 */
#define X86_FEATURE_XMM4_2		( 4*32+20) /* "sse4_2" SSE-4.2 */
#define X86_FEATURE_X2APIC		( 4*32+21) /* X2APIC */
#define X86_FEATURE_MOVBE		( 4*32+22) /* MOVBE instruction */
#define X86_FEATURE_POPCNT		( 4*32+23) /* POPCNT instruction */
#define X86_FEATURE_TSC_DEADLINE_TIMER	( 4*32+24) /* TSC deadline timer */
#define X86_FEATURE_AES			( 4*32+25) /* AES instructions */
#define X86_FEATURE_XSAVE		( 4*32+26) /* XSAVE/XRSTOR/XSETBV/XGETBV instructions */
#define X86_FEATURE_OSXSAVE		( 4*32+27) /* "" XSAVE instruction enabled in the OS */
#define X86_FEATURE_AVX			( 4*32+28) /* Advanced Vector Extensions */
#define X86_FEATURE_F16C		( 4*32+29) /* 16-bit FP conversions */
#define X86_FEATURE_RDRAND		( 4*32+30) /* RDRAND instruction */
#define X86_FEATURE_HYPERVISOR		( 4*32+31) /* Running on a hypervisor */

/* VIA/Cyrix/Centaur-defined CPU features, CPUID level 0xC0000001, word 5 */
#define X86_FEATURE_XSTORE		( 5*32+ 2) /* "rng" RNG present (xstore) */
#define X86_FEATURE_XSTORE_EN		( 5*32+ 3) /* "rng_en" RNG enabled */
#define X86_FEATURE_XCRYPT		( 5*32+ 6) /* "ace" on-CPU crypto (xcrypt) */
#define X86_FEATURE_XCRYPT_EN		( 5*32+ 7) /* "ace_en" on-CPU crypto enabled */
#define X86_FEATURE_ACE2		( 5*32+ 8) /* Advanced Cryptography Engine v2 */
#define X86_FEATURE_ACE2_EN		( 5*32+ 9) /* ACE v2 enabled */
#define X86_FEATURE_PHE			( 5*32+10) /* PadLock Hash Engine */
#define X86_FEATURE_PHE_EN		( 5*32+11) /* PHE enabled */
#define X86_FEATURE_PMM			( 5*32+12) /* PadLock Montgomery Multiplier */
#define X86_FEATURE_PMM_EN		( 5*32+13) /* PMM enabled */

/* More extended AMD flags: CPUID level 0x80000001, ECX, word 6 */
#define X86_FEATURE_LAHF_LM		( 6*32+ 0) /* LAHF/SAHF in long mode */
#define X86_FEATURE_CMP_LEGACY		( 6*32+ 1) /* If yes HyperThreading not valid */
#define X86_FEATURE_SVM			( 6*32+ 2) /* Secure Virtual Machine */
#define X86_FEATURE_EXTAPIC		( 6*32+ 3) /* Extended APIC space */
#define X86_FEATURE_CR8_LEGACY		( 6*32+ 4) /* CR8 in 32-bit mode */
#define X86_FEATURE_ABM			( 6*32+ 5) /* Advanced bit manipulation */
#define X86_FEATURE_SSE4A		( 6*32+ 6) /* SSE-4A */
#define X86_FEATURE_MISALIGNSSE		( 6*32+ 7) /* Misaligned SSE mode */
#define X86_FEATURE_3DNOWPREFETCH	( 6*32+ 8) /* 3DNow prefetch instructions */
#define X86_FEATURE_OSVW		( 6*32+ 9) /* OS Visible Workaround */
#define X86_FEATURE_IBS			( 6*32+10) /* Instruction Based Sampling */
#define X86_FEATURE_XOP			( 6*32+11) /* extended AVX instructions */
#define X86_FEATURE_SKINIT		( 6*32+12) /* SKINIT/STGI instructions */
#define X86_FEATURE_WDT			( 6*32+13) /* Watchdog timer */
#define X86_FEATURE_LWP			( 6*32+15) /* Light Weight Profiling */
#define X86_FEATURE_FMA4		( 6*32+16) /* 4 operands MAC instructions */
#define X86_FEATURE_TCE			( 6*32+17) /* Translation Cache Extension */
#define X86_FEATURE_NODEID_MSR		( 6*32+19) /* NodeId MSR */
#define X86_FEATURE_TBM			( 6*32+21) /* Trailing Bit Manipulations */
#define X86_FEATURE_TOPOEXT		( 6*32+22) /* Topology extensions CPUID leafs */
#define X86_FEATURE_PERFCTR_CORE	( 6*32+23) /* Core performance counter extensions */
#define X86_FEATURE_PERFCTR_NB		( 6*32+24) /* NB performance counter extensions */
#define X86_FEATURE_BPEXT		( 6*32+26) /* Data breakpoint extension */
#define X86_FEATURE_PTSC		( 6*32+27) /* Performance time-stamp counter */
#define X86_FEATURE_PERFCTR_LLC		( 6*32+28) /* Last Level Cache performance counter extensions */
#define X86_FEATURE_MWAITX		( 6*32+29) /* MWAIT extension (MONITORX/MWAITX instructions) */

/*
 * Auxiliary flags: Linux defined - For features scattered in various
 * CPUID levels like 0x6, 0xA etc, word 7.
 *
 * Reuse free bits when adding new feature flags!
 */
#define X86_FEATURE_RING3MWAIT		( 7*32+ 0) /* Ring 3 MONITOR/MWAIT instructions */
#define X86_FEATURE_CPUID_FAULT		( 7*32+ 1) /* Intel CPUID faulting */
#define X86_FEATURE_CPB			( 7*32+ 2) /* AMD Core Performance Boost */
#define X86_FEATURE_EPB			( 7*32+ 3) /* IA32_ENERGY_PERF_BIAS support */
#define X86_FEATURE_CAT_L3		( 7*32+ 4) /* Cache Allocation Technology L3 */
#define X86_FEATURE_CAT_L2		( 7*32+ 5) /* Cache Allocation Technology L2 */
#define X86_FEATURE_CDP_L3		( 7*32+ 6) /* Code and Data Prioritization L3 */
#define X86_FEATURE_INVPCID_SINGLE	( 7*32+ 7) /* Effectively INVPCID && CR4.PCIDE=1 */

#define X86_FEATURE_HW_PSTATE		( 7*32+ 8) /* AMD HW-PState */
#define X86_FEATURE_PROC_FEEDBACK	( 7*32+ 9) /* AMD ProcFeedbackInterface */
#define X86_FEATURE_SME			( 7*32+10) /* AMD Secure Memory Encryption */
#define X86_FEATURE_PTI			( 7*32+11) /* Kernel Page Table Isolation enabled */
#define X86_FEATURE_INTEL_PPIN		( 7*32+14) /* Intel Processor Inventory Number */
#define X86_FEATURE_INTEL_PT		( 7*32+15) /* Intel Processor Trace */
#define X86_FEATURE_AVX512_4VNNIW	( 7*32+16) /* AVX-512 Neural Network Instructions */
#define X86_FEATURE_AVX512_4FMAPS	( 7*32+17) /* AVX-512 Multiply Accumulation Single precision */

#define X86_FEATURE_MBA			( 7*32+18) /* Memory Bandwidth Allocation */

/* Virtualization flags: Linux defined, word 8 */
#define X86_FEATURE_TPR_SHADOW		( 8*32+ 0) /* Intel TPR Shadow */
#define X86_FEATURE_VNMI		( 8*32+ 1) /* Intel Virtual NMI */
#define X86_FEATURE_FLEXPRIORITY	( 8*32+ 2) /* Intel FlexPriority */
#define X86_FEATURE_EPT			( 8*32+ 3) /* Intel Extended Page Table */
#define X86_FEATURE_VPID		( 8*32+ 4) /* Intel Virtual Processor ID */

#define X86_FEATURE_VMMCALL		( 8*32+15) /* Prefer VMMCALL to VMCALL */
#define X86_FEATURE_XENPV		( 8*32+16) /* "" Xen paravirtual guest */


/* Intel-defined CPU features, CPUID level 0x00000007:0 (EBX), word 9 */
#define X86_FEATURE_FSGSBASE		( 9*32+ 0) /* RDFSBASE, WRFSBASE, RDGSBASE, WRGSBASE instructions*/
#define X86_FEATURE_TSC_ADJUST		( 9*32+ 1) /* TSC adjustment MSR 0x3B */
#define X86_FEATURE_BMI1		( 9*32+ 3) /* 1st group bit manipulation extensions */
#define X86_FEATURE_HLE			( 9*32+ 4) /* Hardware Lock Elision */
#define X86_FEATURE_AVX2		( 9*32+ 5) /* AVX2 instructions */
#define X86_FEATURE_SMEP		( 9*32+ 7) /* Supervisor Mode Execution Protection */
#define X86_FEATURE_BMI2		( 9*32+ 8) /* 2nd group bit manipulation extensions */
#define X86_FEATURE_ERMS		( 9*32+ 9) /* Enhanced REP MOVSB/STOSB instructions */
#define X86_FEATURE_INVPCID		( 9*32+10) /* Invalidate Processor Context ID */
#define X86_FEATURE_RTM			( 9*32+11) /* Restricted Transactional Memory */
#define X86_FEATURE_CQM			( 9*32+12) /* Cache QoS Monitoring */
#define X86_FEATURE_MPX			( 9*32+14) /* Memory Protection Extension */
#define X86_FEATURE_RDT_A		( 9*32+15) /* Resource Director Technology Allocation */
#define X86_FEATURE_AVX512F		( 9*32+16) /* AVX-512 Foundation */
#define X86_FEATURE_AVX512DQ		( 9*32+17) /* AVX-512 DQ (Double/Quad granular) Instructions */
#define X86_FEATURE_RDSEED		( 9*32+18) /* RDSEED instruction */
#define X86_FEATURE_ADX			( 9*32+19) /* ADCX and ADOX instructions */
#define X86_FEATURE_SMAP		( 9*32+20) /* Supervisor Mode Access Prevention */
#define X86_FEATURE_AVX512IFMA		( 9*32+21) /* AVX-512 Integer Fused Multiply-Add instructions */
#define X86_FEATURE_CLFLUSHOPT		( 9*32+23) /* CLFLUSHOPT instruction */
#define X86_FEATURE_CLWB		( 9*32+24) /* CLWB instruction */
#define X86_FEATURE_AVX512PF		( 9*32+26) /* AVX-512 Prefetch */
#define X86_FEATURE_AVX512ER		( 9*32+27) /* AVX-512 Exponential and Reciprocal */
#define X86_FEATURE_AVX512CD		( 9*32+28) /* AVX-512 Conflict Detection */
#define X86_FEATURE_SHA_NI		( 9*32+29) /* SHA1/SHA256 Instruction Extensions */
#define X86_FEATURE_AVX512BW		( 9*32+30) /* AVX-512 BW (Byte/Word granular) Instructions */
#define X86_FEATURE_AVX512VL		( 9*32+31) /* AVX-512 VL (128/256 Vector Length) Extensions */

/* Extended state features, CPUID level 0x0000000d:1 (EAX), word 10 */
#define X86_FEATURE_XSAVEOPT		(10*32+ 0) /* XSAVEOPT instruction */
#define X86_FEATURE_XSAVEC		(10*32+ 1) /* XSAVEC instruction */
#define X86_FEATURE_XGETBV1		(10*32+ 2) /* XGETBV with ECX = 1 instruction */
#define X86_FEATURE_XSAVES		(10*32+ 3) /* XSAVES/XRSTORS instructions */

/* Intel-defined CPU QoS Sub-leaf, CPUID level 0x0000000F:0 (EDX), word 11 */
#define X86_FEATURE_CQM_LLC		(11*32+ 1) /* LLC QoS if 1 */

/* Intel-defined CPU QoS Sub-leaf, CPUID level 0x0000000F:1 (EDX), word 12 */
#define X86_FEATURE_CQM_OCCUP_LLC	(12*32+ 0) /* LLC occupancy monitoring */
#define X86_FEATURE_CQM_MBM_TOTAL	(12*32+ 1) /* LLC Total MBM monitoring */
#define X86_FEATURE_CQM_MBM_LOCAL	(12*32+ 2) /* LLC Local MBM monitoring */

/* AMD-defined CPU features, CPUID level 0x80000008 (EBX), word 13 */
#define X86_FEATURE_CLZERO		(13*32+ 0) /* CLZERO instruction */
#define X86_FEATURE_IRPERF		(13*32+ 1) /* Instructions Retired Count */
#define X86_FEATURE_XSAVEERPTR		(13*32+ 2) /* Always save/restore FP error pointers */

/* Thermal and Power Management Leaf, CPUID level 0x00000006 (EAX), word 14 */
#define X86_FEATURE_DTHERM		(14*32+ 0) /* Digital Thermal Sensor */
#define X86_FEATURE_IDA			(14*32+ 1) /* Intel Dynamic Acceleration */
#define X86_FEATURE_ARAT		(14*32+ 2) /* Always Running APIC Timer */
#define X86_FEATURE_PLN			(14*32+ 4) /* Intel Power Limit Notification */
#define X86_FEATURE_PTS			(14*32+ 6) /* Intel Package Thermal Status */
#define X86_FEATURE_HWP			(14*32+ 7) /* Intel Hardware P-states */
#define X86_FEATURE_HWP_NOTIFY		(14*32+ 8) /* HWP Notification */
#define X86_FEATURE_HWP_ACT_WINDOW	(14*32+ 9) /* HWP Activity Window */
#define X86_FEATURE_HWP_EPP		(14*32+10) /* HWP Energy Perf. Preference */
#define X86_FEATURE_HWP_PKG_REQ		(14*32+11) /* HWP Package Level Request */

/* AMD SVM Feature Identification, CPUID level 0x8000000a (EDX), word 15 */
#define X86_FEATURE_NPT			(15*32+ 0) /* Nested Page Table support */
#define X86_FEATURE_LBRV		(15*32+ 1) /* LBR Virtualization support */
#define X86_FEATURE_SVML		(15*32+ 2) /* "svm_lock" SVM locking MSR */
#define X86_FEATURE_NRIPS		(15*32+ 3) /* "nrip_save" SVM next_rip save */
#define X86_FEATURE_TSCRATEMSR		(15*32+ 4) /* "tsc_scale" TSC scaling support */
#define X86_FEATURE_VMCBCLEAN		(15*32+ 5) /* "vmcb_clean" VMCB clean bits support */
#define X86_FEATURE_FLUSHBYASID		(15*32+ 6) /* flush-by-ASID support */
#define X86_FEATURE_DECODEASSISTS	(15*32+ 7) /* Decode Assists support */
#define X86_FEATURE_PAUSEFILTER		(15*32+10) /* filtered pause intercept */
#define X86_FEATURE_PFTHRESHOLD		(15*32+12) /* pause filter threshold */
#define X86_FEATURE_AVIC		(15*32+13) /* Virtual Interrupt Controller */
#define X86_FEATURE_V_VMSAVE_VMLOAD	(15*32+15) /* Virtual VMSAVE VMLOAD */
#define X86_FEATURE_VGIF		(15*32+16) /* Virtual GIF */

/* Intel-defined CPU features, CPUID level 0x00000007:0 (ECX), word 16 */
#define X86_FEATURE_AVX512VBMI		(16*32+ 1) /* AVX512 Vector Bit Manipulation instructions*/
#define X86_FEATURE_UMIP		(16*32+ 2) /* User Mode Instruction Protection */
#define X86_FEATURE_PKU			(16*32+ 3) /* Protection Keys for Userspace */
#define X86_FEATURE_OSPKE		(16*32+ 4) /* OS Protection Keys Enable */
#define X86_FEATURE_AVX512_VBMI2	(16*32+ 6) /* Additional AVX512 Vector Bit Manipulation Instructions */
#define X86_FEATURE_GFNI		(16*32+ 8) /* Galois Field New Instructions */
#define X86_FEATURE_VAES		(16*32+ 9) /* Vector AES */
#define X86_FEATURE_VPCLMULQDQ		(16*32+10) /* Carry-Less Multiplication Double Quadword */
#define X86_FEATURE_AVX512_VNNI		(16*32+11) /* Vector Neural Network Instructions */
#define X86_FEATURE_AVX512_BITALG	(16*32+12) /* Support for VPOPCNT[B,W] and VPSHUF-BITQMB instructions */
#define X86_FEATURE_AVX512_VPOPCNTDQ	(16*32+14) /* POPCNT for vectors of DW/QW */
#define X86_FEATURE_LA57		(16*32+16) /* 5-level page tables */
#define X86_FEATURE_RDPID		(16*32+22) /* RDPID instruction */

/* AMD-defined CPU features, CPUID level 0x80000007 (EBX), word 17 */
#define X86_FEATURE_OVERFLOW_RECOV	(17*32+ 0) /* MCA overflow recovery support */
#define X86_FEATURE_SUCCOR		(17*32+ 1) /* Uncorrectable error containment and recovery */
#define X86_FEATURE_SMCA		(17*32+ 3) /* Scalable MCA */

// Source=/include/asm-generic/bitsperlong.h
#ifdef CONFIG_64BIT
#define BITS_PER_LONG 64
#else
#define BITS_PER_LONG 32
#endif /* CONFIG_64BIT */

// Source=/arch/x86/include/asm/asm.h
# define _EXPAND_EXTABLE_HANDLE(x) #x
# define _ASM_EXTABLE_HANDLE(from, to, handler)			\
	" .pushsection \"__ex_table\",\"a\"\n"			\
	" .balign 4\n"						\
	" .long (" #from ") - .\n"				\
	" .long (" #to ") - .\n"				\
	" .long (" _EXPAND_EXTABLE_HANDLE(handler) ") - .\n"	\
	" .popsection\n"

# define _ASM_EXTABLE(from, to)					\
	_ASM_EXTABLE_HANDLE(from, to, ex_handler_default)

// source=/arch/x86/include/asm/asm.h
#ifdef __GCC_ASM_FLAG_OUTPUTS__
# define CC_SET(c) "\n\t/* output condition code " #c "*/\n"
# define CC_OUT(c) "=@cc" #c
#else
# define CC_SET(c) "\n\tset" #c " %[_cc_" #c "]\n"
# define CC_OUT(c) [_cc_ ## c] "=qm"
#endif

// Source=/arch/x86/include/asm/bitops.h
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 1)
/* Technically wrong, but this avoids compilation errors on some gcc
   versions. */
#define BITOP_ADDR(x) "=m" (*(volatile long *) (x))
#else
#define BITOP_ADDR(x) "+m" (*(volatile long *) (x))
#endif

#define ADDR				BITOP_ADDR(addr)

#if BITS_PER_LONG == 32
# define _BITOPS_LONG_SHIFT 5
#elif BITS_PER_LONG == 64
# define _BITOPS_LONG_SHIFT 6
#else
# error "Unexpected BITS_PER_LONG"
#endif

static __always_inline void __set_bit(long nr, volatile unsigned long *addr)
{
	asm volatile("bts %1,%0" : ADDR : "Ir" (nr) : "memory");
}

static __always_inline bool __test_and_set_bit(long nr, volatile unsigned long *addr)
{
	bool oldbit;

	asm("bts %2,%1"
	    CC_SET(c)
	    : CC_OUT(c) (oldbit), ADDR
	    : "Ir" (nr));
	return oldbit;
}

// Source=/include/uapi/linux/kernel.h
#define __KERNEL_DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
// Source=/include/linux/kernel.h
#define DIV_ROUND_UP __KERNEL_DIV_ROUND_UP

// Source=/include/linux/bitops.h
#define BIT(nr)			(1UL << (nr))
#define BIT_ULL(nr)		(1ULL << (nr))
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)	(1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)	((nr) / BITS_PER_LONG_LONG)
#define BITS_PER_BYTE		8
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

#define S8_C(x)  x
#define U8_C(x)  x ## U
#define S16_C(x) x
#define U16_C(x) x ## U
#define S32_C(x) x
#define U32_C(x) x ## U
#define S64_C(x) x ## LL
#define U64_C(x) x ## ULL

#define BIT_64(n)			(U64_C(1) << (n))

// Source=/include/linux/types.h
#define DECLARE_BITMAP(name,bits) \
	unsigned long name[BITS_TO_LONGS(bits)]

// Source=/tools/include/linux/irqflags.h
#define local_irq_save(flags) ((flags) = 0)
#define local_irq_restore(flags) ((void)(flags))

// Source=/include/asm-generic/bitops/atomic.h
#  define _atomic_spin_lock_irqsave(l,f) do { local_irq_save(f); } while (0)
#  define _atomic_spin_unlock_irqrestore(l,f) do { local_irq_restore(f); } while (0)

static inline void set_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long flags;

	_atomic_spin_lock_irqsave(p, flags);
	*p  |= mask;
	_atomic_spin_unlock_irqrestore(p, flags);
}

static inline void clear_bit(int nr, volatile unsigned long *addr)
{
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long flags;

	_atomic_spin_lock_irqsave(p, flags);
	*p &= ~mask;
	_atomic_spin_unlock_irqrestore(p, flags);
}

// Source=/arch/x86/include/asm/asm.h
#ifdef __GCC_ASM_FLAG_OUTPUTS__
# define CC_SET(c) "\n\t/* output condition code " #c "*/\n"
# define CC_OUT(c) "=@cc" #c
#else
# define CC_SET(c) "\n\tset" #c " %[_cc_" #c "]\n"
# define CC_OUT(c) [_cc_ ## c] "=qm"
#endif

static __always_inline bool constant_test_bit(long nr, const volatile unsigned long *addr)
{
	return ((1UL << (nr & (BITS_PER_LONG-1))) &
		(addr[nr >> _BITOPS_LONG_SHIFT])) != 0;
}

static __always_inline bool variable_test_bit(long nr, volatile const unsigned long *addr)
{
	bool oldbit;

	asm volatile("bt %2,%1"
		     CC_SET(c)
		     : CC_OUT(c) (oldbit)
		     : "m" (*(unsigned long *)addr), "Ir" (nr));

	return oldbit;
}

// Source=/arch/x86/boot/bitops.h
#define test_bit(nr,addr) \
(__builtin_constant_p(nr) ? \
 constant_test_bit((nr),(addr)) : \
 variable_test_bit((nr),(addr)))

// Source=/arch/x86/include/asm/required-features.h
#ifndef CONFIG_MATH_EMULATION
# define NEED_FPU	(1<<(X86_FEATURE_FPU & 31))
#else
# define NEED_FPU	0
#endif

#if defined(CONFIG_X86_PAE) || defined(CONFIG_X86_64)
# define NEED_PAE	(1<<(X86_FEATURE_PAE & 31))
#else
# define NEED_PAE	0
#endif

#ifdef CONFIG_X86_CMPXCHG64
# define NEED_CX8	(1<<(X86_FEATURE_CX8 & 31))
#else
# define NEED_CX8	0
#endif

#if defined(CONFIG_X86_CMOV) || defined(CONFIG_X86_64)
# define NEED_CMOV	(1<<(X86_FEATURE_CMOV & 31))
#else
# define NEED_CMOV	0
#endif

#ifdef CONFIG_X86_USE_3DNOW
# define NEED_3DNOW	(1<<(X86_FEATURE_3DNOW & 31))
#else
# define NEED_3DNOW	0
#endif

#if defined(CONFIG_X86_P6_NOP) || defined(CONFIG_X86_64)
# define NEED_NOPL	(1<<(X86_FEATURE_NOPL & 31))
#else
# define NEED_NOPL	0
#endif

#ifdef CONFIG_MATOM
# define NEED_MOVBE	(1<<(X86_FEATURE_MOVBE & 31))
#else
# define NEED_MOVBE	0
#endif

#ifdef CONFIG_X86_5LEVEL
# define NEED_LA57	(1<<(X86_FEATURE_LA57 & 31))
#else
# define NEED_LA57	0
#endif

#ifdef CONFIG_X86_64
#ifdef CONFIG_PARAVIRT
/* Paravirtualized systems may not have PSE or PGE available */
#define NEED_PSE	0
#define NEED_PGE	0
#else
#define NEED_PSE	(1<<(X86_FEATURE_PSE) & 31)
#define NEED_PGE	(1<<(X86_FEATURE_PGE) & 31)
#endif
#define NEED_MSR	(1<<(X86_FEATURE_MSR & 31))
#define NEED_FXSR	(1<<(X86_FEATURE_FXSR & 31))
#define NEED_XMM	(1<<(X86_FEATURE_XMM & 31))
#define NEED_XMM2	(1<<(X86_FEATURE_XMM2 & 31))
#define NEED_LM		(1<<(X86_FEATURE_LM & 31))
#else
#define NEED_PSE	0
#define NEED_MSR	0
#define NEED_PGE	0
#define NEED_FXSR	0
#define NEED_XMM	0
#define NEED_XMM2	0
#define NEED_LM		0
#endif

#define REQUIRED_MASK0	(NEED_FPU|NEED_PSE|NEED_MSR|NEED_PAE|\
			 NEED_CX8|NEED_PGE|NEED_FXSR|NEED_CMOV|\
			 NEED_XMM|NEED_XMM2)
#define SSE_MASK	(NEED_XMM|NEED_XMM2)

#define REQUIRED_MASK1	(NEED_LM|NEED_3DNOW)

#define REQUIRED_MASK2	0
#define REQUIRED_MASK3	(NEED_NOPL)
#define REQUIRED_MASK4	(NEED_MOVBE)
#define REQUIRED_MASK5	0
#define REQUIRED_MASK6	0
#define REQUIRED_MASK7	0
#define REQUIRED_MASK8	0
#define REQUIRED_MASK9	0
#define REQUIRED_MASK10	0
#define REQUIRED_MASK11	0
#define REQUIRED_MASK12	0
#define REQUIRED_MASK13	0
#define REQUIRED_MASK14	0
#define REQUIRED_MASK15	0
#define REQUIRED_MASK16	(NEED_LA57)
#define REQUIRED_MASK17	0
#define REQUIRED_MASK_CHECK BUILD_BUG_ON_ZERO(NCAPINTS != 18)

// Source=/include/linux/build_bug.h
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:(-!!(e)); }))

// Source=/arch/x86/include/asm/cpufeature.h
#define NCAPINTS			18	   /* N 32-bit words worth of info */
#define NBUGINTS			1	   /* N 32-bit bug flags */

#define CHECK_BIT_IN_MASK_WORD(maskname, word, bit)	\
	(((bit)>>5)==(word) && (1UL<<((bit)&31) & maskname##word ))

#define REQUIRED_MASK_BIT_SET(feature_bit)		\
	 ( CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK,  0, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK,  1, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK,  2, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK,  3, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK,  4, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK,  5, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK,  6, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK,  7, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK,  8, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK,  9, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK, 10, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK, 11, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK, 12, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK, 13, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK, 14, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK, 15, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK, 16, feature_bit) ||	\
	   CHECK_BIT_IN_MASK_WORD(REQUIRED_MASK, 17, feature_bit) ||	\
	   REQUIRED_MASK_CHECK					  ||	\
	   BUILD_BUG_ON_ZERO(NCAPINTS != 18))

/*
 * BUG word(s)
 */
#define X86_BUG(x)			(NCAPINTS*32 + (x))

#define X86_BUG_CPU_INSECURE		X86_BUG(14) /* CPU is insecure and needs kernel page table isolation */

#define test_cpu_cap(c, bit)						\
	 test_bit(bit, (unsigned long *)((c)->x86_capability))

#define cpu_has(c, bit)							\
	(__builtin_constant_p(bit) && REQUIRED_MASK_BIT_SET(bit) ? 1 :	\
	 test_cpu_cap(c, bit))

#define cpu_has_bug(c, bit)		cpu_has(c, (bit))

#define setup_force_cpu_cap(bit) do { \
	set_cpu_cap(&boot_cpu_data, bit);	\
	set_bit(bit, (unsigned long *)cpu_caps_set);	\
} while (0)

#define setup_force_cpu_bug(bit) setup_force_cpu_cap(bit)

// Source=/arch/x86/include/asm/cpufeatures.h
#define X86_FEATURE_ALWAYS		( 3*32+21) /* "" Always-present feature */
/* ... */
#define X86_FEATURE_CPUID		( 3*32+25) /* CPU has CPUID instruction itself */

// Source=/arch/x86/boot/tools/build.c
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

// Source=/include/uapi/asm-generic/int-l64.h
typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

typedef __signed__ long __s64;
typedef unsigned long __u64;

// Source=/include/asm-generic/int-ll64.h
typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

// Source=/arch/x86/include/asm/processor.h
enum cpuid_regs_idx {
	CPUID_EAX = 0,
	CPUID_EBX,
	CPUID_ECX,
	CPUID_EDX,
};

struct cpuinfo_x86 {
	__u8			x86;		/* CPU family */
	__u8			x86_vendor;	/* CPU vendor */
	__u8			x86_model;
	__u8			x86_mask;
#ifdef CONFIG_X86_64
	/* Number of 4K pages in DTLB/ITLB combined(in pages): */
	int			x86_tlbsize;
#endif
	__u8			x86_virt_bits;
	__u8			x86_phys_bits;
	/* CPUID returned core id bits: */
	__u8			x86_coreid_bits;
	__u8			cu_id;
	/* Max extended CPUID function supported: */
	__u32			extended_cpuid_level;
	/* Maximum supported CPUID level, -1=no CPUID: */
	int			cpuid_level;
	__u32			x86_capability[NCAPINTS + NBUGINTS];
	char			x86_vendor_id[16];
	char			x86_model_id[64];
	/* in KB - valid for CPUS which support this call: */
	int			x86_cache_size;
	int			x86_cache_alignment;	/* In bytes */
	/* Cache QoS architectural values: */
	int			x86_cache_max_rmid;	/* max index */
	int			x86_cache_occ_scale;	/* scale to bytes */
	int			x86_power;
	unsigned long		loops_per_jiffy;
	/* cpuid returned max cores value: */
	u16			 x86_max_cores;
	u16			apicid;
	u16			initial_apicid;
	u16			x86_clflush_size;
	/* number of cores as seen by the OS: */
	u16			booted_cores;
	/* Physical processor id: */
	u16			phys_proc_id;
	/* Logical processor id: */
	u16			logical_proc_id;
	/* Core id: */
	u16			cpu_core_id;
	/* Index into per_cpu list: */
	u16			cpu_index;
	u32			microcode;
} __randomize_layout;

static inline int have_cpuid_p(void)
{
	return 1;
}

static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
				unsigned int *ecx, unsigned int *edx)
{
	/* ecx is often an input as well as an output. */
	asm volatile("cpuid"
	    : "=a" (*eax),
	      "=b" (*ebx),
	      "=c" (*ecx),
	      "=d" (*edx)
	    : "0" (*eax), "2" (*ecx)
	    : "memory");
}

#define __cpuid			native_cpuid

static inline void cpuid(unsigned int op,
			 unsigned int *eax, unsigned int *ebx,
			 unsigned int *ecx, unsigned int *edx)
{
	*eax = op;
	*ecx = 0;
	__cpuid(eax, ebx, ecx, edx);
}

static inline void cpuid_count(unsigned int op, int count,
			       unsigned int *eax, unsigned int *ebx,
			       unsigned int *ecx, unsigned int *edx)
{
	*eax = op;
	*ecx = count;
	__cpuid(eax, ebx, ecx, edx);
}

// Source=/arch/x86/kernel/setup.c
struct cpuinfo_x86 boot_cpu_data /*__read_mostly*/;

// Source=/arch/x86/include/asm/processor.h
static inline unsigned int cpuid_eax(unsigned int op)
{
	unsigned int eax, ebx, ecx, edx;

	cpuid(op, &eax, &ebx, &ecx, &edx);

	return eax;
}

static inline unsigned int cpuid_ebx(unsigned int op)
{
	unsigned int eax, ebx, ecx, edx;

	cpuid(op, &eax, &ebx, &ecx, &edx);

	return ebx;
}

static inline unsigned int cpuid_ecx(unsigned int op)
{
	unsigned int eax, ebx, ecx, edx;

	cpuid(op, &eax, &ebx, &ecx, &edx);

	return ecx;
}

static inline unsigned int cpuid_edx(unsigned int op)
{
	unsigned int eax, ebx, ecx, edx;

	cpuid(op, &eax, &ebx, &ecx, &edx);

	return edx;
}

// Source=/arch/x86/lib/cpu.c
unsigned int x86_family(unsigned int sig)
{
	unsigned int x86;

	x86 = (sig >> 8) & 0xf;

	if (x86 == 0xf)
		x86 += (sig >> 20) & 0xff;

	return x86;
}

unsigned int x86_model(unsigned int sig)
{
	unsigned int fam, model;

	 fam = x86_family(sig);

	model = (sig >> 4) & 0xf;

	if (fam >= 0x6)
		model += ((sig >> 16) & 0xf) << 4;

	return model;
}

unsigned int x86_stepping(unsigned int sig)
{
	return sig & 0xf;
}

#define X86_VENDOR_INTEL	0
#define X86_VENDOR_CYRIX	1
#define X86_VENDOR_AMD		2
#define X86_VENDOR_UMC		3
#define X86_VENDOR_CENTAUR	5
#define X86_VENDOR_TRANSMETA	7
#define X86_VENDOR_NSC		8
#define X86_VENDOR_NUM		9

#define X86_VENDOR_UNKNOWN	0xff

// Source=/arch/x86/include/asm/cpufeature.h
enum cpuid_leafs
{
	CPUID_1_EDX		= 0,
	CPUID_8000_0001_EDX,
	CPUID_8086_0001_EDX,
	CPUID_LNX_1,
	CPUID_1_ECX,
	CPUID_C000_0001_EDX,
	CPUID_8000_0001_ECX,
	CPUID_LNX_2,
	CPUID_LNX_3,
	CPUID_7_0_EBX,
	CPUID_D_1_EAX,
	CPUID_F_0_EDX,
	CPUID_F_1_EDX,
	CPUID_8000_0008_EBX,
	CPUID_6_EAX,
	CPUID_8000_000A_EDX,
	CPUID_7_ECX,
	CPUID_8000_0007_EBX,
};

#define X86_FEATURE_CQM_LLC		(11*32+ 1) /* LLC QoS if 1 */

#define X86_FEATURE_CQM_OCCUP_LLC	(12*32+ 0) /* LLC occupancy monitoring */
#define X86_FEATURE_CQM_MBM_TOTAL	(12*32+ 1) /* LLC Total MBM monitoring */
#define X86_FEATURE_CQM_MBM_LOCAL	(12*32+ 2) /* LLC Local MBM monitoring */

#define set_cpu_cap(c, bit)	set_bit(bit, (unsigned long *)((c)->x86_capability))

#define setup_force_cpu_cap(bit) do { \
	set_cpu_cap(&boot_cpu_data, bit);	\
	set_bit(bit, (unsigned long *)cpu_caps_set);	\
} while (0)

#define X86_CAP_FMT "%d:%d"
#define x86_cap_flag(flag) ((flag) >> 5), ((flag) & 31)

// Source=/arch/x86/kernel/cpu/cpu.h
struct cpu_dev {
	const char	*c_vendor;

	/* some have two possibilities for cpuid string */
	const char	*c_ident[2];

	void            (*c_early_init)(struct cpuinfo_x86 *);
	void		(*c_bsp_init)(struct cpuinfo_x86 *);
	void		(*c_init)(struct cpuinfo_x86 *);
	void		(*c_identify)(struct cpuinfo_x86 *);
	void		(*c_detect_tlb)(struct cpuinfo_x86 *);
	void		(*c_bsp_resume)(struct cpuinfo_x86 *);
	int		c_x86_vendor;
#ifdef CONFIG_X86_32
	/* Optional vendor specific routine to obtain the cache size. */
	unsigned int	(*legacy_cache_size)(struct cpuinfo_x86 *,
					     unsigned int);

	/* Family/stepping-based lookup table for model names. */
	struct legacy_cpu_model_info {
		int		family;
		const char	*model_names[16];
	}		legacy_models[5];
#endif
};

// Source=/arch/x86/kernel/cpu/common.c
__u32 cpu_caps_cleared[NCAPINTS + NBUGINTS];
__u32 cpu_caps_set[NCAPINTS + NBUGINTS];

// Source=/arch/x86/kernel/cpu/cpuid-deps.c
/*#ifdef CONFIG_X86_32*/
struct cpuid_dep {
	unsigned int	feature;
	unsigned int	depends;
};

/*
 * Table of CPUID features that depend on others.
 *
 * This only includes dependencies that can be usefully disabled, not
 * features part of the base set (like FPU).
 *
 * Note this all is not __init / __initdata because it can be
 * called from cpu hotplug. It shouldn't do anything in this case,
 * but it's difficult to tell that to the init reference checker.
 */
const static struct cpuid_dep cpuid_deps[] = {
	{ X86_FEATURE_XSAVEOPT,		X86_FEATURE_XSAVE     },
	{ X86_FEATURE_XSAVEC,		X86_FEATURE_XSAVE     },
	{ X86_FEATURE_XSAVES,		X86_FEATURE_XSAVE     },
	{ X86_FEATURE_AVX,		X86_FEATURE_XSAVE     },
	{ X86_FEATURE_PKU,		X86_FEATURE_XSAVE     },
	{ X86_FEATURE_MPX,		X86_FEATURE_XSAVE     },
	{ X86_FEATURE_XGETBV1,		X86_FEATURE_XSAVE     },
	{ X86_FEATURE_FXSR_OPT,		X86_FEATURE_FXSR      },
	{ X86_FEATURE_XMM,		X86_FEATURE_FXSR      },
	{ X86_FEATURE_XMM2,		X86_FEATURE_XMM       },
	{ X86_FEATURE_XMM3,		X86_FEATURE_XMM2      },
	{ X86_FEATURE_XMM4_1,		X86_FEATURE_XMM2      },
	{ X86_FEATURE_XMM4_2,		X86_FEATURE_XMM2      },
	{ X86_FEATURE_XMM3,		X86_FEATURE_XMM2      },
	{ X86_FEATURE_PCLMULQDQ,	X86_FEATURE_XMM2      },
	{ X86_FEATURE_SSSE3,		X86_FEATURE_XMM2,     },
	{ X86_FEATURE_F16C,		X86_FEATURE_XMM2,     },
	{ X86_FEATURE_AES,		X86_FEATURE_XMM2      },
	{ X86_FEATURE_SHA_NI,		X86_FEATURE_XMM2      },
	{ X86_FEATURE_FMA,		X86_FEATURE_AVX       },
	{ X86_FEATURE_AVX2,		X86_FEATURE_AVX,      },
	{ X86_FEATURE_AVX512F,		X86_FEATURE_AVX,      },
	{ X86_FEATURE_AVX512IFMA,	X86_FEATURE_AVX512F   },
	{ X86_FEATURE_AVX512PF,		X86_FEATURE_AVX512F   },
	{ X86_FEATURE_AVX512ER,		X86_FEATURE_AVX512F   },
	{ X86_FEATURE_AVX512CD,		X86_FEATURE_AVX512F   },
	{ X86_FEATURE_AVX512DQ,		X86_FEATURE_AVX512F   },
	{ X86_FEATURE_AVX512BW,		X86_FEATURE_AVX512F   },
	{ X86_FEATURE_AVX512VL,		X86_FEATURE_AVX512F   },
	{ X86_FEATURE_AVX512VBMI,	X86_FEATURE_AVX512F   },
	{ X86_FEATURE_AVX512_VBMI2,	X86_FEATURE_AVX512VL  },
	{ X86_FEATURE_GFNI,		X86_FEATURE_AVX512VL  },
	{ X86_FEATURE_VAES,		X86_FEATURE_AVX512VL  },
	{ X86_FEATURE_VPCLMULQDQ,	X86_FEATURE_AVX512VL  },
	{ X86_FEATURE_AVX512_VNNI,	X86_FEATURE_AVX512VL  },
	{ X86_FEATURE_AVX512_BITALG,	X86_FEATURE_AVX512VL  },
	{ X86_FEATURE_AVX512_4VNNIW,	X86_FEATURE_AVX512F   },
	{ X86_FEATURE_AVX512_4FMAPS,	X86_FEATURE_AVX512F   },
	{ X86_FEATURE_AVX512_VPOPCNTDQ, X86_FEATURE_AVX512F   },
	{}
};

/* Take the capabilities and the BUG bits into account */
#define MAX_FEATURE_BITS ((NCAPINTS + NBUGINTS) * sizeof(u32) * 8)

void clear_cpu_cap(struct cpuinfo_x86 *c, unsigned int feature) ;

static inline void clear_feature(struct cpuinfo_x86 *c, unsigned int feature)
{
	/*
	 * Note: This could use the non atomic __*_bit() variants, but the
	 * rest of the cpufeature code uses atomics as well, so keep it for
	 * consistency. Cleanup all of it separately.
	 */
	if (!c) {
		clear_cpu_cap(&boot_cpu_data, feature);
		set_bit(feature, (unsigned long *)cpu_caps_cleared);
	} else {
		clear_bit(feature, (unsigned long *)c->x86_capability);
	}
}

static void do_clear_cpu_cap(struct cpuinfo_x86 *c, unsigned int feature)
{
	DECLARE_BITMAP(disable, MAX_FEATURE_BITS);
	const struct cpuid_dep *d;
	bool changed;

	if (WARN_ON(feature >= MAX_FEATURE_BITS))
		return;

	clear_feature(c, feature);

	/* Collect all features to disable, handling dependencies */
	memset(disable, 0, sizeof(disable));
	__set_bit(feature, disable);

	/* Loop until we get a stable state. */
	do {
		changed = false;
		for (d = cpuid_deps; d->feature; d++) {
			if (!test_bit(d->depends, disable))
				continue;
			if (__test_and_set_bit(d->feature, disable))
				continue;

			changed = true;
			clear_feature(c, d->feature);
		}
	} while (changed);
}

void clear_cpu_cap(struct cpuinfo_x86 *c, unsigned int feature)
{
	do_clear_cpu_cap(c, feature);
}

void setup_clear_cpu_cap(unsigned int feature)
{
	do_clear_cpu_cap(NULL, feature);
}
/*#endif*/

// Source=/arch/x86/kernel/cpu/common.c
struct cpuid_dependent_feature {
	u32 feature;
	u32 level;
};

static const struct cpuid_dependent_feature
cpuid_dependent_features[] = {
	{ X86_FEATURE_MWAIT,		0x00000005 },
	{ X86_FEATURE_DCA,		0x00000009 },
	{ X86_FEATURE_XSAVE,		0x0000000d },
	{ 0, 0 }
};

static void filter_cpuid_features(struct cpuinfo_x86 *c, bool warn)
{
	const struct cpuid_dependent_feature *df;

	for (df = cpuid_dependent_features; df->feature; df++) {

		if (!cpu_has(c, df->feature))
			continue;
		/*
		 * Note: cpuid_level is set to -1 if unavailable, but
		 * extended_extended_level is set to 0 if unavailable
		 * and the legitimate extended levels are all negative
		 * when signed; hence the weird messing around with
		 * signs here...
		 */
		if (!((s32)df->level < 0 ?
		     (u32)df->level > (u32)c->extended_cpuid_level :
		     (s32)df->level > (s32)c->cpuid_level))
			continue;

		clear_cpu_cap(c, df->feature);
		if (!warn)
			continue;

		pr_warn("CPU: CPU feature " X86_CAP_FMT " disabled, no CPUID level 0x%x\n",
			x86_cap_flag(df->feature), df->level);
	}
}

static void identify_cpu_without_cpuid(struct cpuinfo_x86 *c)
{
#ifdef CONFIG_X86_32
/* ... */
#endif
}

static void default_init(struct cpuinfo_x86 *c)
{
#ifdef CONFIG_X86_64
	cpu_detect_cache_sizes(c);
#else
	/* Not much we can do here... */
	/* Check if at least it has cpuid */
	if (c->cpuid_level == -1) {
		/* No cpuid. It must be an ancient CPU */
		if (c->x86 == 4)
			strcpy(c->x86_model_id, "486");
		else if (c->x86 == 3)
			strcpy(c->x86_model_id, "386");
	}
#endif
}

static const struct cpu_dev default_cpu = {
	.c_init		= default_init,
	.c_vendor	= "Unknown",
	.c_x86_vendor	= X86_VENDOR_UNKNOWN,
};

static const struct cpu_dev *this_cpu = &default_cpu;

// Source=/arch/x86/kernel/cpu/scattered.c
struct cpuid_bit {
	u16 feature;
	u8 reg;
	u8 bit;
	u32 level;
	u32 sub_leaf;
};

static const struct cpuid_bit cpuid_bits[] = {
	{ X86_FEATURE_APERFMPERF,       CPUID_ECX,  0, 0x00000006, 0 },
	{ X86_FEATURE_EPB,		CPUID_ECX,  3, 0x00000006, 0 },
	{ X86_FEATURE_INTEL_PT,		CPUID_EBX, 25, 0x00000007, 0 },
	{ X86_FEATURE_AVX512_4VNNIW,    CPUID_EDX,  2, 0x00000007, 0 },
	{ X86_FEATURE_AVX512_4FMAPS,    CPUID_EDX,  3, 0x00000007, 0 },
	{ X86_FEATURE_CAT_L3,		CPUID_EBX,  1, 0x00000010, 0 },
	{ X86_FEATURE_CAT_L2,		CPUID_EBX,  2, 0x00000010, 0 },
	{ X86_FEATURE_CDP_L3,		CPUID_ECX,  2, 0x00000010, 1 },
	{ X86_FEATURE_MBA,		CPUID_EBX,  3, 0x00000010, 0 },
	{ X86_FEATURE_HW_PSTATE,	CPUID_EDX,  7, 0x80000007, 0 },
	{ X86_FEATURE_CPB,		CPUID_EDX,  9, 0x80000007, 0 },
	{ X86_FEATURE_PROC_FEEDBACK,    CPUID_EDX, 11, 0x80000007, 0 },
	{ X86_FEATURE_SME,		CPUID_EAX,  0, 0x8000001f, 0 },
	{ 0, 0, 0, 0, 0 }
};

void init_scattered_cpuid_features(struct cpuinfo_x86 *c)
{
	u32 max_level;
	u32 regs[4];
	const struct cpuid_bit *cb;

	for (cb = cpuid_bits; cb->feature; cb++) {

		/* Verify that the level is valid */
		max_level = cpuid_eax(cb->level & 0xffff0000);
		if (max_level < cb->level ||
		    max_level > (cb->level | 0xffff))
			continue;

		cpuid_count(cb->level, cb->sub_leaf, &regs[CPUID_EAX],
			    &regs[CPUID_EBX], &regs[CPUID_ECX],
			    &regs[CPUID_EDX]);

		if (regs[cb->reg] & (1 << cb->bit))
			set_cpu_cap(c, cb->feature);
	}
}

// Source=/arch/x86/kernel/cpu/common.c
static void apply_forced_caps(struct cpuinfo_x86 *c)
{
	int i;

	for (i = 0; i < NCAPINTS + NBUGINTS; i++) {
		c->x86_capability[i] &= ~cpu_caps_cleared[i];
		c->x86_capability[i] |= cpu_caps_set[i];
	}
}
/*
static const struct cpu_dev *cpu_devs[X86_VENDOR_NUM] = {};
*/
static struct cpu_dev *cpu_devs[X86_VENDOR_NUM] = {};

void cpu_detect(struct cpuinfo_x86 *c)
{
	/* Get vendor name */
	cpuid(0x00000000, (unsigned int *)&c->cpuid_level,
	      (unsigned int *)&c->x86_vendor_id[0],
	      (unsigned int *)&c->x86_vendor_id[8],
	      (unsigned int *)&c->x86_vendor_id[4]);

	c->x86 = 4;
	/* Intel-defined flags: level 0x00000001 */
	if (c->cpuid_level >= 0x00000001) {
		u32 junk, tfms, cap0, misc;

		cpuid(0x00000001, &tfms, &misc, &junk, &cap0);
		c->x86		= x86_family(tfms);
		c->x86_model	= x86_model(tfms);
		c->x86_mask	= x86_stepping(tfms);

		if (cap0 & (1<<19)) {
			c->x86_clflush_size = ((misc >> 8) & 0xff) * 8;
			c->x86_cache_alignment = c->x86_clflush_size;
		}
	}
}

static void get_cpu_vendor(struct cpuinfo_x86 *c)
{
	char *v = c->x86_vendor_id;
	int i;

	for (i = 0; i < X86_VENDOR_NUM; i++) {
		if (!cpu_devs[i])
			break;

		if (!strcmp(v, cpu_devs[i]->c_ident[0]) ||
		    (cpu_devs[i]->c_ident[1] &&
		     !strcmp(v, cpu_devs[i]->c_ident[1]))) {

			this_cpu = cpu_devs[i];
			c->x86_vendor = this_cpu->c_x86_vendor;
			return;
		}
	}

	pr_err_once("CPU: vendor_id '%s' unknown, using generic init.\n" \
		    "CPU: Your system may be unstable.\n", v);

	c->x86_vendor = X86_VENDOR_UNKNOWN;
	this_cpu = &default_cpu;
}

void get_cpu_cap(struct cpuinfo_x86 *c)
{
	u32 eax, ebx, ecx, edx;

	/* Intel-defined flags: level 0x00000001 */
	if (c->cpuid_level >= 0x00000001) {
		cpuid(0x00000001, &eax, &ebx, &ecx, &edx);

		c->x86_capability[CPUID_1_ECX] = ecx;
		c->x86_capability[CPUID_1_EDX] = edx;
	}

	/* Thermal and Power Management Leaf: level 0x00000006 (eax) */
	if (c->cpuid_level >= 0x00000006)
		c->x86_capability[CPUID_6_EAX] = cpuid_eax(0x00000006);

	/* Additional Intel-defined flags: level 0x00000007 */
	if (c->cpuid_level >= 0x00000007) {
		cpuid_count(0x00000007, 0, &eax, &ebx, &ecx, &edx);
		c->x86_capability[CPUID_7_0_EBX] = ebx;
		c->x86_capability[CPUID_7_ECX] = ecx;
	}

	/* Extended state features: level 0x0000000d */
	if (c->cpuid_level >= 0x0000000d) {
		cpuid_count(0x0000000d, 1, &eax, &ebx, &ecx, &edx);

		c->x86_capability[CPUID_D_1_EAX] = eax;
	}

	/* Additional Intel-defined flags: level 0x0000000F */
	if (c->cpuid_level >= 0x0000000F) {

		/* QoS sub-leaf, EAX=0Fh, ECX=0 */
		cpuid_count(0x0000000F, 0, &eax, &ebx, &ecx, &edx);
		c->x86_capability[CPUID_F_0_EDX] = edx;

		if (cpu_has(c, X86_FEATURE_CQM_LLC)) {
			/* will be overridden if occupancy monitoring exists */
			c->x86_cache_max_rmid = ebx;

			/* QoS sub-leaf, EAX=0Fh, ECX=1 */
			cpuid_count(0x0000000F, 1, &eax, &ebx, &ecx, &edx);
			c->x86_capability[CPUID_F_1_EDX] = edx;

			if ((cpu_has(c, X86_FEATURE_CQM_OCCUP_LLC)) ||
			      ((cpu_has(c, X86_FEATURE_CQM_MBM_TOTAL)) ||
			       (cpu_has(c, X86_FEATURE_CQM_MBM_LOCAL)))) {
				c->x86_cache_max_rmid = ecx;
				c->x86_cache_occ_scale = ebx;
			}
		} else {
			c->x86_cache_max_rmid = -1;
			c->x86_cache_occ_scale = -1;
		}
	}

	/* AMD-defined flags: level 0x80000001 */
	eax = cpuid_eax(0x80000000);
	c->extended_cpuid_level = eax;

	if ((eax & 0xffff0000) == 0x80000000) {
		if (eax >= 0x80000001) {
			cpuid(0x80000001, &eax, &ebx, &ecx, &edx);

			c->x86_capability[CPUID_8000_0001_ECX] = ecx;
			c->x86_capability[CPUID_8000_0001_EDX] = edx;
		}
	}

	if (c->extended_cpuid_level >= 0x80000007) {
		cpuid(0x80000007, &eax, &ebx, &ecx, &edx);

		c->x86_capability[CPUID_8000_0007_EBX] = ebx;
		c->x86_power = edx;
	}

	if (c->extended_cpuid_level >= 0x80000008) {
		cpuid(0x80000008, &eax, &ebx, &ecx, &edx);

		c->x86_virt_bits = (eax >> 8) & 0xff;
		c->x86_phys_bits = eax & 0xff;
		c->x86_capability[CPUID_8000_0008_EBX] = ebx;
	}
#ifdef CONFIG_X86_32
	else if (cpu_has(c, X86_FEATURE_PAE) || cpu_has(c, X86_FEATURE_PSE36))
		c->x86_phys_bits = 36;
#endif

	if (c->extended_cpuid_level >= 0x8000000a)
		c->x86_capability[CPUID_8000_000A_EDX] = cpuid_edx(0x8000000a);

	init_scattered_cpuid_features(c);

	/*
	 * Clear/Set all flags overridden by options, after probe.
	 * This needs to happen each time we re-probe, which may happen
	 * several times during CPU initialization.
	 */
	apply_forced_caps(c);
}

// Source=/arch/x86/include/asm/msr.h
#define DECLARE_ARGS(val, low, high)	unsigned long low, high
#define EAX_EDX_VAL(val, low, high)	((low) | (high) << 32)
#define EAX_EDX_RET(val, low, high)	"=a" (low), "=d" (high)

static inline unsigned long long /*notrace*/ __rdmsr(unsigned int msr)
{
	DECLARE_ARGS(val, low, high);

	asm volatile("1: rdmsr\n"
		     "2:\n"
		     _ASM_EXTABLE_HANDLE(1b, 2b, ex_handler_rdmsr_unsafe)
		     : EAX_EDX_RET(val, low, high) : "c" (msr));

	return EAX_EDX_VAL(val, low, high);
}

static inline void /*notrace*/ __wrmsr(unsigned int msr, u32 low, u32 high)
{
	asm volatile("1: wrmsr\n"
		     "2:\n"
		     _ASM_EXTABLE_HANDLE(1b, 2b, ex_handler_wrmsr_unsafe)
		     : : "c" (msr), "a"(low), "d" (high) : "memory");
}

// Source=/arch/x86/lib/msr.c
struct msr {
	union {
		struct {
			u32 l;
			u32 h;
		};
		u64 q;
	};
};

static inline unsigned long long native_read_msr_safe(unsigned int msr,
						      int *err)
{
	DECLARE_ARGS(val, low, high);

	asm volatile("2: rdmsr ; xor %[err],%[err]\n"
		     "1:\n\t"
		     ".section .fixup,\"ax\"\n\t"
		     "3: mov %[fault],%[err]\n\t"
		     "xorl %%eax, %%eax\n\t"
		     "xorl %%edx, %%edx\n\t"
		     "jmp 1b\n\t"
		     ".previous\n\t"
		     _ASM_EXTABLE(2b, 3b)
		     : [err] "=r" (*err), EAX_EDX_RET(val, low, high)
		     : "c" (msr), [fault] "i" (-EIO));
/*
	if (msr_tracepoint_active(__tracepoint_read_msr))
		do_trace_read_msr(msr, EAX_EDX_VAL(val, low, high), *err);
*/
	return EAX_EDX_VAL(val, low, high);
}

static inline int /*notrace*/ native_write_msr_safe(unsigned int msr, u32 low, u32 high)
{
	int err;

	asm volatile("2: wrmsr ; xor %[err],%[err]\n"
		     "1:\n\t"
		     ".section .fixup,\"ax\"\n\t"
		     "3:  mov %[fault],%[err] ; jmp 1b\n\t"
		     ".previous\n\t"
		     _ASM_EXTABLE(2b, 3b)
		     : [err] "=a" (err)
		     : "c" (msr), "0" (low), "d" (high),
		       [fault] "i" (-EIO)
		     : "memory");
/*
	if (msr_tracepoint_active(__tracepoint_write_msr))
		do_trace_write_msr(msr, ((u64)high << 32 | low), err);
*/
	return err;
}

#define rdmsrl(msr, val)			\
	((val) = native_read_msr((msr)))

static inline int wrmsr_safe(unsigned int msr, u32 low, u32 high)
{
	return native_write_msr_safe(msr, low, high);
}

static inline int rdmsrl_safe(unsigned int msr, unsigned long long *p)
{
	int err;

	*p = native_read_msr_safe(msr, &err);
	return err;
}

static inline int wrmsrl_safe(u32 msr, u64 val)
{
	return wrmsr_safe(msr, (u32)val,  (u32)(val >> 32));
}

int msr_read(u32 msr, struct msr *m)
{
	int err;
	u64 val;

	err = rdmsrl_safe(msr, &val);
	if (!err)
		m->q = val;

	return err;
}

int msr_write(u32 msr, struct msr *m)
{
	return wrmsrl_safe(msr, m->q);
}

static inline int __flip_bit(u32 msr, u8 bit, bool set)
{
	struct msr m, m1;
	int err = -EINVAL;

	if (bit > 63)
		return err;

	err = msr_read(msr, &m);
	if (err)
		return err;

	m1 = m;
	if (set)
		m1.q |=  BIT_64(bit);
	else
		m1.q &= ~BIT_64(bit);

	if (m1.q == m.q)
		return 0;

	err = msr_write(msr, &m1);
	if (err)
		return err;

	return 1;
}

int msr_set_bit(u32 msr, u8 bit)
{
	return __flip_bit(msr, bit, true);
}

int msr_clear_bit(u32 msr, u8 bit)
{
	return __flip_bit(msr, bit, false);
}

// Source=/arch/x86/kernel/cpu/common.c
static void /*__init*/ early_identify_cpu(struct cpuinfo_x86 *c)
{
#ifdef CONFIG_X86_64
	c->x86_clflush_size = 64;
	c->x86_phys_bits = 36;
	c->x86_virt_bits = 48;
#else
	c->x86_clflush_size = 32;
	c->x86_phys_bits = 32;
	c->x86_virt_bits = 32;
#endif
	c->x86_cache_alignment = c->x86_clflush_size;

	memset(&c->x86_capability, 0, sizeof c->x86_capability);
	c->extended_cpuid_level = 0;

	/* cyrix could have cpuid enabled via c_identify()*/
	if (have_cpuid_p()) {
		cpu_detect(c);
		get_cpu_vendor(c);
		get_cpu_cap(c);
		setup_force_cpu_cap(X86_FEATURE_CPUID);

		if (this_cpu->c_early_init)
			this_cpu->c_early_init(c);

		c->cpu_index = 0;
		filter_cpuid_features(c, false);

		if (this_cpu->c_bsp_init)
			this_cpu->c_bsp_init(c);
	} else {
		identify_cpu_without_cpuid(c);
		setup_clear_cpu_cap(X86_FEATURE_CPUID);
	}

	setup_force_cpu_cap(X86_FEATURE_ALWAYS);

	if (c->x86_vendor != X86_VENDOR_AMD)
		setup_force_cpu_bug(X86_BUG_CPU_INSECURE);

/*	fpu__init_system(c);	*/
}

// Source=/arch/x86/include/asm/microcode_intel.h
#define native_rdmsr(msr, val1, val2)			\
do {							\
	u64 __val = __rdmsr((msr));			\
	(void)((val1) = (u32)__val);			\
	(void)((val2) = (u32)(__val >> 32));		\
} while (0)

#define native_wrmsr(msr, low, high)			\
	__wrmsr(msr, low, high)

#define native_wrmsrl(msr, val)				\
	__wrmsr((msr), (u32)((u64)(val)),		\
		       (u32)((u64)(val) >> 32))

static inline u32 intel_get_microcode_revision(void)
{
	u32 rev, dummy;

	native_wrmsrl(MSR_IA32_UCODE_REV, 0);

	/* As documented in the SDM: Do a CPUID 1 here */
	native_cpuid_eax(1);

	/* get the current revision from MSR 0x8B */
	native_rdmsr(MSR_IA32_UCODE_REV, dummy, rev);

	return rev;
}

// Source=/arch/x86/kernel/cpu/intel.c
static void early_init_intel(struct cpuinfo_x86 *c)
{
	u64 misc_enable;

	/* Unmask CPUID levels if masked: */
	if (c->x86 > 6 || (c->x86 == 6 && c->x86_model >= 0xd)) {
		if (msr_clear_bit(MSR_IA32_MISC_ENABLE,
				  MSR_IA32_MISC_ENABLE_LIMIT_CPUID_BIT) > 0) {
			c->cpuid_level = cpuid_eax(0);
			get_cpu_cap(c);
		}
	}

	if ((c->x86 == 0xf && c->x86_model >= 0x03) ||
		(c->x86 == 0x6 && c->x86_model >= 0x0e))
		set_cpu_cap(c, X86_FEATURE_CONSTANT_TSC);

	if (c->x86 >= 6 && !cpu_has(c, X86_FEATURE_IA64))
		c->microcode = intel_get_microcode_revision();

	/*
	 * Atom erratum AAE44/AAF40/AAG38/AAH41:
	 *
	 * A race condition between speculative fetches and invalidating
	 * a large page.  This is worked around in microcode, but we
	 * need the microcode to have already been loaded... so if it is
	 * not, recommend a BIOS update and disable large pages.
	 */
	if (c->x86 == 6 && c->x86_model == 0x1c && c->x86_mask <= 2 &&
	    c->microcode < 0x20e) {
		pr_warn("Atom PSE erratum detected, BIOS microcode update recommended\n");
		clear_cpu_cap(c, X86_FEATURE_PSE);
	}

#ifdef CONFIG_X86_64
	set_cpu_cap(c, X86_FEATURE_SYSENTER32);
#else
	/* Netburst reports 64 bytes clflush size, but does IO in 128 bytes */
	if (c->x86 == 15 && c->x86_cache_alignment == 64)
		c->x86_cache_alignment = 128;
#endif

	/* CPUID workaround for 0F33/0F34 CPU */
	if (c->x86 == 0xF && c->x86_model == 0x3
	    && (c->x86_mask == 0x3 || c->x86_mask == 0x4))
		c->x86_phys_bits = 36;

	/*
	 * c->x86_power is 8000_0007 edx. Bit 8 is TSC runs at constant rate
	 * with P/T states and does not stop in deep C-states.
	 *
	 * It is also reliable across cores and sockets. (but not across
	 * cabinets - we turn it off in that case explicitly.)
	 */
	if (c->x86_power & (1 << 8)) {
		set_cpu_cap(c, X86_FEATURE_CONSTANT_TSC);
		set_cpu_cap(c, X86_FEATURE_NONSTOP_TSC);
	}

	/* Penwell and Cloverview have the TSC which doesn't sleep on S3 */
	if (c->x86 == 6) {
		switch (c->x86_model) {
		case 0x27:	/* Penwell */
		case 0x35:	/* Cloverview */
		case 0x4a:	/* Merrifield */
			set_cpu_cap(c, X86_FEATURE_NONSTOP_TSC_S3);
			break;
		default:
			break;
		}
	}

	/*
	 * There is a known erratum on Pentium III and Core Solo
	 * and Core Duo CPUs.
	 * " Page with PAT set to WC while associated MTRR is UC
	 *   may consolidate to UC "
	 * Because of this erratum, it is better to stick with
	 * setting WC in MTRR rather than using PAT on these CPUs.
	 *
	 * Enable PAT WC only on P4, Core 2 or later CPUs.
	 */
	if (c->x86 == 6 && c->x86_model < 15)
		clear_cpu_cap(c, X86_FEATURE_PAT);

#ifdef CONFIG_KMEMCHECK
	/*
	 * P4s have a "fast strings" feature which causes single-
	 * stepping REP instructions to only generate a #DB on
	 * cache-line boundaries.
	 *
	 * Ingo Molnar reported a Pentium D (model 6) and a Xeon
	 * (model 2) with the same problem.
	 */
	if (c->x86 == 15)
		if (msr_clear_bit(MSR_IA32_MISC_ENABLE,
				  MSR_IA32_MISC_ENABLE_FAST_STRING_BIT) > 0)
			pr_info("kmemcheck: Disabling fast string operations\n");
#endif

	/*
	 * If fast string is not enabled in IA32_MISC_ENABLE for any reason,
	 * clear the fast string and enhanced fast string CPU capabilities.
	 */
	if (c->x86 > 6 || (c->x86 == 6 && c->x86_model >= 0xd)) {
		rdmsrl(MSR_IA32_MISC_ENABLE, misc_enable);
		if (!(misc_enable & MSR_IA32_MISC_ENABLE_FAST_STRING)) {
			pr_info("Disabled fast string operations\n");
			setup_clear_cpu_cap(X86_FEATURE_REP_GOOD);
			setup_clear_cpu_cap(X86_FEATURE_ERMS);
		}
	}

	/*
	 * Intel Quark Core DevMan_001.pdf section 6.4.11
	 * "The operating system also is required to invalidate (i.e., flush)
	 *  the TLB when any changes are made to any of the page table entries.
	 *  The operating system must reload CR3 to cause the TLB to be flushed"
	 *
	 * As a result, boot_cpu_has(X86_FEATURE_PGE) in arch/x86/include/asm/tlbflush.h
	 * should be false so that __flush_tlb_all() causes CR3 insted of CR4.PGE
	 * to be modified.
	 */
	if (c->x86 == 5 && c->x86_model == 9) {
		pr_info("Disabling PGE capability bit\n");
		setup_clear_cpu_cap(X86_FEATURE_PGE);
	}

	if (c->cpuid_level >= 0x00000001) {
		u32 eax, ebx, ecx, edx;

		cpuid(0x00000001, &eax, &ebx, &ecx, &edx);
		/*
		 * If HTT (EDX[28]) is set EBX[16:23] contain the number of
		 * apicids which are reserved per package. Store the resulting
		 * shift value for the package management code.
		 */
		if (edx & (1U << 28))
			c->x86_coreid_bits = get_count_order((ebx >> 16) & 0xff);
	}

	check_mpx_erratum(c);
}

// Source=/arch/x86/kernel/cpu/common.c
void /*__init*/ early_cpu_init(void)
{
/*
	const struct cpu_dev *const *cdev;
	int count = 0;

#ifdef CONFIG_PROCESSOR_SELECT
	pr_info("KERNEL supported cpus:\n");
#endif

	for (cdev = cpu_devs[X86_VENDOR_INTEL]; cdev < cpu_devs[X86_VENDOR_NUM]; cdev++) {
		const struct cpu_dev *cpudev = *cdev;

		if (count >= X86_VENDOR_NUM)
			break;
		cpu_devs[count] = cpudev;
		count++;

#ifdef CONFIG_PROCESSOR_SELECT
		{
			unsigned int j;

			for (j = 0; j < 2; j++) {
				if (!cpudev->c_ident[j])
					continue;
				pr_info("  %s %s\n", cpudev->c_vendor,
					cpudev->c_ident[j]);
			}
		}
#endif
	}
*/
	cpu_devs[0]->c_vendor;
	cpu_devs[0]->c_ident[2];
	cpu_devs[0]->c_early_init = early_init_intel;
	cpu_devs[0]->c_bsp_init;
	cpu_devs[0]->c_init;
	cpu_devs[0]->c_identify;
	cpu_devs[0]->c_detect_tlb;
	cpu_devs[0]->c_bsp_resume;
	cpu_devs[0]->c_x86_vendor;

	early_identify_cpu(&boot_cpu_data);
}

// Source=/arch/x86/include/asm/cpufeature.h
#define boot_cpu_has_bug(bit)		cpu_has_bug(&boot_cpu_data, (bit))

// Source=/arch/x86/mm/pti.c
static void pti_print_if_insecure(const char *reason)
{
	if (boot_cpu_has_bug(X86_BUG_CPU_INSECURE))
		pr_info("%s\n", reason);
}

static void pti_print_if_secure(const char *reason)
{
	if (!boot_cpu_has_bug(X86_BUG_CPU_INSECURE))
		pr_info("%s\n", reason);
}

int main(int argc, char *argv[])
{
	early_cpu_init();
	pti_print_if_insecure("disabled on command line.");
	pti_print_if_secure("force enabled on command line.");
}