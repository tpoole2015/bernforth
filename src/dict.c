#include "dict.h"
#include "tok.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

boolean dict_init(Dictionary *d)
{
  d->here = (cell *)malloc(DICT_INIT_SIZE_CELLS * sizeof(cell));
  if (!d->here) {
    fprintf(stderr, "Error allocating memory for dictionary.\n");
    return FALSE;
  }
  d->latest = NULL;
  d->cells_remaining = DICT_INIT_SIZE_CELLS;
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
  d->here += CELLS_PER_WORD;
  d->cells_remaining -= CELLS_PER_WORD;
  return d->here;
}

void dict_append_cell(Dictionary *d, const cell data)
{
  *(d->here) = data;
  ++d->here;
  --d->cells_remaining;  
}
