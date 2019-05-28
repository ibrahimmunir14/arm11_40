#include "instructionDecoding.h"

enum InstrType getInstrType(WORD instr) {
    switch (getBitsFromWord(instr, 27, 2)) {
        case 2: // 10 in bits 27-26; branch
            return instrBranch;
        case 1: // 01 in bits 27-26; single data transfer
            return instrSDT;
        case 0: { // 00 in bits 27-26
            bool iFlag = (bool) getBitsFromWord(instr, 25, 1);
            bool seventhBit = (bool) getBitsFromWord(instr, 7, 1);
            bool fourthBit = (bool) getBitsFromWord(instr, 4, 1);
            return (!iFlag && seventhBit && fourthBit)
                   ? instrMultiply : instrDataProcessing;
        }
        default: // unknown instruction type
            return instrUnknown;
    }
}

enum DataProcType getDataProcType(WORD instr) {
    bool iFlag = (bool) getBitsFromWord(instr, 25, 1);

    if (iFlag) { // immediate
        return dataProcOp2Imm;
    }

    bool fourthBit = (bool) getBitsFromWord(instr, 4, 1);

    // shifted register
    return fourthBit ? dataProcOp2RegShiftReg : dataProcOp2RegShiftConst;
}
enum SdtType getSdtType(WORD instr) {
    bool iFlag = (bool) getBitsFromWord(instr, 25, 1);

    if (!iFlag) { // immediate
        return sdtOffsetImm;
    }

    bool fourthBit = (bool) getBitsFromWord(instr, 4, 1);

    // shifted register
    return fourthBit ? sdtOffsetRegShiftReg : sdtOffsetRegShiftConst;
}

enum OpCode getOpCode(WORD instr) {
    return (enum OpCode) getBitsFromWord(instr, 24, 4);
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
    WORD immValue = getBitsFromWord(operandBits, 7, 8);
    BYTE rotAmount = 2 * getBitsFromWord(operandBits, 11, 4);
    return shiftAndUpdate(immValue, rotAmount, modifyCPSR, ROR, state);
}
WORD getOperandFromRegisterShift(WORD operandBits, bool regShift, bool modifyCPSR, struct MachineState *state) {
    REGNUMBER rm = getBitsFromWord((WORD) operandBits, 3, 4);
    REGISTER rmContents = state->registers[rm];
    enum ShiftType shiftType = getBitsFromWord(operandBits, 6, 2);

    // calculate shiftAndUpdate, specified by a 5-bit unsigned int
    BYTE shiftAmount;
    if (regShift) { // shiftAndUpdate specified by register, bottom byte
        REGNUMBER rs = getBitsFromWord(operandBits, 11, 4);
        REGISTER rsContents = state->registers[rs];
        shiftAmount = rsContents & fullBits(8);
    } else { // shiftAndUpdate by a constant amount
        shiftAmount = getBitsFromWord(operandBits, 11, 5);
    }

    return shiftAndUpdate(rmContents, shiftAmount, modifyCPSR, shiftType, state);
}

WORD shiftAndUpdate(WORD val, BYTE shiftAmount, bool updateCPSR, enum ShiftType shiftType, struct MachineState *state) {
    bool *carryOutBit = 0;
    WORD shifted = shift(val, shiftAmount, shiftType, carryOutBit);

    // update status register with carry out bit
    if (updateCPSR) {
        *carryOutBit ? setFlag(C, state) : clearFlag(C, state);
    }
    return shifted;
}