#ifndef BERNFORTH_DICT_H
#define BERNFORTH_DICT_H

#include "types.h"

// returns pointer to word with given token, NULL if word couldn't be found
Word *dict_get_word(const Dictionary *d, const Token *t);

// append word to dictionary
Word *dict_append_word(Dictionary *d, const WordProps *props);

// append a cell to the dictionary
void dict_append_cell(Dictionary *d, const cell data);

#endif
