#include "builtin.h"
#include "dict.h"

static const WordProps ATOMIC_WORDS[] = {
  {{"+", 1}, F_NOTSET},
  {{"BASE", 4}, F_NOTSET},
  {{"BRANCH", 6}, F_NOTSET},
  {{"0BRANCH", 7}, F_NOTSET},
  {{"CHAR", 4}, F_NOTSET},
  {{"CREATE", 6}, F_NOTSET},
  {{",", 1}, F_NOTSET},
  {{"DECIMAL", 7}, F_NOTSET},
  {{"/MOD", 4}, F_NOTSET},
  {{"DOCOL", 5}, F_NOTSET},
  {{".", 1}, F_NOTSET},
  {{"DROP", 4}, F_NOTSET},
  {{"DUP", 3}, F_NOTSET},
  {{"EMIT", 4}, F_NOTSET},
  {{"EXIT", 4}, F_NOTSET},
  {{"@", 1}, F_NOTSET},
  {{"FIND", 4}, F_NOTSET},
  {{"HEX", 3}, F_NOTSET},
  {{"HIDE", 4}, F_NOTSET},
  {{"HIDDEN", 6}, F_NOTSET},
  {{"IMMEDIATE", 9}, F_IMMED},
  {{"INTERPRET", 9}, F_NOTSET},
  {{"LATEST", 6}, F_NOTSET},
  {{"[", 1}, F_IMMED}, // note the immdediate!!!
  {{"LIT", 3}, F_NOTSET},
  {{"*", 1}, F_NOTSET},
  {{"]", 1}, F_NOTSET},
  {{"!", 1}, F_NOTSET},
  {{"-", 1}, F_NOTSET},
  {{"SWAP", 4}, F_NOTSET},
  {{">CFA", 4}, F_NOTSET},
  {{"WORD", 4}, F_NOTSET},
  {{"0=", 2}, F_NOTSET}
};

void add_atomic_words(Dictionary *d)
{
  for(size_t i = 0; i < sizeof(ATOMIC_WORDS)/sizeof(WordProps); ++i) {
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
  dict_append_cell(d, (cell)x); \

#define LIT(x) \
  dict_append_cell(d, (cell)dict_get_word(d, &(ATOMIC_WORDS[LIT].tok))->codeword_p); \
  dict_append_cell(d, (cell)x); \

// : QUIT INTERPRET BRANCH -2 ;
  const WordProps quit = {{"QUIT", 4}, F_NOTSET};
  dict_append_word(d, &quit);
  VAL(DOCOL)
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
