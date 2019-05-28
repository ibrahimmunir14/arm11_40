#ifndef ARM11_40_MEMACCESS_H
#define ARM11_40_MEMACCESS_H

#include "machine.h"

// returns a word (4 bytes) directly from memory given the address of the first byte
WORD readFourBytes(ADDRESS startAddress, struct MachineState *state);

// returns a word (4 bytes) from memory given the address of the first byte; converts to big-endian
WORD readWord(ADDRESS startAddress, struct MachineState *state);

// write a word (4 bytes) to memory in little-endian given the start address and big-endian word
void writeWord(WORD word, ADDRESS startAddress, struct MachineState *state);

#endif //ARM11_40_MEMACCESS_H
