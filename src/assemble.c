#include "assemble.h"
#include "fileIO.h"

// TODO move this function into binary ops and maybe create a nibble type for value

int main(int argc, char **argv) {
  return EXIT_SUCCESS;
  // TODO function to get the number of lines in the input file
  // TODO need a set bits in word in binary ops
  // TODO how to store 17 registers with 4 bits, which register left out
}



WORD assembleMultiply(REGNUMBER rd, REGNUMBER rm, REGNUMBER rs, REGNUMBER rn, bool aFlag) {
//    int aPos = 21;
//    int sPos = 20;
//    int rdPos = 19;
//    int rnPos = 15;
//    int rsPos = 11;
//    int rmPos = 3;
//    WORD value = 0;
//    value = setBitsInWord(value, 31, 8, 11100000);
//    if (aFlag) {
//        value = setBitsInWord(value, 23, 4, 0010);
//    } else {
//        value = setBitsInWord(value, 23, 4, 0000);
//    }
//    value = setBitsInWord(value, rdPos, 4, rd); // TODO could change to sizeof(nibble)
//    value = setBitsInWord(value, rnPos, 4, rn); // TODO do we want to feed in number format or binary format
//    value = setBitsInWord(value, rsPos, 4, rs);
//    value = setBitsInWord(value, rmPos, 4, rm);

    return 0;
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
  instruction &= GETBITS(rn, 3, 4);

  instruction <<= 4;
  instruction &= GETBITS(rd, 3, 4);

  instruction <<= 12;
  instruction &= GETBITS(offset, 11, 12);

  return instruction;
}
