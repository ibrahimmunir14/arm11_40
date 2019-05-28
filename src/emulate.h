#ifndef ARM11_40_EMULATE_H
#define ARM11_40_EMULATE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "machine.h"
#include "binaryOps.h"
#include "binaryFileLoader.h"
#include "instructionExecution.h"
#include "instructionDecoding.h"
#include "memAccess.h"

// increment program counter
void incrementPC(struct MachineState *state);

// print current state of machine
void printResults(struct MachineState *state);

#endif //ARM11_40_EMULATE_H