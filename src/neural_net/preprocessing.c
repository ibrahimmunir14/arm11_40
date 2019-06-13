#include "preprocessing.h"
#include <assert.h>
#include <malloc.h>

// TODO parse_csv
// TODO PUT THESE INTO OTHER FILES
// TODO allocate inputs and prices elsewhere

int numOfDays(int x) {
  static int numOfDays = MAX_NUMBER_OF_DAYS;
  if (x != 0) numOfDays = x;
  return numOfDays;
}

int input_creator(void) {
  double **inputs = calloc(MAX_NUMBER_OF_DAYS, sizeof(double*));
  double *prices = calloc(MAX_NUMBER_OF_DAYS, sizeof(double));
  char **dates = calloc(MAX_NUMBER_OF_DAYS, sizeof(char*));
  double *volumes = calloc(MAX_NUMBER_OF_DAYS, sizeof(double));
  double *shortEMAs = calloc(MAX_NUMBER_OF_DAYS, sizeof(double));
  double *longEMAs = calloc(MAX_NUMBER_OF_DAYS, sizeof(double));
  double *logEMAs = calloc(MAX_NUMBER_OF_DAYS, sizeof(double));
  if (!inputs || !prices || !volumes || !dates
  || !shortEMAs || !longEMAs || !logEMAs
  ) {
    perror("Calloc for arrays failed\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < MAX_NUMBER_OF_DAYS; i ++) {
    dates[i] = malloc(DATE_SIZE * sizeof(char));
    inputs[i] = calloc(NUMBER_OF_INPUTS, sizeof(double));
    if (!inputs[i] || !dates[i]) {
      perror("calloc/malloc for inner arrays failed\n");
      exit(EXIT_FAILURE);
    }
  }

  // call parse_csv with pointer to all arrays
  parse_csv(dates, volumes, prices);

  // create 2d input array
  for (int i = 0; i < numOfDays(0); i ++) {
    create_one_input_entry(i, inputs, prices, volumes, shortEMAs, longEMAs, logEMAs);
  }

  // realloc arrays
  logEMAs   = (double *)  realloc(logEMAs, (size_t) numOfDays(0)+1);
  prices    = (double *)  realloc(prices, (size_t) numOfDays(0)+1);
  volumes   = (double *)  realloc(volumes, (size_t) numOfDays(0)+1);
  shortEMAs = (double *)  realloc(shortEMAs, (size_t) numOfDays(0)+1);
  longEMAs  = (double *)  realloc(longEMAs, (size_t) numOfDays(0)+1);
  dates     = (char **)   reallocarray(dates, (size_t) numOfDays(0)+1, sizeof(char *));
  inputs    = (double **) reallocarray(inputs, (size_t) numOfDays(0)+1, sizeof(double *));
  if (!prices || !volumes || !shortEMAs || !longEMAs || !logEMAs || !dates || !inputs) {
    perror("realloc for 2darrays failed\n");
    exit(EXIT_FAILURE);
  }
  dates[numOfDays(0)] = NULL;
  inputs[numOfDays(0)] = NULL;

  // display inputs data
  for (int i = 0; i < numOfDays(0); i++) {
    for (int j = 0; j < NUMBER_OF_INPUTS; j++) {
      printf("%f      ", inputs[i][j]);
    }
    printf("\n");
  }

  printf("NUMNOFDAYS: %i", numOfDays(0));
  // free all memory
  free(prices);
  free(volumes);
  free(shortEMAs);
  free(longEMAs);
  free(logEMAs);
  free_2darray(dates);

  return 0;
}


int parse_csv(char **dates, double *volumes, double *prices) {
  FILE *file = fopen("neural_net/AAPL2.csv", "r");
  if (!file) {
    perror("fopen failure");
    exit(EXIT_FAILURE);
  }
  char *line = (char *) malloc(101 * sizeof(char)); // store each line
  fgets(line, 100, file); // line stores first line
  printf("header: %s", line);
  int i = 0;
  for (; fgets(line, 100, file); i++) {
    // for each line in csv file
    strncpy(dates[i], strtok(line, ","), DATE_SIZE);
    for (int j = 0; j < 4; j++) strtok(NULL, ",");
    prices[i] = strtof(strtok(NULL, ","), NULL);
    volumes[i] = strtof(strtok(NULL, ","), NULL);
    printf("date %s, price %f, volume %f\n", dates[i], prices[i], volumes[i]);
  }
  free(line);
  fclose(file);
  numOfDays(i);
  return i;
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

void free_2darray(char **array) {
  assert(array);
  for (int i = 0; array[i]; i++) {
    free(array[i]);
  }
  free(array);
}

double log_return(int index, double *prices) {
  if (index > 0 && index < MAX_NUMBER_OF_DAYS) {
    double ret = prices[index] / prices[index - 1];
    return log(ret);
  } else {
    printf("Out of bounds of array in roc function\n");
    return 0;
  }
}

double rsi(int index, double *prices) {
  if (index > RSI_PERIOD && index < MAX_NUMBER_OF_DAYS) {
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
  if (index > 0 && index < MAX_NUMBER_OF_DAYS) {
    double k = 2.0f / (timePeriod + 1);
    EMAs[index] = log_return(index, prices) * k + EMAs[index - 1] * (1 - k);
  } else {
    printf("Out of bounds of array in calculate_log_return_ema_ema function\n");
    EMAs[index] = 0;
  }
}

void calculate_ema(int index, double *prices, double *EMAs, int timePeriod) {
  if (index > 0 && index < MAX_NUMBER_OF_DAYS) {
    double k = 2.0f / (timePeriod + 1);
    EMAs[index] = prices[index] * k + EMAs[index - 1] * (1 - k);
  } else {
    printf("Out of bounds of array in calculate_ema function\n");
    EMAs[index] = 0;
  }
}

double macd(int index, double *shortEMAs, double *longEMAs) {
  if (index >= 0 && index < MAX_NUMBER_OF_DAYS) {
    return shortEMAs[index] - longEMAs[index];
  } else {
    printf("Out of bounds of array in macd function\n");
    return 0;
  }
}

double roc(int index, double *prices) {
  // ( (close - close n periods ago) / close n periods ago )  * 100
  if (index >= ROC_PERIODS && index < MAX_NUMBER_OF_DAYS) {
    double ret = ((prices[index] - prices[index - ROC_PERIODS]) / prices[index - ROC_PERIODS]) * 100;
    return ret;
  } else {
    printf("Out of bounds of array in roc function\n");
    return 0;
  }
}