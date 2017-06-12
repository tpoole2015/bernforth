CFLAGS=-g
CC=gcc
OBJS=src/dict.o\
     src/tok.o\
     src/main.o\
  
all: bernforth
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

bernforth: $(OBJS)
	$(CC) -I. $(CFLAGS) $(OBJS) -o $@ 

clean:
	rm -r src/*.o

.PHONY: all clean

