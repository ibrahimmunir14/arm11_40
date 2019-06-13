#include "preprocessing.h"

// TODO parse_csv
// TODO PUT THESE INTO OTHER FILES
// TODO allocate inputs and prices elsewhere

int input_creator(void) {
  double **inputs = calloc(NUMBER_OF_DAYS, sizeof(double *));
  double *prices = calloc(NUMBER_OF_DAYS, sizeof(double));


  char **dates = calloc(NUMBER_OF_DAYS, sizeof(char *));
  double *volumes = calloc(NUMBER_OF_DAYS, sizeof(double));
  double *shortEMAs = calloc(NUMBER_OF_DAYS, sizeof(double));
  double *longEMAs = calloc(NUMBER_OF_DAYS, sizeof(double));
  double *logEMAs = calloc(NUMBER_OF_DAYS, sizeof(double));
  for (int i = 0; i < NUMBER_OF_DAYS; i ++) {
    dates[i] = calloc(DATE_SIZE, sizeof(char));
    inputs[i] = calloc(NUMBER_OF_INPUTS, sizeof(double));
  }

  // call parse_csv with pointer to all arrays
  parse_csv(dates, volumes, prices);

  // create 2d input array
  for (int i = 0; i < NUMBER_OF_DAYS; i ++) {
    create_one_input_entry(i, inputs, prices, volumes, shortEMAs, longEMAs, logEMAs);
  }

  // free all memory
  free(volumes);
  free(shortEMAs);
  free(longEMAs);
  free(logEMAs);
  free_2darray(dates);

  return 0;
}

void parse_csv(char **dates, double *volumes, double *prices) {
  FILE *file = fopen("neural_net/AAPL2.csv", "r");
  if (!file) {
    perror("fopen failure");
    exit(EXIT_FAILURE);
  }
  char *line = NULL;
  size_t len = 0;
  ssize_t nread;
  int i = 0;
  nread = getline(&line, &len, file);
  printf("Reading the header: %s", line);
  while ((nread = getline(&line, &len, file)) != -1) {
//    printf("%s Characters Read: %d\n", line, (int) nread);
    dates[i] = strtok_r(line, ",", &line);
    int drop_columns = 4;
    for (int j = 0; j < drop_columns; j ++) {
      strtok_r(line, ",", &line);
    }
    char *adj_close = strtok_r(line, ",", &line);
    char *volume = strtok_r(line, ",", &line);
    prices[i] = strtof(adj_close, &adj_close);
    volumes[i] = strtof(volume, &volume);
    printf("i %d, date %s, price %f, volume %f \n", i, dates[i], prices[i], volumes[i]);
    i ++;
  }
  fclose(file);
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
  int i = 0;
  while (array[i]) {
    free(array[i]);
  }
  free(array);
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