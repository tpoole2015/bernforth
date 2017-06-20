#ifndef BERNFORTH_TYPES_H
#define BERNFORTH_TYPES_H

#include <stdio.h>
#include <stdint.h>

#ifdef DEBUG
#define P(x) printf(#x"\n");
#else
#define P(x)
#endif

typedef int64_t cell; // cell has to be large enough to hold the address of any variable

typedef enum {FALSE, TRUE} boolean;
typedef enum {BLANK, COMMENT, WRITING} read_state;
typedef enum {F_NOTSET, F_IMMED = 2, F_HIDDEN = 4} word_flags;
typedef enum {EXECUTE, COMPILE} interpreter_state;

#define TOK_LEN 32 
typedef struct Word {
  struct Word *prev;
  char flags;
  char len;
// unlike C strings Forth strings are not null terminated
  char tok[TOK_LEN];
} Word;

typedef struct {
  FILE *fp;
  Word *latest;
  cell *here;
  uint64_t cells_remaining;
} Dictionary;

#endif


