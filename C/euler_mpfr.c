/*
 * euler_mpfr.c by CyrIng
 * gcc euler_mpfr.c -o euler_mpfr -lmpfr
 */
#define PRECISION 524288
#include <stdlib.h>
#include <stdio.h>
#include <mpfr.h>

#include "euler_utah.h"

void Compute_e(mpfr_t e)
{
	mpfr_t n, term, sum;
	mpfr_inits(n, term, sum, NULL);

	mpfr_set_ui(n, 0, MPFR_RNDN);
	mpfr_set_ui(term, 1, MPFR_RNDN);
	mpfr_set_ui(sum, 1, MPFR_RNDN);

	while (mpfr_cmp_ui(term, 0) != 0) {
		mpfr_add_ui(n, n, 1, MPFR_RNDN);
		mpfr_div_ui(term, term, mpfr_get_ui(n, MPFR_RNDN), MPFR_RNDN);
		mpfr_add(sum, sum, term, MPFR_RNDN);
	}

	mpfr_set(e, sum, MPFR_RNDN);
	mpfr_clears(n, term, sum, NULL);
}

int main(int argc, char *argv[])
{
	FILE *fdo = fmemopen(NULL, PRECISION + 1, "w+");
    if (fdo != NULL)
    {
	int c, n = PRECISION, i = 0;
	mpfr_set_default_prec(PRECISION);

	mpfr_t e;
	mpfr_init(e);

	Compute_e(e);

	mpfr_out_str(fdo, 10, 0, e, MPFR_RNDN);
	fseek(fdo, SEEK_SET, 0);

	fprintf(stdout, "e = ");
	while ((c = fgetc(fdo)) != '\0' && c != '\n' && c != 'e' && n-- > 0) {
		fputc(c, stdout);
		if (Euler[i++] != c) {
			fprintf(stdout, ":non-matching digit at position %d", i);
			break;
		}
	}
	fputc('\n', stdout);

	mpfr_clear(e);
	fclose(fdo);
	return 0;
    }
	return 1;
}
