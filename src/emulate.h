#ifndef ARM11_40_EMULATE_H
#define ARM11_40_EMULATE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "binaryOps.h"
#include "binaryFileLoader.h"

// define types to aid readability
typedef uint8_t REGNUMBER;  // register numbers are 4 bits, unsigned because register numbers positive
typedef int32_t SDTOFFSET;     // SDT offset: imm offset 12 bits; reg offset is 32 bits
typedef uint32_t DPOPERAND2;     // imm operand2 8 bits; reg operand2 is 32 bits
typedef int32_t BRANCHOFFSET; // branch offset is signed 24-bit offset

// define constants related to registers
#define NUM_REG 17
#define MEM_SIZE 65536
#define NUM_GENERAL_REG 13
#define REG_PC 15
#define REG_CPSR 16

struct MachineState {
    REGISTER registers[NUM_REG]; // (R13=SP; R14=LR; R15=PC; R16=CPSR)
    BYTE memory[MEM_SIZE];       // 64kb memory capacity, 1 word is 4 bytes
    WORD instrToExecute;
    WORD instrToDecode;
};

// enums for common values/types
enum StatusFlag {V=1, C=2, Z=4, N=8};
enum CondCode {EQ=0, NE=1, GE=10, LT=11, GT=12, LE=13, AL=14};
enum OpCode {AND=0, EOR=1, SUB=2, RSB=3, ADD=4, TST=8, TEQ=9, CMP=10, ORR=12, MOV=13};
enum ShiftType {LSL=0, LSR=1, ASR=2, ROR=3};
enum InstrType {instrUnknown, instrDataProcessing, instrMultiply, instrSDT, instrBranch};
enum DataProcType {dataProcOp2RegShiftConst, dataProcOp2RegShiftReg, dataProcOp2Imm};
enum SdtType {sdtOffsetRegShiftConst, sdtOffsetRegShiftReg, sdtOffsetImm};

// apply a shift on val specified by shift amount, optionally update status bits
WORD shift(WORD val, BYTE shiftAmount, bool updateCPSR, enum ShiftType shiftType, struct MachineState *state);

/* functions related to memory access */
// returns a word (4 bytes) directly from memory given the address of the first byte
WORD readFourBytes(ADDRESS startAddress, struct MachineState *state);
// returns a word (4 bytes) from memory given the address of the first byte; converts to big-endian
WORD readWord(ADDRESS startAddress, struct MachineState *state);
// write a word (4 bytes) to memory in little-endian given the start address and big-endian word
void writeWord(WORD word, ADDRESS startAddress, struct MachineState *state);

/* helper functions for main program */
// increment program counter
void incrementPC(struct MachineState *state);
// read the next instruction from memory pointed to by PC register
WORD readNextInstr(struct MachineState *state);
// print current state of machine
void printResults(struct MachineState *state);
// check condition code
bool checkCondition(enum CondCode condCode, struct MachineState *state);

/* helper functions related to CPSR status bits */
// check flag is set
bool isSet(enum StatusFlag, struct MachineState *state);
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