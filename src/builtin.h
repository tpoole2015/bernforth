#ifndef BERNFORTH_BUILTIN_H
#define BERNFORTH_BUILTIN_H

#include "types.h"

// built-in words come in two flavours, atomic and non-atomic.
// atomic words
enum {
  ADD,
  BASE,
  BRANCH,
  CREATE,
  COMMA,
  DECIMAL,
  DOCOL,
  DOT,
  DUP,
  EXIT,
  HIDDEN,
  HEX,
  IMMEDIATE,
  INTERPRET,
  LBRAC,
  LIT,
  MUL,
  RBRAC,
  SUB,
  WORD
};

void add_atomic_words(Dictionary *d);
void add_nonatomic_words(Dictionary *d);

#endif


