#ifndef ARM11_40_SRC_BINARYOPS_H_
#define ARM11_40_SRC_BINARYOPS_H_

#endif //ARM11_40_SRC_BINARYOPS_H_

#include "binaryTypes.h"

static inline WORD fullBits(BYTE numBits);
WORD getBitsFromWord(WORD word, BYTE startBitNo, BYTE numBits);
WORD signExtend(WORD val, BYTE originalLength);