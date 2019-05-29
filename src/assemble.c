#include "assemble.h"

int main(int argc, char **argv) {
  return EXIT_SUCCESS;
  // TODO function to get the number of lines in the input file
}

void binaryFileWriter(WORD* instructions) {
    FILE * fPointer;
    // To create a new file use wb, to append to a file use ab
    fPointer = fopen("test.bin", "wb");
    int size = sizeof(instructions) / sizeof(WORD);
    for (int i = 0; i < size; i ++) {
        unsigned int n = instructions[i];
        unsigned char bytes[4];
        bytes[0] = (n >> 24) & 0xFF;
        bytes[1] = (n >> 16) & 0xFF;
        bytes[2] = (n >> 8) & 0xFF;
        bytes[3] = n & 0xFF;
        fwrite(bytes, sizeof(bytes), 1, fPointer);
        fwrite("\n", sizeof(char), 1, fPointer);
    }
    fclose(fPointer);
}
