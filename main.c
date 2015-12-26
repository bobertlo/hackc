#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include "parser.h"

int main(int argc, char *argv[]) {
	FILE *in;
	parser_ctx *p;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <input.asm>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	in = fopen(argv[1], "r");
	if (in == NULL) {
		perror("could not open input file");
	}

	p = new_parser(in);
	parse(p);
	link(p);
	emit(p, stdout);

	return EXIT_SUCCESS;
}
