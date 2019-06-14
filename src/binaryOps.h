#ifndef ARM11_40_SRC_BINARYOPS_H_
#define ARM11_40_SRC_BINARYOPS_H_

#include <stdbool.h>
#include "machineDataTypes.h"

enum ShiftType {LSL=0, LSR=1, ASR=2, ROR=3};

// macro: value representing numBits 1s in a row
#define FULLBITS(numBits) ((1u << ((BYTE) (numBits))) - 1u)
// macro: extract the chosen bits (using number scheme from spec, big-endian), and return right-aligned bits
#define GETBIT(word, startBitNo) ((word >> (BYTE) startBitNo) & 1)
// macro: extract the chosen bit from word
#define GETBITS(word, startBitNo, numBits) (FULLBITS(numBits) & ((word) >> (BYTE) (1 + (startBitNo) - (numBits))))
// macro: extract the chosen nibble from word
#define GETNIBBLE(word, startBitNo) (0xF & ((word) >> (BYTE) (1 + (startBitNo) - NIBBLE_BITS)))

// sign extend val to 32 bits, given the original length of val
WORD signExtend(WORD val, BYTE originalLength);
// append bits to end of word
WORD appendBits(unsigned int numOfBits, WORD bitString, int addition);
// append nibble to end of word
WORD appendNibble(WORD bitString, int addition);
// shift word by shift amount
WORD shift(WORD val, BYTE shiftAmount, enum ShiftType shiftType, bool *carryOutBit);

WORD rotateRight(WORD num, unsigned int shiftAmount);

WORD rotateLeft(WORD num, unsigned int shiftAmount);

#endif //ARM11_40_SRC_BINARYOPS_H_

