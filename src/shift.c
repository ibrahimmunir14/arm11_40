//
// Created by ljt18 on 24/05/19.
//
#include "shift.h"

int shift(int val, int shamt, enum ShiftType shiftType) {
    switch (shiftType) {
        case LSL : return val << shamt;
        case LSR : return logicalRightShift(val, shamt);
        case ASR : return val >> shamt;
        case ROR : return rotateRightShift(val, shamt);
    }
    return 0;
}


int rotateRightShift(int val, int shamt) {
    return (val << shamt) | (val >> (sizeof(int) - shamt));
}

int logicalRightShift(int val, int shamt) {
    return (int) ((unsigned int) val >> shamt);
}
