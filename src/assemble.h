//
// Created by Umer on 5/27/2019.
//

#ifndef ARM11_40_ASSEMBLE_H
#define ARM11_40_ASSEMBLE_H

#endif //ARM11_40_ASSEMBLE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>


// TODO separate types into another file and include in both header files
// TODO maybe create an extra address type for "expressions" which maybe addresses
// TODO build a symbol table abstract data type
// TODO binary file writer

/* functions for IO */
char* tokenizer(char** lines);


/* functions for encoding instructions */

/* parseInstruction delegates to the specific encoding functions - big switch statement in here */
int encodeInstruction();
/* each encode instruction returns a 32 bit integer instruction */
int assembleBranch(enum CondCode condCode, int expression);
int assembleSDT();
int assembleMultiply(REGNUMBER rd, REGNUMBER rm, REGNUMER rs, REGNUMER rn, bool aFlag);
int assembleDataProc(enum OpCode opCode, REGNUMBER rd, REGNUMBER rn, int operand2);
/* special is for andeq and lsl, can be split into 2 different functions later */
/* can call encodeDataProc from inside this instruction */
int assembleSpecial(enum ExtendedOpCode opCode, REGNUMBER rn, int expression);

/* helper functions for encoding DataProc */
int assembleDataProcResult(enum OpCode opCode, REGNUMBER rd, REGNUMBER rn, int operand2);
int assembleMov(REGNUMBER rd, int operand2);
int assembleDataProcFlags(enum OpCode opCode, REGNUMBER rn, int operand2);

/* helper functions for encodingBranch */
BRANCHOFFSET calculateBranchOffset(int newAddress);
BYTE calculateBranchCond(enum CondCode condCode);