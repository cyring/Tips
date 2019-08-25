/*
	bitops.c
	# Non regression tests of CoreFreq bit operations
	cc -I "../CoreFreq/" bitops.c -o bitops
	./bitops ; echo $?
	# 1 is the expected value
*/

#include "bitasm.h"

int main(int argc, char *argv[])
{
/*
                ...6.........5.........4.........3.........2.........1.........0
                3210987654321098765432109876543210987654321098765432109876543210
*/
#define B64   0b1000000000000000000000000000000000000000000000000000000000000000
#define B32   0b0000000000000000000000000000000010000000000000000000000000000000
#define B16   0b0000000000000000000000000000000000000000000000001000000000000000

	unsigned long long	b64 = B64;
	unsigned int		b32 = B32;
	unsigned short		b16 = B16;

	int i64[2] = {BITVAL(BUS_LOCK, b64, 63), BITVAL(LOCKLESS, b64, 63)};
	int i32[2] = {BITVAL(BUS_LOCK, b32, 31), BITVAL(LOCKLESS, b32, 31)};
	int i16[2] = {BITVAL(BUS_LOCK, b16, 15), BITVAL(LOCKLESS, b16, 15)};

	unsigned long long o63 = 63;
	int t64[2] = {BITVAL(BUS_LOCK, b64, o63), BITVAL(LOCKLESS, b64, o63)};

	unsigned int o32 = 31;
	int t32[2] = {BITVAL(BUS_LOCK, b32, o32), BITVAL(LOCKLESS, b32, o32)};

	unsigned int o16 = 15;
	int t16[2] = {BITVAL(BUS_LOCK, b16, o16), BITVAL(LOCKLESS, b16, o16)};

	int i, ret = 1;
    for (i = 0; i < 2; i++) {
	ret = ret && i64[i] && i32[i] && i16[i] && t64[i] && t32[i] && t16[i];
    }
	return(ret);
}

