#ifndef ARM11_40_SRC_ASSEMBLEPARSING_H_
#define ARM11_40_SRC_ASSEMBLEPARSING_H_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "machineDataTypes.h"
#include "assembleUtils.h"
#include "binaryOps.h"

#define HEX_BASE 16u
#define DEC_BASE 10u

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

#endif //ARM11_40_SRC_ASSEMBLEPARSING_H_
