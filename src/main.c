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
      POP(PS, a)
      POP(PS, b)
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
    case COMMA: // , ( a -- )
    {
      P(COMMA)
      POP(PS, a)
      dict_append_cell(&d, a);
      NEXT
      break;
    }
    case CREATE: // CREATE ( address len -- )
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
    case DOCOL:
    {
      P(DOCOL)
      PUSH(RS, (cell)IP)
      IP = ++W;
      NEXT
      break;
    }
    case DOT: // (a -- )
    {
      P(DOT)
      POP(PS, a)
      printf("\t%lld\n", (int64_t)a);
      NEXT  
      break;
    }
    case DUP: // (a -- a a)
    {
      P(DUP)
      PEAK(PS, a)
      PUSH(PS, a)
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
    case HIDDEN: // ( -- )
    {
      P(HIDDEN)
      d.latest->props.flags ^= F_HIDDEN;
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
      if (!get_next_tok(&itok)) {
        // EOF
        printf("GOOD BYE!\n");
        return 0;
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
    case SUB: // - ( a b -- a-b )
    {
      P(ADD)
      POP(PS, a)
      POP(PS, b)
      PUSH(PS, a - b)
      NEXT
      break;
    }
    case WORD: // ( -- address len )
    {
      P(WORD)
      if (!get_next_tok(&wtok)) {
        // EOF
        printf("GOOD BYE!\n");
        return 0;
      }
      PUSH(PS, (cell)wtok.buf)
      PUSH(PS, (cell)wtok.size)
      NEXT
      break;
    } 
    default:
      fprintf(stderr, "ERROR: invalid command %lld\n", (int64_t)*W);
      return 1;
    };
  goto main_loop;
} 
     
