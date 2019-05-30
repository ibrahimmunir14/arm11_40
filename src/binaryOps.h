#ifndef ARM11_40_SRC_BINARYOPS_H_
#define ARM11_40_SRC_BINARYOPS_H_

#include <math.h>
#include "machineDataTypes.h"

// macro: value representing numBits 1s in a row
#define FULLBITS(numBits) ((1 << (numBits)) - 1)
// macro: extract the chosen bits (using number scheme from spec, big-endian), and return right-aligned bits
#define GETBITS(word, startBitNo, numBits) (FULLBITS(numBits) & ((word) >> (BYTE) (1 + (startBitNo) - (numBits))))

// sign extend val to 32 bits, given the original length of val
WORD signExtend(WORD val, BYTE originalLength);



#endif //ARM11_40_SRC_BINARYOPS_H_

