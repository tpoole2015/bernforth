#ifndef BERNFORTH_MEM_H
#define BERNFORTH_MEM_H

#include "types.h"

#define WRITE_BYTE(dst, x)\
  *(char*)dst = (x);\
  dst = (cell*)((char*)dst+1);\

#define READ_BYTE(src, x)\
  x = *(char*)src;\
  src = (cell*)((char*)src+1);\

#define WRITE_CELL(dst, x) *dst++ = (x);
#define READ_CELL(src, x) x = *src++;

cell ALIGN8(cell x);

cell *write_bytes(cell *dst, const char *bytes, const int n);
cell *read_bytes(const cell *src, char *bytes, const int n);

cell *write_word(cell *dst, const Word *w); 
cell *read_word(const cell *src, Word *w);

#endif


