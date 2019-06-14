#include "loader.h"
#include "preprocessing.h"
#include <math.h>
#include <assert.h>

// TODO FREE INPUT ARRAY

// creates fake data for testing
double **makeFakeDataArray(void) {
    double **data = (double **) calloc(NUM_LINES, sizeof(double *));
    if (!data) {
        perror("make_data data");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < NUM_LINES; i++) {
        data[i] = calloc(NUM_INPUTS + NUM_OUTPUTS, sizeof(double));
        if (!data[i]) {
            perror("make_data d1");
            exit(EXIT_FAILURE);
        }
    }
    data[0][0] = 0;
    data[0][1] = 1;
    data[0][2] = 2;
    data[0][3] = 3;
    data[0][4] = 4;
    data[0][5] = 5;
    data[0][6] = 6;

    data[1][0] = 100;
    data[1][1] = 200;
    data[1][2] = 300;
    data[1][3] = 35;
    data[1][4] = 40;
    data[1][5] = 50;
    data[1][6] = 500;

    return data;
}
// number is between 0 and 1 to show confidence that price will be in a given range
// training data should say 0 or 1 depending on if the resulting price is in the range for this NN
// outputs: range [0-100] [100-200] [200-300]

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


// prints mappings
void printMappings(dataMapping_t *dataMapping, int numOfEntries) {
    for (int i = 0; i < numOfEntries; i++) {
        for (int j = 0; j < numOfEntries; ++j) {
            printf("%f ", dataMapping[i].inputs[j]);
        }
        printf("%f \n", dataMapping[i].expectedOutput);
    }
}


dataMapping_t *makeFakeData(void) {
  double **dataArray = makeFakeDataArray();
  return process(dataArray, NUM_LINES);
}