#ifndef ARM11_40_ASSEMBLE_H
#define ARM11_40_ASSEMBLE_H

#endif //ARM11_40_ASSEMBLE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "binaryOps.h"
#include "machineDataTypes.h"
#include "hashmapAbstract.h"
#include "fileIO.h"
#include <regex.h>
#include <ctype.h>

// TODO create some sort of enum expression to fulfill all different types of expressions
// TODO create array of lines inputted in main - write to the same array index for the integer instruction?

/* first pass */
char** importAssemblyInstr(char *fileName, int *numLines, node_t **map);

/* functions for encoding instructions */

/* parseInstruction delegates to the specific encoding functions - big switch statement in here */
WORD encodeInstruction(char* line, ADDRESS currentAddress, WORD *nextReserveMemory, int *numReserve);
/* each encode instruction returns a 32 bit integer instruction */

WORD assembleBranch(enum CondCode condCode, char* target, ADDRESS currentAddress);
WORD assembleSDT(bool lFlag, REGNUMBER rd, char* sdtAddressParameter, ADDRESS currentAddress, WORD *nextReserveMemory, int *numReserve);

WORD assembleMultiply(REGNUMBER rd, REGNUMBER rm, REGNUMBER rs, REGNUMBER rn, bool aFlag);
WORD assembleDataProc(enum OpCode opCode, REGNUMBER rd, REGNUMBER rn, char* operand2);
/* special is for andeq and lsl, can be split into 2 different functions later */
/* can call encodeDataProc from inside this instruction */
WORD assembleAndEq(void);
WORD assembleLSL(REGNUMBER rn, int value);


/* helper functions for encoding DataProc */
WORD assembleDataProcResult(enum OpCode opCode, REGNUMBER rd, REGNUMBER rn, int value, bool iFlag);
WORD assembleMov(REGNUMBER rd, int value, bool iFlag);
WORD assembleDataProcFlags(enum OpCode opCode, REGNUMBER rn, int value, bool iFlag);

/* helper functions for encodingBranch */
BRANCHOFFSET calculateBranchOffset(char* target, ADDRESS currentAddress);

/* helper functions for parsing */

/* this parses the different types of values that can be placed in operand2 and passes it to the above 3 helper functions */
int parseOperand2(char* operand2); // used by assemble dataproc and result passed to dataproc helpers
int parseImmediateValue(char *expression); // used by assembleSpecial and possibly by SDT + Branch

bool match(const char *string, const char *pattern);
REGNUMBER getRegisterNumber(char *regString, char *restOfOperand);

int getIFlag(char* operand2);