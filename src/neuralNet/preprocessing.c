#include "preprocessing.h"

int numOfDays(int x) {
  static int numOfDays = MAX_NUMBER_OF_DAYS;
  if (x != 0) numOfDays = x;
  return numOfDays;
}

int inputCreator(double **inputs) {
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
    inputs[i] = calloc(NUMBER_OF_DOUBLES, sizeof(double));
    if (!inputs[i] || !dates[i]) {
      perror("calloc/malloc for inner arrays failed\n");
      exit(EXIT_FAILURE);
    }
  }

  // call parseCSV with pointer to all arrays
  parseCSV(dates, volumes, prices);

  // create 2d input array
  for (int i = 1; i < numOfDays(0); i ++) {
    createOneInputEntry(i, inputs, prices, volumes, shortEMAs, longEMAs, logEMAs);
  }

  // reallocate arrays to match the actual size
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
    for (int j = 0; j < NUMBER_OF_DOUBLES; j++) {
      printf("%f      ", inputs[i][j]);
    }
    printf("\n");
  }

  printf("NUMNOFDAYS: %i\n", numOfDays(0));
  // free all memory
  free(prices);
  free(volumes);
  free(shortEMAs);
  free(longEMAs);
  free(logEMAs);
  free2dArray(dates);

  return numOfDays(0);
}

int parseCSV(char **dates, double *volumes, double *prices) {
  FILE *file = fopen("neuralNet/data/KO.csv", "r");
  if (!file) {
    perror("fopen failure");
    exit(EXIT_FAILURE);
  }
  char *line = (char *) malloc(MAX_LINE_SIZE * sizeof(char)); // store each line
  fgets(line, MAX_LINE_SIZE, file); // line stores first line
  printf("header: %s", line);
  int i = 0;
  for (; fgets(line, MAX_LINE_SIZE, file); i++) {
    int dropColumns = 4;
    // for each line in csv file
    // dates in first column
    strncpy(dates[i], strtok(line, ","), DATE_SIZE);
    // drop 4 columns
    for (int j = 0; j < dropColumns; j++) strtok(NULL, ",");
    // prices in fifth column
    prices[i] = strtof(strtok(NULL, ","), NULL);
    // volumes in sixth column
    volumes[i] = strtof(strtok(NULL, ","), NULL);
    printf("date %s, price %f, volume %f\n", dates[i], prices[i], volumes[i]);
  }
  free(line);
  fclose(file);
  numOfDays(i);
  return i;
}

// function for creating and writing into csv file
void writeToCSV(char **dates, double *prices, int num_data) {
  FILE *fp;

  //create csv file
  fp = fopen("csvfile.csv","w+");

  for(int i = 0; i < num_data; i++) {
    fprintf(fp, "%s,%f\n", dates[i], prices[i]);
  }
  fclose(fp);
}

void createOneInputEntry(int index, double **inputs, double *prices, double *volumes, double *shortEMAs,
                         double *longEMAs, double *logEMAs) {
  /* 6 values in array are:
   * 1. log return
   * 2. volume
   * 3. EMA of log return
   * 4. RSI
   * 5. MACD
   * 6. ROC
   * 7. expected output price of that day
   * */

  // the old index is so that we don't use data from the current day to predict the current day's price.
  // this would be very counter intuitive!
  int oldIndex = index - 1;
  calculateLogReturnEMA(oldIndex, prices, logEMAs, SHORT_EMA_PERIOD);
  calculateEMA(oldIndex, prices, shortEMAs, SHORT_EMA_PERIOD);
  calculateEMA(oldIndex, prices, longEMAs, LONG_EMA_PERIOD);
  normaliseValues(volumes, volumes, getMin(volumes, numOfDays(0)), getMax(volumes, numOfDays(0)), numOfDays(0));
  inputs[index][0] = logReturn(oldIndex, prices);
  inputs[index][1] = volumes[oldIndex];
  inputs[index][2] = logEMAs[oldIndex];
  inputs[index][3] = rsi(oldIndex, prices);
  inputs[index][4] = macd(oldIndex, shortEMAs, longEMAs);
  inputs[index][5] = roc(oldIndex, prices);
  inputs[index][6] = prices[index];
}

void free2dArray(char **array) {
  assert(array);
  for (int i = 0; array[i]; i++) {
    free(array[i]);
  }
  free(array);
}

double logReturn(int index, double *prices) {
  if (index > 0 && index < MAX_NUMBER_OF_DAYS) {
    double ret = prices[index] / prices[index - 1];
    return log(ret);
  } else {
    printf("Out of bounds of array or not enough data in roc function\n");
    return 0;
  }
}

double rsi(int index, double *prices) {
  // RSI period chosen as 14 time periods which is the default
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

    // the index is calculated from relative strength
    double relativeStrength = sumGain / sumLoss;
    return 100.0 - (100.0 / (1 + relativeStrength));
  } else {
    printf("Out of bounds of array or not enough data in rsi function\n");
    return 0;
  }
}

void calculateLogReturnEMA(int index, double *prices, double *EMAs, int timePeriod) {
  if (index > 0 && index < MAX_NUMBER_OF_DAYS) {
    double k = 2.0f / (timePeriod + 1);
    EMAs[index] = logReturn(index, prices) * k + EMAs[index - 1] * (1 - k);
  } else {
    printf("Out of bounds of array or not enough data in calculate_log_return_ema_ema function\n");
    EMAs[index] = 0;
  }
}

void calculateEMA(int index, double *prices, double *EMAs, int timePeriod) {
  if (index > 0 && index < MAX_NUMBER_OF_DAYS) {
    double k = 2.0f / (timePeriod + 1);
    // the previous EMA multiplied by a ratio < 1 hence, older EMAs are less significant
    EMAs[index] = prices[index] * k + EMAs[index - 1] * (1 - k);
  } else {
    printf("Out of bounds of array or not enough data in calculateEMA function\n");
    EMAs[index] = 0;
  }
}

double macd(int index, double *shortEMAs, double *longEMAs) {
  if (index >= 0 && index < MAX_NUMBER_OF_DAYS) {
    return shortEMAs[index] - longEMAs[index];
  } else {
    printf("Out of bounds of array or not enough data in macd function\n");
    return 0;
  }
}

double roc(int index, double *prices) {
  // ( (close - close n periods ago) / close n periods ago )  * 100
  if (index >= ROC_PERIODS && index < MAX_NUMBER_OF_DAYS) {
    double ret = ((prices[index] - prices[index - ROC_PERIODS]) / prices[index - ROC_PERIODS]) * 100;
    return ret;
  } else {
    printf("Out of bounds of array or not enough data in roc function\n");
    return 0;
  }
}