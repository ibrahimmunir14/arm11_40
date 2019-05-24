#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

// define types to aid readability
typedef uint32_t REGISTER;	// registers are 32 bits
typedef uint32_t WORD;		// words are 32 bits
typedef uint16_t ADDRESS;   // addresses are 16 bits
typedef uint8_t BYTE;		// bytes are 8 bits

// define constants related to registers
// store registers in an array of register (R13=SP; R14=LR; R15=PC; R16=CPSR)
// store memory as array of byte, 64kb memory capacity, 1 word is 4 bytes
#define NUM_REG 17
#define MEM_SIZE 65536
#define NUM_GENERAL_REG 13
#define REG_PC 15
#define REG_CPSR 16
REGISTER registers[NUM_REG];
BYTE memory[MEM_SIZE];

enum CondFlag {V=1, C=2, Z=4, N=8};
enum CondCode {EQ=0, NE=1, GE=10, LT=11, GT=12, LE=13, AL=14};
enum OpCode {AND=0, EOR=1, SUB=2, RSB=3, ADD=4, TST=8, TEQ=9, CMP=10, ORR=12, MOV=13};

// function declarations
WORD readWord(ADDRESS startAddress);
void executeInstruction(WORD inst);
void incrementPC(void);
void printResults(void);

int main(int argc, char **argv) {
    // ensure we have one argument, the filename
    if (argc != 2) { return EXIT_FAILURE; }

    // initialize registers, memory to 0s
    for (int i = 0; i < NUM_REG; i++) { registers[i] = 0; }
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
        WORD word = readWord(i);
        if (word != 0) {
            printf("0x%08x: 0x%08x\n", i, word);
        }
    }
    printf("\n");

    // main pipeline loop
    WORD instrToExecute = 0;
    WORD instrToDecode = 0;
    bool hasInstrToExecute = false;
    bool hasInstrToDecode = false;
    while (registers[REG_PC] < MEM_SIZE) {
        // execute instrToExecute
        if (hasInstrToExecute) {
            if (instrToExecute == 0) {
                printf("TERMINATE: All-0 Instruction\n\n");
                break; // terminate on all-0 instruction
            } else {
                executeInstruction(instrToExecute);
                printf("Executed Instruction 0x%08x\n", instrToExecute);
            }
        }

        // decode instrToDecode and put it in instrToExecute
        // note: decoding is actually done during execution
        if (hasInstrToDecode) {
            hasInstrToExecute = true;
            instrToExecute = instrToDecode;
            printf("Decoded Instruction 0x%08x\n", instrToExecute);

        }

        // fetch next instruction and put it in instrToDecode
        instrToDecode = readWord(registers[REG_PC]);
        printf("Fetched Instruction at 0x%08x : 0x%08x\n", registers[REG_PC], instrToDecode);
        hasInstrToDecode = true;

        // increment program counter
        incrementPC();
        printf("Incremented PC to 0x%08x\n\n", registers[REG_PC]);
    }

    printResults();
    return EXIT_SUCCESS;
}

// decode the given instruction and delegate to appropriate helper functions
void executeInstruction(WORD instr) {
    // TODO: Implement this function to decode instruction and delegate to appropriate functions

}

// increment the PC register to the address of the next word instruction
void incrementPC(void) {
    registers[REG_PC] += 4;
}

// output the contents of registers and non-zero memory
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
        WORD word = readWord(i);
        if (word != 0) {
            printf("0x%08x: 0x%08x\n", i, word);
        }
    }
}

// returns a word (4 bytes) given the address of the first byte
WORD readWord(ADDRESS startAddress) {
    return memory[startAddress] << 24 | memory[startAddress+1] << 16
           | memory[startAddress+2] << 8 | memory[startAddress+3];
}

// helper functions related to CPSR status flags
int isSet(int flag) {
    // flag is set if appropriate bit in statusRegister is set
    return (registers[REG_CPSR] & flag << 4) == flag << 4;
}
void setFlag(int flag) {
    // perform bitwise 'or' to update appropriate bit in statusRegister
    registers[REG_CPSR] = registers[REG_CPSR] | flag << 4; }
void clearFlag(int flag) {
    // perform bitwise 'and' to update appropriate bits in statusRegister
    registers[REG_CPSR] = registers[REG_CPSR] & (~(flag << 4));
}

int check_code(int instruction, int cpsr) {
    return instruction >> 28 == cpsr >> 28;
}
