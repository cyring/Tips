#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define RDTSC(_lo, _hi) 						\
__asm__ volatile							\
(									\
	"lfence"		"\n\t"					\
	"rdtsc" 							\
	: "=a" (_lo),							\
	  "=d" (_hi)							\
)

int main(int argc, char *argv[])
{
	struct CELL {
		struct CELL *prev;
		unsigned char *ptr __attribute__ ((aligned (64)));
	} *cell = NULL, *tail = NULL, *back = NULL;

	long long loops = 10, size = 0xffffff, cells = 0, errors = 0;
	size_t total = 0, reclaim = 0;

	switch (argc) {
	case 3:
		loops = atoll(argv[2]);
	case 2:
		size = atoll(argv[1]);
	case 1:
		break;
	default:
		printf("%s [cell size] <cell count>\n", argv[0]);
		exit(1);
	}

	while ((cell = (struct CELL *) malloc(sizeof(struct CELL))) != NULL) {
		total += malloc_usable_size(cell);

		cell->prev = tail;
		cell->ptr = NULL;
		tail = cell;
		cells++;
		if (cells < loops) {
			if ((cell->ptr = (unsigned char*) malloc(size)) == NULL)
			{
				printf("Out of memory !\n");
				break;
			} else
				total += malloc_usable_size(cell->ptr);
		} else
			break;
	}
	printf( "%lld cell(s) allocated in %zd(B) %zd(KB) %zd(MB) of memory.\n"
		"Press a key to test ...",
		cells, total, total / 1024, total / (1024 * 1024));
	getchar();

	back = tail;
	do {
		cell = back;
		if (cell != NULL)
		{
			if (cell->ptr != NULL) {
				register long long idx;
				for (idx = 0; idx < size; idx++) {
					register unsigned int EAX, EDX;
					register unsigned char any;
					RDTSC(EAX, EDX);
					any = EAX & 0xff;
					cell->ptr[idx] = any;
					cell->ptr[idx] ^= any;
					cell->ptr[idx] ^= any;
					if (cell->ptr[idx] != any)
						errors++;
				}
			}
			back = cell->prev;
		}
	} while (back != NULL);

	printf( "%lld memory error(s).\nPress a key to deallocate %lld cell(s).",
		errors, cells);
	getchar();

	back = tail;
	do {
		cell = back;
		if (cell != NULL)
		{
			reclaim += malloc_usable_size(cell);
			if (cell->ptr != NULL) {
				reclaim += malloc_usable_size(cell->ptr);
				free(cell->ptr);
			}
			back = cell->prev;
			free(cell);
			cells--;
		}
	} while (back != NULL);

	if ((cells > 0) || (total != reclaim) ){
		printf( "Warning: %lld cell(s) unallocated in"
			" %zd(B) of reclaimed memory.\n", cells, reclaim);
	}
	return(0);
}

