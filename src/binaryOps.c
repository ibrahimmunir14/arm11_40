#include <math.h>
#include <stdbool.h>
#include "binaryOps.h"

WORD signExtend(WORD val, BYTE originalLength) {
  BYTE numEmptyBits = 32u - originalLength;
  bool topBitOne = GETBITS(val, originalLength - 1, 1) == 1;
  return topBitOne
         ? val | (FULLBITS(numEmptyBits) << originalLength)
         : val;
}

