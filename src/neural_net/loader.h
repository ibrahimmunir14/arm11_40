#ifndef C_PROJECT_LOADER_H
#define C_PROJECT_LOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_INPUTS 6
#define NUM_OUTPUTS 1

#define NUM_LINES 2

typedef struct dataMapping {
  int numEntries;
  double avg;
  double range;
  double *inputs;
  double expectedOutput;
} dataMapping_t;

double **makeFakeDataArray(void);
dataMapping_t *process(double **dataArrays, int numOfEntries);
void printMappings(dataMapping_t *dataMapping, int numOfEntries);
dataMapping_t *makeFakeData(void);
double getMin(double *values, int numValues);
double getMax(double *values, int numValues);
double *normalise(double *values, double range, double avg, double num_values);
double getAvg(double *values, int numVals);
double inverseNormalise(double value, double range, double avg);
dataMapping_t *makeData(void);
#endif //C_PROJECT_LOADER_H
