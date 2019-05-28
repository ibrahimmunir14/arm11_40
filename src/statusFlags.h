#ifndef ARM11_40_STATUSFLAGS_H
#define ARM11_40_STATUSFLAGS_H

#include "machine.h"
#include "binaryOps.h"

enum StatusFlag {V=1, C=2, Z=4, N=8};
enum CondCode {EQ=0, NE=1, GE=10, LT=11, GT=12, LE=13, AL=14};

// check flag is set
bool isSet(enum StatusFlag, struct MachineState *state);
// set flag (to 1)
void setFlag(enum StatusFlag, struct MachineState *state);
// clear flag (to 0)
void clearFlag(enum StatusFlag, struct MachineState *state);
// check condition code
bool checkCondition(enum CondCode condCode, struct MachineState *state);

#endif //ARM11_40_STATUSFLAGS_H
