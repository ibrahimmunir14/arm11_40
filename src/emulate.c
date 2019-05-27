#include "emulate.h"

// NOTE: Words in memory are stored Big-Endian; Words in registers are stored Big-Endian
//       Instructions in this code are Little-Endian
//       use readWord/storeWord to read/write from Memory, auto taking care of conversions

// TODO: Clean-up so all code has consistent style
// TODO: Organise everything into headers and c files
// TODO: Fix segmentation faults

bool debug = false;

int main(int argc, char **argv) {
    // ensure we have one argument, the filename
    if (argc != 2) { return EXIT_FAILURE; }

    // initialize state, registers, memory to 0s
    struct MachineState state;
    for (int i = 0; i < NUM_REG; i++) { state.registers[i] = 0; }
    for (int i = 0; i < MEM_SIZE; i++) { state.memory[i] = 0; }

    // set up memory array with instructions
    char *fileName = argv[1];
    FILE *fPointer = fopen(fileName, "rb");
    fseek(fPointer, 0, SEEK_END);
    int size = (int) ftell(fPointer);
    fseek(fPointer, 0, SEEK_SET);
    for (int i = 0; i < size; i++) { state.memory[i] = (BYTE) getc(fPointer); }

    // output memory contents after loading instructions (for testing)
    if (debug) {
        printf("Initial Memory Contents:\n");
        for (int i = 0; i < MEM_SIZE; i += 4) {
            WORD word = readFourBytes((ADDRESS) i, &state);
            if (word != 0) {
                printf("0x%08x: 0x%08x\n", i, word);
            }
        }
        printf("\n");
    }

    // main pipeline loop
    WORD instrToExecute = 0;
    WORD instrToDecode = 0;
    state.hasInstrToDecode = false;
    state.hasInstrToExecute = false;
    while (state.registers[REG_PC] < MEM_SIZE) {
        // execute instrToExecute
        if (state.hasInstrToExecute) {
            if (instrToExecute == 0) {
                if (debug) printf("TERMINATE: All-0 Instruction\n\n");
                break; // terminate on all-0 instruction
            } else {
                executeInstruction(instrToExecute, &state);
            }
        }

        // decode instrToDecode and put it in instrToExecute
        // note: decoding is actually done during execution
        if (state.hasInstrToDecode) {
            state.hasInstrToExecute = true;
            instrToExecute = instrToDecode;
            if (debug) printf("Decoded Instruction 0x%08x\n", instrToExecute);
        }

        // fetch next instruction and put it in instrToDecode
        instrToDecode = readWord((ADDRESS) state.registers[REG_PC], &state);
        if (debug) printf("Fetched Instruction at 0x%08x : 0x%08x\n", state.registers[REG_PC], instrToDecode);
        state.hasInstrToDecode = true;

        // increment program counter
        incrementPC(&state);
        if (debug) printf("Incremented PC to 0x%08x\n\n", state.registers[REG_PC]);
    }

    printResults(&state);
    return EXIT_SUCCESS;
}

/* useful bit-related functions */
WORD getBitsFromWord(WORD word, BYTE startBitNo, BYTE numBits) {
    WORD andOp = fullBits(numBits);
    WORD wordShifted = word >> (BYTE) (1 + startBitNo - numBits);
    return andOp & wordShifted;
}
WORD shift(WORD val, BYTE shiftAmount, bool updateCPSR, enum ShiftType shiftType, struct MachineState *state) {
    bool carryOutBit = 0;
    WORD result;
    switch (shiftType) {
        case LSL : {
            carryOutBit = getBitsFromWord(val, 32-shiftAmount, 1); // least sig discarded bit
            result = val << shiftAmount;
            break;
        }
        case LSR : {
            carryOutBit = getBitsFromWord(val, shiftAmount - 1, 1); // most sig discarded bit
            result = val >> shiftAmount;
            break;
        }
        case ASR : {
            carryOutBit = getBitsFromWord(val, shiftAmount - 1, 1); // most sig discarded bit
            result = signExtend(val >> shiftAmount, 32u - shiftAmount);
            break;
        }
        case ROR : {
            carryOutBit = getBitsFromWord(val, shiftAmount - 1, 1); // most sig discarded bit
            result = val >> shiftAmount | val << (32u - shiftAmount);
            break;
        }
        default:
            return 0;
    }
    if (updateCPSR) {
        if (carryOutBit) setFlag(C, state);
        else clearFlag(C, state);
    }
    return result;
}
WORD signExtend(WORD val, BYTE originalLength) {
    BYTE numEmptyBits = 32u - originalLength;
    bool topBitOne = getBitsFromWord(val, originalLength - 1, 1) == 1;
    return topBitOne
           ? val | (fullBits(numEmptyBits) << originalLength)
           : val;
}

/* functions related to memory access */
WORD readFourBytes(ADDRESS startAddress, struct MachineState *state) {
    return (WORD) state->memory[startAddress] << 24u  | (WORD) state->memory[startAddress+1] << 16u
           | (WORD) state->memory[startAddress+2] << 8u | (WORD) state->memory[startAddress+3];
}
WORD readWord(ADDRESS startAddress, struct MachineState *state) {
    return (WORD) state->memory[startAddress+3] << 24u | (WORD) state->memory[startAddress+2] << 16u
           | (WORD) state->memory[startAddress+1] << 8u | (WORD) state->memory[startAddress];
}
void writeWord(WORD word, ADDRESS startAddress, struct MachineState *state) {
    state->memory[startAddress] = (BYTE) (word & fullBits(8));
    state->memory[startAddress+1] = (BYTE) ((word >> 8u) & fullBits(8));
    state->memory[startAddress+2] = (BYTE) ((word >> 16u) & fullBits(8));
    state->memory[startAddress+3] = (BYTE) ((word >> 24u) & fullBits(8));
}

/* helper functions for main program */
void incrementPC(struct MachineState *state) {
    state->registers[REG_PC] += 4;
}
void printResults(struct MachineState *state) {
    printf("Registers:\n");
    // print contents of general registers R0-R12
    for (int i = 0; i < NUM_GENERAL_REG; i++) {
        printf("$%-2i : %10i (0x%08x)\n", i, state->registers[i], state->registers[i]);
    }
    // print contents of pc and cpsr
    printf("PC  : %10i (0x%08x)\n", state->registers[REG_PC], state->registers[REG_PC]);
    printf("CPSR: %10i (0x%08x)\n", state->registers[REG_CPSR], state->registers[REG_CPSR]);

    // print contents of non-zero memory locations
    printf("Non-zero memory:\n");
    for (int i = 0; i < MEM_SIZE; i += 4) {
        WORD word = readFourBytes((ADDRESS) i, state);
        if (word != 0) {
            printf("0x%08x: 0x%08x\n", i, word);
        }
    }
}
bool checkCondition(enum CondCode condCode, struct MachineState *state) {
    switch (condCode) {
        case AL : return true;
        case EQ : return isSet(Z, state);
        case NE : return !isSet(Z, state);
        case GE : return isSet(N, state) == isSet(V, state);
        case LT : return isSet(N, state) != isSet(V, state);
        case GT : return !isSet(Z, state) && (isSet(N, state) == isSet(V, state));
        case LE : return isSet(Z, state) || (isSet(N, state) != isSet(V, state));
        default : return false;
    }
}

/* helper functions related to CPSR status bits */
bool isSet(enum StatusFlag flag, struct MachineState *state) {
    return (getBitsFromWord(state->registers[REG_CPSR], 31, 4) & (BYTE) flag) == flag;
}
void setFlag(enum StatusFlag flag, struct MachineState *state) {
    state->registers[REG_CPSR] = state->registers[REG_CPSR] | (WORD) ((BYTE) flag << 28u);
}
void clearFlag(enum StatusFlag flag, struct MachineState *state) {
    state->registers[REG_CPSR] = state->registers[REG_CPSR] & (~ (WORD) ((BYTE) flag << 28u));
}

/* functions for execution of instructions */
void executeInstruction(WORD instr, struct MachineState *state) {
    // check if instruction should be executed
    bool doExecute = checkCondition((enum CondCode) getBitsFromWord(instr, 31, 4), state);
    if (doExecute) {
        // calculate parameters and delegate to relevant function
        switch (getInstrType(instr)) {
            case instrBranch: {
                if (debug) printf("Branch Operation: (0x%08x)\n", instr);
                WORD offsetBits = getBitsFromWord(instr, 23, 24);
                performBranch(offsetBits, state);
                break;
            }
            case instrSDT: {
                if (debug) printf("SDT Operation: (0x%08x)\n", instr);
                enum SdtType sdtType = getSdtType(instr);
                bool pFlag = getBitsFromWord(instr, 24, 1);
                bool uFlag = getBitsFromWord(instr, 23, 1);
                bool lFlag = getBitsFromWord(instr, 20, 1);
                REGNUMBER rn = getBitsFromWord(instr, 19, 4);
                REGNUMBER rd = getBitsFromWord(instr, 15, 4);
                WORD offsetBits = getBitsFromWord(instr, 11, 12);
                performSdt(sdtType, pFlag, uFlag, lFlag, rn, rd, offsetBits, state);
                break;
            }
            case instrMultiply: {
                if (debug) printf("Multiply Operation: (0x%08x)\n", instr);
                bool aFlag = getBitsFromWord(instr, 21, 1);
                bool sFlag = getBitsFromWord(instr, 20, 1);
                REGNUMBER rd = getBitsFromWord(instr, 19, 4);
                REGNUMBER rn = getBitsFromWord(instr, 15, 4);
                REGNUMBER rs = getBitsFromWord(instr, 11, 4);
                REGNUMBER rm = getBitsFromWord(instr, 3, 4);
                performMultiply(aFlag, sFlag, rd, rn, rs, rm, state);
                break;
            }
            case instrDataProcessing: {
                if (debug) printf("DataProcessing Operation: (0x%08x)\n", instr);
                enum DataProcType dataProcType = getDataProcType(instr);
                enum OpCode opCode = getOpCode(instr);
                bool sFlag = getBitsFromWord(instr, 20, 1);
                REGNUMBER rn = getBitsFromWord(instr, 19, 4);
                REGNUMBER rd = getBitsFromWord(instr, 15, 4);
                WORD operand2Bits = getBitsFromWord(instr, 11, 12);
                performDataProc(dataProcType, opCode, sFlag, rn, rd, operand2Bits, state);
                break;
            }
            default:
                printf("Error: Unknown Instruction: 0x%08x\n", instr);
        }
    }
}
void performBranch(WORD offsetBits, struct MachineState *state) {
    // falsify hasInstr booleans
    state->hasInstrToExecute = false;
    state->hasInstrToDecode = false;
    // set PC to wherever the next instruction is
    // TODO: ensure test if this works with negative offsets
    BRANCHOFFSET branchOffset = signExtend(offsetBits << 2u, 26);
    if (debug) printf("  Offset Value: %i\n", branchOffset);
    if (debug) printf("  REG_PC incremented from 0x%08x to ", state->registers[REG_PC]);
    state->registers[REG_PC] += branchOffset;
    if (debug) printf("0x%08x\n", state->registers[REG_PC]);
}
void performSdt(enum SdtType sdtType, bool pFlag, bool upFlag, bool ldstFlag, REGNUMBER rn, REGNUMBER rd, WORD offsetBits, struct MachineState *state) {
    if (debug) printf("  Rn $%i; Rd $%i; Offset (0x%03x); \n", rn, rd, offsetBits);

    // read address reference from source/dest register and calculate offset value
    REGISTER memAddress = state->registers[rn];
    SDTOFFSET offsetValue = getSDTOffset(sdtType, offsetBits, state);
    if (!upFlag) offsetValue = -offsetValue;

    if (pFlag) { // transfer data using address after offset
        // ensure address is in bounds
        if (memAddress + offsetValue > fullBits(16)) {
            printf("Error: Out of bounds memory access at address 0x%08x\n", memAddress + offsetValue);
            return;
        }
        if (ldstFlag) { // load word from memory
            if (debug) printf("  Load (pre-index) from memory[0x%04x + 0x%04x = 0x%04x] to register[$%i]\n",
                              memAddress, offsetValue, (ADDRESS) memAddress + offsetValue, rd);
            state->registers[rd] = readWord((ADDRESS) memAddress + offsetValue, state);
        } else { // store word in memory
            if (debug) printf("  Store (pre-index) from register[$%i] to memory[0x%04x + 0x%04x = 0x%04x]\n",
                              rd, memAddress, offsetValue, memAddress + offsetValue);
            writeWord((WORD) state->registers[rd], (ADDRESS) memAddress + offsetValue, state);
        }
    } else { // transfer data then update base register
        if (memAddress > fullBits(16)) {
            printf("Error: Out of bounds memory access at address 0x%08x\n", memAddress);
            return;
        }
        if (ldstFlag) { // load word from memory
            if (debug) printf("  Load (post-index) from memory[0x%04x] to register[$%i]\n", memAddress, rd);
            state->registers[rd] = readWord((ADDRESS) memAddress, state);
        } else { // store word in memory
            if (debug) printf("  Store (post-index) from register[$%i] to memory[0x%04x]\n", rd, memAddress);
            writeWord((WORD) state->registers[rd], (ADDRESS) memAddress, state);
        }
        // apply offset to source/des register contents
        if (debug) printf("  Incremented register[$%i] by offsetValue (0x%04x)", rn, offsetValue);
        state->registers[rn] += offsetValue;
    }
}
void performMultiply(bool aFlag, bool sFlag, REGNUMBER rd, REGNUMBER rn, REGNUMBER rs, REGNUMBER rm, struct MachineState *state) {
    if (debug) printf("  Rn $%i; Rd $%i; Rs $%i; Rm $%i\n", rn, rd, rs, rm);
    if (debug) printf("  accumulate: %i;  statusupdate: %i\n", aFlag, sFlag);
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
        if (result == 0) setFlag(Z, state);
        else clearFlag(Z, state);
        // set the N flag to bit 31 of result
        if (getBitsFromWord(result, 31, 1)) setFlag(N, state);
        else clearFlag(N, state);
    }
}
void performDataProc(enum DataProcType dataProcType, enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, WORD operand2Bits, struct MachineState *state) {
    if (debug) printf("OpCode: %i  Rn $%i; Rd $%i; operand2Bits (0x%03x)\n", opCode, rn, rd, operand2Bits);
    DPOPERAND2 operand2 = getDPOperand2(dataProcType, operand2Bits, sFlag, state);
    if (debug) printf("  operand2: 0x%08x\n", operand2);

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

    if (sFlag) {
        if ((WORD) result == 0) setFlag(Z, state);
        else clearFlag(Z, state);

        if (getBitsFromWord((WORD) result, 31, 1)) setFlag(N, state);
        else clearFlag(N, state);

        if (aluCarry) setFlag(C, state);
        else clearFlag(C, state);
    }

}

/* helper functions for execution of instructions */
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
            if (!iFlag && seventhBit && fourthBit) { return instrMultiply; }
            return instrDataProcessing;
        }
        default: // unknown instruction type
            return instrUnknown;
    }
}
enum DataProcType getDataProcType(WORD instr) {
    bool iFlag = (bool) getBitsFromWord(instr, 25, 1);
    if (iFlag) { return dataProcOp2Imm; }
    bool fourthBit = (bool) getBitsFromWord(instr, 4, 1);
    if (fourthBit) { return dataProcOp2RegShiftReg; }
    else { return dataProcOp2RegShiftConst; }
}
enum SdtType getSdtType(WORD instr) {
    bool iFlag = (bool) getBitsFromWord(instr, 25, 1);
    if (!iFlag) { return sdtOffsetImm; }
    bool fourthBit = (bool) getBitsFromWord(instr, 4, 1);
    if (fourthBit) { return sdtOffsetRegShiftReg; }
    else { return sdtOffsetRegShiftConst; }
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
    return shift(immValue, rotAmount, modifyCPSR, ROR, state);
}
WORD getOperandFromRegisterShift(WORD operandBits, bool regShift, bool modifyCPSR, struct MachineState *state) {
    REGNUMBER rm = getBitsFromWord((WORD) operandBits, 3, 4);
    REGISTER rmContents = state->registers[rm];
    enum ShiftType shiftType = getBitsFromWord(operandBits, 6, 2);

    // calculate shift, specified by a 5-bit unsigned int
    BYTE shiftAmount;
    if (regShift) { // shift specified by register, bottom byte
        REGNUMBER rs = getBitsFromWord(operandBits, 11, 4);
        REGISTER rsContents = state->registers[rs];
        shiftAmount = rsContents & fullBits(8);
    } else { // shift by a constant amount
        shiftAmount = getBitsFromWord(operandBits, 11, 5);
    }

    return shift(rmContents, shiftAmount, modifyCPSR, shiftType, state);
}