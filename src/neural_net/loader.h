#ifndef C_PROJECT_LOADER_H
#define C_PROJECT_LOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_INPUTS 6
#define NUM_OUTPUTS 1

#define NUM_LINES 2

#define STOCK_MIN 0
#define STOCK_MAX 200

typedef struct dataMapping {
  double max;
  double min;
  double *inputs;
  double *inputsNormalised;
  double expectedOutput;
  double expectedOutputNormalised;
} dataMapping_t;

double **makeFakeDataArray(void);
dataMapping_t *process(double **dataArrays, int numOfEntries);
void printMappings(dataMapping_t *dataMapping, int numOfEntries);
dataMapping_t *makeFakeData(void);
double getMin(double *values, int numValues);
double getMax(double *values, int numValues);
double *normalise(double *values, double min, double max, int num_values);
double getAvg(double *values, int numVals);
double denormalise(double value, double min, double max);
#endif //C_PROJECT_LOADER_H
