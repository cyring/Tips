/*
https://www.math.utah.edu/~pa/math/e.html
e = 2.71828 18284 59045 23536 02874 71352 66249 77572 47093 69995 95749 66967 62772 40766 30353 54759 45713 82178 52516 64274 27466 39193 20030 59921 81741 35966 29043 57290 03342 95260 59563 07381 32328 62794 34907 63233 82988 07531 95251
cc euler_f128.c -o euler_f128 -lquadmath
*/
#include <stdio.h>
#include <quadmath.h>

typedef __float128 euler_t;

void Compute_e(euler_t *e)
{
	euler_t sum = 1.0Q;
	euler_t term = 1.0Q;
	for (int i = 1; i < 34; i++) {
		term *= i;
		sum += 1.0Q / term;
	}
	*e = sum;
}

int main(int argc, char *argv[])
{
	euler_t e;
	Compute_e(&e);

	char str[1/*integer*/+1/*dot*/+34/*digits*/+1/*end of string*/];
	quadmath_snprintf(str, sizeof(str), "%.35Qf", e);
	printf("e = %s\n", str);
	return 0;
}
