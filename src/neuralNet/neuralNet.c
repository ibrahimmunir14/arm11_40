#include "neuralNet.h"

/* Creates and returns a new neuralNet. */
neuralNet_t *createNeuralNet(int numLayers, int *outputs) {
  assert(outputs != NULL);

  neuralNet_t *newNeuralNet = malloc(sizeof(neuralNet_t));

  if (newNeuralNet == NULL) {
    return NULL;
  }

  layer_t *layers[numLayers];

  for (int i = 0; i < numLayers; i++) {
    layers[i] = layer_create();

    if (layers[i] == NULL) {
      return NULL;
    }

    //CHECK THIS FOR NULL
    layer_init(layers[i], outputs[i], i == 0 ? NULL : layers[i - 1]);
  }

  for (int i = 0; i < numLayers; i++) {
    layers[i]->next = i == numLayers - 1 ? NULL : layers[i + 1];
  }

  newNeuralNet->inputLayer = layers[0];
  newNeuralNet->outputLayer = layers[numLayers - 1];

  return newNeuralNet;
}

/* Frees the space allocated to neuralNet. */
void freeNeuralNet(neuralNet_t *neuralNet) {
  assert(neuralNet != NULL);

  layer_t *currentLayer = neuralNet->inputLayer;

  do {
    layer_t *nextLayer = currentLayer->next;
    layer_free(currentLayer);
    currentLayer = nextLayer;
  } while (currentLayer != NULL);
  free(neuralNet);
}

/* Forward run of given neuralNet with inputs. */
void computeOutputs(neuralNet_t const *neuralNet, double const *inputs) {
  assert(neuralNet != NULL);
  assert(inputs != NULL);

  for (int i = 0; i < neuralNet->inputLayer->num_outputs; i++) {
    neuralNet->inputLayer->outputs[i] = inputs[i];
  }

  layer_t *currentLayer = neuralNet->inputLayer->next;

  while (currentLayer != NULL) {
    layer_compute_outputs(currentLayer);
    currentLayer = currentLayer->next;
  }
}

/* Trains the neuralNet with single backprop update. */
void trainNeuralNet(neuralNet_t const *neuralNet, double learningRate, double const *inputs, double const *targets) {
  assert(neuralNet != NULL);
  assert(inputs != NULL);
  assert(targets != NULL);
  assert(learningRate > 0);

  /* Run forward pass. */
  computeOutputs(neuralNet, inputs);

  for (int j = 0; j < neuralNet->outputLayer->num_outputs; j++) {
    neuralNet->outputLayer->deltas[j] = sigmoidprime(neuralNet->outputLayer->outputs[j]) * (targets[j] - neuralNet->outputLayer->outputs[j]);
  }

  layer_t *currentLayer = neuralNet->outputLayer->prev;
  while (currentLayer != neuralNet->inputLayer) {
    layer_compute_deltas(currentLayer);
    currentLayer = currentLayer->prev;
  }

  while (currentLayer != NULL) {
    layer_update(currentLayer, learningRate);
    currentLayer = currentLayer->next;
  }
}

double getOutput(const neuralNet_t *neural_net) {
  return neural_net->outputLayer->outputs[0];
}