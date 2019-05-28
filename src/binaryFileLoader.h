#ifndef ARM11_40_BINARYFILELOADER_H
#define ARM11_40_BINARYFILELOADER_H

#include <stdio.h>
#include "binaryTypes.h"

// get number of lines in file
int getFileSize(FILE *file);
// import file lines into memory
void importFile(char *fileName, BYTE *memory);

#endif //ARM11_40_BINARYFILELOADER_H
