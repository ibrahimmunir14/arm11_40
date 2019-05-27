#ifndef ARM11_40_EMULATE_H
#define ARM11_40_EMULATE_H

#endif //ARM11_40_EMULATE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

// define types to aid readability
typedef uint32_t REGISTER;	// register contents are 32 bits
typedef uint8_t REGNUMBER;  // register numbers are 4 bits, unsigned because register numbers positive
typedef uint32_t WORD;		// words/instructions are 32 bits
typedef uint16_t ADDRESS;   // memory addresses are 16 bits, unsigned because addresses positive
typedef uint8_t BYTE;		// bytes are 8 bits
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
    bool hasInstrToExecute;
    bool hasInstrToDecode;
};

// enums for common values/types
enum StatusFlag {V=1, C=2, Z=4, N=8};
enum CondCode {EQ=0, NE=1, GE=10, LT=11, GT=12, LE=13, AL=14};
enum OpCode {AND=0, EOR=1, SUB=2, RSB=3, ADD=4, TST=8, TEQ=9, CMP=10, ORR=12, MOV=13};
enum ShiftType {LSL=0, LSR=1, ASR=2, ROR=3};
enum InstrType {instrUnknown, instrDataProcessing, instrMultiply, instrSDT, instrBranch};
enum DataProcType {dataProcOp2RegShiftConst, dataProcOp2RegShiftReg, dataProcOp2Imm};
enum SdtType {sdtOffsetRegShiftConst, sdtOffsetRegShiftReg, sdtOffsetImm};

/* useful bit-related functions */
// return value representing numBits 1s in a row
static inline WORD fullBits(BYTE numBits) {
    return (WORD) pow(2, numBits) - 1;
}
// extract the chosen bits (using number scheme from spec, big-endian), and return right-aligned bits
WORD getBitsFromWord(WORD word, BYTE startBitNo, BYTE numBits);
// apply a shift on val specified by shift amount, optionally update status bits
WORD shift(WORD val, BYTE shiftAmount, bool updateCPSR, enum ShiftType shiftType, struct MachineState *state);
// sign extend val to 32 bits, given the original length of val
WORD signExtend(WORD val, BYTE originalLength);

/* functions related to memory access */
// returns a word (4 bytes) directly from memory given the address of the first byte
WORD readFourBytes(ADDRESS startAddress, struct MachineState *state);
// returns a word (4 bytes) from memory given the address of the first byte; converts to big-endian
WORD readWord(ADDRESS startAddress, struct MachineState *state);
// write a word (4 bytes) to memory in little-endian given the start address and big-endian word
void writeWord(WORD word, ADDRESS startAddress, struct MachineState *state);

/* helper functions for main program */
void incrementPC(struct MachineState *state);
void printResults(struct MachineState *state);
bool checkCondition(enum CondCode condCode, struct MachineState *state);

/* helper functions related to CPSR status bits */
bool isSet(enum StatusFlag, struct MachineState *state);
void setFlag(enum StatusFlag, struct MachineState *state);
void clearFlag(enum StatusFlag, struct MachineState *state);

/* functions for execution of instructions */
void executeInstruction(WORD instruction, struct MachineState *state);
void performBranch(WORD offsetBits, struct MachineState *state);
void performSdt(enum SdtType sdtType, bool pFlag, bool upFlag, bool ldstFlag, REGNUMBER rn, REGNUMBER rd, WORD offsetBits, struct MachineState *state);
void performMultiply(bool aFlag, bool sFlag, REGNUMBER rd, REGNUMBER rn, REGNUMBER rs, REGNUMBER rm, struct MachineState *state);
void performDataProc(enum DataProcType dataProcType, enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, WORD operand2Bits, struct MachineState *state);

/* helper functions for execution of instructions */
enum InstrType getInstrType(WORD instr);
enum SdtType getSdtType(WORD instr);
enum DataProcType getDataProcType(WORD instr);
enum OpCode getOpCode(WORD instr);
SDTOFFSET getSDTOffset(enum SdtType sdtType, WORD offsetBits, struct MachineState *state) ;
DPOPERAND2 getDPOperand2(enum DataProcType dataProcType, WORD operand2Bits, bool modifyCPSR, struct MachineState *state);
DPOPERAND2 getOperandFromImmRotation(WORD operandBits, bool modifyCPSR, struct MachineState *state);
WORD getOperandFromRegisterShift(WORD operandBits, bool regShift, bool modifyCPSR, struct MachineState *state);