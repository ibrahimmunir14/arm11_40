#include "assemble.h"
#include "fileIO.h"
#include <regex.h>
#include <ctype.h>
#include "hashmapAbstract.h"

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

    // TODO: complete implementation of first pass - build symbol table
    // set up head node
    pair_t headPair = {"start", 0};
    node_t headNode = {headPair, 0};
    displayList(&headNode);
    for (int i = 0; i < size; i++) {
        char *line = contents[i];
        ADDRESS currAddress = i * 4;
        /* TODO: if line starts with label :
         *              extract label as char*
         *              add label, currAddress to hashMap
         */
    }



    // TODO: test/check implementation of second pass - assembly phase
    WORD *memory = calloc(size * 2, sizeof(WORD));
    WORD *reserveMemory = memory + size * sizeof(WORD);


    int *nextReserveAddress = malloc(sizeof(int));
    *nextReserveAddress = 0;

    for (int i = 0; i < size; i++) {
        // TODO: encodeInstruction must be modified to carry the hashmap
        int offsetToEmptyReserve = nextReserveAddress - i;
        memory[i] = encodeInstruction(contents[i], headNode, reserveMemory);
    }
    int reserveAddressUsed = nextReserveAddress - size;

    // write instructions to output file
    char *outFileName = argv[2];
    binaryFileWriter(outFileName, memory);
    return EXIT_SUCCESS;
}

WORD encodeInstruction(char* line, node_t **hashmap, int offsetToEmptyReserve) {

}

void addToReserveMemory(WORD* reserveMemory, ) {

}


WORD assembleBranch(enum CondCode condCode, char* target, ADDRESS currentAddress) {
    WORD instr = 0;
    instr = appendNibble(instr, (BYTE) condCode);
    instr = appendNibble(instr, 10); // 0b1010
    instr = appendNibble(instr, calculateBranchOffset(target, currentAddress));
    return instr;
}

BRANCHOFFSET calculateBranchOffset(char* target, ADDRESS currentAddress) {
    // TODO: differentiate between label target and address target
    ADDRESS targetAddress;
    /* TODO: if label target:
     *          targetAddress = lookup label in table
     *       else:
     *          targetAddress = atoi(target) (*4?)
     */
    BRANCHOFFSET offset = targetAddress - currentAddress;
    return (offset >> 2);
    // note: this returns the whole offset in 32 bits, we only store the bottom 24 bits
}

int regexMatch(const char *string, const char *pattern)
{
  regex_t re;
  if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
    return 0;
  }

  int status = regexec(&re, string, 0, NULL, 0);
  regfree(&re);

  return status == 0;
}

WORD assembleDataProc(enum OpCode opCode, REGNUMBER rd, REGNUMBER rn, char* operand2) {
  int operand2Value = parseOperand2(operand2);
  switch (opCode) {
    case TST:
    case TEQ:
    case CMP:

      break;
    default:

      break;
  }

}

WORD assembleDataProcGeneral(enum OpCode opCode, REGNUMBER rd, REGNUMBER rn, int value, bool iFlag, bool sFlag) {
  char instructionString[6] = "111000";
  WORD instruction = strtol(instructionString, NULL, 2);
  instruction = appendBits(1, instruction, iFlag);

  instruction = appendNibble(instruction, opCode);
  instruction = appendBits(1, instruction, sFlag);

  instruction = appendNibble(instruction, rn);
  instruction = appendNibble(instruction, rd);
  instruction = appendBits(12, instruction, value);
  return instruction;
}

WORD assembleDataProcResult(enum OpCode opCode, REGNUMBER rd, REGNUMBER rn, int value, bool iFlag) {
  return assembleDataProcGeneral(opCode, rd, rn, value, iFlag, 0);
}

WORD assembleDataProcFlags(enum OpCode opCode, REGNUMBER rn, int value, bool iFlag) {
  return assembleDataProcGeneral(opCode, 0, rn, value, iFlag, 1);
}

WORD assembleMov(REGNUMBER rd, int value, bool iFlag) {
  return assembleDataProcGeneral(MOV, rd, 0, value, iFlag, 0);
};

WORD assembleMultiply(REGNUMBER rd, REGNUMBER rm, REGNUMBER rs, REGNUMBER rn, bool aFlag) {
    // intialise with cond code and default bits
    WORD value = 224; // 0b11100000
    if (aFlag) {
        value = appendNibble(value, 2); // 0b0010
    } else {
        value = appendNibble(value, 0); // 0b0000
    }
    value = appendNibble(value, rd);
    value = appendNibble(value, rn);
    value = appendNibble(value, rs);
    value = appendNibble(value, 9); // 0b1001
    value = appendNibble(value, rm);
    return value;
}




int parseImmediateValue(char *expression) {
  if (regexMatch(expression, "0x[0-9A-Fa-f]+")) {
    return strtol(&expression[2], NULL, 16);
  }

  return strtol(expression, NULL, 10);
}

WORD assembleSDT(bool lFlag, REGNUMBER rd, REGNUMBER rn, char* address, WORD *reserveMemory) {
  WORD offset = 0;
  bool iFlag = false;
  bool pFlag = false;
  bool uFlag = false;


  if (lFlag) { //ldr
    if (address[0] == '=') {
      int value = parseImmediateValue(&address[1]);

      if (value <= 0xFF) {
        return assembleMov(rd, value);
      }

      *reserveMemory = value;
      offset = *reserveMemory
      reserveMemory += sizeof(WORD);

      //add value as a word to end of assembled program
      //PC is base register
      //offset is offset from current address to address of value

    }
  } else { //str

  }

  char instructionString[6] = "111001";
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

BYTE getRegNum(char *regString, char *restOfOperand) {
  trimWhiteSpace(regString);
  return strtol(&regString[1], &restOfOperand, 10) & FULLBITS(4);
}

int parseOperand2(char* operand2) {
  char *immediatePattern = "#.+";
  char *shiftedRegister = "r([0-9]|1[0-6]).*";
  if (regexMatch(operand2, immediatePattern)) {
    WORD value = parseImmediateValue(&operand2[1]);

    for (WORD rotation = 0; rotation < sizeof(WORD); rotation += 2) {
      WORD rotated = (value >> rotation | value << (sizeof(WORD) - rotation));

      if (rotated < (1 << 8)) {
        return appendBits(8, rotation & FULLBITS(4), (BYTE) rotated);
      }
    }

  } else if (regexMatch(operand2, shiftedRegister)) {
    char *restOfOperand;
    BYTE rm = getRegNum(operand2, restOfOperand);

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

    if (regexMatch(exp, immediatePattern)) {
      shift = parseImmediateValue(exp);
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

