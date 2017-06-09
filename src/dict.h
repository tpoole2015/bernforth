#ifndef BERNFORTH_DICT_H
#define BERNFORTH_DICT_H

#include "types.h"

#define DICT_INIT_SIZE_CELLS 65536

// allocate the memory used by the dictionary
boolean dict_init(Dictionary *d, const char *fn);

// returns pointer to word with given token, NULL if word couldn't be found
Word *dict_get_word(const Dictionary *d, const Token *t);

// append word to dictionary, returns the codeword pointer
cell *dict_append_word(Dictionary *d, const WordProps *props);

// append a cell to the dictionary
void dict_append_cell(Dictionary *d, const cell data);

#endif
