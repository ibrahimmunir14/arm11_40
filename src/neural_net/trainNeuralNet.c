#include "neuralNet.h"
#include "neural_net/loader.h"

#define LAYERS 7

int main() {
  // initializes random number generator
  srand(42);

  // initialize inputs and targets;
  dataMapping_t *testingData = makeFakeData();


//  layer_t *first_l = layer_create();
//  if (!first_l) {
//    printf("Couldn't create the first layer.\n");
//    return EXIT_FAILURE;
//  }
//  if (layer_init(first_l, 2, NULL)) {
//    printf("Couldn't layer_init first layer.\n");
//    return EXIT_FAILURE;
//  }
//
//  layer_t *second_l = layer_create();
//  if (!second_l) {
//    return EXIT_FAILURE;
//  }
//  if (layer_init(second_l, 1, first_l)) {
//    printf("Couldn't layer_init second layer.\n");
//    return EXIT_FAILURE;
//  }
//
//  layer_compute_outputs(second_l);
//  layer_free(second_l);
//  layer_free(first_l);

  int numOfHiddenNeurons = testingData->numOfInputs + testingData->numOfOutputs / 2;
  int layer_outputs[] = {testingData->numOfEntries, numOfHiddenNeurons, 1};

  neural_net_t *neural_net = create_neural_net(3, layer_outputs);
  if (!neural_net) {
    printf("Error: Couldn't create the neural network.\n");
    return EXIT_FAILURE;
  }

  /* Print hidden layer weights, biases and outputs. */
  printf("The current state of the hidden layer:\n");
  for (int i = 0; i < layer_outputs[0]; ++i) {
    for (int j = 0; j < layer_outputs[1]; ++j)
      printf("  weights[%i][%i]: %f\n", i, j, neural_net->input_layer->next->weights[i][j]);
  }
  for (int i = 0; i < layer_outputs[1]; ++i)
    printf("  biases[%i]: %f\n", i, neural_net->input_layer->next->biases[i]);
  for (int i = 0; i < layer_outputs[1]; ++i)
    printf("  outputs[%i]: %f\n", i, neural_net->input_layer->next->outputs[i]);

  /* Dummy run to see random network output. */
  printf("Current random outputs of the network:\n");
  for (int i = 0; i < 4; ++i) {
    forward_run(neural_net, testingData->inputs[i]);
    printf("  [%1.f, %1.f] -> %f\n", testingData->inputs[i][0], testingData->inputs[i][1], neural_net->output_layer->outputs[0]);
  }

  /* Train the network. */
  printf("\nTraining the network...\n");
  for (int i = 0; i < 25000; ++i) {
    /* This is an epoch, running through the entire data. */
    for (int j = 0; j < 4; ++j) {
      /* Training at batch size 1, ie updating weights after every data point. */
      train_neural_net(neural_net, 1.0, testingData->inputs[j], testingData->expectedOutputs + j);
    }
  }

  /* Print hidden layer weights, biases and outputs. */
  printf("The current state of the hidden layer:\n");
  for (int i = 0; i < layer_outputs[0]; ++i) {
    for (int j = 0; j < layer_outputs[1]; ++j)
      printf("  weights[%i][%i]: %f\n", i, j, neural_net->input_layer->next->weights[i][j]);
  }
  for (int i = 0; i < layer_outputs[1]; ++i)
    printf("  biases[%i]: %f\n", i, neural_net->input_layer->next->biases[i]);
  for (int i = 0; i < layer_outputs[1]; ++i)
    printf("  outputs[%i]: %f\n", i, neural_net->input_layer->next->outputs[i]);

  /* Let's see the results. */
  printf("\nAfter training magic happened the outputs are:\n");
  for (int i = 0; i < 4; ++i) {
    forward_run(neural_net, testingData->inputs[i]);
    printf("  [%1.f, %1.f] -> %f\n", testingData->inputs[i][0], testingData->inputs[i][1], neural_net->output_layer->outputs[0]);
  }

  /* Time to clean up. */
  free_neural_net(neural_net);

  return EXIT_SUCCESS;
}
