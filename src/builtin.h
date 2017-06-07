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
  FETCH,
  FIND,
  HEX,
  HIDE,
  HIDDEN,
  IMMEDIATE,
  INTERPRET,
  LATEST,
  LBRAC,
  LIT,
  MUL,
  RBRAC,
  STORE,
  SUB,
  WORD
};

void add_atomic_words(Dictionary *d);
void add_nonatomic_words(Dictionary *d);

#endif


