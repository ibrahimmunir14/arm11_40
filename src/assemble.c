#include "assemble.h"
#include "fileIO.h"
#include <regex.h>
#include <string.h>

// TODO move this function into binary ops and maybe create a nibble type for value
void tokenize(char* line);

int main(int argc, char **argv) {

    char buff[80];
    strcpy (buff, "mov r1 r0");
    tokenize(buff);
//    printf("hello world!\n");
//    char* line1;
//    line1 = "mov something r1 something";
    encodeInstruction(buff);
//    printf("finished\n");
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
    const char branchPattern[] = "b*";
    const char mPattern[] = "m*";

    char* token = strtok(line," ");

    while (token != NULL)
    {
        printf ("%s\n",token);
        token = strtok(NULL, " ");
    }

    if (match(token, branchPattern)) {
        printf("matching on branch");
    } else if (match(token, mPattern)) {
        printf("matching on multiply or mov");
    } else if (match(token, "andeq")) {
        printf("matching on andeq");
    } else if (match(token, "lsl")) {
        printf("matching on lsl");
    } else if (match(token, "ldr")) {
        printf("matching on ldr");
    } else if (match(token, "str")) {
        printf("matching on str");
    } else {
        printf("matching on dataproc");
    }

    return value;
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
