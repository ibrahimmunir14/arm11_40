//
// Created by ljt18 on 24/05/19.
//

#ifndef ARM11_40_SHIFT_H
#define ARM11_40_SHIFT_H

#endif //ARM11_40_SHIFT_H

enum ShiftType {LSL=0, LSR=1, ASR=2, ROR=3};

int shift(int val, int shamt, enum ShiftType shiftType);
int rotateRightShift(int val, int shamt);
int logicalRightShift(int val, int shamt);