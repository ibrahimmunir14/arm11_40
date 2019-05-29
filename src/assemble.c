#include "assemble.h"
#include <string.h>

int main(int argc, char **argv) {
  return EXIT_SUCCESS;
}

int parseExpression(char* expression) {
  int length = strlen(expression);
  if (length > 2 && expression[0] == '0' && expression[1] == 'x') {
    return strtol(expression[2], NULL, 16);
  }

  return strtol(expression, NULL, 10);
}

WORD assembleSDT(bool lFlag, REGNUMBER rd, REGNUMBER rn, char* address) {
  WORD offset = 0;
  bool iFlag = false;
  bool pFlag = false;
  bool uFlag = false;


  if (lFlag) { //ldr
    if (address[0] == '=') {
      int value = parseExpression(address[1]);

      if (value <= 0xFF) {
        return assembleMov(rd, value);
      }

      //add value as a word to end of assembled program
      //PC is base register
      //offset is offset from current address to address of value

    }
  } else { //str

  }

  char instructionString[12];
  sprintf(instructionString, "111001%d%d%d00%d", iFlag, pFlag, uFlag, lFlag);
  WORD instruction = strtol(instructionString, NULL, 2);
  instruction <<= 4;
  instruction &= getBitsFromWord(rn, 3, 4);

  instruction <<= 4;
  instruction &= getBitsFromWord(rd, 3, 4);

  instruction <<= 12;
  instruction &= getBitsFromWord(offset, 11, 12);

  return instruction;
}
