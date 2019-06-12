
#include "loader.h"

// creates fake data for testing
double **makeFakeDataArray(void) {
    double **data = (double **) calloc(NUM_FAKE_LINES, sizeof(double *));
    if (!data) {
        perror("make_data data");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < NUM_FAKE_LINES; i++) {
        data[i] = calloc(NUM_INPUTS + NUM_OUTPUTS, sizeof(double));
        if (!data[i]) {
            perror("make_data d1");
            exit(EXIT_FAILURE);
        }
    }
    data[0] = {0, 1, 2, 3, 4, 5, 4};
    data[1] = {3, 2, 1, 35, 31, 21, 0};
    return data;
}

// processes array of data
dataMapping_t *process(double **dataArrays, int numOfEntries) {
    dataMapping_t *dataMappings = malloc(sizeof(dataMapping_t));
    dataMappings->inputs = calloc(numOfEntries, sizeof(double *));
    dataMappings->expectedOutputs = calloc(numOfEntries, sizeof(double));
    dataMappings->numOfEntries = numOfEntries;
    dataMappings->numOfInputs = NUM_INPUTS;
    dataMappings->numOfOutputs = NUM_OUTPUTS;

    for (int i = 0; i < numOfEntries; i++) {
        double *entryInputs = calloc(NUM_INPUTS, sizeof(double));
        for (int j = 0; j < NUM_INPUTS; j++) {
            entryInputs[j] = dataArrays[i][j];
        }

        double expectedOutput = dataArrays[i][NUM_INPUTS];
        dataMappings->inputs[i] = entryInputs;
        dataMappings->expectedOutputs[i] = expectedOutput;
    }

    return dataMappings;
}

// prints mappings
//void printMappings(dataMapping_t *dataMapping) {
//    for (int i = 0; i < NUM_LINES; i++) {
//        for (int j = 0; j < NUM_INPUTS; ++j) {
//            printf("%f ", dataMapping[i].inputs[j]);
//        }
//        printf("%f \n", dataMapping[i].expectedOutput);
//    }
//}


dataMapping_t *makeFakeData(void) {
  double **dataArray = makeFakeDataArray();
  return process(dataArray, NUM_FAKE_LINES);
}