#include <stdint.h>
#include "symtab.h"

#define SYMBOL_MAX 128
#define LINE_MAX_TOKENS 9

typedef enum {
	I_REF,
	I_CODE,
} instruction_type;

typedef enum {
	TOK_EOF     = -1,
	TOK_STRING  = -2,
	TOK_NUMBER  = -3,
	TOK_NEWLINE = -4,
} tok_type;

typedef struct tok {
	int token;
	union {
		char *string;
		uint16_t number;
	} dat;
} token;

typedef struct instruction instruction;
struct instruction {
	instruction_type type;	// type (I_CODE or I_REF)
	union {
		uint16_t code;	// binary 16-bit word
		char *symbol;	// symbol for reference to be linked
	} dat;
	int line;		// line number from input
	instruction *next;	// next instruction in linked list
};

typedef struct {
	instruction *head;
	instruction *tail;
	int length;
} instruction_list;

typedef struct {
	FILE *in;			// input file stream
	instruction_list ilist;		// instruction list
	int line;			// number of current line being read
	int lastchar;			// most recent character read
	token tok_buf[LINE_MAX_TOKENS];	// token buffer for current line
	symbol *symbols;		// symbol table
	uint16_t nextvar;		// index of first unused variable address
	int eof;			// non-zero if EOF has been read in input
} parser_ctx;

parser_ctx *new_parser(FILE *in);
void parse(parser_ctx *p);
void link(parser_ctx *p);
void emit(parser_ctx *p, FILE *out);
