#include "tests.h"

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


