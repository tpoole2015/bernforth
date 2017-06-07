#include "types.h"
#include "builtin.h"
#include "dict.h"
#include "tok.h"
#include "stack.h"
#include <stdint.h>
#include <string.h>

Dictionary d; 

int main(int argc, char *argv[])
{
  Token itok; // latest token read by INTERPRET
  Token wtok; // latest token read by WORD
  cell a, b; // temporary registers
  cell *W; // working register
  cell *IP; // interpreter pointer
  unsigned int base = 10;
  interpreter_state state = EXECUTE; 

#define STACK_SIZE 100
  INIT_STACK(RS, cell, STACK_SIZE)
  INIT_STACK(PS, cell, STACK_SIZE)

  if (!dict_init(&d))
    return 1;

  add_atomic_words(&d);
  add_nonatomic_words(&d);

  // Start by running the QUIT word
  const Token quit = {"QUIT", 4};
  IP = dict_get_word(&d, &quit)->codeword_p;
  W = IP;
  ++IP;

  const Token lit = {"LIT", 3};
  const cell *lit_cw_p = dict_get_word(&d, &lit)->codeword_p;

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
#define NEXT      \
  W = (cell*)*IP; \
  ++IP;           \

  // Lookup atmoic word
  switch (W ? (int64_t)(*W) : EXIT) {
    case ADD: // + ( a b -- a+b )
    {
      P(ADD)
      POP(PS, b)
      POP(PS, a)
      PUSH(PS, a + b)
      NEXT
      break;
    }
    case BASE: // ( -- base ) 
    {
      P(BASE)
      PUSH(PS, (cell)base)
      NEXT
      break;
    }
    case BRANCH: // ( -- )
    {
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
      break;
    }
    case BRANCHCOND: // 0BRANCH ( a -- )
    {
      P(BRANCHCOND)
      POP(PS, a)
      IP += a ? 1 /* skip offset */ : *IP;
      NEXT
      break;
    } 
    case CHAR: // ( -- char) word
    {
      P(CHAR)
      if (!tok_get_next(fp, &wtok)) {
        PROCESS_EOF
      }
      PUSH(PS, (cell)wtok.buf[0])
      NEXT
      break;
    } 
    case COMMA: // , ( a -- )
    {
      P(COMMA)
      POP(PS, a)
      dict_append_cell(&d, a);
      NEXT
      break;
    }
    case CREATE: // CREATE ( addr len -- )
    {
      P(CREATE)
      POP(PS, a) // a = len
      POP(PS, b) // b = address
      WordProps p;
      p.flags = F_NOTSET;
      tok_cpy(&p.tok, (char*)b, (unsigned int)a);
      dict_append_word(&d, &p);
      NEXT
      break;
    }
    case DECIMAL: // ( -- )
    {
      P(DECIMAL)
      base = 10;
      NEXT
      break;
    }
    case DIVMOD: // ( a b -- a%b a/b )
    {
      P(DIVMOD)
      POP(PS, b)
      POP(PS, a)
      PUSH(PS, a%b)
      PUSH(PS, a/b)
      NEXT
      break;
    }
    case DOCOL:
    {
      P(DOCOL)
      PUSH(RS, (cell)IP)
      IP = ++W;
      NEXT
      break;
    }
    case DOT: // ( a -- )
    {
      P(DOT)
      POP(PS, a)
      printf("\t%lld\n", (int64_t)a);
      NEXT  
      break;
    }
    case DROP: // ( a -- )
    {
      P(DROP)
      POP(PS, a)
      NEXT  
      break;
    }
    case DUP: // ( a -- a a )
    {
      P(DUP)
      PEAK(PS, a)
      PUSH(PS, a)
      NEXT
      break;
    }
    case EMIT: // ( char -- )
    {
      P(EMIT)
      POP(PS, a);
      printf("\t%c\n", (char)a);
      NEXT
      break;
    }
    case EXIT:
    {
      P(EXIT)
      POP(RS, a);
      IP = (cell*)a;
      NEXT
      break;
    }
    case FETCH: // @ ( addr -- n )
    {
      P(FETCH)
      POP(PS, a) 
      PUSH(PS, (cell)(*(cell*)a)) 
      NEXT
      break;
    }
    case FIND: // ( len addr -- addr )
    {
      P(FIND)
      POP(PS, a); // a = addr
      POP(PS, b); // b = len 
      Token t;
      tok_cpy(&t, (char*)b, (unsigned int)a);
      const Word *w = dict_get_word(&d, &t);
      PUSH(PS, (cell)w)
      NEXT
      break;
    }
    case HERE: // ( -- addr )
    {
      P(HERE)
      PUSH(PS, (cell)&(d.here))
      NEXT
      break;
    }
    case HEX: // ( -- )
    {
      P(HEX)
      base = 16;
      NEXT
      break;
    }
    case HIDE: // ( -- )
    {
      P(HIDE)
      d.latest->props.flags ^= F_HIDDEN;
      NEXT
      break;
    }
    case HIDDEN: // ( addr -- )
    {
      P(HIDDEN)
      POP(PS, a)
      Word *w = (Word*)a;
      w->props.flags ^= F_HIDDEN;
      NEXT
      break;
    }
    case IMMEDIATE: // ( -- )
    {
      P(IMMEDIATE)
      d.latest->props.flags ^= F_IMMED;
      NEXT
      break;
    }
    case INTERPRET: // ( -- )
    {
      P(INTERPRET)
get_next_word:
      if (!tok_get_next(fp, &itok)) {
        PROCESS_EOF
      }

      boolean islit = FALSE;
      const Word *w = dict_get_word(&d, &itok);
      long int n;
      if (!w) {
        // word not in dictionary
        if (!tok_tonum(&itok, base, &n)) {
          char str[TOK_LEN+1] = {0};
          memcpy(str, itok.buf, itok.size);
          fprintf(stderr, "ERROR: couldn't parse %s\n", str);
          goto get_next_word;
        }

        islit = TRUE;
        if (EXECUTE == state) {
          PUSH(PS, (cell)n)
          NEXT
          break;
        }
      } else {
        // found word in dictionary
        W = w->codeword_p;
        if (state == EXECUTE || (w->props.flags & F_IMMED))
          break; // word has to be executed immediately
      }

      P(COMPILING)
      if (islit) {
        dict_append_cell(&d, (cell)lit_cw_p);
        dict_append_cell(&d, (cell)n);
      } else 
        dict_append_cell(&d, (cell)W);

      NEXT
      break;
    }
    case LBRAC: // [ ( -- )
    {
      P(LBRAC)
      state = EXECUTE;
      NEXT  
      break;
    }
    case LATEST: // ( -- addr )
    {
      P(LATEST)
      PUSH(PS, (cell)&(d.latest))
      NEXT
      break;
    }
    case LIT: // ( -- *IP )
    {
      P(LIT)
      PUSH(PS, *IP)
      ++IP;
      NEXT
      break;
    }
    case MUL: // * ( a b -- a*b )
    {
      P(MUL)
      POP(PS, b)
      POP(PS, a)
      PUSH(PS, a*b)
      NEXT
      break;
    }
    case RBRAC: // ] ( -- )
    {
      P(RBRAC)
      state = COMPILE;
      NEXT  
      break;
    }
    case STORE: // ! ( n addr -- )
    {
      P(STORE)
      POP(PS, a) // a = addr
      POP(PS, b) // b = n
      *(cell*)a = b;
      NEXT
      break;
    }
    case SUB: // - ( a b -- a-b )
    {
      P(SUB)
      POP(PS, b)
      POP(PS, a)
      PUSH(PS, a - b)
      NEXT
      break;
    }
    case SWAP: // ( a b -- b a )
    {
      P(SWAP)
      POP(PS, b)
      POP(PS, a)
      PUSH(PS, b)
      PUSH(PS, a)
      NEXT
      break;
    }
    case TOCFA: // >CFA ( addr -- addr )
    {
      P(TOCFA)
      POP(PS, a)
      PUSH(PS, (cell)((Word*)a)->codeword_p)
      NEXT
      break;
    }
    case WORD: // ( -- addr len ) word
    {
      P(WORD)
      if (!tok_get_next(fp, &wtok)) {
        PROCESS_EOF
      }
      PUSH(PS, (cell)wtok.buf)
      PUSH(PS, (cell)wtok.size)
      NEXT
      break;
    } 
    case ZEQU: // ( a -- !a )
    {
      P(ZEQU)
      POP(PS, a)
      PUSH(PS, !a)
      NEXT    
      break;
    }
    default:
      fprintf(stderr, "ERROR: invalid command %lld\n", (int64_t)*W);
      return 1;
    };
  goto main_loop;
} 
     
