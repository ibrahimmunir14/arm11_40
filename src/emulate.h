//
// Created by ljt18 on 24/05/19.
//

#ifndef ARM11_40_EMULATE_H
#define ARM11_40_EMULATE_H

#endif //ARM11_40_EMULATE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

// define types to aid readability
typedef uint32_t REGISTER;	// registers are 32 bits
typedef uint32_t WORD;		// words are 32 bits
typedef uint16_t ADDRESS;   // addresses are 16 bits
typedef uint8_t BYTE;		// bytes are 8 bits
typedef int32_t OFFSET;     // offsets are 32 bits, signed

// define constants related to registers
// store registers in an array of register (R13=SP; R14=LR; R15=PC; R16=CPSR)
// store memory as array of byte, 64kb memory capacity, 1 word is 4 bytes
#define NUM_REG 17
#define MEM_SIZE 65536
#define NUM_GENERAL_REG 13
#define REG_PC 15
#define REG_CPSR 16

struct MachineState {
    REGISTER registers[NUM_REG];
    BYTE memory[MEM_SIZE];
};

enum CondFlag {V=1, C=2, Z=4, N=8};
enum CondCode {EQ=0, NE=1, GE=10, LT=11, GT=12, LE=13, AL=14};
enum OpCode {AND=0, EOR=1, SUB=2, RSB=3, ADD=4, TST=8, TEQ=9, CMP=10, ORR=12, MOV=13};
enum ShiftType {LSL=0, LSR=1, ASR=2, ROR=3};
enum instrType {instrDataProcessing, instrMultiply, instrSDT, instrBranch, instrUnknown};
enum dataProcType {dataProcOp2Imm, dataProcOp2RegShiftReg, dataProcOp2RegShiftConst};
enum sdtType {sdtOffsetImm, sdtOffsetRegShiftReg, sdtOffsetRegShiftConst};

// function declarations
void incrementPC(struct MachineState *state);
void printResults(struct MachineState *state);
WORD readWord(ADDRESS, struct MachineState *state);
bool checkCondition(enum CondCode, struct MachineState *state);
bool isSet(enum CondFlag, struct MachineState *state);
void setFlag(enum CondFlag, struct MachineState *state);
void clearFlag(enum CondFlag, struct MachineState *state);

void executeInstruction(WORD, struct MachineState *state);
enum instrType getInstrType(WORD instr);
enum dataProcType getDataProcType(WORD instr);
enum sdtType getSdtType(WORD instr);

void performBranch(OFFSET offset, struct MachineState *state);
void performSdtOffsetImm(bool pFlag, bool upFlag, bool ldstFlag, BYTE rn, BYTE rd, OFFSET offset, struct MachineState *state);
void performSdtOffsetRegShiftReg(bool pFlag, bool upFlag, bool ldstFlag, BYTE rn, BYTE rd, BYTE rs, enum ShiftType shiftType, struct MachineState *state);
void performSdtOffsetRegShiftConst(bool pFlag, bool upFlag, bool ldstFlag, BYTE rn, BYTE rd, int shiftAmount, enum ShiftType shiftType, struct MachineState *state);
void performMultiply(bool aFlag, bool sFlag, BYTE rd, BYTE rn, BYTE rs, BYTE rm, struct MachineState *state);
void performDataProcOp2Imm(enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, BYTE rotate, BYTE immVal, struct MachineState *state);
void performDataProcOp2RegShiftConst(enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, int shiftAmount, enum ShiftType shiftType, BYTE rm, struct MachineState *state);
void performDataProcOp2RegShiftReg(enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, BYTE rs, enum ShiftType shiftType, BYTE rm, struct MachineState *state);

// extract the chosen bits (using number scheme from spec, big-endian), and return right-aligned bits
WORD getBitsFromWord(WORD word, BYTE startBitNo, BYTE numBits);