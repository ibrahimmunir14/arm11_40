#include "assemble.h"
#include "hashmapAbstract.h"

// TODO getRegNum and getRegisterNumber maybe duplicated
// TODO add documentation to all functions
// TODO separate functions out into separate files
// TODO add assembleSpecial functions
// TODO finish off SDT - luke
// TODO finish off DataProcGeneral - luke
// TODO understand parts of the codebase

int main(int argc, char **argv) {
    // ensure we have two argument, the filenames
    if (argc != 3) {
        return EXIT_FAILURE;
    }
    // import data into instructions array and symbol table
    char *inFileName = argv[1];
    int numInstructions;
    node_t **symbolTable = init();
    char **assInstructions = importAssemblyInstr(inFileName, &numInstructions, symbolTable);

    // print contents, for debugging purposes
    for (int i = 0; i < numInstructions; i++) {
        printf("[%s]\n", assInstructions[i]);
    }
    display(symbolTable);

    // set up instruction and reserve memory space
    WORD* armInstructions = calloc(numInstructions, sizeof(WORD));
    WORD* reserveMemory = calloc(numInstructions, sizeof(WORD));
    int numReserve = 0; // number of reserve memory locations used up (in WORDS)

    // encode assembly instructions into arm words sequentially
    for (int i = 0; i < numInstructions; i++) {
        armInstructions[i] = encodeInstruction(assInstructions[i], i * 4, &reserveMemory[numReserve], &numReserve);
    }

    // write instructions and reserved memory to output file
    char *outFileName = argv[2];
    binaryFileWriter(outFileName, armInstructions, reserveMemory, numInstructions, numReserve);
    return EXIT_SUCCESS;
}

/*
 * Note: This function takes in the instruction string, currentAddress, pointer to next free reserve memory location,
 *       and pointer to counter of number of reserve locations used.
 *       - currentAddress is needed for branch operations and SDT, to calculate offsets
 *       - *nextReserveMemory is to be passed to SDT function, and the value/contents is to be updated directly
 *       - *numReserve is to be passed to SDT function, and should be incremented if *nextReserveMemory is used
 */
WORD encodeInstruction(char* line, ADDRESS currentAddress, WORD *nextReserveMemory, int *numReserve) {
    WORD value = 0;
//    char str1[100] = "beq label";
    char strArray[10][10];
    int i,j,ctr;

    j=0; ctr=0;
    for(i=0;i<=(strlen(line));i++)
    {
        // if space or NULL found, assign NULL into newString[ctr]
        if(line[i]==' '||line[i]=='\0'||line[i]==','||line[i]=='\n')
        {
            strArray[ctr][j]='\0';
            ctr++;  //for next word
            j=0;    //for next word, init index to 0
        }
        else
        {
            strArray[ctr][j]=line[i];
            j++;
        }
    }
    for(i=0;i < ctr-1;i++)
        printf(" '%s'\n",strArray[i]);

    if (match(strArray[0], "^b")) {
        printf("matching on branch\n");
    } else if (match(strArray[0], "^mov")) {
        printf("matching on mov\n");
//        return assembleMov(getRegisterNumber(strArray[1]), parseOperand2(strArray[2]), getIFlag(strArray[2]));
    } else if (match(strArray[0], "^mul")) {
        printf("matching on mul\n");
        return assembleMultiply(getRegisterNumber(strArray[1]), getRegisterNumber(strArray[2]), getRegisterNumber(strArray[3]), 0, false);
    } else if (match(strArray[0], "^mla")) {
        printf("matching on mla\n");
        return assembleMultiply(getRegisterNumber(strArray[1]), getRegisterNumber(strArray[2]), getRegisterNumber(strArray[3]), getRegisterNumber(strArray[4]), true);
    } else if (match(strArray[0], "^andeq")) {
        printf("matching on andeq\n");
//        return assembleAndEq();
    } else if (match(strArray[0], "^lsl")) {
        printf("matching on lsl\n");
//        return assembleLSL(getRegisterNumber(strArray[1]), parseOperand2(strArray[2]));
    } else if (match(strArray[0], "^ldr")) {
        printf("matching on ldr\n");
        return assembleSDT(false, getRegisterNumber(strArray[1]), strArray[2], currentAddress, nextReserveMemory, numReserve);
    } else if (match(strArray[0], "^str")) {
        printf("matching on str\n");
//        return assembleSDT(false, 0, getRegisterNumber(strArray[1]), &currentAddress, nextReserveMemory, numReserve);
    } else {
        printf("matching on dataproc\n");
        //  return assembleDataProc()
    }
    return value;
}

/* assembling functions */

WORD assembleBranch(enum CondCode condCode, char* target, ADDRESS currentAddress) {
    WORD instr = 0;
    instr = appendNibble(instr, (BYTE) condCode);
    instr = appendNibble(instr, 10); // 0b1010
    instr = appendNibble(instr, calculateBranchOffset(target, currentAddress));
    return instr;
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

// below 3 functions call assembleDataProcGeneral with correct arguments
WORD assembleDataProcResult(enum OpCode opCode, REGNUMBER rd, REGNUMBER rn, int value, bool iFlag) {
  return assembleDataProcGeneral(opCode, rd, rn, value, iFlag, 0);
}

WORD assembleDataProcFlags(enum OpCode opCode, REGNUMBER rn, int value, bool iFlag) {
  return assembleDataProcGeneral(opCode, 0, rn, value, iFlag, 1);
}

WORD assembleMov(REGNUMBER rd, int value, bool iFlag) {
  return assembleDataProcGeneral(MOV, rd, 0, value, iFlag, 0);
};

// assembling instructions for MUL and MLA commands
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

/*
 * Note for Luke: Ibrahim and Umer have discussed and come up with a simpler way of passing and updating reserve memory.
 *                See note above executeInstruction. Please text if any confusion.
 *                Function declaration will need to be modified, I haven't done so to avoid conflict.
 */
WORD assembleSDT(bool lFlag, REGNUMBER rd, char* sdtAddressParameter, ADDRESS currentAddress, WORD *nextReserveMemory, int *numReserve) {
  REGNUMBER rn;
  WORD offset = 0;
  bool iFlag = false;
  bool pFlag = false;
  bool uFlag = false;


  if (lFlag) { //ldr
    if (sdtAddressParameter[0] == '=') {
      int value = parseImmediateValue(&sdtAddressParameter[1]);

      if (value <= 0xFF) {
        return assembleMov(rd, value, 1);
      }

      *nextReserveMemory = value;
      offset = nextReserveMemory - currentAddress;
      *numReserve++;
      rn = REG_PC;

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
        } else { // line read in is an instruction: keep stored in tempContents
            instrNum++;
        }
    }
    fclose(file);
    *numLines = instrNum - 1;

    char **assemblyInstructions = (char **) calloc(*numLines, sizeof(char*));
    for (int i = 0; i < *numLines; i++) {
        assemblyInstructions[i] = tempContents[i];
    }
    free(tempContents);

    return assemblyInstructions;
}

// used by assemble branch
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

// used to REGEX match instructions
bool match(const char *string, const char *pattern)
{
    regex_t re;
    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return false;
    int status = regexec(&re, string, 0, NULL, 0);
    regfree(&re);
    return status == 0;
}

void trimWhiteSpace(char *string) {
  while (isspace(string[0])) {
    string++;
  }
}

bool checkIfImmediate(char* operand2) {
  return match(operand2, "#.+");
}

bool checkIfShiftedRegister(char* operand2) {
  return match(operand2, "r([0-9]|1[0-6]).*");
}

int findPos(char *string, char *strArray[], int arraySize) {
    for (int i = 0; i < arraySize; i++) {
        if (strArray[i] == string) {
            return i;
        }
    }
    return -1;
}

REGNUMBER getRegisterNumber(char *regString, char *restOfOperand) {
    trimWhiteSpace(regString);
    return strtol(&regString[1], &restOfOperand, 10);
}


int getIFlag(char* operand2) {
  return checkIfImmediate(operand2) ? 1 : 0;
}

/* parsing functions used by SDT, DataProc, and encodeInstruction functions */

// this is the main function which delegates to the 3 helper functions below
int parseOperand2(char* operand2) {
  if (checkIfImmediate(operand2)) {
    parseImmediateOperand2(operand2);
  } else if (checkIfShiftedRegister(operand2)) {
    parseShiftedRegister(operand2);
  }

  printf("Error: Invalid operand2.\n");
  return -1;
}


WORD assembleAndEq(void) {

}

WORD assembleLSL(REGNUMBER rn, int value) {

}

int parseShiftedRegister(char* operand2) {
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

    if (checkIfImmediate(restOfOperand)) {
        shift = parseImmediateValue(exp);
        shift = appendBits(2, shift, shiftTypeBin);
        shift <<= 1;

    } else if (checkIfShiftedRegister(restOfOperand)){
        shift = getRegNum(exp, NULL);
        shift <<= 1;
        char *exp = strtok(NULL, " ");
        shift <<= 1;
        shift++;
    }

    return shift & FULLBITS(12);
}

int parseImmediateOperand2(char* operand2) {
    WORD value = parseImmediateValue(&operand2[1]);

    for (WORD rotation = 0; rotation < sizeof(WORD); rotation += 2) {
        WORD rotated = (value >> rotation | value << (sizeof(WORD) - rotation));

        if (rotated < (1 << 8)) {
            return appendBits(8, rotation & FULLBITS(4), (BYTE) rotated);
        }
    }
}

int parseImmediateValue(char *expression) {
    if (match(expression, "0x[0-9A-Fa-f]+")) {
        return strtol(&expression[2], NULL, 16);
    }

    return strtol(expression, NULL, 10);
}
>>>>>>> d715e11b48dd621e33e574f11eca3e68f5ad27a1
