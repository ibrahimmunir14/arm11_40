#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

//Defines some types of our own. note: base these on platform independent types defined in <stdint.h>
typedef uint32_t REGISTER;		//registers are 32 bits
typedef uint32_t WORD;			//words are 32 bits
typedef uint8_t BYTE;			//bytes are 8 bits
typedef uint8_t BIT;			//define bit data type

//store registers in an array of register
// R13 = SP; R14 = LR; R15 = PC; R16 = CPSR
REGISTER registers[17];
//64kb memory capacity, 1 word is 4 bytes
WORD memory[16384];

int main(int argc, char **argv) {
    // ensure we have one argument, the filename
    if (argc != 2) { return EXIT_FAILURE; }

    // initialize registers to 0s
    for (int i = 0; i < 17; i++) { registers[i] = 0; }

    // set up memory array with instructions
    FILE *fPointer;
    char *fileName = argv[1];
    fPointer = fopen(fileName, "rb");

    fseek(fPointer, 0, SEEK_END);
    int size = (int) ftell(fPointer) / sizeof(int);
    fseek(fPointer, 0, SEEK_SET);

    for (int i = 0; i < size; i++) {
        int word[sizeof(int)];
        for (int j = 0; j < sizeof(int); j ++) {
            word[j] = getc(fPointer);
        }
        memory[i] = word[0] << 24 | word[1] << 16 | word[2] << 8 | word[3];
    }

    fclose(fPointer);

    // print contents of memory
    for (int i = 0; i < size; i++) {
        printf("%d\n", memory[i]);
    }

    return EXIT_SUCCESS;
}

/* CHECKLIST
 *
 * 1. from the input, take each line and put it in an index of an array (how do we find the size?)
 * 2. define a struct which includes the pipeline (decode and execute variables), the array of registers, and all words in memory
 * 3. process each type of the 4 instructions with its separate function
 *
 * */
