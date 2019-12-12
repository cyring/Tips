/*
 * Build instruction:
 *	cc code4zen.c -o code4zen
 * Run instruction:
 *	code4zen 0x123456abcdef7890
 *
 * By CyrIng , based on the CoreFreq project
*/
#include <stdio.h>

typedef union
{
	unsigned long long value;
    struct
    {
	unsigned long long	 /* MSRC001_0064 [P-state [7:0]]	*/
	CpuFid		:  8-0,  /* Core Frequency ID. RW: FFh-10h <Value>*25 */
	CpuDfsId	: 14-8,  /* Core Divisor ID. RW			*/
	CpuVid		: 22-14, /* Core Voltage ID. RW			*/
	IddValue	: 30-22, /* Current Dissipation in amps. RW	*/
	IddDiv		: 32-30, /* Current Dissipation Divisor. RW	*/
	Reserved	: 63-32,
	PstateEn	: 64-63; /* RW: Is this Pstate MSR valid ?	*/
    };
} PSTATEDEF;

unsigned int AMD_Zen_CoreCOF(unsigned int FID, unsigned int DID)
{/* Source: PPR for AMD Family 17h Model 01h, Revision B1 Processors
    CoreCOF = (PStateDef[CpuFid[7:0]] / PStateDef[CpuDfsId]) * 200 */
	unsigned int COF;
	if (DID != 0) {
		COF = (FID << 1) / DID;
	} else {
		COF = FID >> 2;
	}
	return (COF);
}

unsigned int AMD_Zen_CoreFID(unsigned int COF, unsigned int DID)
{
	unsigned int FID;
	if (DID != 0) {
		FID = (COF * DID) >> 1;
	} else {
		FID = COF << 2;
	}
	return (FID);
}

double AMD_Zen_Voltage(unsigned int VID)
{
	return (1.550 - (0.00625 * (double) (VID)));
}

int main(int argc, char *argv[])
{
  if (argc == 2) {
	PSTATEDEF PstateDef = {.value = 0};

    if (sscanf(argv[1], "%llx%c", &PstateDef.value) == 1)
    {
	unsigned int COF = AMD_Zen_CoreCOF(PstateDef.CpuFid,PstateDef.CpuDfsId);
	double Vcore = AMD_Zen_Voltage(PstateDef.CpuVid);

	printf( " COF [%10u]      FID[%10u]      DID[%10u]\n"	\
		"Vcore[%10.4f]      VID[%10u]\n",
		COF, PstateDef.CpuFid, PstateDef.CpuDfsId,
		Vcore, PstateDef.CpuVid );
    }
	return (0);
  } else {
	printf( "Usage: %s <val>\n"	\
		"where <val> is the register hexadecimal value\n", argv[0]);
	return (1);
  }
}

