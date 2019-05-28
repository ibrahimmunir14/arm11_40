#ifndef ARM11_40_SRC_BINARYTYPES_H_
#define ARM11_40_SRC_BINARYTYPES_H_

#include <stdint.h>

typedef uint32_t REGISTER;	// register contents are 32 bits
typedef uint32_t WORD;		// words/instructions are 32 bits
typedef uint16_t ADDRESS;   // memory addresses are 16 bits, unsigned because addresses positive
typedef uint8_t BYTE;		// bytes are 8 bits

#endif //ARM11_40_SRC_BINARYTYPES_H_