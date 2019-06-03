#ifndef ARM11_40_ASSEMBLE_H
#define ARM11_40_ASSEMBLE_H

#endif //ARM11_40_ASSEMBLE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include "binaryOps.h"
#include "machineDataTypes.h"
#include "hashmapAbstract.h"
#include "fileIO.h"
#include "hashmapAbstract.h"
#include "mnemonicParser.h"

#define HEX_BASE 16u
#define DEC_BASE 10u

/* functions for encoding instructions */


// converts an assembly instruction into binary by delegating to helper functions
WORD encodeInstruction(char* line, ADDRESS currentAddress, WORD *nextReserveMemory, ADDRESS *reserveAddress, node_t **symbolTable);
/*
 * Note: This function takes in the instruction string, currentAddress, pointer to next free reserve memory location,
 *       and pointer to total address number of next free reserve memory location
 *       - currentAddress is needed for branch operations and SDT, to calculate offsets
 *       - *nextReserveMemory is to be passed to SDT function, and the value/contents is to be updated directly
 *       - *reserveAddress is to be passed to SDT function to calculate offset, and should be increased by 4 if used
 */


// return array of assembly instructions and symbol table from labels to addresses
char** importAssemblyInstructions(char *fileName, int *numLines, node_t **map);

/* assembling functions */
WORD assembleBranch(enum CondCode condCode, char* target, ADDRESS currentAddress, node_t **symbolTable);
WORD assembleMultiply(REGNUMBER rd, REGNUMBER rm, REGNUMBER rs, REGNUMBER rn, bool aFlag);
WORD assembleSDT(bool lFlag, REGNUMBER rd, char* sdtAddressParameter, ADDRESS currentAddress, WORD *nextReserveMemory, ADDRESS *reserveAddress);
// general data proc assembly that takes all arguments
WORD assembleDataProcGeneral(enum OpCode opCode, REGNUMBER rd, REGNUMBER rn, int value, bool iFlag, bool sFlag);

/* helper functions for encoding DataProc for specific instructions */
WORD assembleDataProcResult(enum OpCode opCode, REGNUMBER rd, REGNUMBER rn, int value, bool iFlag);
WORD assembleDataProcFlags(enum OpCode opCode, REGNUMBER rn, int value, bool iFlag);
WORD assembleMov(REGNUMBER rd, int value, bool iFlag);

/* special instruction assembling functions */
WORD assembleAndEq(void);
WORD assembleLSL(REGNUMBER rn, char* operand2);

// returns the offset of a branch instruction (in 32 bits)
BRANCHOFFSET calculateBranchOffset(char* target, ADDRESS currentAddress, node_t **symbolTable);


/* helper functions for parsing */
// struct to store operand2 and iflag after parsing
typedef struct opFlagPair {
    int operand2;
    int iflag;
} OpFlagPair;

// parses a dec or hex value into an int
int parseImmediateValue(char *expression);
// parses a dec or hex value into an int and return remaining string
int parseImmediateValueWithRest(char *expression, char *restOfString);
// used by assemble dataproc and result passed to dataproc helpers
OpFlagPair parseOperand2(char* operand2);
// get reg number
REGNUMBER getRegisterNumber(char *regString);
// get reg number and return remaining string
REGNUMBER getRegNumWithRest(char *regString, char *restOfOperand);


/* helper functions for parsing operand2 */
bool checkIfImmediate(const char* operand2);
bool checkIfShiftedRegister(const char* operand2);
int parseImmediateOperand2(char* operand2);
int parseShiftedRegister(char* operand2);

/* utility functions */
// check if string matches a regex pattern
bool regexMatch(const char *string, const char *pattern);
// return string with leading whitespace characters removed
char* trimWhiteSpace(char *string);