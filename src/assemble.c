#include "assemble.h"
#include "fileIO.h"
#include <regex.h>
#include <ctype.h>

// TODO move this function into binary ops and maybe create a nibble type for value

int main(int argc, char **argv) {
  return EXIT_SUCCESS;
  // TODO function to get the number of lines in the input file
  // TODO how to store 17 registers with 4 bits, which register left out
}

int match(const char *string, const char *pattern)
{
  regex_t re;
  if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return 0;
  int status = regexec(&re, string, 0, NULL, 0);
  regfree(&re);
  if (status != 0) return 0;
  return 1;
}

WORD assembleMultiply(REGNUMBER rd, REGNUMBER rm, REGNUMBER rs, REGNUMBER rn, bool aFlag) {
    // intialise with cond code and default bits
    WORD value = 11100000;
    if (aFlag) {
        appendNibble(value, 0010);
    } else {
        appendNibble(value, 0000);
    }
    appendNibble(value, rd);
    appendNibble(value, rn);
    appendNibble(value, rs);
    appendNibble(value, 1001);
    appendNibble(value, rm);
    return value;
}

WORD assembleMov(REGNUMBER rd, int value) {
    return 0;
}


int parseExpression(char* expression) {
  if (match(expression, "0x[0-9A-Fa-f]+")) {
    return strtol(&expression[2], NULL, 16);
  }

  return strtol(expression, NULL, 10);
}

WORD assembleSDT(bool lFlag, REGNUMBER rd, REGNUMBER rn, char* address, WORD *number) {
  WORD offset = 0;
  bool iFlag = false;
  bool pFlag = false;
  bool uFlag = false;


  if (lFlag) { //ldr
    if (address[0] == '=') {
      int value = parseExpression(&address[1]);

      if (value <= 0xFF) {
        return assembleMov(rd, value);
      }

      *number = value;

      //add value as a word to end of assembled program
      //PC is base register
      //offset is offset from current address to address of value

    }
  } else { //str

  }

  char instructionString[12] = "111001";
  WORD instruction = strtol(instructionString, NULL, 2);

  instruction = appendBits(1, instruction, iFlag);
  instruction = appendBits(1, instruction, pFlag);
  instruction = appendBits(1, instruction, uFlag);
  instruction <<= 2;

  instruction = appendBits(1, instruction, lFlag);
  instruction = appendBits(4, instruction, rn);
  instruction = appendBits(4, instruction, rd);
  instruction = appendBits(12, instruction, offset);

  return instruction;
}

void trimWhiteSpace(char *string) {
  while (isspace(string[0])) {
    string++;
  }
}

int findPos(char *string, char *strArray[], int arraySize) {
  for (int i = 0; i < arraySize; i++) {
    if (strArray[i] == string) {
      return i;
    }
  }
  return -1;
}

NIBBLE getRegNum(char *regString, char *restOfOperand) {
  trimWhiteSpace(regString);
  return strtol(&regString[1], &restOfOperand, 10);
}

int parseOperand2(char* operand2) {
  char *immediatePattern = "#.+";
  char *shiftedRegister = "r([0-9]|1[0-6]).*";
  if (match(operand2, immediatePattern)) {
    WORD value = parseExpression(&operand2[1]);

    for (WORD rotation = 0; rotation < sizeof(WORD); rotation += 2) {
      WORD rotated = (value >> rotation | value << (sizeof(WORD) - rotation));

      if (rotated < (1 << 8)) {
        return appendBits(8, (NIBBLE) rotation, (BYTE) rotated);
      }
    }

  } else if (match(operand2, shiftedRegister)) {
    char *restOfOperand;
    NIBBLE rm = getRegNum(operand2, restOfOperand);

    if (*restOfOperand == '\0') {
      return rm;
    }

    restOfOperand++;
    trimWhiteSpace(restOfOperand);

    int shift;
    char *shiftType = strtok(restOfOperand, " ");
    char *shifts[4] = {"lsl", "lsr", "asr", "ror"};
    int shiftTypeBin = findPos(shiftType, shifts, 4);

    char *exp = strtok(NULL, " ");

    if (match(exp, immediatePattern)) {
      shift = parseExpression(exp);
      shift = appendBits(2, shift, shiftTypeBin);
      shift <<= 1;
    } else {
      shift = getRegNum(exp, NULL);
      shift <<= 1;
      char *exp = strtok(NULL, " ");
      shift <<= 1;
      shift++;
    }

    return shift & FULLBITS(12);
  }

  printf("Error: %s is invalid operand2.\n", operand2);
  return 0;
}

