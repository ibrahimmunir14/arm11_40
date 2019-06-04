#include "emulateGetting.h"

/* helper functions for execution of instructions */
enum InstrType getInstrType(WORD instr) {
  switch (GETBITS(instr, 27, 2)) {
    case 2: { // 10 in bits 27-26; branch
      return instrBranch;
    }
    case 1: { // 01 in bits 27-26; single data transfer
      return instrSDT;
    }
    case 0: { // 00 in bits 27-26
      bool iFlag = GETBIT(instr, 25);
      bool seventhBit = GETBIT(instr, 7);
      bool fourthBit = GETBIT(instr, 4);

      if (!iFlag && seventhBit && fourthBit) {
        return instrMultiply;
      }

      return instrDataProcessing;
    }
    default: {// unknown instruction type
      return instrUnknown;
    }
  }
}

enum DataProcType getDataProcType(WORD instr) {
  bool iFlag = GETBIT(instr, 25);
  // immediate value
  if (iFlag) {
    return dataProcOp2Imm;
  }

  // shifted register
  bool fourthBit = GETBIT(instr, 4);
  if (fourthBit) {
    return dataProcOp2RegShiftReg;
  }

  return dataProcOp2RegShiftConst;
}

enum SdtType getSdtType(WORD instr) {
  bool iFlag = GETBIT(instr, 25);
  // immediate value offset
  if (!iFlag) {
    return sdtOffsetImm;
  }

  // shifted register offset
  bool fourthBit = GETBIT(instr, 4);
  if (fourthBit) {
    return sdtOffsetRegShiftReg;
  }

  return sdtOffsetRegShiftConst;
}

enum OpCode getOpCode(WORD instr) {
  return (enum OpCode) GETNIBBLE(instr, 24);
}

SDTOFFSET getSDTOffset(enum SdtType sdtType, WORD offsetBits, struct MachineState *state) {
  return (sdtType == sdtOffsetImm)
         ? offsetBits // offset is immediate value
         : getOperandFromRegisterShift(offsetBits, (sdtType == sdtOffsetRegShiftReg), false, state); // shifted reg
}

DPOPERAND2 getDPOperand2(enum DataProcType dataProcType, WORD operand2Bits, bool modifyCPSR, struct MachineState *state) {
  return (dataProcType == dataProcOp2Imm)
         ? getOperandFromImmRotation(operand2Bits, modifyCPSR, state) // operand2 is immediate value
         : getOperandFromRegisterShift(operand2Bits, (dataProcType == dataProcOp2RegShiftReg), modifyCPSR, state); // shifted reg
}

DPOPERAND2 getOperandFromImmRotation(WORD operandBits, bool modifyCPSR, struct MachineState *state) {
  WORD immValue = GETBITS(operandBits, 7, 8);
  BYTE rotAmount = 2 * GETNIBBLE(operandBits, 11);
  return shiftAndUpdateStatus(immValue, rotAmount, modifyCPSR, ROR, state);
}

WORD getOperandFromRegisterShift(WORD operandBits, bool regShift, bool modifyCPSR, struct MachineState *state) {
  REGNUMBER rm = GETNIBBLE((WORD) operandBits, 3);
  REGISTER rmContents = state->registers[rm];
  enum ShiftType shiftType = GETBITS(operandBits, 6, 2);

  // calculate shiftAndUpdateStatus, specified by a 5-bit unsigned int
  BYTE shiftAmount;
  if (regShift) { // shiftAndUpdateStatus specified by register, bottom byte
    REGNUMBER rs = GETNIBBLE(operandBits, 11);
    REGISTER rsContents = state->registers[rs];
    shiftAmount = rsContents & FULLBITS(8);
  } else { // shiftAndUpdateStatus by a constant amount
    shiftAmount = GETBITS(operandBits, 11, 5);
  }

  return shiftAndUpdateStatus(rmContents, shiftAmount, modifyCPSR, shiftType, state);
}