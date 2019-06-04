#include "emulateStatus.h"

WORD shiftAndUpdateStatus(WORD val, BYTE shiftAmount, bool updateCPSR, enum ShiftType shiftType, struct MachineState *state) {
  bool carryOutBit = false;
  // shift value
  WORD result = shift(val, shiftAmount, shiftType, &carryOutBit);

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

bool isSet(enum StatusFlag flag, const struct MachineState *state) {
  return (GETBITS(state->registers[REG_CPSR], WORD_BITS - 1, NIBBLE_BITS) & (BYTE) flag) == flag;
}

void setFlag(enum StatusFlag flag, struct MachineState *state) {
  state->registers[REG_CPSR] = state->registers[REG_CPSR] | (WORD) ((BYTE) flag << CPSR_POS);
}

void clearFlag(enum StatusFlag flag, struct MachineState *state) {
  state->registers[REG_CPSR] = state->registers[REG_CPSR] & (~ (WORD) ((BYTE) flag << CPSR_POS));
}

bool checkCondition(enum CondCode condCode, const struct MachineState *state) {
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
