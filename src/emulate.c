#include "emulate.h"

// NOTE:
// Big-endian = Most significant to least significant
// Little-endian = Least significant to most significant
// Words in memory are stored Big-Endian
// Words in registers are stored Big-Endian
// Instructions in this code are Little-Endian
// use readWord/storeWord to read/write from Memory, auto taking care of conversions

// TODO: Make loop01 work

int main(int argc, char **argv) {
    // ensure we have one argument, the filename
    if (argc != 2) {
      return EXIT_FAILURE;
    }

    struct MachineState state = {{0}, {0}, false, false};

    // import file into memory
    char *fileName = argv[1];
    importFile(fileName, state.memory);

    /* Main Pipeline Loop */
    // initialize pipeline
    WORD instrToExecute = 0;
    WORD instrToDecode = 0;

    while (state.registers[REG_PC] < MEM_SIZE) {
        // execute instrToExecute
        if (state.hasInstrToExecute) {
            if (instrToExecute == 0) {
                break; // terminate on all-0 instruction
            }
            executeInstruction(instrToExecute, &state);
        }

        // decode instrToDecode and put it in instrToExecute
        // note: decoding is actually done during execution
        if (state.hasInstrToDecode) {
            state.hasInstrToExecute = true;
            instrToExecute = instrToDecode;
        }

        // fetch next instruction and put it in instrToDecode
        instrToDecode = readWord((ADDRESS) state.registers[REG_PC], &state);
        state.hasInstrToDecode = true;

        incrementPC(&state);
    }

    printResults(&state);
    return EXIT_SUCCESS;
}

void incrementPC(struct MachineState *state) {
    state->registers[REG_PC] += 4;
}

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
        WORD word = readFourBytes((ADDRESS) i, state);
        if (word != 0) {
            printf("0x%08x: 0x%08x\n", i, word);
        }
    }
}