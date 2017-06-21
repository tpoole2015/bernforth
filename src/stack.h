#ifndef BERNFORTH_STACK_H
#define BERNFORTH_STACK_H

#include <stdio.h>

#define INIT_STACK(x, type, size)\
  type STACK_##x[(size)];        \
  const int SIZE_##x = (size);   \
  int TOP_##x = -1;              \

#define DSPTOP(x, ptr)\
  ptr = STACK_##x;    \

#define DSPBOT(x, ptr)      \
  ptr = STACK_##x + TOP_##x;\

#define PUSH(x, elem)                                \
  if (TOP_##x == SIZE_##x - 1)                       \
    fprintf(stderr, "ERROR:" #x " stack overflow\n");\
  else                                               \
    STACK_##x[++TOP_##x] = (elem);                   \

#define POP(x, elem)                                  \
  if (TOP_##x < 0)                                    \
    fprintf(stderr, "ERROR:" #x " stack underflow\n");\
  else                                                \ 
    (elem) = STACK_##x[TOP_##x--];\

#define PEAK(x, elem)         \
  (elem) = STACK_##x[TOP_##x];\

#endif


