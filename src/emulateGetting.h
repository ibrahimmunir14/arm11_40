#ifndef ARM11_40_SRC_EMULATEGETTING_H_
#define ARM11_40_SRC_EMULATEGETTING_H_

#include <stdbool.h>
#include "binaryOps.h"
#include "emulateStatus.h"

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
SDTOFFSET getSDTOffset(enum SdtType sdtType, WORD offsetBits, struct MachineState *state);
// get operand2 from data processing instruction
DPOPERAND2 getDPOperand2(enum DataProcType dataProcType, WORD operand2Bits, bool modifyCPSR, struct MachineState *state);
// get operand from rotated immediate value data processing instruction
DPOPERAND2 getOperandFromImmRotation(WORD operandBits, bool modifyCPSR, struct MachineState *state);
// get operand from shifted register data processing instruction
WORD getOperandFromRegisterShift(WORD operandBits, bool regShift, bool modifyCPSR, struct MachineState *state);

#endif //ARM11_40_SRC_EMULATEGETTING_H_
