#ifndef __ANN_H__
#define __ANN_H__

#include "neural_net/layer.h"

/* Represents a N layer artificial neural network. */
typedef struct neuralNet {
    /* The head and tail of layers doubly linked list. */
    layer_t *input_layer;
    layer_t *output_layer;
} neural_net_t;

/* Creates and returns a new neuralNet. */
neural_net_t *create_neural_net(int num_layers, int *layer_outputs);
/* Frees the space allocated to neuralNet. */
void free_neural_net(neural_net_t *neural_net);
/* Forward run of given neuralNet with inputs. */
void forward_run(neural_net_t const *neural_net, double const *inputs);
/* Trains the neuralNet with single backprop update. */
void train_neural_net(const neural_net_t *neural_net, double learning_rate, double const *inputs, double const *targets);

#endif
