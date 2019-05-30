#ifndef ARM11_40_BINARYFILELOADER_H
#define ARM11_40_BINARYFILELOADER_H

#include <stdio.h>
#include <stdbool.h>
#include "machineDataTypes.h"

// get number of lines in file
int getFileSize(FILE *file);
// import file lines into memory, return success bool
bool importFile(char *fileName, BYTE *memory);

#endif //ARM11_40_BINARYFILELOADER_H
