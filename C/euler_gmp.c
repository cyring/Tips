/*
gcc -o euler_gmp -Wall -Wextra -Wpedantic -std=c89 -lgmp euler_gmp.c
*/
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>

int main(void)
{
	char *euler_str;
	int rc = 1, i, n;

	mpf_t euler_num, term, factorial;
	mpf_set_default_prec(10001);

	mpf_init(euler_num);
	mpf_init(term);
	mpf_init(factorial);
	mpf_set_ui(euler_num, 0);
	mpf_set_ui(term, 1);
	mpf_set_ui(factorial, 1);

	/* Calculate Euler's number using the Taylor series expansion	*/
	for (i = 1; i <= 10000; i++) {
		mpf_div_ui(term, term, i);
		mpf_add(euler_num, euler_num, term);
		mpf_mul_ui(factorial, factorial, i);
	}

	euler_str = malloc(1+1+10000+1);
	if (euler_str != NULL) {
		n = gmp_snprintf(euler_str, 1+1+10000+1, "%.*Ff", 10000, euler_num);
		printf("[%d] e = %s\n", n, euler_str);

		free(euler_str);
		rc = 0;
	}

	mpf_clear(euler_num);
	mpf_clear(term);
	mpf_clear(factorial);

	return rc;
}
