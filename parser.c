#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <error.h>

#include "data.h"
#include "parser.h"
#include "util.h"

void init_symbols(symbol **table) {
	symtab_insert(table, "R0",  0x0000);
	symtab_insert(table, "R1",  0x0001);
	symtab_insert(table, "R2",  0x0002);
	symtab_insert(table, "R3",  0x0003);
	symtab_insert(table, "R4",  0x0004);
	symtab_insert(table, "R5",  0x0005);
	symtab_insert(table, "R6",  0x0006);
	symtab_insert(table, "R7",  0x0007);
	symtab_insert(table, "R8",  0x0008);
	symtab_insert(table, "R9",  0x0009);
	symtab_insert(table, "R10", 0x0010);
	symtab_insert(table, "R11", 0x0011);
	symtab_insert(table, "R12", 0x0012);
	symtab_insert(table, "R13", 0x0013);
	symtab_insert(table, "R14", 0x0014);
	symtab_insert(table, "R15", 0x0015);

	symtab_insert(table, "SP",     0x0000);
	symtab_insert(table, "LCL",    0x0001);
	symtab_insert(table, "ARG",    0x0002);
	symtab_insert(table, "THIS",   0x0003);
	symtab_insert(table, "THAT",   0x0004);
	symtab_insert(table, "KBD",    0x4000);
	symtab_insert(table, "SCREEN", 0x6000);
}

parser_ctx *new_parser(FILE *in) {
	parser_ctx *p = malloc(sizeof(parser_ctx));

	p->in = in;
	p->ilist.head = NULL;
	p->ilist.tail = NULL;
	p->ilist.length = 0;
	p->line = 0;
	p->lastchar = ' ';
	p->nextvar = 0x0010;

	init_symbols(&p->symbols);

	return p;
}

void insert_instruction(instruction_list *l, instruction *i) {
	if (l->head == NULL) {
		i->next = NULL;
		l->head = i;
		l->length = 1;
	} else if (l->tail == NULL) {
		i->next = NULL;
		l->head->next = i;
		l->tail = i;
		l->length = 2;
	} else {
		i->next = NULL;
		l->tail->next = i;
		l->tail = i;
		l->length++;
	}
	return;
}

instruction *new_code_instruction(uint16_t code) {
	instruction *i = malloc(sizeof(instruction));
	i->type = I_CODE;
	i->dat.code = code;
	i->next = NULL;
	return i;
}

instruction *new_literal_instruction(uint16_t value) {
	return new_code_instruction(value & 0x7FFF);
}

instruction *new_reference(const char *string, int line) {
	instruction *i = malloc(sizeof(instruction));
	i->type = I_REF;
	i->dat.symbol = strdup(string);
	i->line = line;
	i->next = NULL;
	return i;
}

// read from the input file and return the next token found
// NOTE: string tokens contain a char* which must be freed
token gettok(parser_ctx *p) {
	token t;

	// eat non-newline whitespaces
	while(isblank(p->lastchar) || p->lastchar == '\r')
		p->lastchar = fgetc(p->in);

	// read identifiers into a buffer and return a TOK_STRING
	if(isalpha(p->lastchar)) {
		char sbuf[SYMBOL_MAX];
		int i = 1;
		sbuf[0] = p->lastchar;

		char c = fgetc(p->in);
		for (;;) {
			// process valid characters into the buffer, up to SYMBOL_MAX
			if (isalnum(c) || c == '$' || c == '.' || c == '_') {
				if (i==SYMBOL_MAX-1) {
					plnerror(p->line, "String overrun");		
				}
				sbuf[i] = c;
				i++;
				c = fgetc(p->in);
			// otherwise, terminate buffer and push invalid char back onto stack
			} else {
				p->lastchar = c;
				sbuf[i] = '\0';
				break;
			}
		}

		t.token = TOK_STRING;
		t.dat.string = strdup(sbuf);
		return t;

	
	// read (positive) integers and return TOK_NUMBER
	} else if (isdigit(p->lastchar)) {
		uint16_t acc = p->lastchar - '0';

		for (;;) {
			char c = fgetc(p->in);
			if (isdigit(c)) {
				acc *= 10;
				acc += c - '0';
			} else {
				p->lastchar = c;
				break;
			}
		}

		t.token = TOK_NUMBER;
		t.dat.number = acc;
		return t;


	// eat comments
	} else if (p->lastchar == '/') {
		p->lastchar = fgetc(p->in);
		if (p->lastchar == '/') {
			// eat characters until '\n' or EOF, then recurse to return a token
			for (;;) {
				int c = fgetc(p->in);
				if (c == '\n' || c == EOF) {
					p->lastchar = c;
					return gettok(p);
				}
			}
		} else {
			// push '/' back onto stack, which is never valid .. but complete
			t.token = '/';
			return t;
		}


	// handle '\n'
	} else if (p->lastchar == '\n') {
		p->line++;
		p->lastchar = fgetc(p->in);
		t.token = TOK_NEWLINE;
		return t;

	// handle EOF
	} else if (p->lastchar == EOF) {
		t.token = TOK_EOF;
		p->eof = 1;
		return t;

	// all other cases return an ASCII as a token
	} else {
		t.token = p->lastchar;
		p->lastchar = fgetc(p->in);
		return t;
	}
}

void lex_line(parser_ctx *p) {
	int i;

	// read tokens until newline into p->tok_buf[LINE_MAX_TOKENS]
	for (i=0; i < LINE_MAX_TOKENS; i++) {
		token t = gettok(p);
		p->tok_buf[i] = t;
		if (t.token = TOK_NEWLINE || t.token == TOK_EOF)
			break;
	}
	if (i == LINE_MAX_TOKENS) {
		plnerror(p->line, "Too many tokens.\n");
	}
}

void parse_address(parser_ctx *p, token *t) {
	if (t->token == TOK_NUMBER) {
		insert_instruction(&p->ilist, new_literal_instruction(t->dat.number));
	} else if (t->token == TOK_STRING) {
		insert_instruction(&p->ilist, new_reference(t->dat.string, p->line));
	} else {
		plnerror(p->line, "expected identifier or literal after '@'\n");
	}

	if (t[1].token != TOK_NEWLINE) {
		plnerror(p->line, "expected newline after A-Instruction\n");
	}
}

void parse_label(parser_ctx *p, token *t) {
	if (t->token == TOK_STRING) {
		symtab_insert(&p->symbols, t->dat.string, p->ilist.length);
		if (t[1].token != ')') {
			plnerror(p->line, "expected ')' after identifier\n");
		} else if (t[2].token != TOK_NEWLINE) {
			plnerror(p->line, "expected newline after label\n");
		}
	} else {
		plnerror(p->line, "expected identifier after '('\n");
	}
}

uint16_t parse_dest(parser_ctx *p, const char *str) {
	uint16_t dest = 0x0000;

	if (strlen(str) < 1 || strlen(str) > 3) {
		plnerror(p->line, "invalid dest mnemonic\n");
	}

	for (const char *cp = str; *cp != '\0'; cp++) {
		if (*cp == 'A') {
			dest = dest | A_DEST_MASK;
		} else if (*cp == 'M') {
			dest = dest | M_DEST_MASK;
		} else if (*cp == 'D') {
			dest = dest | D_DEST_MASK;
		} else {
			plnerror(p->line, "invalid dest mnemonic\n");
		}
	}

	return dest;
}


