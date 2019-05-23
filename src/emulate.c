#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// define types to aid readability
typedef uint32_t REGISTER;		//registers are 32 bits
typedef uint32_t WORD;			//words are 32 bits
typedef uint8_t BYTE;			//bytes are 8 bits
typedef uint8_t BIT;			//define bit data type

// store registers in an array of register (R13=SP; R14=LR; R15=PC; R16=CPSR)
REGISTER registers[17];
// store memory as array of word, 64kb memory capacity, 1 word is 4 bytes
WORD memory[16384];

// define CPSR status bits
#define STATUS_NEG (1<<7) // negative bit
#define STATUS_ZER (1<<6) // zero bit
#define STATUS_CAR (1<<5) // carry bit
#define STATUS_OVF (1<<4) // overflow bit
BYTE statusRegister = 0; // byte corresponding to status register

// helper functions related to CPSR status flags
int isSet(int flag) {
    // flag is set if bitwise 'and' operation results in flag,
    //  i.e. appropriate bit in statusRegister is set
    return (statusRegister & flag) == flag;
}
void setFlag(int flag) {
    // perform bitwise 'or' to update appropriate bit in statusRegister
    statusRegister = statusRegister | flag;
}
void clearFlag(int flag) {
    // perform bitwise 'and' to update appropriate bits in statusRegister
    statusRegister = statusRegister & (~flag);
}

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

void printResults(void);

int main(int argc, char **argv) {
    // ensure we have one argument, the filename
    if (argc != 2) { return EXIT_FAILURE; }

    // initialize registers to 0s
    for (int i = 0; i < 17; i++) { registers[i] = 0; }

    // set up memory array with instructions
    FILE *fPointer;
    char *fileName = argv[1];
    fPointer = fopen(fileName, "rb");

    fseek(fPointer, 0, SEEK_END);
    int size = (int) ftell(fPointer) / sizeof(int);
    fseek(fPointer, 0, SEEK_SET);

    for (int i = 0; i < size; i++) {
        int word[sizeof(int)];
        for (int j = 0; j < sizeof(int); j++) {
            word[j] = getc(fPointer);
        }
        memory[i] = word[0] << 24 | word[1] << 16 | word[2] << 8 | word[3];
    }

    // output contents for testing purposes
    for (int i = 0; i < size; i ++) {
        printf("%d\n", memory[i]);
    }

    // main pipeline loop
    State currentState;
    //currentState.registers = &registers;
    //while (currentState.fetchInstIndex < size) {

    //}

    printResults();
    return EXIT_SUCCESS;
}

void printResults(void) {
    printf("Registers:\n");
    // print contents of general registers R0-R12
    for (int i = 0; i <= 12; i++) {
        printf("$%-2i : %10i (0x%08x)\n", i, registers[i], registers[i]);
    }
    // print contents of pc and cpsr
    printf("PC  : %10i (0x%08x)\n", registers[15], registers[15]);
    printf("CPSR: %10i (0x%08x)\n", registers[16], registers[16]);
    // TODO: print contents of memory
    printf("Non-zero memory:\n");
    // This depends on our implementation of memory - store as words or bytes? to dicuss!

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
