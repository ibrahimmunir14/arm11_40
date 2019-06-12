#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// TODO parse_csv
// TODO free_array
// TODO PUT THESE INTO OTHER FILES

#define NUMBER_OF_DAYS (2000)
#define NUMBER_OF_DOUBLES (6)
#define NUMBER_OF_INPUTS (6)
#define LINE_SIZE (80)
#define DATE_SIZE (10)
#define ROC_PERIODS (12)
#define SHORT_EMA_PERIOD (12)
#define LONG_EMA_PERIOD (26)
#define RSI_PERIOD (14)

void free_array(char *array);
void parse_csv(char **dates, double *volumes, double *prices);
double log_return(int index, double *prices);
double rsi(int index, double *prices);
double macd(int index, double *shortEMAs, double *longEMAs);
double roc(int index, double *prices);
void calculate_ema(int index, double *prices, double *EMAs, int timePeriod);
void calculate_log_return_ema(int index, double *prices, double *EMAs, int timePeriod);
void create_one_input_entry(int index, double **inputs, double *prices, double *volumes, double *shortEMAs,
                            double *longEMAs, double *logEMAs);


int input_creator(void) {
  // allocate date array
  // allocate double array
  // allocate expected price array
  // allocate neural network inputs array
  // allocate EMA arrays
  char **dates = calloc(NUMBER_OF_DAYS, sizeof(char *));
  double **doubles = calloc(NUMBER_OF_DAYS, sizeof(double *));
  double *prices = calloc(NUMBER_OF_DAYS, sizeof(double));
  double *volumes = calloc(NUMBER_OF_DAYS, sizeof(double));
  double **inputs = calloc(NUMBER_OF_DAYS, sizeof(double *));
  double *shortEMAs = calloc(NUMBER_OF_DAYS, sizeof(double));
  double *longEMAs = calloc(NUMBER_OF_DAYS, sizeof(double));
  double *logEMAs = calloc(NUMBER_OF_DAYS, sizeof(double));
  for (int i = 0; i < NUMBER_OF_DAYS; i ++) {
    dates[i] = calloc(DATE_SIZE, sizeof(char));
    doubles[i] = calloc(NUMBER_OF_DOUBLES, sizeof(double));
    inputs[i] = calloc(NUMBER_OF_INPUTS, sizeof(double));
  }

  // call parse_csv with pointer to all arrays
  parse_csv(dates, doubles, prices);

  // create 2d input array
  for (int i = 0; i < NUMBER_OF_DAYS; i ++) {
    create_one_input_entry(i, inputs, prices, volumes, shortEMAs, longEMAs, logEMAs);
  }

  // free all memory


  return 0;
}

void parse_csv(char **dates, double *volumes, double *prices) {

}

void create_one_input_entry(int index, double **inputs, double *prices, double *volumes, double *shortEMAs,
                            double *longEMAs, double *logEMAs) {
  /* 6 values in array are:
   * 1. log return
   * 2. volume
   * 3. EMA of log return
   * 4. RSI
   * 5. MACD
   * 6. ROC
   *
   * */
  calculate_log_return_ema(index, prices, logEMAs, SHORT_EMA_PERIOD);
  calculate_ema(index, prices, shortEMAs, SHORT_EMA_PERIOD);
  calculate_ema(index, prices, longEMAs, LONG_EMA_PERIOD);
  inputs[index][0] = log_return(index, prices);
  inputs[index][1] = volumes[index];
  inputs[index][2] = logEMAs[index];
  inputs[index][3] = rsi(index, prices);
  inputs[index][4] = macd(index, shortEMAs, longEMAs);
  inputs[index][5] = roc(index, prices);
}

double log_return(int index, double *prices) {
  if (index > 0 && index < NUMBER_OF_DAYS) {
    double ret = prices[index] / prices[index - 1];
    return log(ret);
  } else {
    printf("Out of bounds of array in roc function\n");
    return 0;
  }
}

double rsi(int index, double *prices) {
  if (index > RSI_PERIOD && index < NUMBER_OF_DAYS) {
    double sumGain = 0;
    double sumLoss = 0;
    for (int i = index - RSI_PERIOD; i < index; i++)
    {
      double difference = prices[i] - prices[i - 1];
      if (difference >= 0) {
        sumGain += difference;
      } else {
        sumLoss -= difference;
      }
    }
    if (sumGain == 0) return 0;
//    if (Math.Abs(sumLoss) < Tolerance) return 100;

    double relativeStrength = sumGain / sumLoss;
    return 100.0 - (100.0 / (1 + relativeStrength));
  } else {
    printf("Out of bounds of array in rsi function\n");
    return 0;
  }
}

void calculate_log_return_ema(int index, double *prices, double *EMAs, int timePeriod) {
  if (index > 0 && index < NUMBER_OF_DAYS) {
    double k = 2.0f / (timePeriod + 1);
    EMAs[index] = log_return(index, prices) * k + EMAs[index - 1] * (1 - k);
  } else {
    printf("Out of bounds of array in calculate_log_return_ema_ema function\n");
    EMAs[index] = 0;
  }
}

void calculate_ema(int index, double *prices, double *EMAs, int timePeriod) {
  if (index > 0 && index < NUMBER_OF_DAYS) {
    double k = 2.0f / (timePeriod + 1);
    EMAs[index] = prices[index] * k + EMAs[index - 1] * (1 - k);
  } else {
    printf("Out of bounds of array in calculate_ema function\n");
    EMAs[index] = 0;
  }
}

double macd(int index, double *shortEMAs, double *longEMAs) {
  if (index >= 0 && index < NUMBER_OF_DAYS) {
    return shortEMAs[index] - longEMAs[index];
  } else {
    printf("Out of bounds of array in macd function\n");
    return 0;
  }
}

double roc(int index, double *prices) {
  // ( (close - close n periods ago) / close n periods ago )  * 100
  if (index >= ROC_PERIODS && index < NUMBER_OF_DAYS) {
    double ret = ((prices[index] - prices[index - ROC_PERIODS]) / prices[index - ROC_PERIODS]) * 100;
    return ret;
  } else {
    printf("Out of bounds of array in roc function\n");
    return 0;
  }
}