#include "memAccess.h"

WORD readFourBytes(ADDRESS startAddress, struct MachineState *state) {
    return (WORD) state->memory[startAddress] << 24u  | (WORD) state->memory[startAddress+1] << 16u
           | (WORD) state->memory[startAddress+2] << 8u | (WORD) state->memory[startAddress+3];
}

WORD readWord(ADDRESS startAddress, struct MachineState *state) {
    return (WORD) state->memory[startAddress+3] << 24u | (WORD) state->memory[startAddress+2] << 16u
           | (WORD) state->memory[startAddress+1] << 8u | (WORD) state->memory[startAddress];
}

void writeWord(WORD word, ADDRESS startAddress, struct MachineState *state) {
    state->memory[startAddress] = (BYTE) word;
    state->memory[startAddress+1] = (BYTE) (word >> 8u);
    state->memory[startAddress+2] = (BYTE) (word >> 16u);
    state->memory[startAddress+3] = (BYTE) (word >> 24u);
}