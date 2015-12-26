#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

// insert symbol entry in table (key->val)
// TODO: return error value on symbol redefinition instead of exit(EXIT_FAILURE)
void symtab_insert(symbol **table, const char *key, uint16_t val) {
	symbol *sp = malloc(sizeof(symbol));
	if (symtab_lookup(*table, key) != NULL) {
		fprintf(stderr, "Sybol '%s' already defined, aborting.\n", key);
		exit(EXIT_FAILURE);
	}
	sp->key = strdup(key);
	sp->val = val;
	sp->next = *table;
	*table = sp;
	return;
}

// lookup entry matching key, returning symbol pointer (or NULL)
symbol *symtab_lookup(symbol *table, const char *key) {
	for (symbol *sp = table; sp != NULL; sp = sp->next) {
		if (strcmp(sp->key, key) == 0) {
			return sp;
		}
	}
	return NULL;
}
