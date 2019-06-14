#ifndef ARM11_40_EMULATE_H
#define ARM11_40_EMULATE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "binaryOps.h"
#include "fileIO.h"
#include "machineDataTypes.h"
#include "emulateGetting.h"

/* functions related to memory access */
// returns a word (4 bytes) directly from memory given the address of the first byte
WORD readFourBytes(ADDRESS startAddress, const struct MachineState *state);
// returns a word (4 bytes) from memory given the address of the first byte; converts to big-endian
WORD readWord(ADDRESS startAddress, const struct MachineState *state);
// write a word (4 bytes) to memory in little-endian given the start address and big-endian word
void writeWord(WORD word, ADDRESS startAddress, struct MachineState *state);

/* helper functions for main program */
// read the next instruction from memory pointed to by PC register
WORD readNextInstr(const struct MachineState *state);
// print current state of machine
void printResults(const struct MachineState *state);


/* functions for execution of instructions */
// execute instruction with the current state
void executeInstruction(WORD instruction, struct MachineState *state);
// perform a branch instruction with the given parameters and state
void performBranch(WORD offsetBits, struct MachineState *state);
// perform an sdt instruction with the given parameters and state
void performSdt(enum SdtType sdtType, bool pFlag, bool upFlag, bool ldstFlag, REGNUMBER rn, REGNUMBER rd, WORD offsetBits, struct MachineState *state);
// perform a multiply instruction with the given parameters and state
void performMultiply(bool aFlag, bool sFlag, REGNUMBER rd, REGNUMBER rn, REGNUMBER rs, REGNUMBER rm, struct MachineState *state);
// perform a data processing instruction with the given parameters and state
void performDataProc(enum DataProcType dataProcType, enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, WORD operand2Bits, struct MachineState *state);

bool checkGPIOInstruction(REGISTER regContents);
#endif //ARM11_40_EMULATE_H