#ifndef ARM11_40_SRC_BINARYOPS_H_
#define ARM11_40_SRC_BINARYOPS_H_

#include <stdbool.h>
#include <math.h>
#include "binaryTypes.h"

enum ShiftType {LSL=0, LSR=1, ASR=2, ROR=3};

// return value which is numBits 1s in a row
WORD fullBits(BYTE numBits);
// extracts specific bits from word
WORD getBitsFromWord(WORD word, BYTE startBitNo, BYTE numBits);
// sign extend word
WORD signExtend(WORD val, BYTE originalLength);
// shift word based on shift type
WORD shift(WORD val, BYTE shiftAmount, enum ShiftType shiftType, bool *carryOutBit);

#endif //ARM11_40_SRC_BINARYOPS_H_