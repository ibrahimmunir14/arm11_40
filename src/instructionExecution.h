#ifndef ARM11_40_INSTRUCTIONEXECUTION_H
#define ARM11_40_INSTRUCTIONEXECUTION_H

#include <stdio.h>
#include "machine.h"
#include "binaryOps.h"
#include "instructionDecoding.h"
#include "statusFlags.h"
#include "memAccess.h"

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

#endif //ARM11_40_INSTRUCTIONEXECUTION_H
