#ifndef __ANN_H__
#define __ANN_H__

#include "neuralNet/layer.h"

/* Represents a N layer artificial neural network. */
typedef struct neuralNet {
    /* The head and tail of layers doubly linked list. */
    layer_t *inputLayer;
    layer_t *outputLayer;
} neuralNet_t;

/* Creates and returns a new neuralNet. */
neuralNet_t *createNeuralNet(int numLayers, int *outputs);
/* Frees the space allocated to neuralNet. */
void freeNeuralNet(neuralNet_t *neuralNet);
/* Forward run of given neuralNet with inputs. */
void computeOutputs(neuralNet_t const *neuralNet, double const *inputs);
/* Trains the neuralNet with single backprop update. */
void trainNeuralNet(const neuralNet_t *neuralNet, double learningRate, double const *inputs, double const *targets);

double getOutput(const neuralNet_t *neural_net);
#endif
