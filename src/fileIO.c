#include "fileIO.h"
#include "binaryOps.h"
#include <stdlib.h>
#include <string.h>

int getFileSize(FILE *file) {
    int original = (int) ftell(file);

    fseek(file, 0, SEEK_END);
    int size = (int) ftell(file);

    fseek(file, 0, original);
    return size;
}

bool importBinaryFile(char *fileName, BYTE *memory) {
    FILE *file;
    if ((file = fopen(fileName, "rb")) == NULL) {
        return false;
    }
    int size = getFileSize(file);
    for (int i = 0; i < size; i++) {
        memory[i] = (BYTE) getc(file);
    }
    fclose(file);
    return true;
}

void binaryFileWriter(char *fileName, const WORD* instructions, const WORD* reserveMemory, int numInstructions, int numReserve) {
    FILE * fPointer;
    fPointer = fopen(fileName, "wb");
    printf("\n\n*** Output: Instructions ***\n");
    for (int i = 0; i < numInstructions; i ++) {
        WORD n = instructions[i];
        BYTE bytes[4];
        bytes[3] = (n >> 24u) & FULLBITS(8);
        bytes[2] = (n >> 16u) & FULLBITS(8);
        bytes[1] = (n >> 8u) & FULLBITS(8);
        bytes[0] = n & FULLBITS(8);
        fwrite(bytes, sizeof(bytes), 1, fPointer);
        if (ferror(fPointer)) {
            perror("Error writing to binary output file.");
        }
        printf("'%02x%02x%02x%02x'\n", bytes[0], bytes[1], bytes[2], bytes[3]);
    }
    printf("\n*** Output: ReserveMemory ***\n");
    for (int i = 0; i < numReserve; i ++) {
        WORD n = reserveMemory[i];
        //printf("'%08x'\n", n);
        BYTE bytes[4];
        bytes[3] = (n >> 24u) & FULLBITS(8);
        bytes[2] = (n >> 16u) & FULLBITS(8);
        bytes[1] = (n >> 8u) & FULLBITS(8);
        bytes[0] = n & FULLBITS(8);
        fwrite(bytes, sizeof(bytes), 1, fPointer);
        if (ferror(fPointer)) {
            perror("Error writing to binary output file.");
        }
        printf("'%02x%02x%02x%02x'\n", bytes[0], bytes[1], bytes[2], bytes[3]);
    }

    fclose(fPointer);
}

char** importAsciiFile(char *fileName, int *numLines) {
    FILE *file;
    if ((file = fopen(fileName, "r")) == NULL) {
        perror("Error: could not open input file.");
        return false;
    }

    char** tempContents = (char **) calloc(MAX_LINES_ASCII, sizeof(char*));
    int i = 0;
    for (; i < MAX_LINES_ASCII; i++) {
        if (feof(file)) {
            break;
        }
        tempContents[i] = (char *) calloc(MAX_LINE_LENGTH, sizeof(char));
        fgets(tempContents[i], MAX_LINE_LENGTH, file);
    }
    fclose(file);
    *numLines = i - 1;

    char** contents = (char **) calloc(*numLines, sizeof(char*));
    for (int i = 0; i < *numLines; i++) {
        contents[i] = tempContents[i];
    }
    free(tempContents);

    return contents;
}

