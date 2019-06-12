#ifndef C_PROJECT_LOADER_H
#define C_PROJECT_LOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_INPUTS 6
#define NUM_OUTPUTS 1

#define NUM_FAKE_LINES 2

typedef struct dataMapping {
  int numOfInputs;
  int numOfOutputs;
  int numOfEntries;
  double **inputs;
  double *expectedOutputs;
} dataMapping_t;

double **makeFakeDataArray(void);
dataMapping_t *process(double **dataArrays, int numOfEntries);
void printMappings(dataMapping_t *dataMapping);
dataMapping_t *makeFakeData(void);

#endif //C_PROJECT_LOADER_H
