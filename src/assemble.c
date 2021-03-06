#include "assemble.h"

int main(int argc, char **argv) {
    // ensure we have two argument, the filenames
    if (argc != 3) {
        return EXIT_FAILURE;
    }

    // import data into instructions array and symbol table
    char *inFileName = argv[1];
    int numInstructions;

    // create array of instructions and symbol table
    node_t **symbolTable = initHashmap();
    char **assInstructions = importAssemblyInstructions(inFileName, &numInstructions, symbolTable);

    // print contents, for debugging purposes
    for (int i = 0; i < numInstructions; i++) {
        printf("[%s]\n", assInstructions[i]);
    }
    display(symbolTable);

    // set up instruction and reserve memory space
    WORD* armInstructions = calloc(numInstructions, sizeof(WORD));
    WORD* reserveMemory = calloc(numInstructions, sizeof(WORD));
    ADDRESS reserveAddress = 4 * numInstructions; // address of next free reserve memory location (in BYTEs)

    // encode assembly instructions into arm words sequentially
    for (int i = 0; i < numInstructions; i++) {
        armInstructions[i] = encodeInstruction(assInstructions[i], i * 4, &reserveMemory[(reserveAddress / 4) - numInstructions], &reserveAddress, symbolTable);
    }

    // write instructions and reserved memory to output file
    char *outFileName = argv[2];
    writeToBinaryFile(outFileName, armInstructions, reserveMemory, numInstructions, (reserveAddress / 4) - numInstructions);
    return EXIT_SUCCESS;
}

// super function delegating to all relevant assembling functions with arguments correctly parsed
WORD encodeInstruction(char* line, ADDRESS currentAddress, WORD *nextReserveMemory, ADDRESS *reserveAddress, node_t **symbolTable) {
    char* remainder = line;
    char* command = strtok_r(line, " ", &remainder);
    remainder = trimWhiteSpace(remainder);

    if (regexMatch(command, "^b")) {
        printf("matching on branch\n");
        return assembleBranch(branchEnum(command), remainder, currentAddress, symbolTable);
    } else if (regexMatch(command, "^andeq")) {
        printf("matching on andeq\n");
        return assembleAndEq();
    } else {
        // the rest of the instructions specify a register as their 2nd argument
        char* reg1 = strtok_r(remainder, ",", &remainder);
        remainder = trimWhiteSpace(remainder);
        if (regexMatch(command, "^mov")) {
            printf("matching on mov\n");
            OpFlagPair opFlag = parseOperand2(remainder);
            return assembleMov(getRegisterNumber(reg1), opFlag.operand2, opFlag.iflag);
        } else if (regexMatch(command, "^(tst|teq|cmp)")) {
          printf("matching on tst/teq/cmp\n");
          OpFlagPair opFlag = parseOperand2(remainder);
          return assembleDataProcFlags(dataProcEnum(command), getRegisterNumber(reg1), opFlag.operand2, opFlag.iflag);

        } else if (regexMatch(command, "^lsl")) {
            printf("matching on lsl\n");
            return assembleLSL(getRegisterNumber(reg1), remainder);
        } else if (regexMatch(command, "^ldr")) {
            printf("matching on ldr\n");
            return assembleSDT(true, getRegisterNumber(reg1), remainder, currentAddress, nextReserveMemory, reserveAddress);
        } else if (regexMatch(command, "^str")) {
            printf("matching on str\n");
            return assembleSDT(false, getRegisterNumber(reg1), remainder, currentAddress, nextReserveMemory, reserveAddress);
        } else {
            // the rest of the instructions specify a register as their 3rd argument
            char* reg2 = strtok_r(remainder, ",", &remainder);
            remainder = trimWhiteSpace(remainder);
            if (regexMatch(command, "^mul")) {
                printf("matching on mul\n");
                char* reg3 = strtok_r(remainder, ",", &remainder);
                return assembleMultiply(getRegisterNumber(reg1), getRegisterNumber(reg2), getRegisterNumber(reg3), 0, false);
            } else if (regexMatch(command, "^mla")) {
                printf("matching on mla\n");
                char* reg3 = strtok_r(remainder, ",", &remainder);
                char* reg4 = trimWhiteSpace(strtok_r(remainder, ",", &remainder));
                return assembleMultiply(getRegisterNumber(reg1), getRegisterNumber(reg2), getRegisterNumber(reg3), getRegisterNumber(reg4), true);
            } else {
                printf("matching on general dataproc\n");
                OpFlagPair opFlag = parseOperand2(remainder);
                return assembleDataProcResult(dataProcEnum(command), getRegisterNumber(reg1), getRegisterNumber(reg2), opFlag.operand2, opFlag.iflag);
            }
        }
    }
}

// feeding each line in a file into the index of an array
char** importAssemblyInstructions(char *fileName, int *numLines, node_t **map) {
  // open input file
  FILE *file;
  if ((file = fopen(fileName, "r")) == NULL) {
    perror("Error: could not open input file.");
    exit(EXIT_FAILURE);
  }

  // store instruction in tempContents, store labels in map
  char** tempContents = (char **) calloc(MAX_LINES_ASCII, sizeof(char*));
  int instrNum = 0; // number of instructions stored in tempContents
  for (int i = 0; i < MAX_LINES_ASCII; i++) {
    // stop loop at end of file
    if (feof(file)) {
      break;
    }
    // allocate memory and read in line
    tempContents[instrNum] = (char *) calloc(MAX_LINE_LENGTH, sizeof(char));
    fgets(tempContents[instrNum], MAX_LINE_LENGTH, file);
    if (regexMatch(tempContents[instrNum], ".*:")) {
      // line read in is a label: add to map, allow tempContents entry to be overwritten
      char *key = strtok(tempContents[instrNum], ":");
      ADDRESS value = instrNum * 4;
      addHashmapEntry(map, key, value);
    } else if (tempContents[instrNum][0] != '\n') {
      // line read in is an instruction: keep stored in tempContents
      instrNum++;
    }
  }
  fclose(file);
  *numLines = (tempContents[instrNum-1][0] == '\0') ? instrNum - 1 : instrNum;

  char **assemblyInstructions = (char **) calloc(*numLines, sizeof(char*));
  for (int i = 0; i < *numLines; i++) {
    assemblyInstructions[i] = strtok(tempContents[i], "\n");
  }
  free(tempContents);

  return assemblyInstructions;
}

/* assembling functions */
WORD assembleBranch(enum CondCode condCode, char* target, ADDRESS currentAddress, node_t **symbolTable) {
    // initialise with cond code
    WORD instruction = condCode;
    instruction = appendNibble(instruction, 10);
    instruction = appendBits(24, instruction, calculateBranchOffset(target, currentAddress, symbolTable));
    return instruction;
}

WORD assembleMultiply(REGNUMBER rd, REGNUMBER rm, REGNUMBER rs, REGNUMBER rn, bool aFlag) {
    // initialise with cond code and default bits
    char instructionString[8] = "11100000";
    WORD instruction = strtol(instructionString, NULL, 2);

    instruction = aFlag ? appendNibble(instruction, 2) : appendNibble(instruction, 0);

    instruction = appendNibble(instruction, rd);
    instruction = appendNibble(instruction, rn);
    instruction = appendNibble(instruction, rs);
    instruction = appendNibble(instruction, 9);
    instruction = appendNibble(instruction, rm);
    return instruction;
}

WORD assembleSDT(bool lFlag, REGNUMBER rd, char* sdtAddressParameter, ADDRESS currentAddress, WORD *nextReserveMemory, ADDRESS *reserveAddress) {
    REGNUMBER rn;
    char *normalRegPattern = "^\\[r([0-9]|1[0-6])\\]$"; // e.g. [r5], aka offset is 0
    char *preIndexPattern = "^\\[r([0-9]|1[0-6]),.+\\]"; // e.g. [r5, X] : offset 'X' is immediate or shifted reg
    char *postIndexPattern = "^\\[r([0-9]|1[0-6])\\],.+"; // e.g. [r5] X : offset 'X' is immediate or shifted reg

    WORD offset = 0;
    bool iFlag = true;
    bool pFlag = true;
    bool uFlag = true;

    if (sdtAddressParameter[0] == '=') {
        // address is an immediate expression, use PC as base, put an offset etc.
        int value = parseImmediateValue(&sdtAddressParameter[1]);

        if (value <= 0xFF) {
            return assembleMov(rd, value, 1);
        }

        *nextReserveMemory = value;
        offset = *reserveAddress - currentAddress - BYTE_BITS;
        *reserveAddress += 4;
        rn = REG_PC;
        iFlag = false;

    } else {
        // address is register rn with some offset applied
        if (regexMatch(sdtAddressParameter, normalRegPattern)) {
            // address is register, offset is 0
            rn = getRegisterNumber(trimWhiteSpace(strtok(&sdtAddressParameter[1], "]")));
            offset = 0;
            iFlag = false;
        } else {
            // address is register plus some offset
            if (regexMatch(sdtAddressParameter, preIndexPattern)) {
                pFlag = true;
                rn = getRegisterNumber(trimWhiteSpace(strtok(&sdtAddressParameter[1], ",")));
                char* offsetStr = trimWhiteSpace(strtok(NULL, "]"));
                if (offsetStr[0] == '#') {
                    // offset is immediate
                    uFlag = offsetStr[1] != '-';
                    offset = parseImmediateValue(&offsetStr[uFlag ? 1 : 2]);
                    iFlag = false;
                } else {
                    // offset is shifted register
                    uFlag = offsetStr[0] != '-';
                    offset = parseShiftedRegister(&offsetStr[uFlag ? 0 : 1]);
                    iFlag = true;
                }
            } else if (regexMatch(sdtAddressParameter, postIndexPattern)) {
                pFlag = false;
                rn = getRegisterNumber(trimWhiteSpace(strtok(&sdtAddressParameter[1], "]")));
                char* offsetStr = strtok(NULL, ",");
                if (offsetStr[0] == '#') {
                    // offset is immediate
                    uFlag = offsetStr[1] != '-';
                    offset = parseImmediateValue(&offsetStr[uFlag ? 1 : 2]);
                    iFlag = false;
                } else {
                    // offset is shifted register
                    uFlag = offsetStr[0] != '-';
                    offset = parseShiftedRegister(&offsetStr[uFlag ? 0 : 1]);
                    iFlag = true;
                }
            }
        }
    }

    // initialise with cond code and default bits
    char instructionString[6] = "111001";
    WORD instruction = strtol(instructionString, NULL, 2);

    instruction = appendBits(1, instruction, iFlag);
    instruction = appendBits(1, instruction, pFlag);
    instruction = appendBits(1, instruction, uFlag);
    instruction <<= 2;

    instruction = appendBits(1, instruction, lFlag);
    instruction = appendNibble(instruction, rn);
    instruction = appendNibble(instruction, rd);
    instruction = appendBits(12, instruction, offset);

    return instruction;
}

WORD assembleDataProcGeneral(enum OpCode opCode, REGNUMBER rd, REGNUMBER rn, int value, bool iFlag, bool sFlag) {
  // initialise with cond code and default bits
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
}

/* special instruction assembling functions */
WORD assembleAndEq(void) {
  return 0;
}
WORD assembleLSL(REGNUMBER rn, char *expression) {
  int lslShift = parseImmediateValue(&expression[1]);
  lslShift = appendBits(2, lslShift, LSL);
  lslShift <<= 1;
  lslShift = appendNibble(lslShift, rn);

  // parse as a mov instruction with a shift
  return assembleMov(rn, lslShift & FULLBITS(12), 0);
}

BRANCHOFFSET calculateBranchOffset(char* target, ADDRESS currentAddress, node_t **symbolTable) {
  // use symbol table to find the difference in addresses
  ADDRESS targetAddress = getHashmapValue(symbolTable, target);
  BRANCHOFFSET offset = targetAddress - currentAddress - BYTE_BITS;
  return (offset >> 2);
}