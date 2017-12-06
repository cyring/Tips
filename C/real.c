#include <stdio.h>
#include <math.h>

#define frtostr(r, d, pstr)				\
({							\
	int p = d - ((int) log10(fabs(r))) - 2;		\
	sprintf(pstr, "%*.*f", d, p, r);		\
	pstr;						\
})

int main(int argc, char *argv[])
{
	double r, s = 0.25, m = 102.5;
	char str[16 + 1];

	for (r = -1 * m; r < m; r += s) {
		printf("|%s|\t", frtostr(r, 4, str));
	}
	printf("\n");
	return(0);
}
