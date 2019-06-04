#ifndef ARM11_40_SRC_EMULATESTATUS_H_
#define ARM11_40_SRC_EMULATESTATUS_H_

#include <stdbool.h>
#include "binaryOps.h"

// apply a shift on val specified by shift amount, optionally update status bits
WORD shiftAndUpdateStatus(WORD val, BYTE shiftAmount, bool updateCPSR, enum ShiftType shiftType, struct MachineState *state);

// check condition code
bool checkCondition(enum CondCode condCode, const struct MachineState *state);

/* helper functions related to CPSR status bits */
// check flag is set
bool isSet(enum StatusFlag, const struct MachineState *state);
// set flag (to 1)
void setFlag(enum StatusFlag, struct MachineState *state);
// clear flag (to 0)
void clearFlag(enum StatusFlag, struct MachineState *state);


#endif //ARM11_40_SRC_EMULATESTATUS_H_
