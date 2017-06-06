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

WordProps ATOMIC_WORDS[] = {{
  {{"+", 1}, F_NOTSET},
  {{"BASE", 4}, F_NOTSET},
  {{"BRANCH", 6}, F_NOTSET},
  {{"CREATE", 6}, F_NOTSET},
  {{",", 1}, F_NOTSET},
  {{"DECIMAL", 7}, F_NOTSET},
  {{"DOCOL", 5}, F_NOTSET},
  {{".", 1}, F_NOTSET},
  {{"DUP", 3}, F_NOTSET},
  {{"EXIT", 4}, F_NOTSET},
  {{"HIDDEN", 6}, F_NOTSET},
  {{"HEX", 3}, F_NOTSET},
  {{"IMMEDIATE", 9}, F_NOTSET},
  {{"INTERPRET", 9}, F_NOTSET},
  {{"[", 1}, F_IMMED}, // note the immdediate!!!
  {{"LIT", 3}, F_NOTSET},
  {{"MUL", 3}, F_NOTSET},
  {{"]", 1}, F_NOTSET},
  {{"SUB", 3}, F_NOTSET},
  {{"WORD", 4}, F_NOTSET}
};

void add_atomic_words(Dictionary *d);
void add_nonatomic_words(Dictionary *d);

#endif


