#include "fileIO.h"
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

void binaryFileWriter(char *fileName, WORD* instructions) {
    FILE * fPointer;
    // To create a new file use wb, to append to a file use ab
    fPointer = fopen(fileName, "wb");
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

