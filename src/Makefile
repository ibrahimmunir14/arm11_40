CC      = gcc
CFLAGS  = -Wall -g -D_DEFAULT_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
LDLIBS = -lm

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o

emulate: emulate.o binaryOps.o binaryFileLoader.o instructionExecution.o instructionDecoding.o statusFlags.o memAccess.o
	$(CC) emulate.o binaryOps.o binaryFileLoader.o instructionExecution.o instructionDecoding.o statusFlags.o memAccess.o -o emulate $(LDLIBS)

emulate.o: emulate.c emulate.h binaryOps.h binaryFileLoader.h machine.h instructionExecution.h
	$(CC) $(CFLAGS) emulate.c -c -o emulate.o

binaryOps.o: binaryOps.c binaryOps.h 
	$(CC) $(CFLAGS) binaryOps.c -c -o binaryOps.o

binaryFileLoader.o: binaryFileLoader.c binaryFileLoader.h binaryTypes.h
	$(CC) $(CFLAGS) binaryFileLoader.c -c -o binaryFileLoader.o

instructionDecoding.o: instructionDecoding.c instructionDecoding.h binaryOps.h machine.h statusFlags.h
	$(CC) $(CFLAGS) instructionDecoding.c -c -o instructionDecoding.o

instructionExecution.o: instructionExecution.c instructionExecution.h machine.h instructionDecoding.h statusFlags.h binaryOps.h memAccess.h 
	$(CC) $(CFLAGS) instructionExecution.c -c -o instructionExecution.o

statusFlags.o: statusFlags.c statusFlags.h machine.h binaryOps.h 
	$(CC) $(CFLAGS) statusFlags.c -c -o statusFlags.o

memAccess.o: memAccess.c machine.h
	$(CC) $(CFLAGS) memAccess.c -c -o memAccess.o

binaryOps.h: binaryTypes.h

machine.h: binaryTypes.h

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
