#include "emulate.h"
//#include <time.h>

// NOTE: Words in memory are stored Big-Endian; Words in registers are stored Big-Endian
//       Instructions in this code are Little-Endian
//       use readWord/storeWord to read/write from Memory, auto taking care of conversions

/* TODO: clarify how and when to use certain types: use int or uint for positive values? use BYTE or int?\
         being specific vs readability vs correctness */

int main(int argc, char **argv) {
    // ensure we have one argument, the filename
    if (argc != 2) {
      return EXIT_FAILURE;
    }

    //clock_t start = clock();
    // initialise state, registers and memory
    static struct MachineState state;
    state.instrToExecute = 0;
    state.instrToDecode = 0;
    state.registers = (REGISTER *) calloc(NUM_REG, sizeof(REGISTER));
    state.memory = (BYTE *) calloc(MEM_SIZE, sizeof(BYTE));
    if (state.registers == NULL || state.memory == NULL) {
        perror("Error: calloc failed during initialisation of machine state.");
        return EXIT_FAILURE;
    }

    // import file into memory
    char *fileName = argv[1];
    if (!importFile(fileName, state.memory)) {
        perror("Error: could not open file.");
        return EXIT_FAILURE;
    }

    /* Main Pipeline Loop */
    // fill pipeline initially
    state.instrToExecute = readNextInstr(&state);
    state.registers[REG_PC] += 4;
    state.instrToDecode = readNextInstr(&state);
    state.registers[REG_PC] += 4;

    while (state.registers[REG_PC] < MEM_SIZE) {
        // execute instrToExecute
        if (state.instrToExecute == 0) {
            break; // terminate on all-0 instruction
        }
        executeInstruction(state.instrToExecute, &state);

        // decode instrToDecode and put it in instrToExecute
        // note: decoding is actually done during execution
        state.instrToExecute = state.instrToDecode;

        // fetch next instruction and put it in instrToDecode
        state.instrToDecode = readNextInstr(&state);

        // increment PC
        state.registers[REG_PC] += 4;
    }

    printResults(&state);
    free(state.memory);
    free(state.registers);
//    clock_t stop = clock();
//    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
//    printf("\nTime elapsed: %.5f\n", elapsed);
    return EXIT_SUCCESS;
}

WORD shift(WORD val, BYTE shiftAmount, bool updateCPSR, enum ShiftType shiftType, struct MachineState *state) {
    bool carryOutBit = 0;
    WORD result;

    if (shiftAmount <= 0) {
      return val;
    }

    switch (shiftType) {
        case LSL: // logical shift left
            carryOutBit = GETBITS(val, 32 - shiftAmount, 1); // least sig discarded bit
            result = val << shiftAmount;
            break;
        case LSR: // logical shift right
            carryOutBit = GETBITS(val, shiftAmount - 1, 1); // most sig discarded bit
            result = val >> shiftAmount;
            break;
        case ASR: // arithmetic shift right
            carryOutBit = GETBITS(val, shiftAmount - 1, 1); // most sig discarded bit
            result = signExtend(val >> shiftAmount, 32u - shiftAmount);
            break;
        case ROR: // rotate right
            carryOutBit = GETBITS(val, shiftAmount - 1, 1); // most sig discarded bit
            result = val >> shiftAmount | val << (32u - shiftAmount);
            break;
        default:
            return 0;
    }

    // update status register with carry out bit
    if (updateCPSR) {
        if (carryOutBit) {
          setFlag(C, state);
        } else {
          clearFlag(C, state);
        }
    }

    return result;
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
    state->memory[startAddress] = (BYTE) word;
    state->memory[startAddress+1] = (BYTE) (word >> 8u);
    state->memory[startAddress+2] = (BYTE) (word >> 16u);
    state->memory[startAddress+3] = (BYTE) (word >> 24u);
}

/* helper functions for main program */
WORD readNextInstr(struct MachineState *state) {
    return readWord((ADDRESS) state->registers[REG_PC], state);
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
    // check condition is met
    switch (condCode) {
        case AL: return true;
        case EQ: return isSet(Z, state);
        case NE: return !isSet(Z, state);
        case GE: return isSet(N, state) == isSet(V, state);
        case LT: return isSet(N, state) != isSet(V, state);
        case GT: return !isSet(Z, state) && (isSet(N, state) == isSet(V, state));
        case LE: return isSet(Z, state) || (isSet(N, state) != isSet(V, state));
        default: return false;
    }
}

/* helper functions related to CPSR status bits */
bool isSet(enum StatusFlag flag, struct MachineState *state) {
    return (GETBITS(state->registers[REG_CPSR], 31, 4) & (BYTE) flag) == flag;
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
    bool doExecute = checkCondition((enum CondCode) GETBITS(instr, 31, 4), state);
    if (doExecute) {
        // calculate parameters and delegate to relevant function
        switch (getInstrType(instr)) {
            case instrBranch: {
                WORD offsetBits = GETBITS(instr, 23, 24);
                performBranch(offsetBits, state);
                break;
            }
            case instrSDT: {
                enum SdtType sdtType = getSdtType(instr);
                bool pFlag = GETBITS(instr, 24, 1);
                bool uFlag = GETBITS(instr, 23, 1);
                bool lFlag = GETBITS(instr, 20, 1);
                REGNUMBER rn = GETBITS(instr, 19, 4);
                REGNUMBER rd = GETBITS(instr, 15, 4);
                WORD offsetBits = GETBITS(instr, 11, 12);
                performSdt(sdtType, pFlag, uFlag, lFlag, rn, rd, offsetBits, state);
                break;
            }
            case instrMultiply: {
                bool aFlag = GETBITS(instr, 21, 1);
                bool sFlag = GETBITS(instr, 20, 1);
                REGNUMBER rd = GETBITS(instr, 19, 4);
                REGNUMBER rn = GETBITS(instr, 15, 4);
                REGNUMBER rs = GETBITS(instr, 11, 4);
                REGNUMBER rm = GETBITS(instr, 3, 4);
                performMultiply(aFlag, sFlag, rd, rn, rs, rm, state);
                break;
            }
            case instrDataProcessing: {
                enum DataProcType dataProcType = getDataProcType(instr);
                enum OpCode opCode = getOpCode(instr);
                bool sFlag = GETBITS(instr, 20, 1);
                REGNUMBER rn = GETBITS(instr, 19, 4);
                REGNUMBER rd = GETBITS(instr, 15, 4);
                WORD operand2Bits = GETBITS(instr, 11, 12);
                performDataProc(dataProcType, opCode, sFlag, rn, rd, operand2Bits, state);
                break;
            }
            default: {
                printf("Error: Unknown Instruction: 0x%08x\n", instr);
            }
        }
    }
}
void performBranch(WORD offsetBits, struct MachineState *state) {
    // set PC to wherever the next instruction is
    BRANCHOFFSET branchOffset = signExtend(offsetBits << 2u, 26);
    state->registers[REG_PC] += branchOffset;
    // reload pipeline for next cycle
    state->instrToDecode = readNextInstr(state);
    state->registers[REG_PC] += 4;
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
        if (memAddress + offsetValue > FULLBITS(16)) {
            printf("Error: Out of bounds memory access at address 0x%08x\n", memAddress + offsetValue);
            return;
        }
        if (ldstFlag) { // load word from memory
            state->registers[rd] = readWord((ADDRESS) memAddress + offsetValue, state);
        } else { // store word in memory
            writeWord((WORD) state->registers[rd], (ADDRESS) memAddress + offsetValue, state);
        }
    } else { // transfer data then update base register
        // ensure address is in bounds
        if (memAddress > FULLBITS(16)) {
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
        if (GETBITS(result, 31, 1)) {
          setFlag(N, state);
        } else {
          clearFlag(N, state);
        }
    }
}
void performDataProc(enum DataProcType dataProcType, enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, WORD operand2Bits, struct MachineState *state) {
    DPOPERAND2 operand2 = getDPOperand2(dataProcType, operand2Bits, sFlag, state);
    bool aluCarry = false;
    bool isArithOp = false;

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
        case ORR:
            result = state->registers[rn] | operand2;
            break;
        case MOV:
            result = operand2;
            break;
        case SUB:
        case CMP:
            result = state->registers[rn] - operand2;
            aluCarry = result <= state->registers[rn];
            isArithOp = true;
            break;
        case RSB:
            result = operand2 - state->registers[rn];
            aluCarry = result < operand2;
            isArithOp = true;
            break;
        case ADD:
            result = state->registers[rn] + operand2;
            aluCarry = result >> 32u > 0;
            isArithOp = true;
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
        // set Z flag iff result = 0
        if ((WORD) result == 0) {
          setFlag(Z, state);
        } else {
          clearFlag(Z, state);
        }

        // set N flag to value of bit 31
        if (GETBITS((WORD) result, 31, 1)) {
          setFlag(N, state);
        } else {
          clearFlag(N, state);
        }

        // set C flag to carry bit of arithmetic operation
        if (isArithOp) {
            if (aluCarry) {
                setFlag(C, state);
            } else {
                clearFlag(C, state);
            }
        }
    }
}

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
            bool iFlag = (bool) GETBITS(instr, 25, 1);
            bool seventhBit = (bool) GETBITS(instr, 7, 1);
            bool fourthBit = (bool) GETBITS(instr, 4, 1);
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
    bool iFlag = (bool) GETBITS(instr, 25, 1);
    // immediate value
    if (iFlag) {
      return dataProcOp2Imm;
    }
    // shifted register
    bool fourthBit = (bool) GETBITS(instr, 4, 1);
    if (fourthBit) {
      return dataProcOp2RegShiftReg;
    } else {
      return dataProcOp2RegShiftConst;
    }
}
enum SdtType getSdtType(WORD instr) {
    bool iFlag = (bool) GETBITS(instr, 25, 1);
    // immediate value offset
    if (!iFlag) {
      return sdtOffsetImm;
    }
    // shifted register offset
    bool fourthBit = (bool) GETBITS(instr, 4, 1);
    if (fourthBit) {
      return sdtOffsetRegShiftReg;
    } else {
      return sdtOffsetRegShiftConst;
    }
}
enum OpCode getOpCode(WORD instr) {
    return (enum OpCode) GETBITS(instr, 24, 4);
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
    BYTE rotAmount = 2 * GETBITS(operandBits, 11, 4);
    return shift(immValue, rotAmount, modifyCPSR, ROR, state);
}
WORD getOperandFromRegisterShift(WORD operandBits, bool regShift, bool modifyCPSR, struct MachineState *state) {
    REGNUMBER rm = GETBITS((WORD) operandBits, 3, 4);
    REGISTER rmContents = state->registers[rm];
    enum ShiftType shiftType = GETBITS(operandBits, 6, 2);

    // calculate shift, specified by a 5-bit unsigned int
    BYTE shiftAmount;
    if (regShift) { // shift specified by register, bottom byte
        REGNUMBER rs = GETBITS(operandBits, 11, 4);
        REGISTER rsContents = state->registers[rs];
        shiftAmount = rsContents & FULLBITS(8);
    } else { // shift by a constant amount
        shiftAmount = GETBITS(operandBits, 11, 5);
    }

    return shift(rmContents, shiftAmount, modifyCPSR, shiftType, state);
}