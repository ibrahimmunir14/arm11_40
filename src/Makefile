CC      = gcc
CFLAGS  = -Wall -g -D_DEFAULT_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
LDLIBS = -lm

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o binaryOps.o fileIO.o hashmap.o mnemonicParser.o
	$(CC) assemble.o binaryOps.o fileIO.o hashmap.o mnemonicParser.o -o assemble $(LDLIBS)

emulate: emulate.o binaryOps.o fileIO.o
	$(CC) emulate.o binaryOps.o fileIO.o -o emulate $(LDLIBS)

emulate.o: emulate.h emulate.c binaryOps.h fileIO.h
	$(CC) $(CFLAGS) emulate.c -c -o emulate.o

binaryOps.o: binaryOps.h binaryOps.c
	$(CC) $(CFLAGS) binaryOps.c -c -o binaryOps.o

binaryOps.h: machineDataTypes.h

fileIO.o: fileIO.h machineDataTypes.h fileIO.c
	$(CC) $(CFLAGS) fileIO.c -c -o fileIO.o

hashmap.o: hashmapAbstract.h hashmap.c
	$(CC) $(CFLAGS) hashmap.c -c -o hashmap.o

mnemonicParser.o: mnemonicParser.h machineDataTypes.h mnemonicParser.c
	$(CC) $(CFLAGS) mnemonicParser.c -c -o mnemonicParser.o

mnemonicParser.h: machineDataTypes.h

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
