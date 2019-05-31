#include "mneumonicParser.h"

#define Add 193486030
#define Sub 193506191
#define Rsb 193505036
#define And 193486360
#define Eor 193490763
#define Orr 193501752
#define Tst 193507232
#define Teq 193506767
#define Cmp 193488517

#define Beq 193487165
#define Bne 193487450
#define Bge 193487219
#define Blt 193487399
#define Bgt 193487234
#define Ble 193487384
#define B 177671

int hash(const char *key) {
    unsigned long hash = 5381;
    int c;
    while (c = *key++)
        hash = ((hash << 5) + hash) + c;

    return hash;
}

enum CondCode branchEnum(const char *mneumonic) {
    int hashed = hash(mneumonic);
    switch(hashed){
        case Beq:
            return (Condcode) EQ;
        case Bne:
            return (Condcode) NE;
        case Bge:
            return (Condcode) GE;
        case Blt:
            return (Condcode) LT;
        case Bgt:
            return (Condcode) GT;
        case Ble:
            return (Condcode) LE;
        case B:
            return (Condcode) AL;
        default:
            return (Condcode) NULL;
    }
}

enum OpCode dataProcEnum(const char *mneumonic) {
    int hashed = hash(mneumonic);
    switch(hashed){
        case Add:
            return (Opcode) ADD;
        case Sub:
            return (Opcode) SUB;
        case Rsb:
            return (Opcode) RSB;
        case And:
            return (Opcode) AND;
        case Eor:
            return (Opcode) EOR;
        case Orr:
            return (Opcode) ORR;
        case Tst:
            return (Opcode) TST;
        case Teq:
            return (Opcode) TEQ;
        case Cmp:
            return (Opcode) CMP;
        default:
            return (Opcode) NULL;
    }
}
