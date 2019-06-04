#include "emulate.h"

int main(int argc, char **argv) {
    // ensure we have one argument, the filename
    if (argc != 2) {
      return EXIT_FAILURE;
    }

    struct MachineState state = {(REGISTER *) calloc(NUM_REG, sizeof(REGISTER)),
                                 (BYTE *) calloc(MEM_SIZE, sizeof(BYTE)),
                                 0, 0};

    if (state.registers == NULL || state.memory == NULL) {
        perror("Error: calloc failed during initialisation of machine state.");
        return EXIT_FAILURE;
    }

    // import file into memory
    char *fileName = argv[1];
    if (!importBinaryFile(fileName, state.memory)) {
        perror("Error: could not open file.");
        return EXIT_FAILURE;
    }

    /* Main Pipeline Loop */
    // fill pipeline initially
    state.instrToExecute = readNextInstr(&state);
    state.registers[REG_PC] += sizeof(WORD);
    state.instrToDecode = readNextInstr(&state);
    state.registers[REG_PC] += sizeof(WORD);

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
        state.registers[REG_PC] += sizeof(WORD);
    }

    printResults(&state);
    free(state.memory);
    free(state.registers);
    return EXIT_SUCCESS;
}

/* functions related to memory access */
WORD readFourBytes(ADDRESS startAddress, const struct MachineState *state) {
    return (WORD) state->memory[startAddress] << 24u
      | (WORD) state->memory[startAddress + 1] << 16u
      | (WORD) state->memory[startAddress + 2] << 8u
      | (WORD) state->memory[startAddress + 3];
}
WORD readWord(ADDRESS startAddress, const struct MachineState *state) {
    return (WORD) state->memory[startAddress + 3] << 24u
      | (WORD) state->memory[startAddress + 2] << 16u
      | (WORD) state->memory[startAddress + 1] << 8u
      | (WORD) state->memory[startAddress];
}
void writeWord(WORD word, ADDRESS startAddress, struct MachineState *state) {
    state->memory[startAddress] = (BYTE) word;
    state->memory[startAddress+1] = (BYTE) (word >> 8u);
    state->memory[startAddress+2] = (BYTE) (word >> 16u);
    state->memory[startAddress+3] = (BYTE) (word >> 24u);
}

/* helper functions for main program */
WORD readNextInstr(const struct MachineState *state) {
    return readWord((ADDRESS) state->registers[REG_PC], state);
}
void printResults(const struct MachineState *state) {
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
  for (int i = 0; i < MEM_SIZE; i += sizeof(WORD)) {
    WORD word = readFourBytes((ADDRESS) i, state);
    if (word != 0) {
      printf("0x%08x: 0x%08x\n", i, word);
    }
  }
}

/* functions for execution of instructions */
void executeInstruction(WORD instr, struct MachineState *state) {
    // check if instruction should be executed
    bool doExecute = checkCondition((enum CondCode) GETNIBBLE(instr, 31), state);
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
                bool pFlag = GETBIT(instr, 24);
                bool uFlag = GETBIT(instr, 23);
                bool lFlag = GETBIT(instr, 20);
                REGNUMBER rn = GETNIBBLE(instr, 19);
                REGNUMBER rd = GETNIBBLE(instr, 15);
                WORD offsetBits = GETBITS(instr, 11, 12);
                performSdt(sdtType, pFlag, uFlag, lFlag, rn, rd, offsetBits, state);
                break;
            }
            case instrMultiply: {
                bool aFlag = GETBIT(instr, 21);
                bool sFlag = GETBIT(instr, 20);
                REGNUMBER rd = GETNIBBLE(instr, 19);
                REGNUMBER rn = GETNIBBLE(instr, 15);
                REGNUMBER rs = GETNIBBLE(instr, 11);
                REGNUMBER rm = GETNIBBLE(instr, 3);
                performMultiply(aFlag, sFlag, rd, rn, rs, rm, state);
                break;
            }
            case instrDataProcessing: {
                enum DataProcType dataProcType = getDataProcType(instr);
                enum OpCode opCode = getOpCode(instr);
                bool sFlag = GETBIT(instr, 20);
                REGNUMBER rn = GETNIBBLE(instr, 19);
                REGNUMBER rd = GETNIBBLE(instr, 15);
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
    state->registers[REG_PC] += sizeof(WORD);
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
    WORD result = state-> registers[rm] * state-> registers[rs];

    if (aFlag) {
        result += state->registers[rn];
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
        if (GETBIT(result, 31)) {
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
        if (GETBIT((WORD) result, 31)) {
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