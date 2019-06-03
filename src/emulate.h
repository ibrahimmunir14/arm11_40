#ifndef ARM11_40_EMULATE_H
#define ARM11_40_EMULATE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "binaryOps.h"
#include "fileIO.h"
#include "machineDataTypes.h"

struct MachineState {
    REGISTER *registers; // (R13=SP; R14=LR; R15=PC; R16=CPSR)
    BYTE *memory;       // 64kb memory capacity, 1 word is 4 bytes
    WORD instrToExecute;
    WORD instrToDecode;
};

// apply a shift on val specified by shiftAndUpdateStatus amount, optionally update status bits
WORD shiftAndUpdateStatus(WORD val,
                          BYTE shiftAmount,
                          bool updateCPSR,
                          enum ShiftType shiftType,
                          struct MachineState *state);

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
// check condition code
bool checkCondition(enum CondCode condCode, const struct MachineState *state);

/* helper functions related to CPSR status bits */
// check flag is set
bool isSet(enum StatusFlag, const struct MachineState *state);
// set flag (to 1)
void setFlag(enum StatusFlag, struct MachineState *state);
// clear flag (to 0)
void clearFlag(enum StatusFlag, struct MachineState *state);

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

/* helper functions for execution of instructions */
// extract instruction type from the instruction bit string
enum InstrType getInstrType(WORD instr);
// extract type of sdt instruction being executed
enum SdtType getSdtType(WORD instr);
// extract type of data processing instruction being executed
enum DataProcType getDataProcType(WORD instr);
// extract opcode from instruction bit string
enum OpCode getOpCode(WORD instr);
// get offset from sdt instruction
SDTOFFSET getSDTOffset(enum SdtType sdtType, WORD offsetBits, struct MachineState *state) ;
// get operand2 from data processing instruction
DPOPERAND2 getDPOperand2(enum DataProcType dataProcType, WORD operand2Bits, bool modifyCPSR, struct MachineState *state);
// get operand from rotated immediate value data processing instruction
DPOPERAND2 getOperandFromImmRotation(WORD operandBits, bool modifyCPSR, struct MachineState *state);
// get operand from shifted register data processing instruction
WORD getOperandFromRegisterShift(WORD operandBits, bool regShift, bool modifyCPSR, struct MachineState *state);

#endif //ARM11_40_EMULATE_H