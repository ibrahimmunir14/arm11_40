#include <stdlib.h>
#include <stdio.h>

int * make_array(char **argv, int *size, int *words);
void init_regs(int regs[17]);

struct state
{
    int *memory;
    int *registers;
    int executeInst;
    int decodeInst;
    int fetchInst;
};

int main(int argc, char **argv) {
  if (argc != 2) {
    return EXIT_FAILURE;
  }

  int zero = 0;

  struct state currentState;
  int registers[17];
  init_regs(registers);
  currentState.registers = &registers;
  int *size = &zero;
  int *words = &zero;
  currentState.memory = make_array(argv, size, words);
  while (currentState.fetchInst < size) {
    
  }

  return EXIT_SUCCESS;
}

int * make_array(char **argv, int *size, int *words) {

  FILE *fPointer;
  char *fileName = argv[1];
  fPointer = fopen(fileName, "r");

  fseek(fPointer, 0, SEEK_END);
  *size = (int) ftell(fPointer) / sizeof(int);
  fseek(fPointer, 0, SEEK_SET);

  words = (int *) malloc(*size * sizeof(int));
  int i;

  for (i = 0; i < *size; i ++) {
    int word[sizeof(int)];
    for (int j = 0; j < sizeof(int); j ++) {
      word[j] = getc(fPointer);
    }
    words[i] = word[0] << 24 | word[1] << 16 | word[2] << 8 | word[3];
  }

  fclose(fPointer);

  return words;
}

void init_regs(int regs[17]) {
    for (int i = 0; i < 17; i++) {
        regs[i] = 0;
    }
}


/* CHECKLIST
 *
 * 1. from the input, take each line and put it in an index of an array (how do we find the size?)
 * 2. define a struct which includes the pipeline (decode and execute variables), the array of registers, and all words in memory
 * 3. process each type of the 4 instructions with its separate function
 *
 * */
