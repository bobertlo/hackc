#include <stdlib.h>
#include <stdio.h>

void plnerror(int p, char *s) {
	fprintf(stderr, "%i: %s\n", p, s);
	exit(EXIT_FAILURE);
}
