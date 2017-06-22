#include "types.h"
#include "dict.h"
#include "tok.h"
#include "stack.h"
#include "mem.h"
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define STACK_SIZE 100

Dictionary d; 
#define ADD_ATOMIC(label, buf, flags)\
cell *CWP_##label;                        \
{                                         \
  const Token t = {(char)(sizeof(buf)-1), buf};\
  CWP_##label = dict_append_word(&d, flags, &t); \
  dict_append_cell(&d, (cell)&&label);    \
}                                         \

#define COMMA(x) dict_append_cell(&d, (cell)x);
#define CWP(x) dict_append_cell(&d, (cell)CWP_##x);

int main(int argc, char *argv[])
{
  Token itok; // latest token read by INTERPRET
  Token wtok; // latest token read by WORD
  Word w;     // last word looked up 
  cell a, b, c; // temporary registers
  cell *W; // working register
  cell *IP; // interpreter pointer
  cell base = 10;
  cell state = EXECUTE; 

  INIT_STACK(RS, cell, STACK_SIZE)
  INIT_STACK(PS, cell, STACK_SIZE)

  if (!dict_init(&d, ".bernforth_dict"))
    return 1;

  ADD_ATOMIC(ADD, "+", F_NOTSET)
  ADD_ATOMIC(BASE, "BASE", F_NOTSET)
  ADD_ATOMIC(BRANCH, "BRANCH", F_NOTSET)
  ADD_ATOMIC(BRANCHCOND, "0BRANCH", F_NOTSET)
  ADD_ATOMIC(CHAR, "CHAR", F_NOTSET)
  ADD_ATOMIC(CREATE, "CREATE", F_NOTSET)
  ADD_ATOMIC(COMMA, ",", F_NOTSET)
  ADD_ATOMIC(DIVMOD, "/MOD", F_NOTSET)
  ADD_ATOMIC(DOCOL, "DOCOL", F_NOTSET)
  ADD_ATOMIC(DROP, "DROP", F_NOTSET)
  ADD_ATOMIC(DUP, "DUP", F_NOTSET)
  ADD_ATOMIC(EMIT, "EMIT", F_NOTSET)
  ADD_ATOMIC(EXIT, "EXIT", F_NOTSET)
  ADD_ATOMIC(FETCH, "@", F_NOTSET)
  ADD_ATOMIC(FIND, "FIND", F_NOTSET)
  ADD_ATOMIC(HERE, "HERE", F_NOTSET)
  ADD_ATOMIC(HIDE, "HIDE", F_NOTSET)
  ADD_ATOMIC(HIDDEN, "HIDDEN", F_NOTSET)
  ADD_ATOMIC(IMMEDIATE, "IMMEDIATE", F_IMMED)
  ADD_ATOMIC(INTERPRET, "INTERPRET", F_NOTSET)
  ADD_ATOMIC(LATEST, "LATEST", F_NOTSET)
  ADD_ATOMIC(LBRAC, "[", F_IMMED)
  ADD_ATOMIC(LIT, "LIT", F_NOTSET)
  ADD_ATOMIC(MUL, "*", F_NOTSET)
  ADD_ATOMIC(OVER, "OVER", F_NOTSET)
  ADD_ATOMIC(RBRAC, "]", F_NOTSET)
  ADD_ATOMIC(STORE, "!", F_NOTSET)
  ADD_ATOMIC(SUB, "-", F_NOTSET)
  ADD_ATOMIC(SWAP, "SWAP", F_NOTSET)
  ADD_ATOMIC(TOCFA, ">CFA", F_NOTSET)
  ADD_ATOMIC(WORD, "WORD", F_NOTSET)
  ADD_ATOMIC(ZEQU, "0=", F_NOTSET)
  ADD_ATOMIC(GT, ">", F_NOTSET)
  ADD_ATOMIC(GTEQ, ">=", F_NOTSET)
  ADD_ATOMIC(LT, "<", F_NOTSET)
  ADD_ATOMIC(LTEQ, "<=", F_NOTSET)
  ADD_ATOMIC(EQ, "=", F_NOTSET)
  ADD_ATOMIC(KEY, "KEY", F_NOTSET)
  ADD_ATOMIC(DSPBOT, "DSP@", F_NOTSET)
  ADD_ATOMIC(DSPTOP, "DSP0", F_NOTSET)
  ADD_ATOMIC(ROT, "ROT", F_NOTSET)
  ADD_ATOMIC(NROT, "-ROT", F_NOTSET)
  ADD_ATOMIC(AND, "AND", F_NOTSET)
  ADD_ATOMIC(OR, "OR", F_NOTSET)
  ADD_ATOMIC(INVERT, "INVERT", F_NOTSET)
  ADD_ATOMIC(STATE, "STATE", F_NOTSET)
  ADD_ATOMIC(CSTORE, "C!", F_NOTSET)
  ADD_ATOMIC(CFETCH, "C@", F_NOTSET)
  ADD_ATOMIC(TWODUP, "2DUP", F_NOTSET)
  ADD_ATOMIC(LITSTRING, "LITSTRING", F_NOTSET)
  ADD_ATOMIC(TELL, "TELL", F_NOTSET)
  ADD_ATOMIC(FIMMED, "F_IMMED", F_NOTSET)
  ADD_ATOMIC(FHIDDEN, "F_HIDDEN", F_NOTSET)

// : QUIT INTERPRET BRANCH -2 ;
  const Token quit = {4, "QUIT"};
  dict_append_word(&d, F_NOTSET, &quit);
  COMMA(&&DOCOL)
  CWP(INTERPRET)
  CWP(BRANCH)
  COMMA(-2) // go back 2 cells

// : 
  const Token colon= {1, ":"};
  dict_append_word(&d, F_NOTSET, &colon);
  COMMA(&&DOCOL)
  CWP(WORD)
  CWP(CREATE)
  CWP(HIDE)
  CWP(RBRAC)
  CWP(EXIT)

// ; 
  const Token semicolon= {1, ";"};
  dict_append_word(&d, F_IMMED, &semicolon);
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
  goto *(void*)*W; \

  // Start by running the QUIT word
  dict_lookup_word(&d, &quit, &w);
  IP = w.cwp;
  W = IP;
  goto *(void*)(*(cell*)W);

FIMMED: // ( -- )
  P(FIMMED)
  PUSH(PS, (cell)F_IMMED)
  NEXT

FHIDDEN: // ( -- )
  P(FHIDDEN)
  PUSH(PS, (cell)F_HIDDEN)
  NEXT

TELL: // ( addr len -- )
  P(TELL)
  POP(PS, a) // a = len
  POP(PS, b) // b = addr 
  write(1, (char*)b, a);
  NEXT

LITSTRING: // ( -- addr len )
  P(LITSTRING)
{
  const uint64_t byte_len = *IP++; // length of string in bytes
  PUSH(PS, (cell)IP)
  IP += ALIGN8(byte_len);
  PUSH(PS, (cell)byte_len)
}
  NEXT

TWODUP: // 2DUP ( a b -- a b a b )
  P(2DUP)
  POP(PS, b) 
  POP(PS, a) 
  PUSH(PS, a) 
  PUSH(PS, b) 
  PUSH(PS, a) 
  PUSH(PS, b) 
  NEXT

CFETCH: // C@ ( addr -- char )
  P(C@)
  POP(PS, a)
{
  char * const c = (char*)a;
  PUSH(PS, (cell)*c)
}
  NEXT

CSTORE: // C! ( char addr -- )
  P(C!)
  POP(PS, a) // a = addr
  POP(PS, b) // b = char
{
  char * const c = (char*)a;
  *c = (char)b;
}
  NEXT

STATE: // ( -- addr )
  P(STATE)
  PUSH(PS, (cell)&state)
  NEXT

INVERT: // ( a -- ~a )
  P(AND)
  POP(PS, a)
  PUSH(PS, ~a)
  NEXT

OR: // ( a b -- a|b )
  P(AND)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, a|b)
  NEXT

AND: // ( a b -- a&b )
  P(AND)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, a&b)
  NEXT

NROT: // ( c b a -- a c b )
  P(-ROT)
  POP(PS, a)
  POP(PS, b)
  POP(PS, c)
  PUSH(PS, a)
  PUSH(PS, c)
  PUSH(PS, b)
  NEXT

ROT : // ( a b c -- b c a )
  P(ROT)
  POP(PS, c)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, b)
  PUSH(PS, c)
  PUSH(PS, a)
  NEXT

LTEQ: // <= ( a b -- a<=b)
  P(GT)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, (cell)(a<=b))
  NEXT

GTEQ: // >= ( a b -- a>=b)
  P(GT)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, (cell)(a>=b))
  NEXT

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

DSPTOP: // DSP0 ( -- addr )
  P(DSP0)
{
  cell *sp;
  DSPTOP(PS, sp)
  PUSH(PS, (cell)sp)
}
  NEXT

DSPBOT: // DSP@ ( -- addr )
  P(DSP@)
{
  cell *sp;
  DSPBOT(PS, sp)
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
  Token tok;
  tok_cpy(&tok, (char*)b, (unsigned int)a);
  dict_append_word(&d, F_NOTSET, &tok);
  COMMA(&&DOCOL)
}
  NEXT

DIVMOD: // ( a b -- a%b a/b )
  P(DIVMOD)
  POP(PS, b)
  POP(PS, a)
  PUSH(PS, a%b)
  PUSH(PS, (cell)(a/b))
  NEXT

DOCOL:
  P(DOCOL)
  PUSH(RS, (cell)IP)
  IP = ++W;
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
  write(1, (char*)&a, 1);
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
  PUSH(PS, (cell)(dict_lookup_word(&d, &t, &w)))
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
{
  char *flag_ptr = GET_FLAGPTR(d.latest);
  *flag_ptr ^= F_HIDDEN;
}
  NEXT

HIDDEN: // ( addr -- )
  P(HIDDEN)
  POP(PS, a)
{
  char *flag_ptr = GET_FLAGPTR((cell*)a);
  *flag_ptr ^= F_HIDDEN;
}
  NEXT

IMMEDIATE: // ( -- )
  P(IMMEDIATE)
{
  char *flag_ptr = GET_FLAGPTR(d.latest);
  *flag_ptr ^= F_IMMED;
}
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
  Word w;
  int64_t n;
  if (!dict_lookup_word(&d, &itok, &w)) {
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
    W = w.cwp;
    if (state == EXECUTE || (w.flags & F_IMMED)) {
      if (state == COMPILE) fprintf(d.fp, "%s\n", str);
      goto *(void*)*W; // word has to be executed immediately
    }
  }

  P(COMPILING)
  if (islit) {
    fprintf(d.fp, "LIT ");     CWP(LIT)
    fprintf(d.fp, "%lld ", n); COMMA(n)
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
  PUSH(PS, (cell)(read_word((cell*)a, &w)))
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
     
