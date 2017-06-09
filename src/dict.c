#include "dict.h"
#include "tok.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

boolean dict_init(Dictionary *d, const char *fn)
{
  d->here = (cell *)malloc(DICT_INIT_SIZE_CELLS * sizeof(cell));
  if (!d->here) {
    fprintf(stderr, "Error allocating memory for dictionary.\n");
    return FALSE;
  }
  d->latest = NULL;
  d->cells_remaining = DICT_INIT_SIZE_CELLS;

  // open file where we'll write the contents of the dictionary to
  if ( (d->fp = fopen(fn, "w")) == NULL) {
    fprintf(stderr, "error opening file %s\n", fn);
    return FALSE;
  }
 
  return TRUE;
}

Word *dict_get_word(const Dictionary *d, const Token *t)
{
  Word *cur = d->latest;
  while (cur) {
    if (!(cur->props.flags & F_HIDDEN) && tok_cmp(t, &(cur->props.tok)))
      break;
    cur = cur->prev;
  }
  return cur;
}

cell *dict_append_word(Dictionary *d, const WordProps *props)
{
#define CELLS_PER_WORD sizeof(Word)/sizeof(cell)
  assert(d->cells_remaining > CELLS_PER_WORD);
  Word w;
  w.prev = d->latest;
  w.props = *props;
  w.codeword_p = d->here + CELLS_PER_WORD;

  d->latest = memcpy(d->here, &w, sizeof(Word));
  char str[TOK_LEN+1] = {0};
  memcpy(str, props->tok.buf, props->tok.size);
  fprintf(d->fp, "%llX:%s\n", (int64_t)d->here, str);

  d->here += CELLS_PER_WORD;
  d->cells_remaining -= CELLS_PER_WORD;
  return d->here;
}

void dict_append_cell(Dictionary *d, const cell data)
{
  *(d->here) = data;
  fprintf(d->fp, "%llX:%llX\n", (int64_t)d->here, (int64_t)data);

  ++d->here;
  --d->cells_remaining;  
}
