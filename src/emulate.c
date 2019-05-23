#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int * make_array(char **argv, int *size, int *words);
void init_regs(int regs[17]);

enum CondFlag {V=1, C=2, Z=4, N=8};

enum CondCode {EQ=0, NE=1, GE=10, LT=11, GT=12, LE=13, AL=14};


enum OpCode {AND=0, EOR=1, SUB=2, RSB=3, ADD=4, TST=8, TEQ=9, CMP=10, ORR=12, MOV=13};

typedef struct {
    int *memory;
    int *registers;
    int executeInst;
    int decodeInst;
    int fetchInstIndex;
} State;

int main(int argc, char **argv) {
  if (argc != 2) {
    return EXIT_FAILURE;
  }

  int zero = 0;

  State currentState;
  int registers[17];
  init_regs(registers);
  currentState.registers = &registers;
  int *size = &zero;
  int *words = &zero;
  currentState.memory = make_array(argv, size, words);
  while (currentState.fetchInstIndex < size) {

  }

  return EXIT_SUCCESS;
}

void update_pipeline(State* currentState) {
//  possible ideas for code below
//  currentState.executeInst = currentState.decodeInst;
//  currentState.decodeInst = currentState.memory[currentState.fetchInstIndex];
//  currentState.fetchInstIndex ++;
  return;
}

void parse_data_processing(State* currentState) {

}

void parse_multiply(State* currentState) {

}

void parse_single_data_transfer(State* currentState) {

}

void parse_branch(State* currentState) {
  // this instruction will not update the pipeline in the usual way
  // it will clear the pipeline
  // and then set the new variables
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

int check_code(int instruction, int cpsr) {
    return instruction >> 28 == cpsr >> 28;
}



/* CHECKLIST
 *
 * 1. from the input, take each line and put it in an index of an array (how do we find the size?)
 * 2. define a struct which includes the pipeline (decode and execute variables), the array of registers, and all words in memory
 * 3. process each type of the 4 instructions with its separate function
 *
 * */
