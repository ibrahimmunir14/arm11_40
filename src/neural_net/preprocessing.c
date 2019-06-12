#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define NUMBER_OF_DAYS (2000)
#define NUMBER_OF_DOUBLES (6)
#define NUMBER_OF_INPUTS (6)
#define LINE_SIZE (80)
#define DATE_SIZE (10)
#define ROC_PERIODS (12)

void free_arrays(char **dates, double **doubles, double *prices);
void parse_csv(char **dates, double **doubles, double *prices);
double log_return(int index, double **doubles);
double ema_log_return(int index, double **doubles);
double rsi(int index, double *prices);
double macd(int index, double shortEMA, double longEMA);
double roc(int index, double *prices);



int input_creator(void) {
  // allocate date array
  // allocate double array
  // allocate expected price array
  // allocated neural network inputs array
  char **dates = calloc(NUMBER_OF_DAYS, sizeof(char *));
  double **doubles = calloc(NUMBER_OF_DAYS, sizeof(double *));
  double *prices = calloc(NUMBER_OF_DAYS, sizeof(double));
  double **inputs = calloc(NUMBER_OF_DAYS, sizeof(double *));
  for (int i = 0; i < NUMBER_OF_DAYS; i ++) {
    dates[i] = calloc(DATE_SIZE, sizeof(char));
    doubles[i] = calloc(NUMBER_OF_DOUBLES, sizeof(double));
    inputs[i] = calloc(NUMBER_OF_INPUTS, sizeof(double));
  }

  // call parse_csv with pointer to all arrays

  // free all memory

  return 0;
}



double roc(int index, double *prices) {
  // ( (close - close n periods ago) / close n periods ago )  * 100
  if (index >= ROC_PERIODS) {
    double ret = ((prices[index] - prices[index - ROC_PERIODS]) / prices[index - ROC_PERIODS]) * 100;
    return ret;
  } else {
    return 0;
  }
}

