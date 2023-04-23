/*
https://www.math.utah.edu/~pa/math/e.html
e = 2.71828 18284 59045 23536 02874 71352 66249 77572 47093 69995 95749 66967 62772 40766 30353 54759 45713 82178 52516 64274 27466 39193 20030 59921 81741 35966 29043 57290 03342 95260 59563 07381 32328 62794 34907 63233 82988 07531 95251
cc euler_mpfr.c -o euler_mpfr -lmpfr
*/
#include <stdio.h>
#include <mpfr.h>

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
	mpfr_set_default_prec(100 * 3.32192809488736218170856773213);

	mpfr_t e;
	mpfr_init(e);
	Compute_e(e);

	printf("e = ");
	mpfr_out_str(stdout, 10, 0, e, MPFR_RNDN);
	printf("\n");

	mpfr_clear(e);
	return 0;
}
