#include "binaryOps.h"

WORD fullBits(BYTE numBits) {
  return (WORD) pow(2, numBits) - 1;
}

WORD getBitsFromWord(WORD word, BYTE startBitNo, BYTE numBits) {
  WORD andOp = fullBits(numBits);
  WORD wordShifted = word >> (BYTE) (1 + startBitNo - numBits);
  return andOp & wordShifted;
}

WORD signExtend(WORD val, BYTE originalLength) {
  BYTE numEmptyBits = 32u - originalLength;
  bool topBitOne = getBitsFromWord(val, originalLength - 1, 1) == 1;
  return topBitOne
         ? val | (fullBits(numEmptyBits) << originalLength)
         : val;
}

WORD shift(WORD val, BYTE shiftAmount, enum ShiftType shiftType, bool *carryOutBit) {
  *carryOutBit = 0;

  if (shiftAmount <= 0) {
    return val;
  }

  switch (shiftType) {
    case LSL: // logical shift left
      *carryOutBit = getBitsFromWord(val, 32 - shiftAmount, 1); // least sig discarded bit
          return val << shiftAmount;
    case LSR: // logical shift right
      *carryOutBit = getBitsFromWord(val, shiftAmount - 1, 1); // most sig discarded bit
          return val >> shiftAmount;
    case ASR: // arithmetic shift right
      *carryOutBit = getBitsFromWord(val, shiftAmount - 1, 1); // most sig discarded bit
          return signExtend(val >> shiftAmount, 32u - shiftAmount);
    case ROR: // rotate right
      *carryOutBit = getBitsFromWord(val, shiftAmount - 1, 1); // most sig discarded bit
          return val >> shiftAmount | val << (32u - shiftAmount);
    default:
          return val;
  }
}