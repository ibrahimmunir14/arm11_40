#include <math.h>
#include <stdbool.h>
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