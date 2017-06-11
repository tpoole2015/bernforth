#include "types.h"
#include "dict.h"
#include "tok.h"
#include "stack.h"
#include <stdint.h>
#include <string.h>


int main(int argc, char *argv[])
{
  Token itok; // latest token read by INTERPRET
  Token wtok; // latest token read by WORD
  cell a, b; // temporary registers
  cell *W; // working register
  cell *IP; // interpreter pointer
  cell base = 10;
  interpreter_state state = EXECUTE; 

#define STACK_SIZE 100
  INIT_STACK(RS, cell, STACK_SIZE)
  INIT_STACK(PS, cell, STACK_SIZE)

  Dictionary d; 
  if (!dict_init(&d, ".bernforth_dict"))
    return 1;

#define ADD_ATOMIC(label, buf, len, flags)\
cell *CWP_##label;                        \
{                                         \
  const WordProps p = {{buf, len}, flags};\
  CWP_##label = dict_append_word(&d, &p); \
  dict_append_cell(&d, (cell)&&label);    \
}                                         \

#define COMMA(x) dict_append_cell(&d, (cell)x);
#define CWP(x) dict_append_cell(&d, (cell)CWP_##x);

  ADD_ATOMIC(ADD, "+", 1, F_NOTSET)
  ADD_ATOMIC(BASE, "BASE", 4, F_NOTSET)
  ADD_ATOMIC(BRANCH, "BRANCH", 6, F_NOTSET)
  ADD_ATOMIC(BRANCHCOND, "0BRANCH", 7, F_NOTSET)
  ADD_ATOMIC(CELLSIZE, "CELLSIZE", 8, F_NOTSET)
  ADD_ATOMIC(CHAR, "CHAR", 4, F_NOTSET)
  ADD_ATOMIC(CREATE, "CREATE", 6, F_NOTSET)
  ADD_ATOMIC(COMMA, ",", 1, F_NOTSET)
  ADD_ATOMIC(DIVMOD, "/MOD", 4, F_NOTSET)
  ADD_ATOMIC(DOCOL, "DOCOL", 5, F_NOTSET)
  ADD_ATOMIC(DOT, ".", 1, F_NOTSET)
  ADD_ATOMIC(DROP, "DROP", 4, F_NOTSET)
  ADD_ATOMIC(DUP, "DUP", 3, F_NOTSET)
  ADD_ATOMIC(EMIT, "EMIT", 4, F_NOTSET)
  ADD_ATOMIC(EXIT, "EXIT", 4, F_NOTSET)
  ADD_ATOMIC(FETCH, "@", 1, F_NOTSET)
  ADD_ATOMIC(FIND, "FIND", 4, F_NOTSET)
  ADD_ATOMIC(GT, ">", 1, F_NOTSET)
  ADD_ATOMIC(HERE, "HERE", 4, F_NOTSET)
  ADD_ATOMIC(HIDE, "HIDE", 4, F_NOTSET)
  ADD_ATOMIC(HIDDEN, "HIDDEN", 6, F_NOTSET)
  ADD_ATOMIC(IMMEDIATE, "IMMEDIATE", 9, F_IMMED)
  ADD_ATOMIC(INTERPRET, "INTERPRET", 9, F_NOTSET)
  ADD_ATOMIC(LATEST, "LATEST", 6, F_NOTSET)
  ADD_ATOMIC(LBRAC, "[", 1, F_IMMED)
  ADD_ATOMIC(LIT, "LIT", 3, F_NOTSET)
  ADD_ATOMIC(MUL, "*", 1, F_NOTSET)
  ADD_ATOMIC(OVER, "OVER", 4, F_NOTSET)
  ADD_ATOMIC(RBRAC, "]", 1, F_NOTSET)
  ADD_ATOMIC(STORE, "!", 1, F_NOTSET)
  ADD_ATOMIC(SUB, "-", 1, F_NOTSET)
  ADD_ATOMIC(SWAP, "SWAP", 4, F_NOTSET)
  ADD_ATOMIC(TOCFA, ">CFA", 4, F_NOTSET)
  ADD_ATOMIC(WORD, "WORD", 4, F_NOTSET)
  ADD_ATOMIC(ZEQU, "0=", 2, F_NOTSET)
  ADD_ATOMIC(PRINTSTACK, ".S", 2, F_NOTSET)
  ADD_ATOMIC(LT, "<", 1, F_NOTSET)
  ADD_ATOMIC(EQ, "=", 1, F_NOTSET)
  ADD_ATOMIC(KEY, "KEY", 3, F_NOTSET)
  ADD_ATOMIC(DSPFETCH, "DSP@", 4, F_NOTSET)

// : QUIT INTERPRET BRANCH -2 ;
  const WordProps quit = {{"QUIT", 4}, F_NOTSET};
  dict_append_word(&d, &quit);
  COMMA(&&DOCOL)
  CWP(INTERPRET)
  CWP(BRANCH)
  COMMA(-2) // go back 2 cells

// : 
  const WordProps colon= {{":", 1}, F_NOTSET};
  dict_append_word(&d, &colon);
  COMMA(&&DOCOL)
  CWP(WORD)
  CWP(CREATE)
  CWP(LIT)
  COMMA(&&DOCOL)
  CWP(COMMA)
  CWP(HIDE)
  CWP(RBRAC)
  CWP(EXIT)

// ; 
  const WordProps semicolon= {{";", 1}, F_IMMED};
  dict_append_word(&d, &semicolon);
  COMMA(&&DOCOL)
  CWP(LIT)
  CWP(EXIT)
  CWP(COMMA)
  CWP(HIDE) 
  CWP(LBRAC)
  CWP(EXIT)

  FILE *fp = stdin;
  if (argc > 1) {
    if ( (fp = fopen(argv[1], "r")) == NULL) {
      fprintf(stderr, "error opening file %s\n", argv[1]);
      fp = stdin;
    } else 
      printf("loading file %s\n", argv[1]);
  }
#define PROCESS_EOF      \
  if (fp == stdin) {     \
    printf("GOOD BYE\n");\
    return 0;            \
  }                      \
  printf("loaded!\n");   \
  fp = stdin;            \
  goto main_loop;        \

main_loop:

// Every atomic word implemented has to end in NEXT
#define NEXT       \
  W = (cell*)*IP++;\
  goto *(void*)*W;\

  // Start by running the QUIT word
  IP = dict_get_word(&d, &quit.tok)->codeword_p;
  W = IP;
  goto *(void*)(*(cell*)W);

KEY: // ( -- char )
  P(KEY)
{
  const char c = fgetc(fp);
  if (c == EOF) {
    PROCESS_EOF 
  }
  PUSH(PS, (cell)c)
}
  NEXT

DSPFETCH: // DSP@ ( -- DSP )
  P(DSP@)
{
  cell *sp;
  DSPFETCH(PS, sp)
  PUSH(PS, (cell)sp)
}
  NEXT

EQ: // ( a b -- a==b )
  P(EQ)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, (cell)(a==b))
  NEXT

LT: // < ( a b -- a<b)
  P(LT)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, (cell)(a<b))
  NEXT

PRINTSTACK:
  DUMP(PS)
  NEXT

ADD: // + ( a b -- a+b )
  P(ADD)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, a+b)
  NEXT

BASE: // ( -- addr ) 
  P(BASE)
  PUSH(PS, (cell)&base)
  NEXT

BRANCH: // ( -- )
  P(BRANCH)
  /*
    IP points toe the branch offset.
    i.e. our dictionary looks as follows
        ... | BRANCH | offset | ...
                         ^
                         |
                         IP
  */
  IP += *IP;
  NEXT

BRANCHCOND: // 0BRANCH ( a -- )
  P(BRANCHCOND)
  POP(PS, a)
  IP += a ? 1 /* skip offset */ : *IP;
  NEXT

CELLSIZE: // ( -- sizeof(cell))
  P(CELLSIZE)
  PUSH(PS, (cell)sizeof(cell));
  NEXT

CHAR: // ( -- char) word
  P(CHAR)
  if (!tok_get_next(fp, &wtok)) {
    PROCESS_EOF
  }
  PUSH(PS, (cell)wtok.buf[0])
  NEXT

COMMA: // , ( a -- )
  P(COMMA)
  POP(PS, a)
  dict_append_cell(&d, a);
  NEXT

CREATE: // CREATE ( addr len -- )
  P(CREATE)
  POP(PS, a) // a = len
  POP(PS, b) // b = address
{
  WordProps p;
  p.flags = F_NOTSET;
  tok_cpy(&p.tok, (char*)b, (unsigned int)a);
  dict_append_word(&d, &p);
}
  NEXT

DIVMOD: // ( a b -- a%b a/b )
  P(DIVMOD)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, a%b)
  PUSH(PS, (cell)((int64_t)a/(int64_t)b))
  NEXT

DOCOL:
  P(DOCOL)
  PUSH(RS, (cell)IP)
  IP = ++W;
  NEXT

DOT: // ( a -- )
  P(DOT)
  POP(PS, a)
  printf("\t%lld\n", (int64_t)a);
  NEXT  

DROP: // ( a -- )
  P(DROP)
  POP(PS, a)
  NEXT  

DUP: // ( a -- a a )
  P(DUP)
  PEAK(PS, a)
  PUSH(PS, a)
  NEXT

EMIT: // ( char -- )
  P(EMIT)
  POP(PS, a);
  printf("%c", (char)a);
  NEXT

EXIT:
  P(EXIT)
  POP(RS, a);
  IP = (cell*)a;
  NEXT

FETCH: // @ ( addr -- n )
  P(FETCH)
  POP(PS, a) 
  PUSH(PS, (cell)(*(cell*)a)) 
  NEXT

FIND: // ( len addr -- addr )
  P(FIND)
  POP(PS, a); // a = addr
  POP(PS, b); // b = len 
{
  Token t;
  tok_cpy(&t, (char*)b, (unsigned int)a);
  const Word *w = dict_get_word(&d, &t);
  PUSH(PS, (cell)w)
}
  NEXT

GT: // > ( a b -- a>b)
  P(GT)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, (cell)(a>b))
  NEXT

HERE: // ( -- addr )
  P(HERE)
  PUSH(PS, (cell)&(d.here))
  NEXT

HIDE: // ( -- )
  P(HIDE)
  d.latest->props.flags ^= F_HIDDEN;
  NEXT

HIDDEN: // ( addr -- )
  P(HIDDEN)
  POP(PS, a)
{
  Word *w = (Word*)a;
  w->props.flags ^= F_HIDDEN;
}
  NEXT

IMMEDIATE: // ( -- )
  P(IMMEDIATE)
  d.latest->props.flags ^= F_IMMED;
  NEXT

INTERPRET: // ( -- )
  P(INTERPRET)
get_next_word:
{
  if (!tok_get_next(fp, &itok)) {
    PROCESS_EOF
  }

  char str[TOK_LEN+1] = {0};
  memcpy(str, itok.buf, itok.size);
 
  boolean islit = FALSE;
  const Word *w = dict_get_word(&d, &itok);
  long int n;
  if (!w) {
    // word not in dictionary
    if (!tok_tonum(&itok, base, &n)) {
      fprintf(stderr, "ERROR: couldn't parse %s\n", str);
      goto get_next_word;
    }

    islit = TRUE;
    if (EXECUTE == state) {
      PUSH(PS, (cell)n)
      NEXT
    }
  } else {
    // found word in dictionary
    W = w->codeword_p;
    if (state == EXECUTE || (w->props.flags & F_IMMED)) {
      if (state == COMPILE) fprintf(d.fp, "%s\n", str);
      goto *(void*)*W; // word has to be executed immediately
    }
  }

  P(COMPILING)
  if (islit) {
    fprintf(d.fp, "LIT ");     CWP(LIT)
    fprintf(d.fp, "%lld ", (int64_t)n); COMMA(n)
  } else {
    fprintf(d.fp, "%s ", str); COMMA(W)
  }

  NEXT
}

LBRAC: // [ ( -- )
  P(LBRAC)
  state = EXECUTE;
  NEXT  

LATEST: // ( -- addr )
  P(LATEST)
  PUSH(PS, (cell)&(d.latest))
  NEXT

LIT: // ( -- *IP )
  P(LIT)
  PUSH(PS, *IP)
  ++IP;
  NEXT

MUL: // * ( a b -- a*b )
  P(MUL)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, a*b)
  NEXT

OVER: // ( a b -- a b a )
  P(OVER)
  POP(PS, b)
  PEAK(PS, a)
  PUSH(PS, b)
  PUSH(PS, a)
  NEXT

RBRAC: // ] ( -- )
  P(RBRAC)
  state = COMPILE;
  NEXT  

STORE: // ! ( n addr -- )
  P(STORE)
  POP(PS, a) // a = addr
  POP(PS, b) // b = n
  *(cell*)a = b;
  NEXT

SUB: // - ( a b -- a-b )
  P(SUB)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, a - b)
  NEXT

SWAP: // ( a b -- b a )
  P(SWAP)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, b)
  PUSH(PS, a)
  NEXT

TOCFA: // >CFA ( addr -- addr )
  P(TOCFA)
  POP(PS, a)
  PUSH(PS, (cell)((Word*)a)->codeword_p)
  NEXT

WORD: // ( -- addr len ) word
  P(WORD)
  if (!tok_get_next(fp, &wtok)) {
    PROCESS_EOF
  }
  PUSH(PS, (cell)wtok.buf)
  PUSH(PS, (cell)wtok.size)
  NEXT

ZEQU: // ( a -- !a )
  P(ZEQU)
  POP(PS, a)
  PUSH(PS, !a)
  NEXT    
} 
     
