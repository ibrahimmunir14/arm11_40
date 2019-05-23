#include <stdlib.h>
#include <stdio.h>

int * make_array(char **argv, int *size, int *words);
void init_regs(int regs[17]);

struct state
{
    int *mem;
    int regs[17];
    int fetchedInstruction;

};

int main(int argc, char **argv) {
  if (argc != 2) {
    return EXIT_FAILURE;
  }

  struct state currentState;
  int regs[17];
  init_regs(regs);
  *currentState.regs = &regs;

  int zero = 0;
  int *size = &zero;
  int *words = &zero;

  words = make_array(argv, size, words);

  printf("%d\n", *words);

  for (int i = 0; i < *size; i ++) {
    printf("%d\n", words[i]);
  }

  return EXIT_SUCCESS;
}

int * make_array(char **argv, int *size, int *words) {

  FILE *fPointer;
  char *fileName = argv[1];
  fPointer = fopen(fileName, "r");

  fseek(fPointer, 0, SEEK_END);
  *size = (int) ftell(fPointer);
  fseek(fPointer, 0, SEEK_SET);

  words = (int *) malloc(*size * sizeof(int));
  int i;

  for (i = 0; i < *size; i ++) {
    words[i] = getc(fPointer);
  }

  printf("%d\n", *words);

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
