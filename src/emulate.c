#include "emulate.h"

// NOTE: Words in memory are stored Big-Endian; Words in registers are stored Big-Endian
//       Instructions in this code are Little-Endian
//       use readWord/storeWord to read/write from Memory, auto taking care of conversions

// TODO: (1) Complete Data-Processing function(s) [Umer?]
// TODO: (2) Clean-up so all code has consistent style
// TODO: (3) Comment everything properly
// TODO: (4) Organise everything into headers and c files
// TODO: (5) Fix segmentation faults

const bool debug = true;

int main(int argc, char **argv) {
    // ensure we have one argument, the filename
    if (argc != 2) { return EXIT_FAILURE; }

    struct MachineState state;

    // initialize registers, memory to 0s
    for (int i = 0; i < NUM_REG; i++) { state.registers[i] = 0; }
    for (int i = 0; i < MEM_SIZE; i++) { state.memory[i] = 0; }

    // set up memory array with instructions
    char *fileName = argv[1];
    FILE *fPointer = fopen(fileName, "rb");
    fseek(fPointer, 0, SEEK_END);
    int size = (int) ftell(fPointer);
    fseek(fPointer, 0, SEEK_SET);
    for (int i = 0; i < size; i++) {
        state.memory[i] = (BYTE) getc(fPointer);
    }

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

// increment the PC register to the address of the next word instruction
void incrementPC(struct MachineState *state) {
    state->registers[REG_PC] += 4;
}
// output the contents of registers and non-zero memory
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
// returns a word (4 bytes) given the address of the first byte
WORD readFourBytes(ADDRESS startAddress, struct MachineState *state) {
    return (WORD) state->memory[startAddress] << 24u | (WORD) state->memory[startAddress+1] << 16u
         | (WORD) state->memory[startAddress+2] << 8u | (WORD) state->memory[startAddress+3];
}
// returns a word (4 bytes) given the address of the first byte; converts to big-endian
WORD readWord(ADDRESS startAddress, struct MachineState *state) {
    return (WORD) state->memory[startAddress+3] << 24u | (WORD) state->memory[startAddress+2] << 16u
         | (WORD) state->memory[startAddress+1] << 8u | (WORD) state->memory[startAddress];
}
// write a word (4 bytes) in little-endian in memory given the start address and big-endian word
void writeWord(WORD word, ADDRESS startAddress, struct MachineState *state) {
    state->memory[startAddress] = (BYTE) (word & fullBits(8));
    state->memory[startAddress+1] = (BYTE) ((word >> 8u) & fullBits(8));
    state->memory[startAddress+2] = (BYTE) ((word >> 16u) & fullBits(8));
    state->memory[startAddress+3] = (BYTE) ((word >> 24u) & fullBits(8));
}

// checks whether an instruction should be executed based on condition code
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
// helper functions related to CPSR status flags
bool isSet(enum CondFlag flag, struct MachineState *state) {
    return getBitsFromWord(state->registers[REG_CPSR], 31, 4) == flag;
}
void setFlag(enum CondFlag flag, struct MachineState *state) {
    state->registers[REG_CPSR] = state->registers[REG_CPSR] | flag << 28u;
}
void clearFlag(enum CondFlag flag, struct MachineState *state) {
    state->registers[REG_CPSR] = state->registers[REG_CPSR] & (~(flag << 28u));
}

// execute the given instruction
void executeInstruction(WORD instr, struct MachineState *state) {
    // check if instruction should be executed
    bool doExecute = checkCondition((enum CondCode) getBitsFromWord(instr, 31, 4), state);
    // delegate to relevant helper function
    if (doExecute) {
        switch (getInstrType(instr)) {
            case instrBranch: {
                if (debug) printf("Branch Operation: (0x%08x)\n", instr);
                WORD offsetBits = getBitsFromWord(instr, 23, 24);
                performBranch(offsetBits, state);
                break; }
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
            case instrDataProcessing:
                if (debug) printf("DataProcessing Operation: (0x%08x)\n", instr);
                // TODO: (1) delegate to appropriate function
                break;
            default:
                if (debug) printf("Unknown Operation\n");
        }
    }
}

void performBranch(WORD offsetBits, struct MachineState *state) {
    // falsify hasInstr booleans
    state->hasInstrToExecute = false;
    state->hasInstrToDecode = false;
    // set PC to wherever the next instruction is
    BRANCHOFFSET branchOffset = signExtend(offsetBits << 2u, 26);
    if (debug) printf("  Offset Value: %i\n", branchOffset);
    if (debug) printf("  REG_PC incremented from 0x%08x to ", state->registers[REG_PC]);
    state->registers[REG_PC] += branchOffset;
    if (debug) printf("0x%08x\n", state->registers[REG_PC]);
}
void performSdt(enum SdtType sdtType, bool pFlag, bool upFlag, bool ldstFlag, REGNUMBER rn, REGNUMBER rd, WORD offsetBits, struct MachineState *state) {
    ADDRESS address = (ADDRESS) state->registers[rn];
    if (debug) printf("  Rn $%i; Rd $%i; Offset (0x%03x)\n", rn, rd, offsetBits);

    SDTOFFSET offsetValue = getSDTOffset(sdtType, offsetBits, state);

    // upFlag determines whether to add or subtract offset
    if (!upFlag) offsetValue = -offsetValue;

    if (pFlag) { // transfer data using address after offset
        if (ldstFlag) { // load word from memory
            if (debug) printf("  Load (pre-index) from memory[0x%04x + 0x%04x = 0x%04x] to register[$%i]\n",
                    address, offsetValue, address + offsetValue, rd);
            state->registers[rd] = readWord(address + offsetValue, state);
        } else { // store word in memory
            if (debug) printf("  Store (pre-index) from register[$%i] to memory[0x%04x + 0x%04x = 0x%04x]\n",
                   rd, address, offsetValue, address + offsetValue);
            writeWord((WORD) state->registers[rd], address + offsetValue, state);
        }
    } else { // transfer data then update base register
        if (ldstFlag) { // load word from memory
            if (debug) printf("  Load (post-index) from memory[0x%04x] to register[$%i]\n", address, rd);
            state->registers[rd] = readWord(address, state);
        } else { // store word in memory
            if (debug) printf("  Store (post-index) from register[$%i] to memory[0x%04x]\n", rd, address);
            writeWord((WORD) state->registers[rd], address, state);
        }
        if (debug) printf("  Incremented register[$%i] by offsetValue (0x%04x)", rn, offsetValue);
        state->registers[rn] += offsetValue;
    }
}
// given the 12-bit offset bits, calculate and returns the final sdt offset
SDTOFFSET getSDTOffset(enum SdtType sdtType, WORD offsetBits, struct MachineState *state) {
    return (sdtType == sdtOffsetImm)
                        ? offsetBits // offset is immediate value
                        : getOperandFromRegisterShift(offsetBits, (sdtType == sdtOffsetRegShiftReg), state);

}
void performMultiply(bool aFlag, bool sFlag, REGNUMBER rd, REGNUMBER rn, REGNUMBER rs, REGNUMBER rm, struct MachineState *state) {
    if (debug) printf("  Rn $%i; Rd $%i; Rs $%i; Rm $%i\n", rn, rd, rs, rm);
    if (debug) printf("  accumulate: %i;  statusupdate: %i\n", aFlag, sFlag);
    WORD result;
    if (aFlag) {
        result = state->registers[rm] * state->registers[rs] + state->registers[rn];
    } else {
        result = state-> registers[rm] * state-> registers[rs];
    }
    state->registers[rd] = result;
    if (sFlag) {
        // set the Z flag iff result is 0
        if (result == 0) setFlag(Z, state);
        else clearFlag(Z, state);
        // set the N flag to bit 31 of result
        if (getBitsFromWord(result, 31, 1)) setFlag(N, state);
        else clearFlag(N, state);
    }
}

void performDataProc(enum DataProcType dataProcType, enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, OFFSET Operand2, struct MachineState *state) {
    int op2;
    if (dataProcType == dataProcOp2Imm) {
        op2 = getImmValue(Operand2);
    } else if (dataProcType == dataProcOp2RegShiftConst) {
        op2 = getRegValue(true, Operand2);
    } else {
        op2 = getRegValue(false, Operand2);
    }

    bool executeNext = true;

    switch (opCode) {
        case AND:
            state->registers[rd] = state->registers[rn] & op2;
        case EOR:
            state->registers[rd] = state->registers[rn] ^ op2;
        case SUB:
            state->registers[rd] = state->registers[rn] - op2;
        case RSB:
            state->registers[rd] = op2 - state->registers[rn];
        case ADD:
            state->registers[rd] = state->registers[rn] + op2;
        case TST:
            executeNext = state->registers[rn] == (state->registers[rn] & op2);
        case TEQ:
            executeNext = state->registers[rn] == (state->registers[rn] ^ op2);
        case CMP:
            executeNext = state->registers[rn] == (state->registers[rn] - op2) > 0;
        case ORR:
            state->registers[rd] = state->registers[rn] | op2;
        case MOV:
            state->registers[rd] = op2;
    }

    if (!executeNext) {
        state->hasInstrToExecute = false;
        // TODO (1) check if have to make this false or the decode false
    }

    // TODO (1) set flags

}
int getImmValue(OFFSET Operand2) {
    // TODO (1) remove these magic numbers
    int value = Operand2 & 255;
    int rotate = (Operand2 & 3840) >> 7;
    return (value >> rotate)|(value << (32 - rotate));
}
int getRegValue(bool constShift, OFFSET Operand2) {

}

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

// given a 12-bit operand, where operand specified by register, calculates and returns the 32-bit operand
WORD getOperandFromRegisterShift(WORD operandBits, bool regShift, struct MachineState *state) {
    REGNUMBER rm = getBitsFromWord((WORD) operandBits, 3, 4);
    REGISTER rmContents = state->registers[rm];
    enum ShiftType shiftType = getBitsFromWord(operandBits, 6, 2);
    BYTE shiftAmount; // shift specified by 5-bit unsigned int

    if (regShift) { // shift specified by register, bottom byte
        REGNUMBER rs = getBitsFromWord(operandBits, 11, 4);
        REGISTER rsContents = state->registers[rs];
        shiftAmount = rsContents & fullBits(8);
    } else { // shift by a constant amount
        shiftAmount = getBitsFromWord(operandBits, 11, 5);
    }

    //Shift register if shift amount is nonzero
    return shiftAmount == 0 ? rmContents
                            : shift(rmContents, shiftAmount, 0, shiftType, state);
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
    BYTE emptyBits = 32u - originalLength;
    bool topBitOne = getBitsFromWord(val, originalLength - 1, 1) == 1;
    return topBitOne
    ? val | (fullBits(emptyBits) << originalLength)
    : val;
}
WORD fullBits(BYTE numBits) {
    return (WORD) pow(2, numBits) - 1;
}
WORD getBitsFromWord(WORD word, BYTE startBitNo, BYTE numBits) {
    BYTE andOp = (BYTE) (pow(2, numBits) - 1);
    WORD wordShifted = word >> (BYTE) (1 + startBitNo - numBits);
    return andOp & wordShifted;
}
