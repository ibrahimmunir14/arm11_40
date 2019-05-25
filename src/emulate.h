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
typedef uint32_t SDTOFFSET;     // SDT offset always positive: imm offset 12 bits; reg offset is 32 bits

// define constants related to registers
// store registers in an array of register (R13=SP; R14=LR; R15=PC; R16=CPSR)
// store memory as array of byte, 64kb memory capacity, 1 word is 4 bytes
#define NUM_REG 17
#define MEM_SIZE 65536
#define NUM_GENERAL_REG 13
#define REG_PC 15
#define REG_CPSR 16

#define FULL_BYTE 255

struct MachineState {
    REGISTER registers[NUM_REG];
    BYTE memory[MEM_SIZE];
    bool hasInstrToExecute;
    bool hasInstrToDecode;
};

enum CondFlag {V=1, C=2, Z=4, N=8};
enum CondCode {EQ=0, NE=1, GE=10, LT=11, GT=12, LE=13, AL=14};
enum OpCode {AND=0, EOR=1, SUB=2, RSB=3, ADD=4, TST=8, TEQ=9, CMP=10, ORR=12, MOV=13};
enum ShiftType {LSL=0, LSR=1, ASR=2, ROR=3};

enum InstrType {instrDataProcessing, instrMultiply, instrSDT, instrBranch, instrUnknown};
enum DataProcType {dataProcOp2RegShiftConst, dataProcOp2RegShiftReg, dataProcOp2Imm};
enum SdtType {sdtOffsetRegShiftConst, sdtOffsetRegShiftReg, sdtOffsetImm};

// function declarations
void incrementPC(struct MachineState *state);
void printResults(struct MachineState *state);
WORD readFourBytes(ADDRESS, struct MachineState *state);
bool checkCondition(enum CondCode, struct MachineState *state);
bool isSet(enum CondFlag, struct MachineState *state);
void setFlag(enum CondFlag, struct MachineState *state);
void clearFlag(enum CondFlag, struct MachineState *state);

void executeInstruction(WORD, struct MachineState *state);
enum InstrType getInstrType(WORD instr);
enum DataProcType getDataProcType(WORD instr);
enum SdtType getSdtType(WORD instr);
WORD getOperandFromRegisterShift(WORD operandBits, bool regShift, struct MachineState *state);
SDTOFFSET getSDTOffset(enum SdtType sdtType, WORD offsetBits, struct MachineState *state) ;


void performBranch(OFFSET offset, struct MachineState *state);
void performSdt(enum SdtType sdtType, bool pFlag, bool upFlag, bool ldstFlag, REGNUMBER rn, REGNUMBER rd, WORD offsetBits, struct MachineState *state);
void performMultiply(bool aFlag, bool sFlag, REGNUMBER rd, REGNUMBER rn, REGNUMBER rs, REGNUMBER rm, struct MachineState *state);
void performDataProc(enum DataProcType dataProcType, enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, OFFSET Operand2, struct MachineState *state);
int getImmValue(OFFSET Operand2);
int getRegValue(bool constShift, OFFSET Operand2);

// extract the chosen bits (using number scheme from spec, big-endian), and return right-aligned bits
WORD getBitsFromWord(WORD word, BYTE startBitNo, BYTE numBits);
WORD readWord(ADDRESS startAddress, struct MachineState *state);
void writeWord(WORD word, ADDRESS startAddress, struct MachineState *state);
WORD shiftRegister(OFFSET offset, enum SdtType sdtType, struct MachineState *state);

// SHIFT
WORD shift(WORD val, BYTE shiftAmount, bool updateCPSR, enum ShiftType shiftType, struct MachineState *state);

// TODO: declare data processing functions for each opCode