#ifndef ARM11_40_MNEMONICPARSER_H
#define ARM11_40_MNEMONICPARSER_H

#include <stdio.h>
#include <string.h>
#include "machineDataTypes.h"

typedef enum CondCode Condcode;
typedef enum OpCode Opcode;

enum CondCode branchEnum(const char *mnemonic);
enum OpCode dataProcEnum(const char *mnemonic);

#endif //ARM11_40_MNEMONICPARSER_H
