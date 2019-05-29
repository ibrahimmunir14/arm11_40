#ifndef ARM11_40_SRC_BINARYOPS_H_
#define ARM11_40_SRC_BINARYOPS_H_

#include "binaryTypes.h"

// return value representing numBits 1s in a row
WORD fullBits(BYTE numBits);
// extract the chosen bits (using number scheme from spec, big-endian), and return right-aligned bits
WORD getBitsFromWord(WORD word, BYTE startBitNo, BYTE numBits);
// sign extend val to 32 bits, given the original length of val
WORD signExtend(WORD val, BYTE originalLength);

#endif //ARM11_40_SRC_BINARYOPS_H_

