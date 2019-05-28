#ifndef ARM11_40_SRC_BINARYOPS_H_
#define ARM11_40_SRC_BINARYOPS_H_

#include "binaryTypes.h"

// return value which is numBits 1s in a row
WORD fullBits(BYTE numBits);
// extracts specific bits from word
WORD getBitsFromWord(WORD word, BYTE startBitNo, BYTE numBits);
// sign extend word
WORD signExtend(WORD val, BYTE originalLength);

#endif //ARM11_40_SRC_BINARYOPS_H_

