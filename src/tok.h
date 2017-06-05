#ifndef BERNFORTH_TOK_H
#define BERNFORTH_TOK_H

#include "types.h"

#define FORTH_COMMENT '\\'

void tok_init(Token *t);
void tok_cpy(Token *dst, const char *src, const unsigned int size);
boolean tok_cmp(const Token *lhs, const Token *rhs);
boolean tok_tonum(const Token *t, const unsigned int base, long int *n);

boolean get_key(char *c);
// returns size of token (skips over comments and blank spaces)
unsigned int get_next_tok(Token *tok);

#endif

