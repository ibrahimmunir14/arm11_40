#include "instructionExecution.h"

void executeInstruction(WORD instr, struct MachineState *state) {
    // check if instruction should be executed
    bool doExecute = checkCondition((enum CondCode) getBitsFromWord(instr, 31, 4), state);
    if (doExecute) {
        // calculate parameters and delegate to relevant function
        switch (getInstrType(instr)) {
            case instrBranch: {
                WORD offsetBits = getBitsFromWord(instr, 23, 24);

                performBranch(offsetBits, state);
                break;


            } case instrSDT: {
                enum SdtType sdtType = getSdtType(instr);
                bool pFlag = getBitsFromWord(instr, 24, 1);
                bool uFlag = getBitsFromWord(instr, 23, 1);
                bool lFlag = getBitsFromWord(instr, 20, 1);
                REGNUMBER rn = getBitsFromWord(instr, 19, 4);
                REGNUMBER rd = getBitsFromWord(instr, 15, 4);
                WORD offsetBits = getBitsFromWord(instr, 11, 12);

                performSdt(sdtType, pFlag, uFlag, lFlag, rn, rd, offsetBits, state);
                break;


            } case instrMultiply: {
                bool aFlag = getBitsFromWord(instr, 21, 1);
                bool sFlag = getBitsFromWord(instr, 20, 1);
                REGNUMBER rd = getBitsFromWord(instr, 19, 4);
                REGNUMBER rn = getBitsFromWord(instr, 15, 4);
                REGNUMBER rs = getBitsFromWord(instr, 11, 4);
                REGNUMBER rm = getBitsFromWord(instr, 3, 4);

                performMultiply(aFlag, sFlag, rd, rn, rs, rm, state);
                break;


            } case instrDataProcessing: {
                enum DataProcType dataProcType = getDataProcType(instr);
                enum OpCode opCode = getOpCode(instr);
                bool sFlag = getBitsFromWord(instr, 20, 1);
                REGNUMBER rn = getBitsFromWord(instr, 19, 4);
                REGNUMBER rd = getBitsFromWord(instr, 15, 4);
                WORD operand2Bits = getBitsFromWord(instr, 11, 12);

                performDataProc(dataProcType, opCode, sFlag, rn, rd, operand2Bits, state);
                break;


            } default:
                printf("Error: Unknown Instruction: 0x%08x\n", instr);
        }
    }
}

void performBranch(WORD offsetBits, struct MachineState *state) {
    // falsify hasInstr booleans
    state->hasInstrToExecute = false;
    state->hasInstrToDecode = false;
    // set PC to wherever the next instruction is
    BRANCHOFFSET branchOffset = signExtend(offsetBits << 2u, 26);
    state->registers[REG_PC] += branchOffset;
}

void performSdt(enum SdtType sdtType, bool pFlag, bool upFlag, bool ldstFlag, REGNUMBER rn, REGNUMBER rd, WORD offsetBits, struct MachineState *state) {
    // read address reference from source/dest register and calculate offset value
    REGISTER memAddress = state->registers[rn];
    SDTOFFSET offsetValue = getSDTOffset(sdtType, offsetBits, state);
    if (!upFlag) {
        offsetValue = -offsetValue;
    }

    if (pFlag) { // transfer data using address after offset
        // ensure address is in bounds
        if (memAddress + offsetValue > fullBits(16)) {
            printf("Error: Out of bounds memory access at address 0x%08x\n", memAddress + offsetValue);
            return;
        }
        if (ldstFlag) { // load word from memory
            state->registers[rd] = readWord((ADDRESS) memAddress + offsetValue, state);
        } else { // store word in memory
            writeWord((WORD) state->registers[rd], (ADDRESS) memAddress + offsetValue, state);
        }
    } else { // transfer data then update base register
        if (memAddress > fullBits(16)) {
            printf("Error: Out of bounds memory access at address 0x%08x\n", memAddress);
            return;
        }
        if (ldstFlag) { // load word from memory
            state->registers[rd] = readWord((ADDRESS) memAddress, state);
        } else { // store word in memory
            writeWord((WORD) state->registers[rd], (ADDRESS) memAddress, state);
        }
        // apply offset to source/des register contents
        state->registers[rn] += offsetValue;
    }
}
void performMultiply(bool aFlag, bool sFlag, REGNUMBER rd, REGNUMBER rn, REGNUMBER rs, REGNUMBER rm, struct MachineState *state) {
    // perform calculate, take result as last 32 bits, put result in dest register
    // aFlag bit determines whether to multiply or multiply and accumulate
    WORD result;
    if (aFlag) {
        result = state->registers[rm] * state->registers[rs] + state->registers[rn];
    } else {
        result = state-> registers[rm] * state-> registers[rs];
    }
    state->registers[rd] = result;
    // sFlag indicates whether to update CPSR status bits
    if (sFlag) {
        // set the Z flag iff result is 0
        if (result == 0) {
            setFlag(Z, state);
        } else {
            clearFlag(Z, state);
        }

        // set the N flag to bit 31 of result
        if (getBitsFromWord(result, 31, 1)) {
            setFlag(N, state);
        } else {
            clearFlag(N, state);
        }
    }
}
void performDataProc(enum DataProcType dataProcType, enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, WORD operand2Bits, struct MachineState *state) {
    DPOPERAND2 operand2 = getDPOperand2(dataProcType, operand2Bits, sFlag, state);

    bool aluCarry = false;

    // perform calculation based on op-code
    uint64_t result;
    switch (opCode) {
        case AND:
        case TST:
            result = state->registers[rn] & operand2;
            break;
        case EOR:
        case TEQ:
            result = state->registers[rn] ^ operand2;
            break;
        case SUB:
        case CMP:
            result = state->registers[rn] - operand2;
            aluCarry = result <= state->registers[rn];
            break;
        case RSB:
            result = operand2 - state->registers[rn];
            aluCarry = result < operand2;
            break;
        case ADD:
            result = state->registers[rn] + operand2;
            aluCarry = result >> 32u > 0;
            break;
        case ORR:
            result = state->registers[rn] | operand2;
            break;
        case MOV:
            result = operand2;
            break;
        default:
            printf("Error: Unknown Operation Code: %i\n", opCode);
            return;
    }

    // write result to register if not TST, TEQ, CMP
    switch (opCode) {
        case TST:
        case TEQ:
        case CMP:
            break;
        default:
            state->registers[rd] = (WORD) result;
    }

    // set condition codes
    if (sFlag) {
        if ((WORD) result == 0) {
            setFlag(Z, state);
        } else {
            clearFlag(Z, state);
        }

        if (getBitsFromWord((WORD) result, 31, 1)) {
            setFlag(N, state);
        } else {
            clearFlag(N, state);
        }

        if (aluCarry) {
            setFlag(C, state);
        } else {
            clearFlag(C, state);
        }
    }

}