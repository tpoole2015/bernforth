#ifndef BERNFORTH_BUILTIN_H
#define BERNFORTH_BUILTIN_H

#include "types.h"

// built-in words come in two flavours, atomic and non-atomic.
// atomic words
enum {
  ADD,
  BASE,
  BRANCH,
  BRANCHCOND,
  CHAR,
  CREATE,
  COMMA,
  DECIMAL,
  DIVMOD,
  DOCOL,
  DOT,
  DROP,
  DUP,
  EMIT,
  EXIT,
  FETCH,
  FIND,
  HERE,
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
  SWAP,
  TOCFA,
  WORD,
  ZEQU
};

void add_atomic_words(Dictionary *d);
void add_nonatomic_words(Dictionary *d);

#endif


