#include "tok.h"
#include <stdlib.h>
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

boolean tok_tonum(const Token *t, const unsigned int base, long int *n)
{
  char str[TOK_LEN+1] = {0};
  memcpy(str, t->buf, t->size);
  const long int x = strtol(str, NULL, base);
  if (!x && str[0] != '0')
      return FALSE;
  *n = x;
  return TRUE;
}

// maybe replace with Cs getc for thread safety?
boolean get_key(char *c)
{
  // not thread safe!
  static Token t;
  static unsigned int pos = 0; // pos where next char will be read from
  static boolean init = FALSE;
  if (!init) {
    tok_init(&t);
    init = TRUE;
  }

  // try and read char from buf
  if (t.size && pos < t.size) {
    *c = t.buf[pos++];
    return TRUE;
  }

  // buf empty or we've read all chars in it
  pos = 0;
  t.size = read(STDIN_FILENO, t.buf, TOK_LEN);
  return t.size ? get_key(c) : FALSE;
}

// not thread safe!
unsigned int get_next_tok(Token *tok)
{
  read_state state = BLANK;
  char c;

  tok_init(tok);
  while (get_key(&c)) {
    switch (state) {
    case COMMENT: {
       if (c == '\n')
         state = BLANK;
       break;
    }
    case BLANK: {
      if (c == FORTH_COMMENT)
        state = COMMENT;;
      else if (!(c == ' ' || c == '\t' || c == '\n'))
        state = WRITING;
      break;
    }
    case WRITING: {
      if (c == ' ' || c == '\t' || c == '\n' || c == FORTH_COMMENT)
        return tok->size; // finished reading token
      break;
    }
    };

    if (state == WRITING)
      tok->buf[tok->size++] = toupper(c);
  }
  return tok->size;
}
