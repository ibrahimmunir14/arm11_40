#ifndef ARM11_40_INSTRUCTIONDECODING_H
#define ARM11_40_INSTRUCTIONDECODING_H

#include "machine.h"
#include "binaryOps.h"
#include "statusFlags.h"

/* helper functions for execution of instructions */
enum OpCode {AND=0, EOR=1, SUB=2, RSB=3, ADD=4, TST=8, TEQ=9, CMP=10, ORR=12, MOV=13};
enum InstrType {instrUnknown, instrDataProcessing, instrMultiply, instrSDT, instrBranch};
enum SdtType {sdtOffsetRegShiftConst, sdtOffsetRegShiftReg, sdtOffsetImm};
enum DataProcType {dataProcOp2RegShiftConst, dataProcOp2RegShiftReg, dataProcOp2Imm};

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
// apply a shift on val specified by shiftAndUpdate amount, optionally update status bits
WORD shiftAndUpdate(WORD val, BYTE shiftAmount, bool updateCPSR, enum ShiftType shiftType, struct MachineState *state);

#endif //ARM11_40_INSTRUCTIONDECODING_H
