#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    return EXIT_FAILURE;
  }
  FILE *fPointer;
  char *fileName = argv[1];
  fPointer = fopen(fileName, "r");

  fseek(fPointer, 0, SEEK_END);
  int size = ftell(fPointer);
  fseek(fPointer, 0, SEEK_SET);

  int words[size];
  int i;

  for (i = 0; i < size; i ++) {
    words[i] = getc(fPointer);
    printf("%d\n", words[i]);
  }

  fclose(fPointer);


  return EXIT_SUCCESS;
}


/* CHECKLIST
 *
 * 1. from the input, take each line and put it in an index of an array (how do we find the size?)
 * 2. define a struct which includes the pipeline (decode and execute variables), the array of registers, and all words in memory
 * 3. process each type of the 4 instructions with its separate function
 *
 * */
