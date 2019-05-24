#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// define types to aid readability
typedef uint32_t REGISTER;		// registers are 32 bits
typedef uint32_t WORD;			// words are 32 bits
typedef uint8_t BYTE;			// bytes are 8 bits
typedef uint8_t BIT;			// define bit data type

// store registers in an array of register (R13=SP; R14=LR; R15=PC; R16=CPSR)
// store memory as array of byte, 64kb memory capacity, 1 word is 4 bytes
#define NUM_REG 17
#define MEM_SIZE 65536
#define NUM_GENERAL_REG 13
#define REG_PC 15
#define REG_CPSR 16
REGISTER registers[NUM_REG];
BYTE memory[MEM_SIZE];

// define CPSR status bits
#define STATUS_NEG (1<<7) // negative bit
#define STATUS_ZER (1<<6) // zero bit
#define STATUS_CAR (1<<5) // carry bit
#define STATUS_OVF (1<<4) // overflow bit

enum CondFlag {V=1, C=2, Z=4, N=8};
enum CondCode {EQ=0, NE=1, GE=10, LT=11, GT=12, LE=13, AL=14};
enum OpCode {AND=0, EOR=1, SUB=2, RSB=3, ADD=4, TST=8, TEQ=9, CMP=10, ORR=12, MOV=13};

void printResults(void);

int main(int argc, char **argv) {
    // ensure we have one argument, the filename
    if (argc != 2) { return EXIT_FAILURE; }

    // initialize registers to 0s
    for (int i = 0; i < NUM_REG; i++) { registers[i] = 0; }
    // initialize memory to 0s
    for (int i = 0; i < MEM_SIZE; i++) { memory[i] = 0; }

    // set up memory array with instructions
    char *fileName = argv[1];
    FILE *fPointer = fopen(fileName, "rb");
    fseek(fPointer, 0, SEEK_END);
    int size = (int) ftell(fPointer);
    fseek(fPointer, 0, SEEK_SET);
    for (int i = 0; i < size; i++) {
        memory[i] = getc(fPointer);
    }

    // output memory contents after loading instructions (for testing)
    printf("Initial Memory Contents:\n");
    for (int i = 0; i < MEM_SIZE; i += 4) {
        WORD word = memory[i] << 24 | memory[i+1] << 16 | memory[i+2] << 8 | memory[i+3];
        if (word != 0) {
            printf("0x%08x: 0x%08x\n", i, word);
        }
    }
    printf("\n");

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
    for (int i = 0; i < NUM_GENERAL_REG; i++) {
        printf("$%-2i : %10i (0x%08x)\n", i, registers[i], registers[i]);
    }
    // print contents of pc and cpsr
    printf("PC  : %10i (0x%08x)\n", registers[REG_PC], registers[REG_PC]);
    printf("CPSR: %10i (0x%08x)\n", registers[REG_CPSR], registers[REG_CPSR]);

    // print contents of non-zero memory locations
    printf("Non-zero memory:\n");
    for (int i = 0; i < MEM_SIZE; i += 4) {
        WORD word = memory[i] << 24 | memory[i+1] << 16 | memory[i+2] << 8 | memory[i+3];
        if (word != 0) {
            printf("0x%08x: 0x%08x\n", i, word);
        }
    }
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

// helper functions related to CPSR status flags
int isSet(int flag) {
    // flag is set if appropriate bit in statusRegister is set
    return (registers[REG_CPSR] & flag) == flag;
}
void setFlag(int flag) {
    // perform bitwise 'or' to update appropriate bit in statusRegister
    registers[REG_CPSR] = registers[REG_CPSR] | flag; }
void clearFlag(int flag) {
    // perform bitwise 'and' to update appropriate bits in statusRegister
    registers[REG_CPSR] = registers[REG_CPSR] & (~flag);
}

int check_code(int instruction, int cpsr) {
    return instruction >> 28 == cpsr >> 28;
}
