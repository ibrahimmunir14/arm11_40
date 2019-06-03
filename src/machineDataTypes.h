#ifndef ARM11_40_SRC_MACHINEDATATYPES_H_
#define ARM11_40_SRC_MACHINEDATATYPES_H_

#include <stdint.h>

// NOTE: Words in memory are stored Big-Endian; Words in registers are stored Big-Endian
//       Instructions in this code are Little-Endian
//       use readWord/storeWord to read/write from memory, auto taking care of conversions

typedef uint32_t REGISTER;	// register contents are 32 bits
typedef uint32_t WORD;		// words/instructions are 32 bits
typedef uint16_t ADDRESS;   // memory addresses are 16 bits, unsigned because addresses positive
typedef uint8_t BYTE;		// bytes are 8 bits

// define types to aid readability
typedef uint8_t REGNUMBER;  // register numbers are 4 bits, unsigned because register numbers positive
typedef int32_t SDTOFFSET;     // SDT offset: imm offset 12 bits; reg offset is 32 bits
typedef uint32_t DPOPERAND2;     // imm operand2 8 bits; reg operand2 is 32 bits
typedef int32_t BRANCHOFFSET; // branch offset is signed 24-bit offset

// define constants related to registers
#define BYTE_BITS 8u
#define NIBBLE_BITS 4u
#define WORD_BITS 32u
#define CPSR_POS 28u
#define NUM_REG 17u
#define MEM_SIZE 65536u
#define NUM_GENERAL_REG 13u
#define REG_PC 15u
#define REG_CPSR 16u

// enums for common values/types
enum StatusFlag {V=1, C=2, Z=4, N=8};
enum CondCode {EQ=0, NE=1, GE=10, LT=11, GT=12, LE=13, AL=14};
enum OpCode {AND=0, EOR=1, SUB=2, RSB=3, ADD=4, TST=8, TEQ=9, CMP=10, ORR=12, MOV=13};
enum InstrType {instrUnknown, instrDataProcessing, instrMultiply, instrSDT, instrBranch};
enum DataProcType {dataProcOp2RegShiftConst, dataProcOp2RegShiftReg, dataProcOp2Imm};
enum SdtType {sdtOffsetRegShiftConst, sdtOffsetRegShiftReg, sdtOffsetImm};

#endif //ARM11_40_SRC_MACHINEDATATYPES_H_