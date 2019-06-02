#include "assemble.h"

// TODO add documentation to all functions
// TODO separate functions out into separate files
// TODO add assembleSpecial functions
// TODO understand parts of the codebase
// TODO add const where possible
// TODO remove duplicate calls to trimWhiteSpace and other functions

int main(int argc, char **argv) {
    // ensure we have two argument, the filenames
    if (argc != 3) {
        return EXIT_FAILURE;
    }
    // import data into instructions array and symbol table
    char *inFileName = argv[1];
    int numInstructions;
    node_t **symbolTable = initHashmap();
    char **assInstructions = importAssemblyInstr(inFileName, &numInstructions, symbolTable);

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
        printf("0x%08x\n", armInstructions[i]);
    }

    // write instructions and reserved memory to output file
    char *outFileName = argv[2];
    binaryFileWriter(outFileName, armInstructions, reserveMemory, numInstructions, (reserveAddress / 4) - numInstructions);
    return EXIT_SUCCESS;
}

/*
 * Note: This function takes in the instruction string, currentAddress, pointer to next free reserve memory location,
 *       and pointer to total address number of next free reserve memory location
 *       - currentAddress is needed for branch operations and SDT, to calculate offsets
 *       - *nextReserveMemory is to be passed to SDT function, and the value/contents is to be updated directly
 *       - *reserveAddress is to be passed to SDT function to calculate offset, and should be increased by 4 if used
 */
WORD encodeInstruction(char* line, ADDRESS currentAddress, WORD *nextReserveMemory, ADDRESS *reserveAddress, node_t **symbolTable) {
//    WORD value = 0;
//    char str1[100] = "beq label";
//    char strArray[10][10];
//    int i,j,ctr;
//
//    j=0; ctr=0;
//    for(i=0;i<=(strlen(line));i++)
//    {
//        // if space or NULL found, assign NULL into newString[ctr]
//        if(line[i]==' '||line[i]=='\0'||line[i]==','||line[i]=='\n')
//        {
//            strArray[ctr][j]='\0';
//            ctr++;  //for next word
//            j=0;    //for next word, init index to 0
//        }
//        else
//        {
//            strArray[ctr][j]=line[i];
//            j++;
//        }
//    }
//    for(i=0;i < ctr-1;i++)
//        printf(" '%s'\n",strArray[i]);

    char* remainder = line;
    char* command = strtok_r(line, " ", &remainder);

    if (match(command, "^b")) {
        printf("matching on branch\n");
        return assembleBranch(branchEnum(command), remainder, currentAddress, symbolTable);
    } else if (match(command, "^andeq")) {
        printf("matching on andeq\n");
        return assembleAndEq();
    } else {
        // the rest of the instructions specify a register as their 2nd argument
        char* reg1 = trimWhiteSpace(strtok_r(remainder, ",", &remainder));
        if (match(command, "^mov")) {
            printf("matching on mov\n");
            return assembleMov(getRegisterNumber(reg1), parseOperand2(remainder), getIFlag(remainder));
        } else if (match(command, "^lsl")) {
            printf("matching on lsl\n");
            return assembleLSL(getRegisterNumber(reg1), remainder);
        } else if (match(command, "^ldr")) {
            printf("matching on ldr\n");
            return assembleSDT(true, getRegisterNumber(reg1), remainder, currentAddress, nextReserveMemory, reserveAddress);
        } else if (match(command, "^str")) {
            printf("matching on str\n");
            return assembleSDT(false, getRegisterNumber(reg1), remainder, currentAddress, nextReserveMemory, reserveAddress);
        } else {
            // the rest of the instructions specify a register as their 3rd argument
            char* reg2 = trimWhiteSpace(strtok_r(remainder, ",", &remainder));
            if (match(command, "^mul")) {
                printf("matching on mul\n");
                char* reg3 = strtok_r(remainder, ",", &remainder);
                return assembleMultiply(getRegisterNumber(reg1), getRegisterNumber(reg2), getRegisterNumber(reg3), 0, false);
            } else if (match(command, "^mla")) {
                printf("matching on mla\n");
                char* reg3 = trimWhiteSpace(strtok_r(remainder, ",", &remainder));
                char* reg4 = trimWhiteSpace(strtok_r(remainder, ",", &remainder));
                return assembleMultiply(getRegisterNumber(reg1), getRegisterNumber(reg2), getRegisterNumber(reg3), getRegisterNumber(reg4), true);
            } else {
                printf("matching on dataproc\n");
                return assembleDataProc(dataProcEnum(command), getRegisterNumber(reg1), getRegisterNumber(reg2), remainder);
            }
        }
    }
}

/* assembling functions */
WORD assembleBranch(enum CondCode condCode, char* target, ADDRESS currentAddress, node_t **symbolTable) {
    WORD instr = 0;
    instr = appendNibble(instr, (BYTE) condCode);
    instr = appendNibble(instr, 10); // 0b1010
    instr = appendBits(24, instr, calculateBranchOffset(target, currentAddress, symbolTable));
    return instr;
}

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

WORD assembleSDT(bool lFlag, REGNUMBER rd, char* sdtAddressParameter, ADDRESS currentAddress, WORD *nextReserveMemory, ADDRESS *reserveAddress) {
  REGNUMBER rn;
  char *normalRegPattern = "[Rn]";
  char *preIndexPattern = "[Rn,.+]";
  char *postIndexPattern = "[Rn],.+";

  WORD offset = 0;
  bool iFlag = true;
  bool pFlag = true;
  bool uFlag = false; //used for optional parts

  if (sdtAddressParameter[0] == '=') {
    int value = parseImmediateValue(&sdtAddressParameter[1]);

    if (value <= 0xFF) {
      return assembleMov(rd, value, 1);
    }

    *nextReserveMemory = value;
    offset = *reserveAddress - currentAddress;
    *reserveAddress += 4;
    rn = REG_PC;
    iFlag = false;

  } else {
    rn = getRegisterNumber(strtok(sdtAddressParameter, ","));
    sdtAddressParameter = trimWhiteSpace(sdtAddressParameter);

    if (match(sdtAddressParameter, normalRegPattern)) {
      offset = 0;
    } else {
      char *expression = &strtok(NULL, ",")[1];
      offset = parseImmediateValue(expression);
      if (match(sdtAddressParameter, preIndexPattern)) {
        pFlag = true;
      } else if (match(sdtAddressParameter, postIndexPattern)) {
        pFlag = false;
      }
    }
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

WORD assembleDataProc(enum OpCode opCode, REGNUMBER rd, REGNUMBER rn, char* operand2) {
  int operand2Value = parseOperand2(operand2);
  int iFlag = getIFlag(operand2);

  // delegates assembling to helper functions based on opcode
  switch (opCode) {
    case TST:
    case TEQ:
    case CMP:
      return assembleDataProcFlags(opCode, rn, operand2Value, iFlag);
    default:
      return assembleDataProcResult(opCode, rd, rn, operand2Value, iFlag);
  }
}

// general assembly that takes all arguments
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

// data proc that stores result
WORD assembleDataProcResult(enum OpCode opCode, REGNUMBER rd, REGNUMBER rn, int value, bool iFlag) {
  return assembleDataProcGeneral(opCode, rd, rn, value, iFlag, 0);
}

// data proc that only changes flags
WORD assembleDataProcFlags(enum OpCode opCode, REGNUMBER rn, int value, bool iFlag) {
  return assembleDataProcGeneral(opCode, 0, rn, value, iFlag, 1);
}

// mov as a data proc instruction
WORD assembleMov(REGNUMBER rd, int value, bool iFlag) {
  return assembleDataProcGeneral(MOV, rd, 0, value, iFlag, 0);
}

WORD assembleAndEq(void) {
    return 0;
}

WORD assembleLSL(REGNUMBER rn, char* operand2) {
    // TODO - amelia use parse operand 2 to get values for the 2nd arg (ask luke about it)
    return 0;
}


/* helper functions section */

char** importAssemblyInstr(char *fileName, int *numLines, node_t **map) {
    /* requires, as parameters, the input file name, pointer to number of lines, symbol table */
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
        if (match(tempContents[instrNum], ".*:")) {
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
    *numLines = instrNum - 1;

    char **assemblyInstructions = (char **) calloc(*numLines, sizeof(char*));
    for (int i = 0; i < *numLines; i++) {
        assemblyInstructions[i] = strtok(tempContents[i], "\n");
    }
    free(tempContents);

    return assemblyInstructions;
}

// used by assemble branch
BRANCHOFFSET calculateBranchOffset(char* target, ADDRESS currentAddress, node_t **symbolTable) {
    ADDRESS targetAddress;
//    targetAddress = (match(target, "^(\\+|-)?\\d+$"))
//            ? // how to interpret target being a number
//            : getHashmapValue(symbolTable, target);
    targetAddress = getHashmapValue(symbolTable, target);
    BRANCHOFFSET offset = targetAddress - currentAddress - 8;
    return (offset >> 2);
    // note: this returns the whole offset in 32 bits, we only store the bottom 24 bits
}

int parseImmediateValue(char *expression) {
  if (match(expression, "0x[0-9A-Fa-f]+")) {
    // expression in hex
    return strtol(&expression[2], NULL, 16);
  }

  // expression in dec
  return strtol(expression, NULL, 10);
}

/* parsing functions used by SDT, DataProc, and encodeInstruction functions */

// this is the main function which delegates to the 3 helper functions below
int parseOperand2(char* operand2) {
  if (checkIfImmediate(operand2)) {
    return parseImmediateOperand2(operand2);
  } else if (checkIfShiftedRegister(operand2)) {
    return parseShiftedRegister(operand2);
  } else {
      printf("Error: Invalid operand2.\n");
      return -1;
  }
}

REGNUMBER getRegisterNumber(char *regString) {
  return getRegNumWithRest(regString, NULL);
}

REGNUMBER getRegNumWithRest(char *regString, char *restOfOperand) {
  regString = trimWhiteSpace(regString);
  return strtol(&regString[1], &restOfOperand, 10);
}


bool checkIfImmediate(char* operand2) {
  return match(operand2, "(#|=).+");
}

bool checkIfShiftedRegister(char* operand2) {
  return match(operand2, "r([0-9]|1[0-6]).*");
}

int parseImmediateOperand2(char* operand2) {
  WORD value = parseImmediateValue(&operand2[1]);

  //
  if (value < (1 << 8)) {
      return value;
  }
  for (WORD halfRotation = 0; halfRotation * 2 < sizeof(WORD); halfRotation++) {
    WORD rotated = rotateLeft(value, halfRotation * 2);

    if (rotated < (1 << 8)) {
      return appendBits(8, halfRotation & FULLBITS(4), rotated);
    }
  }

  printf("Error: Number cannot be represented as a rotated byte.\n");
  return -1;
}

int parseShiftedRegister(char* operand2) {
    char *shiftString = "\0";
    BYTE rm = getRegNumWithRest(operand2, shiftString);

    // if register is not shifted
    if (*shiftString == '\0') {
        return rm;
    }

    // if register is shifted
    shiftString++; // ignore comma
    shiftString = trimWhiteSpace(shiftString);


    // find binary representation of shift type
    char *shiftType = strtok(shiftString, " ");
    char *shifts[4] = {"lsl", "lsr", "asr", "ror"}; //TODO: Find a better solution to this
    int shiftTypeBin = findPos(shiftType, shifts, 4);

    char *shiftAmount = strtok(NULL, " ");

    int shift;
    if (checkIfImmediate(shiftAmount)) {
        shift = parseImmediateValue(&shiftAmount[1]);
        shift = appendBits(2, shift, shiftTypeBin);
        shift <<= 1;

    } else if (checkIfShiftedRegister(shiftAmount)){
        shift = getRegisterNumber(shiftAmount);
        shift <<= 1;
        shift = appendBits(2, shift, shiftTypeBin);
        shift <<= 1;
        shift++;
    }

    return shift & FULLBITS(12);
}

int getIFlag(char* operand2) {
  return checkIfImmediate(operand2) ? 1 : 0;
}

/* utility functions */

WORD rotateLeft(WORD num, int shiftAmount) {
  return num << shiftAmount | num >> (sizeof(WORD) - shiftAmount);
}

// used to REGEX match instructions
bool match(const char *string, const char *pattern) {
  regex_t re;
  if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
    return false;
  }

  int status = regexec(&re, string, 0, NULL, 0);
  regfree(&re);

  return status == 0;
}

// removes any whitespace from front of string
char* trimWhiteSpace(char *string) {
    // pointer cannot be updated from here, so string++ does not work
    // in place shift does not work because string is a string literal so cannot be modified
    // we must instead return a pointer to the first non-space char
    while (isspace(string[0])) {
        string++;
    }
    return string;
}

// finds the position of a string in a string array
int findPos(char *string, char *strArray[], int arraySize) {
  for (int i = 0; i < arraySize; i++) {
    if (strArray[i] == string) {
      return i;
    }
  }
  return -1;
}