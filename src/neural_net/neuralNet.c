#include "neuralNet.h"

/* Creates and returns a new neuralNet. */
neural_net_t *create_neural_net(int num_layers, int *layer_outputs) {
  assert(layer_outputs != NULL);

  neural_net_t *new_neural_net = malloc(sizeof(neural_net_t));

  if (new_neural_net == NULL) {
    return NULL;
  }

  layer_t *layers[num_layers];

  for (int i = 0; i < num_layers; i++) {
    layers[i] = layer_create();

    if (layers[i] == NULL) {
      return NULL;
    }

    //CHECK THIS FOR NULL
    layer_init(layers[i], layer_outputs[i], i == 0 ? NULL : layers[i - 1]);
  }

  for (int i = 0; i < num_layers; i++) {
    layers[i]->next = i == num_layers - 1 ? NULL : layers[i + 1];
  }

  new_neural_net->input_layer = layers[0];
  new_neural_net->output_layer = layers[num_layers - 1];

  return new_neural_net;
}

/* Frees the space allocated to neuralNet. */
void free_neural_net(neural_net_t *neural_net) {
  assert(neural_net != NULL);

  layer_t *currentLayer = neural_net->input_layer;

  do {
    layer_t *nextLayer = currentLayer->next;
    layer_free(currentLayer);
    currentLayer = nextLayer;
  } while (currentLayer != NULL);

  free(neural_net);
}

/* Forward run of given neuralNet with inputs. */
void forward_run(neural_net_t const *neural_net, double const *inputs) {
  assert(neural_net != NULL);
  assert(inputs != NULL);

  for (int i = 0; i < neural_net->input_layer->num_outputs; i++) {
    neural_net->input_layer->outputs[i] = inputs[i];
  }

  layer_t *currentLayer = neural_net->input_layer->next;

  while (currentLayer != NULL) {
    layer_compute_outputs(currentLayer);
    currentLayer = currentLayer->next;
  }

}

/* Trains the neuralNet with single backprop update. */
void train_neural_net(neural_net_t const *neural_net, double learning_rate, double const *inputs, double const *targets) {
  assert(neural_net != NULL);
  assert(inputs != NULL);
  assert(targets != NULL);
  assert(learning_rate > 0);

  /* Run forward pass. */
  forward_run(neural_net, inputs);

  for (int j = 0; j < neural_net->output_layer->num_outputs; j++) {
    neural_net->output_layer->deltas[j] = sigmoidprime(neural_net->output_layer->outputs[j]) * (targets[j] - neural_net->output_layer->outputs[j]);
  }

  layer_t *currentLayer = neural_net->output_layer->prev;
  while (currentLayer != neural_net->input_layer) {
    layer_compute_deltas(currentLayer);
    currentLayer = currentLayer->prev;
  }

  while (currentLayer != NULL) {
    layer_update(currentLayer, learning_rate);
    currentLayer = currentLayer->next;
  }
}

double getOutput(const neural_net_t *neural_net) {
  return neural_net->output_layer->outputs[0];
}