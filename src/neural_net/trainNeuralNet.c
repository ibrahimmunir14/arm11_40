#include "neuralNet.h"
#include "neural_net/loader.h"
#include "neural_net/preprocessing.h"

// TODO Normalize inputs and denormalize outputs

int main() {
    // initializes random number generator
    srand(42);

    // initialize inputs and targets;
    dataMapping_t *testingData = makeFakeData();

    int numOfHiddenNeurons = (NUM_INPUTS + NUM_OUTPUTS) / 2;
    int layer_outputs[] = {NUM_INPUTS, numOfHiddenNeurons, NUM_OUTPUTS};

    neural_net_t *neural_net = create_neural_net(3, layer_outputs);
    if (!neural_net) {
        printf("Error: Couldn't create the neural network.\n");
        return EXIT_FAILURE;
    }

    printf("Current random outputs of the network:\n");
    for (int i = 0; i < testingData[0].numEntries; ++i) {
        forward_run(neural_net, testingData[i].inputs);
        printf("  [%f, %f, %f, %f, %f, %f]. Expected: %f,  Actual: %f\n",
               testingData[i].inputs[0],
               testingData[i].inputs[1],
               testingData[i].inputs[2],
               testingData[i].inputs[3],
               testingData[i].inputs[4],
               testingData[i].inputs[5],
               testingData[i].expectedOutput,
               neural_net->output_layer->outputs[0]);
    }

    printf("\nTraining the network...\n");
    for (int i = 0; i < 25000; ++i) {
        /* This is an epoch, running through the entire data. */
        for (int j = 0; j < testingData[0].numEntries; ++j) {
            /* Training at batch size 1, ie updating weights after every data point. */
            train_neural_net(neural_net, 1.0, testingData[j].inputs, &testingData[j].expectedOutput);
        }
    }

    printf("Final outputs of the network:\n");
    for (int i = 0; i < testingData[0].numEntries; ++i) {
        forward_run(neural_net, testingData[i].inputs);

        printf("  [%f, %f, %f, %f, %f, %f]. Expected: %f,  Actual: %f\n", testingData[i].inputs[0],
               testingData[i].inputs[1],
               testingData[i].inputs[2],
               testingData[i].inputs[3],
               testingData[i].inputs[4],
               testingData[i].inputs[5],
               testingData[i].expectedOutput,
               inverseNormalise(neural_net->output_layer->outputs[0], testingData[i].range, testingData[i].avg));
    }

    double *test_inputs = calloc(NUM_INPUTS, sizeof(double));
    test_inputs[0] = 20;
    test_inputs[1] = 30;
    test_inputs[2] = 40;
    test_inputs[3] = 50;
    test_inputs[4] = 60;
    test_inputs[5] = 70;
    test_inputs[6] = 0;
    dataMapping_t *test_data = process(&test_inputs, 1);
    forward_run(neural_net, test_inputs);

    printf("  [%f, %f, %f, %f, %f, %f]. Actual: %f\n",
           test_inputs[0],
           test_inputs[1],
           test_inputs[2],
           test_inputs[3],
           test_inputs[4],
           test_inputs[5],
           inverseNormalise(neural_net->output_layer->outputs[0], test_data->range, test_data->avg));

    free_neural_net(neural_net);

    return EXIT_SUCCESS;
}
