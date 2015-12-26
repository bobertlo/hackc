#include <stdint.h>

typedef struct symbol symbol;
struct symbol {
	char *key;
	uint16_t val;
	symbol *next;
};

void symtab_insert(symbol **table, const char *key, uint16_t val);
symbol *symtab_lookup(symbol *table, const char *key);
