#ifndef BERNFORTH_TYPES_H
#define BERNFORTH_TYPES_H

#include <stdint.h>

typedef uintptr_t cell; // cell has to be large enough to hold the address of any variable

typedef enum {FALSE, TRUE} boolean;
typedef enum {BLANK, COMMENT, WRITING} read_state;
typedef enum {F_NOTSET, F_IMMED = 2, F_HIDDEN = 4} word_flags;
typedef enum {EXECUTE, COMPILE} interpreter_state;

// unlike C strings Forth strings are not null terminated
typedef struct {
#define TOK_LEN 32 
  char buf[TOK_LEN];
  unsigned int size;
} Token;

// properties intrinsic to a Word, irrespective of its position in the Dictionary
typedef struct {
  Token tok;
  unsigned int flags;
} WordProps;

typedef struct {
  WordProps props;
  struct Word *prev;
  cell *codeword_p;
} Word;

typedef struct {
  Word *latest;
  cell *here;
  uint64_t cells_remaining;
} Dictionary;

#endif


