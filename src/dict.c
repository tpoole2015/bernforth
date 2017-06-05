#include "dict.h"
#include "tok.h"
#include <assert.h>

Word *dict_get_word(const Dictionary *d, const Token *t)
{
  Word *cur = d->latest;
  while (cur) {
    if (!(cur->props.flags & F_HIDDEN) && tok_cmp(tok, &(cur->props.tok)))
      return cur;
    cur = cur->prev;
  }
  return NULL;
}

Word *dict_append_word(Dictionary *d, const WordProps *props)
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
}

void dict_append_cell(Dictionary *d, const cell data)
{
  *(d->here) = data;
  ++d->here;
  --d->cells_remaining;  
}
