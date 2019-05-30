#include "assemble.h"
#include "fileIO.h"

// TODO move this function into binary ops and maybe create a nibble type for value

int main(int argc, char **argv) {
    // ensure we have two argument, the filenames
    if (argc != 3) {
        return EXIT_FAILURE;
    }
    // import file into contents**, each line has a \n
    int size;
    char *inFileName = argv[1];
    char **contents = importAsciiFile(inFileName, &size);
    // print contents, for debugging purposes
    for (int i = 0; i < size; i++) {
        printf("[%s]\n", contents[i]);
    }

    // instructions list has maximum length = size
    WORD* instructions = calloc(size, sizeof(WORD));
    // TODO: implement first pass - build symbol table
    // TODO: implement second pass - assembly phase

    // write instructions to output file
    char *outFileName = argv[2];
    binaryFileWriter(outFileName, instructions);
    return EXIT_SUCCESS;
  // TODO function to get the number of lines in the input file
  // TODO how to store 17 registers with 4 bits, which register left out
}


WORD assembleBranch(enum CondCode condCode, char* target, ADDRESS currentAddress) {
    WORD instr = 0;
    instr = appendNibble(instr, (BYTE) condCode);
    instr = appendNibble(instr, 0b1010);
    instr = appendNibble(instr, calculateBranchOffset(target, currentAddress));
    return instr;
}

BRANCHOFFSET calculateBranchOffset(char* target, ADDRESS currentAddress) {
    // TODO: differentiate between label target and address target
    ADDRESS targetAddress;
    /* if label target:
     *      targetAddress = lookup label in table
     * else:
     *      targetAddress = atoi(target) (*4?)
     * */
    BRANCHOFFSET offset = targetAddress - currentAddress;
    return (offset >> 2);
    // note: this returns the whole offset in 32 bits, we only store the bottom 24 bits
}



WORD assembleMultiply(REGNUMBER rd, REGNUMBER rm, REGNUMBER rs, REGNUMBER rn, bool aFlag) {
    // intialise with cond code and default bits
    WORD value = 0b11100000;
    if (aFlag) {
        value = appendNibble(value, 0b0010);
    } else {
        value = appendNibble(value, 0b0000);
    }
    value = appendNibble(value, rd);
    value = appendNibble(value, rn);
    value = appendNibble(value, rs);
    value = appendNibble(value, 0b1001);
    value = appendNibble(value, rm);
    return value;
}

WORD assembleMov(REGNUMBER rd, int value) {
    return 0;
}


int parseExpression(char* expression) {
  int length = strlen(expression);
  if (length > 2 && expression[0] == '0' && expression[1] == 'x') {
    return strtol(&expression[2], NULL, 16);
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
      int value = parseExpression(&address[1]);

      if (value <= 0xFF) {
        return assembleMov(rd, value);
      }

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
