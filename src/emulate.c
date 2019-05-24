#include "emulate.h"

// TODO: REMEMBER ENDIANNESS:
// each byte has its bits stored big-endian, i.e. MSB 01010101 LSB
// bytes within a word are stored little-endian, i.e. LSByte a1 03 b2 a3 MSByte
// therefore, first addressable byte contains word bits 7-0; second byte contains word bits 14-8, etc.

int main(int argc, char **argv) {
    // ensure we have one argument, the filename
    if (argc != 2) { return EXIT_FAILURE; }

    struct MachineState state;

    // initialize registers, memory to 0s
    for (int i = 0; i < NUM_REG; i++) { state.registers[i] = 0; }
    for (int i = 0; i < MEM_SIZE; i++) { state.memory[i] = 0; }

    // set up memory array with instructions
    char *fileName = argv[1];
    FILE *fPointer = fopen(fileName, "rb");
    fseek(fPointer, 0, SEEK_END);
    int size = (int) ftell(fPointer);
    fseek(fPointer, 0, SEEK_SET);
    for (int i = 0; i < size; i++) {
        state.memory[i] = getc(fPointer);
    }

    // output memory contents after loading instructions (for testing)
    printf("Initial Memory Contents:\n");
    for (int i = 0; i < MEM_SIZE; i += 4) {
        WORD word = readWord(i, &state);
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
    while (state.registers[REG_PC] < MEM_SIZE) {
        // execute instrToExecute
        if (hasInstrToExecute) {
            if (instrToExecute == 0) {
                printf("TERMINATE: All-0 Instruction\n\n");
                break; // terminate on all-0 instruction
            } else {
                executeInstruction(instrToExecute, &state);
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
        instrToDecode = readWord(state.registers[REG_PC], &state);
        printf("Fetched Instruction at 0x%08x : 0x%08x\n", state.registers[REG_PC], instrToDecode);
        hasInstrToDecode = true;

        // increment program counter
        incrementPC(&state);
        printf("Incremented PC to 0x%08x\n\n", state.registers[REG_PC]);
    }

    printResults(&state);
    return EXIT_SUCCESS;
}

// increment the PC register to the address of the next word instruction
void incrementPC(struct MachineState *state) {
    state->registers[REG_PC] += 4;
}

// output the contents of registers and non-zero memory
void printResults(struct MachineState *state) {
    printf("Registers:\n");
    // print contents of general registers R0-R12
    for (int i = 0; i < NUM_GENERAL_REG; i++) {
        printf("$%-2i : %10i (0x%08x)\n", i, state->registers[i], state->registers[i]);
    }
    // print contents of pc and cpsr
    printf("PC  : %10i (0x%08x)\n", state->registers[REG_PC], state->registers[REG_PC]);
    printf("CPSR: %10i (0x%08x)\n", state->registers[REG_CPSR], state->registers[REG_CPSR]);

    // print contents of non-zero memory locations
    printf("Non-zero memory:\n");
    for (int i = 0; i < MEM_SIZE; i += 4) {
        WORD word = readWord(i, state);
        if (word != 0) {
            printf("0x%08x: 0x%08x\n", i, word);
        }
    }
}

// returns a word (4 bytes) given the address of the first byte
WORD readWord(ADDRESS startAddress, struct MachineState *state) {
    return state->memory[startAddress] << 24 | state->memory[startAddress+1] << 16
           | state->memory[startAddress+2] << 8 | state->memory[startAddress+3];
}

// checks whether an instruction should be executed based on condition code
bool checkCondition(enum CondCode condCode, struct MachineState *state) {
    switch (condCode) {
        case AL : return true;
        case EQ : return isSet(Z, state);
        case NE : return !isSet(Z, state);
        case GE : return isSet(N, state) == isSet(V, state);
        case LT : return isSet(N, state) != isSet(V, state);
        case GT : return !isSet(Z, state) && (isSet(N, state) == isSet(V, state));
        case LE : return isSet(Z, state) || (isSet(N, state) != isSet(V, state));
        default : return false;
    }
}

// helper functions related to CPSR status flags
bool isSet(enum CondFlag flag, struct MachineState *state) {
    return (state->registers[REG_CPSR] & flag << 4) == flag << 4;
}
void setFlag(enum CondFlag flag, struct MachineState *state) {
    state->registers[REG_CPSR] = state->registers[REG_CPSR] | flag << 4;
}
void clearFlag(enum CondFlag flag, struct MachineState *state) {
    state->registers[REG_CPSR] = state->registers[REG_CPSR] & (~(flag << 4));
}

// execute the given instruction
void executeInstruction(WORD instr, struct MachineState *state) {
    // check if instruction should be executed
    bool doExecute = checkCondition((enum CondCode) (instr & (15 << 4)), state); // first 4 bits of msbyte

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


void performBranch(OFFSET offset, struct MachineState *state) {

}

void performSDT(bool iFlag, bool pFlag, bool upFlag, bool ldstFlag, BYTE rn, BYTE rd, OFFSET offset, struct MachineState *state) {

}

void performMultiply(bool aFlag, bool sFlag, BYTE rd, BYTE rn, BYTE rs, BYTE rm, struct MachineState *state) {
    if (aFlag) {
        state->registers[rd] = state->registers[rm] * state->registers[rs] + state->registers[rn];
    } else {
        state-> registers[rd] = state-> registers[rm] * state-> registers[rs];
    }

    if (sFlag) {
        // set the Z flag for 0 values
        if (state->registers[rd] == 0) {
            setFlag(Z, state);
        }
        // set the N flag for <0 values
        if (getBitsFromRegister(rd, 31, 1, state) == 0) {
            clearFlag(N, state);
        } else {
            setFlag(N, state);
        }
    }
}

void performDataProcessOp2Register(enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, BYTE shift, BYTE rm, struct MachineState *state) {

}

void performDataProcessOp2ImmVal(enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, BYTE rotate, BYTE immVal, struct MachineState *state) {

}


