#include "neuralNet.h"
#include "neural_net/loader.h"

#define LAYERS 7

int main() {
  // initializes random number generator
  srand(42);

  // initialize inputs and targets;
  dataMapping_t *testingData = makeFakeData();

  int numOfHiddenNeurons = (testingData->numOfInputs + testingData->numOfOutputs) / 2;
  int numOfInputs = testingData->numOfInputs;
  int numOfOutputs = testingData->numOfOutputs;
  int layer_outputs[] = {testingData->numOfInputs, numOfHiddenNeurons, 1};

  neural_net_t *neural_net = create_neural_net(3, layer_outputs);
  if (!neural_net) {
    printf("Error: Couldn't create the neural network.\n");
    return EXIT_FAILURE;
  }

//  /* Print hidden layer weights, biases and outputs. */
//  printf("The current state of the hidden layer:\n");
//  for (int i = 0; i < numOfInputs; ++i) {
//    for (int j = 0; j < layer_outputs[1]; ++j)
//      printf("  weights[%i][%i]: %f\n", i, j, neural_net->input_layer->next->weights[i][j]);
//  }
//  for (int i = 0; i < layer_outputs[1]; ++i)
//    printf("  biases[%i]: %f\n", i, neural_net->input_layer->next->biases[i]);
//  for (int i = 0; i < layer_outputs[1]; ++i)
//    printf("  outputs[%i]: %f\n", i, neural_net->input_layer->next->outputs[i]);


  /* Dummy run to see random network output. */
  printf("Current random outputs of the network:\n");
  for (int i = 0; i < testingData->numOfEntries; ++i) {
    forward_run(neural_net, testingData->inputs[i]);
    printf("  [%1.f, %1.f] -> %f\n", testingData->inputs[i][0], testingData->inputs[i][1], neural_net->output_layer->outputs[0]);
  }

  /* Train the network. */
  printf("\nTraining the network...\n");
  for (int i = 0; i < 25000; ++i) {
    /* This is an epoch, running through the entire data. */
    for (int j = 0; j < testingData->numOfEntries; ++j) {
      /* Training at batch size 1, ie updating weights after every data point. */
      train_neural_net(neural_net, 1.0, testingData->inputs[j], testingData->expectedOutputs + j);
    }
  }

  /* Print hidden layer weights, biases and outputs. */
  for (int i = 0; i < layer_outputs[0]; ++i) {
    for (int j = 0; j < layer_outputs[1]; ++j)
      printf("  weights[%i][%i]: %f\n", i, j, neural_net->input_layer->next->weights[i][j]);
  }
  for (int i = 0; i < layer_outputs[1]; ++i)
    printf("  biases[%i]: %f\n", i, neural_net->input_layer->next->biases[i]);
  for (int i = 0; i < layer_outputs[1]; ++i)
    printf("  outputs[%i]: %f\n", i, neural_net->input_layer->next->outputs[i]);

  for (int i = 0; i < testingData->numOfEntries; ++i) {
    forward_run(neural_net, testingData->inputs[i]);
    printf("  [%f, %f, %f, %f, %f, %f] -> %f\n", testingData->inputs[i][0],
        testingData->inputs[i][1],
        testingData->inputs[i][2],
        testingData->inputs[i][3],
        testingData->inputs[i][4],
        testingData->inputs[i][5],
        neural_net->output_layer->outputs[0]);
  }

  free_neural_net(neural_net);

  return EXIT_SUCCESS;
}
