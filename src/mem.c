#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define WRITE_BYTE(dst, x)\
  *(char*)dst = (x);\
  dst = (cell*)((char*)dst+1);\

#define READ_BYTE(src, x)\
  x = *(char*)src;\
  src = (cell*)((char*)src+1);\

#define WRITE_CELL(dst, x) *dst++ = (x);
#define READ_CELL(src, x) x = *src++;

cell ALIGN8(cell x)
{
  return (x+7)&~(cell)7;
}

cell *write_bytes(cell *dst, const char *bytes, const int n)
{
  memcpy(dst, bytes, n);
  return (cell*)((char*)dst + n);
}

cell *read_bytes(const cell *src, char *bytes, const int n)
{
  memcpy(bytes, src, n);
  return (cell*)((char*)src + n);
}

cell *write_word(cell *dst, const Word *w)
{
  WRITE_CELL(dst, (cell)w->prev)
  WRITE_BYTE(dst, w->flags)
  WRITE_BYTE(dst, w->tok.size)
  dst = write_bytes(dst, w->tok.buf, (int)w->tok.size);
  return (cell*)ALIGN8((cell)dst);
}

cell *read_word(const cell *src, Word *w)
{
  cell c;
  READ_CELL(src, c)
  w->prev = (cell*)c;
  READ_BYTE(src, w->flags)
  READ_BYTE(src, w->tok.size)
  src = read_bytes(src, (char*)w->tok.buf, (int)w->tok.size);
  return (cell*)ALIGN8((cell)src);
}
