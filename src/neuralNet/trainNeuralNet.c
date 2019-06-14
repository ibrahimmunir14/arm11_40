#include "neuralNet.h"
#include "neuralNet/loader.h"
#include "neuralNet/preprocessing.h"

#define LEARNING_RATE 1
#define EPOCHS 500
#define LAYERS 4
#define FRONT_PADDING 30
#define END_PADDING 30
#define TRAINING_PERCENT 0.7

// TODO: FREE EVERYTHING
// TODO: SORT SOURCE AND HEADER FILES INTO DIRECTORIES

int main() {
  srand(20);

  // initialize inputs and targets;
  double **dataArray = calloc(MAX_NUMBER_OF_DAYS, sizeof(double*));
  int numTotal = inputCreator(dataArray);

  int numTraining = (int) ((numTotal - FRONT_PADDING - END_PADDING) * TRAINING_PERCENT);
  int numTesting = numTotal - numTraining - FRONT_PADDING - END_PADDING;
  dataMapping_t *allData = process(dataArray, numTotal);
  dataMapping_t *trainingData = allData + FRONT_PADDING;
  dataMapping_t *testingData = &trainingData[numTraining];

  int numOfHiddenNeurons = (NUM_INPUTS + NUM_OUTPUTS) / 2;
  int layer_outputs[] = {NUM_INPUTS, numOfHiddenNeurons, numOfHiddenNeurons,  NUM_OUTPUTS};

  neuralNet_t *neuralNet = createNeuralNet(LAYERS, layer_outputs);
  if (!neuralNet) {
    printf("Error: Couldn't create the neural network.\n");
    return EXIT_FAILURE;
  }

  // training the neural network
  for (int epoch = 0; epoch < EPOCHS; epoch++) {
    for (int i = 0; i < numTraining; i++) {
      trainNeuralNet(neuralNet,
                     LEARNING_RATE,
                     trainingData[i].inputsNormalised,
                     &trainingData[i].expectedOutputNormalised);
    }

    // testing the neural network
    printf("\nTESTING EPOCH %d: \n", epoch + 1);

    double differencesSum = 0;
    double numAccurate = 0;

    for (int testNum = 0; testNum < numTesting; testNum++) {
      computeOutputs(neuralNet, testingData[testNum].inputsNormalised);
      double predictedPrice = denormalise(getOutput(neuralNet), STOCK_MIN, STOCK_MAX);

      differencesSum += (testingData[testNum].expectedOutput - predictedPrice) * (testingData[testNum].expectedOutput - predictedPrice);

      double percentDiff = fabs(testingData[testNum].expectedOutput - predictedPrice) / testingData[testNum].expectedOutput;
      if (percentDiff < 0.03) {
        numAccurate++;
      }

//      printf("Test %d: Actual Price: %f,  Predicted Price: %f,  Percent Diff: %f\n",
//             testNum, testingData[testNum].expectedOutput, predictedPrice, percentDiff);
    }

    printf("Percentage of tests <3 percent difference: %f\n", numAccurate / numTesting);
    printf("Mean Squared Error (the smaller, the better): %f\n", differencesSum / numTesting);
  }

  freeNeuralNet(neuralNet);
  freeDataMappings(allData, numTotal);
  //free2dArray(dataArray);

  return EXIT_SUCCESS;
}
