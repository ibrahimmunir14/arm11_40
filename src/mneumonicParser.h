#ifndef ARM11_40_MNEUMONICPARSER_H
#define ARM11_40_MNEUMONICPARSER_H

#include <stdio.h>
#include <string.h>
#include "machineDataTypes.h"

typedef enum CondCode Condcode;
typedef enum OpCode Opcode;

enum CondCode branchEnum(const char *mneumonic);
enum OpCode dataProcEnum(const char *mneumonic);
int hash(const char *key);

#endif //ARM11_40_MNEUMONICPARSER_H
