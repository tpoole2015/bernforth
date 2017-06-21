#ifndef BERNFORTH_DICT_H
#define BERNFORTH_DICT_H

#include "types.h"

#define DICT_INIT_SIZE_CELLS 65536

// allocate the memory used by the dictionary
boolean dict_init(Dictionary *d, const char *fn);

cell *dict_lookup_word(const Dictionary *d, const Token *t, Word *w);

// append word to dictionary, returns the codeword pointer
cell *dict_append_word(Dictionary *d, const char flags, const Token *tok);

// append a cell to the dictionary
void dict_append_cell(Dictionary *d, const cell data);

#endif
