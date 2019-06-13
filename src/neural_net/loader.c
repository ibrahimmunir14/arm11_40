#include "loader.h"
#include <math.h>
#include <assert.h>

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
dataMapping_t  *process(double **dataArrays, int numOfEntries) {
    dataMapping_t *dataMappings = calloc(numOfEntries, sizeof(dataMapping_t));

    for (int i = 0; i < numOfEntries; i++) {
        double *inputPre = calloc(NUM_INPUTS, sizeof(double));
        double expectedOutput = dataArrays[i][NUM_INPUTS];
        for (int j = 0; j < NUM_INPUTS; j++) {
            inputPre[j] = dataArrays[i][j];
        }
        dataMappings[i].numEntries = numOfEntries;

        dataMappings[i].avg = (getAvg(inputPre, NUM_INPUTS + NUM_OUTPUTS));
        dataMappings[i].range = getMax(inputPre, NUM_INPUTS) - getMin(inputPre, NUM_INPUTS);


        double *normalised = normalise(inputPre, dataMappings[i].range, dataMappings[i].avg, NUM_INPUTS);

        dataMappings[i].inputs = normalised;
        dataMappings[i].expectedOutput = (expectedOutput - dataMappings[i].avg) / dataMappings[i].range;
    }
    return dataMappings;
}

double inverseNormalise(double value, double range, double avg) {
    return (value * range) + avg;
}

double *normalise(double *values, double range, double avg, double num_values) {
    double *normalised = calloc(num_values, sizeof(double));
    for (int i = 0; i < num_values; i++) {
        normalised[i] = (values[i] - avg) / range;
    }
    return normalised;
}

//int main(void) {
//    dataMapping_t *test = makeFakeData();
//
//    for (int i = 0; i < test->numEntries; i++) {
//        printf("normalised inputs:\n");
//        for (int j = 0; j < NUM_INPUTS; j++) {
//            printf("input[%d][%d]: %f\n", i, j, test[i].inputs[j]);
//        }
//    }
//    double i = inverseNormalise(test[1].inputs[0], test[1].range, test[1].avg);
//    double i2 = inverseNormalise(test[1].inputs[1], test[1].range, test[1].avg);
//    double i3 = inverseNormalise(test[1].inputs[2], test[1].range, test[1].avg);
//    double i4 = inverseNormalise(test[1].inputs[3], test[1].range, test[1].avg);
//    double i5 = inverseNormalise(test[1].inputs[4], test[1].range, test[1].avg);
//
//    printf("inverse normalised:\n");
//    printf("%f\n", i);
//    printf("%f\n", i2);
//    printf("%f\n", i3);
//    printf("%f\n", i4);
//    printf("%f\n", i5);
//
//
//}

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