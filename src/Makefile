CC      = gcc
CFLAGS  = -Wall -g -D_DEFAULT_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
LDLIBS = -lm

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o binaryOps.o fileIO.o hashmap.o mnemonicParser.o assembleUtils.o assembleParsing.o 
	$(CC) assemble.o binaryOps.o fileIO.o hashmap.o mnemonicParser.o assembleUtils.o assembleParsing.o -o assemble $(LDLIBS)

emulate: emulate.o binaryOps.o fileIO.o emulateGetting.o emulateStatus.o
	$(CC) emulate.o binaryOps.o fileIO.o emulateGetting.o emulateStatus.o -o emulate $(LDLIBS)

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

assembleUtils.o: assembleUtils.h assembleUtils.c
	$(CC) $(CFLAGS) assembleUtils.c -c -o assembleUtils.o

assembleParsing.o: assembleParsing.h assembleUtils.h binaryOps.h assembleParsing.c
	$(CC) $(CFLAGS) assembleParsing.c -c -o assembleParsing.o

assembleParsing.h: machineDataTypes.h

emulateGetting.o: emulateGetting.h binaryOps.h emulateStatus.h emulateGetting.c
	$(CC) $(CFLAGS) emulateGetting.c -c -o emulateGetting.o

emulateStatus.o: emulateStatus.h binaryOps.h emulateStatus.c
	$(CC) $(CFLAGS) emulateStatus.c -c -o emulateStatus.o

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
