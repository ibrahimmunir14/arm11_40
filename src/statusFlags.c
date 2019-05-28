#include "statusFlags.h"

bool isSet(enum StatusFlag flag, struct MachineState *state) {
    return (getBitsFromWord(state->registers[REG_CPSR], 31, 4) & (BYTE) flag) == flag;
}
void setFlag(enum StatusFlag flag, struct MachineState *state) {
    state->registers[REG_CPSR] = state->registers[REG_CPSR] | (WORD) ((BYTE) flag << 28u);
}
void clearFlag(enum StatusFlag flag, struct MachineState *state) {
    state->registers[REG_CPSR] = state->registers[REG_CPSR] & (~ (WORD) ((BYTE) flag << 28u));
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

