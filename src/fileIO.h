#ifndef ARM11_40_FILEIO_H
#define ARM11_40_FILEIO_H

#include <stdio.h>
#include <stdbool.h>
#include "machineDataTypes.h"

// get number of lines in file
int getFileSize(FILE *file);
// import binary file contents into memory, return success bool
bool importBinaryFile(char *fileName, BYTE *memory);
// write list of words to binary file
void binaryFileWriter(char* fileName, WORD* instructions);
// import ascii file contents into char* array, return success bool
bool importAsciiFile(char * fileName, char** contents);

#endif //ARM11_40_FILEIO_H
