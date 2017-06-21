#include "dict.h"
#include "tok.h"
#include "mem.h"
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

cell *dict_lookup_word(const Dictionary *d, const Token *t, Word *w)
{
  cell *cur = d->latest;
  while (cur) {
    w->cwp = read_word(cur, w);
    if (!(w->flags & F_HIDDEN) && tok_cmp(t, &(w->tok)))
      return cur;
    cur = w->prev;
  }
  return NULL;
}

cell *dict_append_word(Dictionary *d, const char flags, const Token *tok)
{
#define CELLS_PER_WORD sizeof(Word)/sizeof(cell)
  assert(d->cells_remaining > CELLS_PER_WORD);
  Word w;
  w.prev = d->latest;
  w.flags = flags;
  w.tok = *tok;

  d->latest = d->here; 
  d->here = write_word(d->here, &w);
  d->cells_remaining -= (d->here - d->latest);

  char str[TOK_LEN+1] = {0};
  memcpy(str, tok->buf, tok->size);
  fprintf(d->fp, "%llX:%s\n", (int64_t)d->here, str);

  return d->here;
}

void dict_append_cell(Dictionary *d, const cell data)
{
  *(d->here) = data;
  fprintf(d->fp, "%llX:%lld\n", (int64_t)d->here, (int64_t)data);

  ++d->here;
  --d->cells_remaining;  
}
