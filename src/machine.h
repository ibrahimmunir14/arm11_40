#ifndef ARM11_40_MACHINE_H
#define ARM11_40_MACHINE_H

#include "binaryTypes.h"
#include <stdbool.h>

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
    bool hasInstrToExecute;
    bool hasInstrToDecode;
};

#endif //ARM11_40_MACHINE_H
