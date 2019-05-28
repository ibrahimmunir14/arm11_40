#include "binaryFileLoader.h"

int getFileSize(FILE *file) {
    int original = (int) ftell(file);

    fseek(file, 0, SEEK_END);
    int size = (int) ftell(file);

    fseek(file, 0, original);
    return size;
}

void importFile(char *fileName, BYTE *memory) {
    FILE *file = fopen(fileName, "rb");

    int size = getFileSize(file);
    for (int i = 0; i < size; i++) {
        memory[i] = (BYTE) getc(file);
    }
}