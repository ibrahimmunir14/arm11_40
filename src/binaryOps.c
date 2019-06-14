#include "binaryOps.h"

WORD signExtend(WORD val, BYTE originalLength) {
  BYTE numEmptyBits = WORD_BITS - originalLength;
  bool msbSet = GETBITS(val, originalLength - 1, 1) == 1;
  return msbSet ? val | (FULLBITS(numEmptyBits) << originalLength) : val;
}

WORD appendBits(unsigned int numOfBits, WORD bitString, int addition) {
  return (bitString << numOfBits) | GETBITS(addition, numOfBits - 1, numOfBits);
}

WORD appendNibble(WORD bitString, int addition) {
  return appendBits(4, bitString, addition);
}

WORD shift(WORD val, BYTE shiftAmount, enum ShiftType shiftType, bool *carryOutBit) {
  if (shiftAmount <= 0) {
    return val;
  }

  switch (shiftType) {
    case LSL: // logical shift left
      *carryOutBit = GETBITS(val, WORD_BITS - shiftAmount, 1); // least sig discarded bit
      return val << shiftAmount;
    case LSR: // logical shift right
      *carryOutBit = GETBITS(val, shiftAmount - 1, 1); // most sig discarded bit
      return val >> shiftAmount;
    case ASR: // arithmetic shift right
      *carryOutBit = GETBITS(val, shiftAmount - 1, 1); // most sig discarded bit
      return signExtend(val >> shiftAmount, WORD_BITS - shiftAmount);
    case ROR: // rotate right
      *carryOutBit = GETBITS(val, shiftAmount - 1, 1); // most sig discarded bit
      return rotateRight(val, shiftAmount);
    default:
      return val;
  }
}

WORD rotateRight(WORD num, unsigned int shiftAmount) {
  return (num >> shiftAmount) | (num << (WORD_BITS - shiftAmount));
}

WORD rotateLeft(WORD num, unsigned int shiftAmount) {
  return (num << shiftAmount) | (num >> (WORD_BITS - shiftAmount));
}

