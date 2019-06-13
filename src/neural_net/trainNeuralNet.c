#include "neuralNet.h"
#include "neural_net/loader.h"
#include "neural_net/preprocessing.h"

// TODO Normalize inputs and denormalize outputs

int main() {
  input_creator();
  return 0;
  // initializes random number generator
  srand(42);

  // initialize inputs and targets;
  dataMapping_t *testingData = makeFakeData();

  int numOfHiddenNeurons = (testingData->numOfInputs + testingData->numOfOutputs) / 2;
  int layer_outputs[] = {testingData->numOfInputs, numOfHiddenNeurons, 1};

  neural_net_t *neural_net = create_neural_net(3, layer_outputs);
  if (!neural_net) {
    printf("Error: Couldn't create the neural network.\n");
    return EXIT_FAILURE;
  }

  printf("Current random outputs of the network:\n");
  for (int i = 0; i < testingData->numOfEntries; ++i) {
    forward_run(neural_net, testingData->inputs[i]);
    printf("  [%f, %f, %f, %f, %f, %f]. Expected: %f,  Actual: %f\n", testingData->inputs[i][0],
           testingData->inputs[i][1],
           testingData->inputs[i][2],
           testingData->inputs[i][3],
           testingData->inputs[i][4],
           testingData->inputs[i][5],
           testingData->expectedOutputs[i],
           neural_net->output_layer->outputs[0]);
  }

  printf("\nTraining the network...\n");
  for (int i = 0; i < 25000; ++i) {
    /* This is an epoch, running through the entire data. */
    for (int j = 0; j < testingData->numOfEntries; ++j) {
      /* Training at batch size 1, ie updating weights after every data point. */
      train_neural_net(neural_net, 1.0, testingData->inputs[j], testingData->expectedOutputs + j);
    }
  }

  printf("Final outputs of the network:\n");
  for (int i = 0; i < testingData->numOfEntries; ++i) {
    forward_run(neural_net, testingData->inputs[i]);
    printf("  [%f, %f, %f, %f, %f, %f]. Expected: %f,  Actual: %f\n", testingData->inputs[i][0],
        testingData->inputs[i][1],
        testingData->inputs[i][2],
        testingData->inputs[i][3],
        testingData->inputs[i][4],
        testingData->inputs[i][5],
        testingData->expectedOutputs[i],
        neural_net->output_layer->outputs[0]);
  }

  free_neural_net(neural_net);

  return EXIT_SUCCESS;
}
