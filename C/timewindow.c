/*
 * Build: cc timewindow.c -o timewindow
 * Run ./timewindow <Y> <Z> <TU>
*/
#include <stdio.h>
#include <stdlib.h>

double TW_Optimize(unsigned short Y, unsigned short Z, double TU)
{
	unsigned long long duration;
	duration = (1 << Y);
	duration *= (4 + Z);
	duration = duration >> 2;

	return TU * (double)duration;
}

double TW_Manufacturer(unsigned short Y, unsigned short Z, double TU)
{
	return (1 << Y) * (1 + Z/4) * TU;
}

int main(int argc, char *argv[])
{
	int rc = 0;
	unsigned short Y, Z;
	double TU = 0.000976562;

    switch (argc) {
    case 2:
    {
	unsigned short bits = atoi(argv[1]);

	Y = (bits >> 5) & 0b11;
	Z = bits & 0b11111;

	break;
    }
    case 4:
	TU = atof(argv[3]);
	/* fallthrough */
    case 3:
	Y = atoi(argv[1]);
	Z = atoi(argv[2]);

	break;
    default:
	rc = 1;
	break;
    }
    if (rc == 0)
    {
	double X[2] = {
		[0] = TW_Optimize(Y, Z, TU),
		[1] = TW_Manufacturer(Y, Z, TU)
	};
	printf("TimeWindow@(%u, %u) > TW_Optimize()=%f > TW_Manufacturer()=%f" \
		" > %s\n", Y, Z, X[0], X[1], X[0] == X[1] ? "OK" : "KO");
    } else {
	printf( "Usage:\t%s <Y> <Z> [TU]\nor\t%s <bits>\n", argv[0], argv[0]);
    }
	return rc;
}
