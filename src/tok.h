#ifndef BERNFORTH_TOK_H
#define BERNFORTH_TOK_H

#include "types.h"
#include <stdio.h>

#define FORTH_COMMENT '\\'

void tok_init(Token *t);
void tok_cpy(Token *dst, const char *src, const unsigned int size);
boolean tok_cmp(const Token *lhs, const Token *rhs);
boolean tok_tonum(const Token *t, const unsigned int base, int64_t *n);

// returns size of token (skips over comments and blank spaces)
unsigned int tok_get_next(FILE *fp, Token *tok);

#endif

