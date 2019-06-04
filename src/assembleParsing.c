#include "assembleParsing.h"

/* helper functions for parsing */
int parseImmediateValueWithRest(char *expression, char *restOfString) {
  if (regexMatch(expression, "0x[0-9A-Fa-f]+")) {
    // expression in hex
    return strtol(&expression[2], &restOfString, HEX_BASE);
  }

  // expression in dec
  return strtol(expression, &restOfString, DEC_BASE);
}

int parseImmediateValue(char *expression) {
  return parseImmediateValueWithRest(expression, NULL);
}

/* helper functions for parsing operand2 */
OpFlagPair parseOperand2(char *operand2) {
  if (checkIfImmediate(operand2)) {
    OpFlagPair opFlagPair = {parseImmediateOperand2(operand2), 1};
    return opFlagPair;
  } else if (checkIfShiftedRegister(operand2)) {
    OpFlagPair opFlagPair = {parseShiftedRegister(operand2), 0};
    return opFlagPair;
  }

  perror("Error: Invalid operand2.\n");
  OpFlagPair errorPair = {-1, -1};
  return errorPair;
}

REGNUMBER getRegisterNumber(char *regString) {
  return getRegNumWithRest(regString, NULL);
}

REGNUMBER getRegNumWithRest(char *regString, char *restOfOperand) {
  regString = trimWhiteSpace(regString);
  return strtol(&regString[1], &restOfOperand, DEC_BASE);
}

bool checkIfImmediate(const char* operand2) {
  return regexMatch(operand2, "(#|=).+");
}

bool checkIfShiftedRegister(const char* operand2) {
  return regexMatch(operand2, "r([0-9]|1[0-6]).*");
}

/* helper functions for parsing operand2 */
int parseImmediateOperand2(char* operand2) {
  WORD value = parseImmediateValue(&operand2[1]);

  // check each even rotation for representation
  for (WORD halfRotation = 0; halfRotation * 2 < WORD_BITS; halfRotation++) {
    WORD rotated = rotateLeft(value, halfRotation * 2);

    // return the value if it is representable in 8 bits
    if (rotated <= 0xFF) {
      return appendBits(8, halfRotation & FULLBITS(4), rotated);
    }
  }

  printf("Error: Number cannot be represented as a rotated byte.\n");
  return -1;
}

int parseShiftedRegister(char* operand2) {
  char *shiftString;
  operand2 = trimWhiteSpace(operand2);
  BYTE rm =   strtol(&operand2[1], &shiftString, DEC_BASE);

  // if register is not shifted
  if (*shiftString == '\0') {
    return rm;
  }

  // if register is shifted
  shiftString++; // ignore comma
  shiftString = trimWhiteSpace(shiftString);


  // find binary representation of shift type
  char *shiftType = strtok(shiftString, " ");
  BYTE shiftTypeBin;
  if (strncmp(shiftType, "lsl", 3) == 0) {
    shiftTypeBin = 0;
  } else if (strncmp(shiftType, "lsr", 3) == 0) {
    shiftTypeBin = 1;
  } else if (strncmp(shiftType, "asr", 3) == 0) {
    shiftTypeBin = 2;
  } else if (strncmp(shiftType, "ror", 3) == 0) {
    shiftTypeBin = 3;
  }


  char *shiftAmount = strtok(NULL, " ");

  int shift;
  if (checkIfImmediate(shiftAmount)) {
    // parse shift amount as an immediate value
    shift = parseImmediateValue(&shiftAmount[1]);
    shift = appendBits(2, shift, shiftTypeBin);
    shift <<= 1;

  } else if (checkIfShiftedRegister(shiftAmount)){
    // parse shift amount as a reg value
    shift = getRegisterNumber(shiftAmount);
    shift <<= 1;
    shift = appendBits(2, shift, shiftTypeBin);
    shift <<= 1;
    shift++;
  }
  // shift currently does not store rm
  int res = appendBits(4, shift, rm);
  return res & FULLBITS(12);
}
