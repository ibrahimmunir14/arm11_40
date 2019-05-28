CC      = gcc
CFLAGS  = -Wall -g -D_DEFAULT_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
LDLIBS = -lm

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o

emulate: emulate.o binaryOps.o
	$(CC) emulate.o binaryOps.o -o emulate $(LDLIBS)

emulate.o: emulate.h emulate.c binaryOps.h
	$(CC) $(CFLAGS) emulate.c -c -o emulate.o

binaryOps.o: binaryOps.h binaryOps.c
	$(CC) $(CFLAGS) binaryOps.c -c -o binaryOps.o

binaryOps.h: binaryTypes.h

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
