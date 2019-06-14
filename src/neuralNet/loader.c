#include "loader.h"

// TODO FREE INPUT ARRAY

// processes array of data into array of structs of data_mapping_t
dataMapping_t *process(double **dataArrays, int numOfEntries) {

  // array of data mappings
    dataMapping_t *dataMappings = calloc(numOfEntries, sizeof(dataMapping_t));

    for (int i = 0; i < numOfEntries; i++) {
      dataMappings[i].expectedOutput = dataArrays[i][NUM_INPUTS];

      dataMappings[i].inputs = calloc(NUM_INPUTS, sizeof(double));

      for (int j = 0; j < NUM_INPUTS; j++) {
        dataMappings[i].inputs[j] = dataArrays[i][j];
      }

      dataMappings[i].min = getMin(dataMappings[i].inputs, NUM_INPUTS);
      dataMappings[i].max = getMax(dataMappings[i].inputs, NUM_INPUTS);


      dataMappings[i].expectedOutputNormalised = normalise(dataMappings[i].expectedOutput, STOCK_MIN, STOCK_MAX);
      dataMappings[i].inputsNormalised = calloc(NUM_INPUTS, sizeof(double));
      normaliseValues(dataMappings[i].inputs, dataMappings[i].inputsNormalised, dataMappings[i].min, dataMappings[i].max, NUM_INPUTS);
    }
    return dataMappings;
}

void freeDataMapping(dataMapping_t mapping) {
  free(mapping.inputs);
  free(mapping.inputsNormalised);
}

void freeDataMappings(dataMapping_t *mappings, int numMappings) {
  for (int i = 0; i < numMappings; i++) {
    freeDataMapping(mappings[i]);
  }

  free(mappings);
}

double denormalise(double value, double min, double max) {
    return (value * (max - min)) + min;
}

double normalise(double value, double min, double max) {
  return (value - min) / (max - min);
}

void normaliseValues(double *originalValues, double *targetArray, double min, double max, int num_values) {
    for (int i = 0; i < num_values; i++) {
      targetArray[i] = normalise(originalValues[i], min, max);
    }
}

double getAvg(double *values, int numVals) {
    assert(values != NULL);
    double sum = 0;
    for (int i = 0; i < numVals; i++) {
        sum += values[i];
    }
    return sum/numVals;
}
double getMin(double *values, int numValues) {
    assert(values != NULL);
    double min = values[0];
    for (int i = 0; i < numValues; i++) {
        min = min < values[i] ? min : values[i];
    }
    return min;
}

double getMax(double *values, int numValues) {
    assert(values != NULL);
    double max = values[0];
    for (int i = 0; i < numValues; i++) {
        max = max > values[i] ? max : values[i];
    }
    return max;
}