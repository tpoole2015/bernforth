#ifndef BERNFORTH_STACK_H
#define BERNFORTH_STACK_H

#include <stdio.h>

#define INIT_STACK(x, type, size)\
  type STACK_##x[(size)];        \
  const int SIZE_##x = (size);   \
  int TOP_##x = -1;              \

#define GET_ADDRESS_STACK(x, ptr)\
  ptr = (STACK_##x);             \

#define PUSH(x, elem)                                \
  if (TOP_##x == SIZE_##x - 1)                       \
    printf(stderr, "ERROR: stack " #x " overflow\n");\
  else                                               \ 
    STACK_##x[++TOP_##X] = (elem);                   \

#define POP(x, elem)                                  \
  if (TOP_##x < 0)                                    \
    printf(stderr, "ERROR: stack " #x " underflow\n");\
  else                                                \ 
    (elem) = STACK_##x[TOP_##X--];                    \

#define PEAK(x, elem)         \
  (elem) = STACK_##x[TOP_##x];\

#endif


