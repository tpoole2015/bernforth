#include "builtin.h"
#include "dict.h"

void add_atomic_words(Dictionary *d)
{
  for(size_t i = 0; i < sizeof(ATOMIC_WORDS)/sizeof(wordProps); ++i) {
    dict_append_word(d, &ATOMIC_WORDS[i]);
    dict_append_cell(d, (cell)i);
    dict_append_cell(d, (cell)0);
  } 
}

void add_nonatomic_words(Dictionary *d)
{
#define CWP(x) \
  dict_append_cell(d, (cell)dict_get_word(d, &(ATOMIC_WORDS[x].tok))->codeword_p); \
#define VAL(x) \
  dict_append_cell(d, (cell)x) \
#define LIT(x) \
  dict_append_cell(d, (cell)dict_get_word(d, &(ATOMIC_WORDS[LIT].tok))->codeword_p); \
  dict_append_cell(d, (cell)x) \

// : QUIT INTERPRET BRANCH -8 ;
  const WordProps quit = {{"QUIT", 4}, F_NOTSET};
  dict_append_word(d, &quit);
  LIT(DOCOL)
  CWP(INTERPRET)
  CWP(BRANCH)
  VAL(-2) // go back 2 cells

// : 
  const WordProps colon= {{":", 1}, F_NOTSET};
  dict_append_word(d, &colon);
  VAL(DOCOL)
  CWP(WORD)
  CWP(CREATE)
  LIT(DOCOL)
  CWP(COMMA)
  CWP(RBRAC)
  VAL(0)

// ; 
  const WordProps semicolon= {{";", 1}, F_IMMED};
  dict_append_word(d, &semicolon);
  VAL(DOCOL)
  LIT(0)
  CWP(COMMA)
  CWP(LBRAC)
  VAL(0)
}
