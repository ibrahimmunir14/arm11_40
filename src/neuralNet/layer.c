#include "layer.h"
#include "preprocessing.h"

double sigmoid(double x) {
  return 1 / (1 + exp(-x));
}

double sigmoidprime(double x) {
  return x * (1 - x);
}

// creates a single layer
layer_t *layer_create() {
  layer_t *emptyLayer = malloc(sizeof(layer_t));

  if (emptyLayer == NULL) {
    return NULL;
  }

  emptyLayer->biases = NULL;
  emptyLayer->deltas = NULL;
  emptyLayer->next = NULL;
  emptyLayer->num_inputs = 0;
  emptyLayer->num_outputs = 0;
  emptyLayer->outputs = NULL;
  emptyLayer->prev = NULL;
  emptyLayer->weights = NULL;

  return emptyLayer;
}

// initialises the given layer
bool layer_init(layer_t *layer, int num_outputs, layer_t *prev) {
  assert(layer != NULL);
  layer->prev = prev;
  layer->num_outputs = num_outputs;
  layer->num_inputs = prev == NULL ? 0 : prev->num_outputs;

  if ((layer->outputs = calloc(num_outputs, sizeof(double))) == NULL
    || (layer->biases = calloc(num_outputs, sizeof(double))) == NULL
    || (layer->deltas = calloc(num_outputs, sizeof(double))) == NULL
    || (layer->weights = calloc(layer->num_inputs, sizeof(double *))) == NULL) {
    return true;
  }

  for (int i = 0; i < layer->num_inputs; i++) {
    layer->weights[i] = calloc(num_outputs, sizeof(double));

    if (layer->weights[i] == NULL) {
      return true;
    }

    for (int j = 0; j < num_outputs; j++) {
      layer->weights[i][j] = ANN_RANDOM();
    }
  }

  return false;
}

// frees a given layer
void layer_free(layer_t *layer) {
  assert(layer != NULL);
  free(layer->outputs);
  free(layer->biases);
  free(layer->deltas);

  for (int i = 0; i < layer->num_inputs; i++) {
    free(layer->weights[i]);
  }

  free(layer->weights);
  free(layer);
}

// computes the outputs of the current layer
void layer_compute_outputs(layer_t const *layer) {
  assert(layer != NULL);
  assert(layer->prev != NULL);

  for (int j = 0; j < layer->num_outputs; j++) {
    double sum = 0;

    for (int i = 0; i < layer->num_inputs; i++) {
      sum += layer->weights[i][j] * layer->prev->outputs[i];
    }

    sum += layer->biases[j];

    layer->outputs[j] = sigmoid(sum);
  }
}

void layer_compute_deltas(layer_t const *layer) {
  for (int i = 0; i < layer->num_outputs; i++) {
    layer->deltas[i] = sigmoidprime(layer->outputs[i]);

    double sum = 0;
    for (int j = 0; j < layer->next->num_outputs; j++) {
      sum += layer->next->weights[i][j] * layer->next->deltas[j];
    }

    layer->deltas[i] *= sum;
  }
}

void layer_update(layer_t const *layer, double l_rate) {
  for (int j = 0; j < layer->num_outputs; j++) {
    for (int i = 0; i < layer->num_inputs; i++) {
      layer->weights[i][j] = l_rate * layer->prev->outputs[i] * layer->deltas[j] + layer->weights[i][j];
    }

    layer->biases[j] += l_rate * layer->deltas[j];
  }
}