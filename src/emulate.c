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
typedef int32_t OFFSET;     // offsets are 32 bits, signed

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
void incrementPC(void);
void printResults(void);
WORD readWord(ADDRESS);
bool checkCondition(enum CondCode);
bool isSet(enum CondFlag);
void setFlag(enum CondFlag);
void clearFlag(enum CondFlag);

void executeInstruction(WORD);
void performBranch(OFFSET offset);
void performSDT(bool iFlag, bool pFlag, bool upFlag, bool ldstFlag, BYTE rn, BYTE rd, OFFSET offset);
void performMultiply(bool aFlag, bool sFlag, BYTE rd, BYTE rn, BYTE rs, BYTE rm);
void performDataProcessOp2Register(enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, BYTE shift, BYTE rm);
void performDataProcessOp2ImmVal(enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, BYTE rotate, BYTE immVal);

// TODO: REMEMBER ENDIANNESS:
// each byte has its bits stored big-endian, i.e. MSB 01010101 LSB
// bytes within a word are stored little-endian, i.e. LSByte a1 03 b2 a3 MSByte
// therefore, first addressable byte contains word bits 7-0; second byte contains word bits 14-8, etc.

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

// checks whether an instruction should be executed based on condition code
bool checkCondition(enum CondCode condCode) {
    switch (condCode) {
        case AL : return true;
        case EQ : return isSet(Z);
        case NE : return !isSet(Z);
        case GE : return isSet(N) == isSet(V);
        case LT : return isSet(N) != isSet(V);
        case GT : return !isSet(Z) && (isSet(N) == isSet(V));
        case LE : return isSet(Z) || (isSet(N) != isSet(V));
        default : return true;
    }
}

// helper functions related to CPSR status flags
bool isSet(enum CondFlag flag) {
    return (registers[REG_CPSR] & flag << 4) == flag << 4;
}
void setFlag(enum CondFlag flag) {
    registers[REG_CPSR] = registers[REG_CPSR] | flag << 4;
}
void clearFlag(enum CondFlag flag) {
    registers[REG_CPSR] = registers[REG_CPSR] & (~(flag << 4));
}

// execute the given instruction
void executeInstruction(WORD instr) {
    // check if instruction should be executed
    bool doExecute = checkCondition((enum CondCode) (instr & (15 << 4))); // first 4 bits of msbyte

    if (doExecute) {
        if (((instr >> 26) & 3) == 2) { // 10 in bits 27-26; branch
            // TODO: calculate 32 bit signed offset
            // TODO: delegate to branch function
        } else if (((instr >> 26) & 3) == 1) { // 01 in bits 27-26; single data transfer
            // TODO: calculate I,P,U,L,Rn,Rd,offset
            // TODO: delegate to single data transfer function
        } else if (((instr >> 26) & 3) == 2) { // 00 in bits 27-26
            // TODO: determine whether this is data processing OR multiply
            // TODO: calculate appropriate offsets, operands, etc.
            // TODO: delegate to appropriate helper functions
        }
    }
}
