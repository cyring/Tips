/* CyrIng (wip)
 ## prerequisites: x86-64 bits processor with an invariant TSC
 $ gcc ubench.c -o ubench
 ## example: run on Core number 2
 $ ./ubench 2
*/

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>

#define LOOPS	1000000000000LLU

#define USERASM						\
		"push	%%rcx"			"\n\t"	\
		"mov	$0,	%%rax"		"\n\t"	\
		"cpuid"				"\n\t"	\
		"pop	%%rcx"			"\n\t"


static unsigned long long Serialized(int *pCore) // 2008 Processors gen.
{	// Time Stamp is a 64 bits counter
	unsigned long long T[3] = {0, 0, 0}, overhead, cycles;

	asm volatile
	(
		"# Read and save TSC in T[0]"	"\n\t"
		"rdtscp"			"\n\t"
		"shl	$32,	%%rdx"		"\n\t"
		"or	%%rdx,	%%rax"		"\n\t"
		"mov	%%rax,	%%r12"		"\n\t"
		"# Intrasic time"		"\n\t"
		USERASM
		"# Read and save TSC in T[1]"	"\n\t"
		"rdtscp"			"\n\t"
		"shl	$32,	%%rdx"		"\n\t"
		"or	%%rdx,	%%rax"		"\n\t"
		"mov	%%rax,	%%r13"		"\n\t"
		"# Execute LOOPS times"		"\n\t"
		"mov	$3,	%%rcx"		"\n\t"
		".do:"				"\n\t"
		USERASM
		"loop .do"			"\n\t"
		"# Read and save TSC in T[2]"	"\n\t"
		"rdtscp"			"\n\t"
		"shl	$32,	%%rdx"		"\n\t"
		"or	%%rdx,	%%rax"		"\n\t"
		"mfence"			"\n\t"
		"mov	%%rax,	%2"		"\n\t"
		"mov	%%r13,	%1"		"\n\t"
		"mov	%%r12,	%0"
		: "=m" (T[0]), "=m" (T[1]), "=m" (T[2])
		: "i" (LOOPS)
		: "%rax","%rcx","%rdx","%r12","%r13","memory"
	);

	overhead= T[1] - T[0];
	cycles  = T[2] - T[1];
	cycles -= overhead;

	return(cycles);
}

int Bind(unsigned long long *pCycles, int *pCore, unsigned long long (*pUserFunc)(int *))
{
	int rc = 0;
	// Set this Core Bitmap
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET((*pCore), &cpuset);
	// Request OS for CPU binding.
	rc = sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
	if (rc == 0) {	 // Now call the user function.
		(*pCycles) = pUserFunc(pCore);
	}
	return(rc);
}

int main(int argc, char *argv[])
{
	unsigned long long cycles = 0;
	int rc = 0;

	if (argc > 1) { // Core number from the command line.
		int core = atoi(argv[1]);

		unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;
		asm volatile
		(
			"# Check rdtscp in RDX[27]"	"\n\t"
			"cpuid"
			: "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
			: "a" (0x80000001)
			:
		);

		if (edx & 0x8000000) {
			// Bind the instructions flow.
			if ((rc = Bind(&cycles, &core, Serialized)) == 0) {
				unsigned long long quo, rem;
				quo = cycles / LOOPS;
				rem = cycles % LOOPS;
				printf("%llu.%llu\n", quo, rem);
			}
		} else {
			printf("rdtscp missing\n");
			rc = -2;
		}
	} else {
		printf("%s: <core#>", argv[0]);
		rc = -1;
	}
	return(rc);
}

