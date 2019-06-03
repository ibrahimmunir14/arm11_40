#ifndef ARM11_40_FILEIO_H
#define ARM11_40_FILEIO_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "binaryOps.h"

#define MAX_LINES_ASCII 50
#define MAX_LINE_LENGTH 512

// get number of lines in file
int getFileSize(FILE *file);
// import binary file contents into memory, return success bool
bool importBinaryFile(const char *fileName, BYTE *memory);
// write list of words to binary file
void writeWords(FILE *fPointer, int numOfWords, const WORD *contents);
// write instructions and reserve memory to binary file
void writeToBinaryFile(const char *fileName,
                       const WORD *instructions,
                       const WORD *reserveMemory,
                       int numInstructions,
                       int numReserve);

#endif //ARM11_40_FILEIO_H
