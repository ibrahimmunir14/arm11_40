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
    WORD* instructions = calloc(size, sizeof(WORD));
    for (int i = 0; i < size; i++) {
        // TODO: encodeInstruction must be modified to carry the hashmap
        instructions[i] = encodeInstruction(contents[i]);
    }

    // write instructions to output file
    char *outFileName = argv[2];
    binaryFileWriter(outFileName, instructions);
    return EXIT_SUCCESS;
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

int match(const char *string, const char *pattern)
{
  regex_t re;
  if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return 0;
  int status = regexec(&re, string, 0, NULL, 0);
  regfree(&re);
  if (status != 0) return 0;
  return 1;
}

bool match(const char *string, const char *pattern)
{
    regex_t re;
    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return false;
    int status = regexec(&re, string, 0, NULL, 0);
    regfree(&re);
    if (status != 0) return false;
    return true;
}

void tokenize(char* line)
{
    char* cmd = strtok(line," ");

    while (cmd != NULL)
    {
        printf ("%s\n",cmd);
        cmd = strtok(NULL, " ");
    }
}

WORD encodeInstruction(char* line) {
    WORD value = 0;
    WORD value = 0;
    const char branchPattern[] = "^b";
    const char mPattern[] = "^m";
    const char sPattern[] = "^andeq";
    const char s2Pattern[] = "^lsl";
    const char stdPattern[] = "^str";
    const char std2Pattern[] = "^ldr";


    char* token = strtok(line," ");
    char str1[100] = "beq label";
    char strArray[10][10];
    int i,j,ctr;


    if (match(token, branchPattern)) {
        j=0; ctr=0;
        for(i=0;i<=(strlen(str1));i++)
        {
            // if space or NULL found, assign NULL into newString[ctr]
            if(str1[i]==' '||str1[i]=='\0')
            {
                strArray[ctr][j]='\0';
                ctr++;  //for next word
                j=0;    //for next word, init index to 0
            }
            else
            {
                strArray[ctr][j]=str1[i];
                j++;
            }
        }
        for(i=0;i < ctr;i++)
            printf(" %s\n",strArray[i]);

        if (match(strArray[0], "b")) {
            printf("matching on branch");
            printf("matching on branch");
        } else if (match(token, mPattern)) {
        } else if (match(strArray[0], "^mov")) {
            printf("matching on multiply or mov");
            printf("matching on mov");
        } else if (match(token, "^andeq")) {
            return assembleMov(getRegisterNumber(strArray[1]), parseOperand2(strArray[2]));
        } else if (match(strArray[0], "^mul")) {
            printf("matching on mul");
            return assembleMultiply(getRegisterNumber(strArray[1]), getRegisterNumber(strArray[2]), getRegisterNumber(strArray[3]), 0, false);
        } else if (match(strArray[0], "^mla")) {
            printf("matching on mla");
            return assembleMultiply(getRegisterNumber(strArray[1]), getRegisterNumber(strArray[2]), getRegisterNumber(strArray[3]), getRegisterNumber(strArray[4]), true);
        } else if (match(strArray[0], "^andeq")) {
            printf("matching on andeq");
            printf("matching on andeq");
        } else if (match(token, "^lsl")) {
            return assembleAndEq();
        } else if (match(strArray[0], "^lsl")) {
            printf("matching on lsl");
            printf("matching on lsl");
        } else if (match(token, "^ldr")) {
            return assembleLSL(getRegisterNumber(strArray[1]), parseOperand2(strArray[2]));
        } else if (match(strArray[0], "^ldr")) {
            printf("matching on ldr");
            printf("matching on ldr");
        } else if (match(token, "^str")) {
            return assembleSDT(true, getRegisterNumber(strArray[1]), NULL, NULL, NULL);
        } else if (match(strArray[0], "^str")) {
            printf("matching on str");
            printf("matching on str");
            return assembleSDT(false, NULL, getRegisterNumber(strArray[1]), NULL, NULL);
        } else {
        } else {
            printf("matching on dataproc");
            printf("matching on dataproc");
        }
    }
    return value;
}


REGNUMBER getRegisterNumber(char* reg) {
    return (u_int8_t) atoi(&reg[1]);
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

BYTE getRegNum(char *regString, char *restOfOperand) {
  trimWhiteSpace(regString);
  return strtol(&regString[1], &restOfOperand, 10) & FULLBITS(4);
}

int parseOperand2(char* operand2) {
  char *immediatePattern = "#.+";
  char *shiftedRegister = "r([0-9]|1[0-6]).*";
  if (match(operand2, immediatePattern)) {
    WORD value = parseExpression(&operand2[1]);

    for (WORD rotation = 0; rotation < sizeof(WORD); rotation += 2) {
      WORD rotated = (value >> rotation | value << (sizeof(WORD) - rotation));

      if (rotated < (1 << 8)) {
        return appendBits(8, rotation & FULLBITS(4), (BYTE) rotated);
      }
    }

  } else if (match(operand2, shiftedRegister)) {
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

