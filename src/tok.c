#include "tok.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void tok_init(Token *t)
{
  memset(t->buf, 0, TOK_LEN);
  t->size = 0;
}

void tok_cpy(Token *dst, const char *src, const unsigned int size)
{
  memcpy(dst->buf, src, size);
  dst->size = size;
}

boolean tok_cmp(const Token *lhs, const Token *rhs)
{
  return ((lhs->size == rhs->size) && !memcmp(lhs->buf, rhs->buf, lhs->size)) ? TRUE : FALSE;
}

boolean tok_tonum(const Token *t, const unsigned int base, int64_t *n)
{
  char str[TOK_LEN+1] = {0};
  memcpy(str, t->buf, t->size);
  const int64_t x = strtoll(str, NULL, base);
  if (!x && str[0] != '0')
      return FALSE;
  *n = x;
  return TRUE;
}

unsigned int tok_get_next(FILE *fp, Token *tok)
{
  read_state state = BLANK;
  char c;

  tok_init(tok);
  while ((c = fgetc(fp)) != EOF) {
    switch (state) {
    case COMMENT: {
       if (c == '\n')
         state = BLANK;
       break;
    }
    case BLANK: {
      if (c == FORTH_COMMENT)
        state = COMMENT;
      else if (!(c == ' ' || c == '\t' || c == '\n'))
        state = WRITING;
      break;
    }
    case WRITING: {
      if (c == ' ' || c == '\t' || c == '\n') {
        return tok->size; // finished reading token
      }
      break;
    }
    };

    if (state == WRITING)
      tok->buf[tok->size++] = c;
  }
  return tok->size;
}
