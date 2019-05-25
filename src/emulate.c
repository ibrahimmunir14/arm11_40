#include "emulate.h"

// NOTE: Words in memory are stored Big-Endian; Words in registers are stored Big-Endian
//       Instructions in this code are Little-Endian
//       use readWord/storeWord to read/write from Memory, auto taking care of conversions

// TODO: Clean-up so all code has consistent style
// TODO: Comment everything properly
// TODO: Fix segmentation faults
// TODO: Ensure all types are correct (BYTE, REGISTER, OFFSET etc.)

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
    printf("Initial Memory Contents:\n");
    for (int i = 0; i < MEM_SIZE; i += 4) {
        WORD word = readFourBytes((ADDRESS) i, &state);
        if (word != 0) {
            printf("0x%08x: 0x%08x\n", i, word);
        }
    }
    printf("\n");

    // main pipeline loop
    WORD instrToExecute = 0;
    WORD instrToDecode = 0;
    bool hasInstrToExecute = false;
    bool hasInstrToDecode = false;
    while (state.registers[REG_PC] < MEM_SIZE) {
        // execute instrToExecute
        if (hasInstrToExecute) {
            if (instrToExecute == 0) {
                printf("TERMINATE: All-0 Instruction\n\n");
                break; // terminate on all-0 instruction
            } else {
                executeInstruction(instrToExecute, &state);
            }
        }

        // decode instrToDecode and put it in instrToExecute
        // note: decoding is actually done during execution
        if (hasInstrToDecode) {
            hasInstrToExecute = true;
            instrToExecute = instrToDecode;
            printf("Decoded Instruction 0x%08x\n", instrToExecute);

        }

        // fetch next instruction and put it in instrToDecode
        instrToDecode = readWord((ADDRESS) state.registers[REG_PC], &state);
        printf("Fetched Instruction at 0x%08x : 0x%08x\n", state.registers[REG_PC], instrToDecode);
        hasInstrToDecode = true;

        // increment program counter
        incrementPC(&state);
        printf("Incremented PC to 0x%08x\n\n", state.registers[REG_PC]);
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
    return state->memory[startAddress] << 24 | state->memory[startAddress+1] << 16
           | state->memory[startAddress+2] << 8 | state->memory[startAddress+3];
}

// returns a word (4 bytes) given the address of the first byte; converts to big-endian
WORD readWord(ADDRESS startAddress, struct MachineState *state) {
    return state->memory[startAddress+3] << 24 | state->memory[startAddress+2] << 16
           | state->memory[startAddress+1] << 8 | state->memory[startAddress];
}

// write a word (4 bytes) given the start byte and word in big-endian; convert to little-endian
void writeWord(WORD word, ADDRESS startAddress, struct MachineState *state) {
    state->memory[startAddress] = (BYTE) (word & (FULL_BYTE));
    state->memory[startAddress+1] = (BYTE) ((word >> 8) & (FULL_BYTE));
    state->memory[startAddress+2] = (BYTE) ((word >> 16) & (FULL_BYTE));
    state->memory[startAddress+3] = (BYTE) ((word >> 24) & (FULL_BYTE));
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
    return (state->registers[REG_CPSR] & flag << 4) == flag << 4;
}
void setFlag(enum CondFlag flag, struct MachineState *state) {
    state->registers[REG_CPSR] = state->registers[REG_CPSR] | flag << 4;
}
void clearFlag(enum CondFlag flag, struct MachineState *state) {
    state->registers[REG_CPSR] = state->registers[REG_CPSR] & (~(flag << 4));
}

// execute the given instruction
void executeInstruction(WORD instr, struct MachineState *state) {
    // check if instruction should be executed
    bool doExecute = checkCondition((enum CondCode) getBitsFromWord(instr, 31, 4), state);
    // delegate to relevant helper function
    if (doExecute) {
        switch (getInstrType(instr)) {
            case instrBranch:
                printf("Branch Operation: (0x%08x)\n", instr);
                // TODO: delegate to branch function
                break;
            case instrSDT: {
                printf("SDT Operation: (0x%08x)\n", instr);
                enum SdtType sdtType = getSdtType(instr);
                bool pFlag = getBitsFromWord(instr, 24, 1);
                bool uFlag = getBitsFromWord(instr, 23, 1);
                bool lFlag = getBitsFromWord(instr, 20, 1);
                BYTE rn = getBitsFromWord(instr, 19, 4);
                BYTE rd = getBitsFromWord(instr, 15, 4);
                OFFSET offset = getBitsFromWord(instr, 11, 12);
                performSdt(sdtType, pFlag, uFlag, lFlag, rn, rd, offset, state);
                break;
            }
            case instrMultiply: {
                printf("Multiply Operation: (0x%08x)\n", instr);
                bool aFlag = getBitsFromWord(instr, 21, 1);
                bool sFlag = getBitsFromWord(instr, 20, 1);
                BYTE rd = getBitsFromWord(instr, 19, 4);
                BYTE rn = getBitsFromWord(instr, 15, 4);
                BYTE rs = getBitsFromWord(instr, 11, 4);
                BYTE rm = getBitsFromWord(instr, 3, 4);
                performMultiply(aFlag, sFlag, rd, rn, rs, rm, state);
                break;
            }
            case instrDataProcessing:
                printf("DataProcessing Operation: (0x%08x)\n", instr);
                // TODO: delegate to appropriate function
                break;
            default:
                printf("Unknown Operation\n");
        }
    }
}

void performBranch(OFFSET offset, struct MachineState *state) {
    // TODO: implement branch operations
}

void performSdt(enum SdtType sdtType, bool pFlag, bool upFlag, bool ldstFlag, BYTE rn, BYTE rd, OFFSET offset, struct MachineState *state) {
    ADDRESS address = (ADDRESS) state->registers[rn];
    //printf("  Rn $%i; Rd $%i; Offset (0x%03x)\n", rn, rd, offset);

    int offsetValue = sdtType == sdtOffsetImm ?
            (unsigned int) offset : //Offset is an immediate value
            shiftRegister(offset, sdtType, state); //Offset is a shifted register value

    // upFlag determines whether to add or subtract offset
    if (!upFlag) {
        offsetValue = -offsetValue;
    }

    if (pFlag) {
        // Transfer data using address that has been offset
        if (ldstFlag) {
            // Load word from memory
            //printf("  Load (pre-index) from memory[0x%04x + 0x%04x = 0x%04x] to register[$%i]\n",
            //        address, offsetValue, address + offsetValue, rd);
            state->registers[rd] = readWord(address + offsetValue, state);
        } else {
            // Store word in memory
            //printf("  Store (pre-index) from register[$%i] to memory[0x%04x + 0x%04x = 0x%04x]\n",
            //       rd, address, offsetValue, address + offsetValue);
            writeWord((WORD) state->registers[rd], address + offsetValue, state);
        }
    } else {
        // Transfer data then update base register
        if (ldstFlag) {
            // Load word from memory
            //printf("  Load (post-index) from memory[0x%04x] to register[$%i]\n", address, rd);
            state->registers[rd] = readWord(address, state);
        } else {
            // Store word in memory
            //printf("  Store (post-index) from register[$%i] to memory[0x%04x]\n", rd, address);
            writeWord((WORD) state->registers[rd], address, state);
        }
        //printf("  Incremented register[$%i] by offsetValue (0x%04x)", rn, offsetValue);
        state->registers[rn] += offsetValue;
    }
}

void performMultiply(bool aFlag, bool sFlag, BYTE rd, BYTE rn, BYTE rs, BYTE rm, struct MachineState *state) {
    //printf("  Rn $%i; Rd $%i; Rs $%i; Rm $%i; Offset (0x%03x)\n", rn, rd, rs, rm, offset);
    //printf("  accumulate: %i;  statusupdate: %i\n", aFlag, sFlag);
    if (aFlag) {
        state->registers[rd] = state->registers[rm] * state->registers[rs] + state->registers[rn];
    } else {
        state-> registers[rd] = state-> registers[rm] * state-> registers[rs];
    }

    if (sFlag) {
        // set the Z flag iff result is 0
        if (state->registers[rd] == 0) setFlag(Z, state);
        else clearFlag(Z, state);
        // set the N flag to bit 31 of result
        if (getBitsFromWord(rd, 31, 1)) setFlag(N, state);
        else setFlag(N, state);
    }
}

void performDataProc(enum DataProcType dataProcType, enum OpCode opCode, bool sFlag, BYTE rn, BYTE rd, OFFSET Operand2, struct MachineState *state) {
    // TODO: implement Data Processing Operations. Delegate to relevant functions per opCode
    // TODO: implement data processing functions for each opCode
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

WORD shiftRegister(OFFSET offset, enum SdtType sdtType, struct MachineState *state) {
    BYTE rm = getBitsFromWord((WORD) offset, 3, 4);
    REGISTER rmContents = state->registers[rm];
    enum ShiftType shiftType = getBitsFromWord((WORD) offset, 6, 2);
    BYTE shiftAmount;

    if (sdtType == sdtOffsetRegShiftReg) {
        //Shift amount is the last byte in register Rs
        BYTE rs = getBitsFromWord((WORD) offset, 11, 4);
        REGISTER rsContents = state->registers[rs];
        shiftAmount = rsContents & ((BYTE) FULL_BYTE);
    } else {
        //Shift amount is immediate value
        shiftAmount = getBitsFromWord((WORD) offset, 11, 5);
    }

    //Shift register if shift amount is nonzero
    return shiftAmount == 0 ? rmContents
                      : shift(rmContents, shiftAmount, 0, shiftType, state);
}
BYTE shift(BYTE val, BYTE shiftAmount, bool updateCPSR, enum ShiftType shiftType, struct MachineState *state) {
    bool carryOutBit = 0;
    BYTE result;
    switch (shiftType) {
        // TODO: Ensure this follows spec, order of shift and retrieving byte
        case LSL : {
            carryOutBit = (val >> (32 - shiftAmount)) & 1; // least sig discarded bit
            result = val << shiftAmount;
        }
        case LSR : {
            carryOutBit = (val >> shiftAmount - 1) & 1; // most sig discarded bit
            result = val >> shiftAmount;
        }
        case ASR : {
            carryOutBit = (val >> shiftAmount - 1) & 1; // most sig discarded bit
            result = val >> shiftAmount | ((WORD) pow(2, shiftAmount) - 1 << (32 - shiftAmount));
        }
        case ROR : {
            carryOutBit = (val >> shiftAmount - 1) & 1; // most sig discarded bit
            result = val >> shiftAmount | val << (32 - shiftAmount);
        }
    }
    if (updateCPSR) {
        if (carryOutBit) setFlag(C, state);
        else clearFlag(C, state);
    }
    return result;
}

WORD getBitsFromWord(WORD word, BYTE startBitNo, BYTE numBits) {
    BYTE andOp = (BYTE) (pow(2, numBits) - 1);
    WORD wordShifted = word >> (1 + startBitNo - numBits);
    return andOp & wordShifted;
}
