#ifndef ARM11_40_PREPROCESSING_H
#define ARM11_40_PREPROCESSING_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <malloc.h>
#include "loader.h"

#define MAX_NUMBER_OF_DAYS (2000)
#define MAX_LINE_SIZE (100)
#define NUMBER_OF_DOUBLES (7)
#define DATE_SIZE (12)
#define ROC_PERIODS (12)
#define SHORT_EMA_PERIOD (12)
#define LONG_EMA_PERIOD (26)
#define RSI_PERIOD (14)

int inputCreator(double **inputs);
void free2dArray(char **array);
int parseCSV(char **dates, double *volumes, double *prices);
void writeToCSV(char **dates, double *prices, int num_data);
double logReturn(int index, double *prices);
double rsi(int index, double *prices);
double macd(int index, double *shortEMAs, double *longEMAs);
double roc(int index, double *prices);
void calculateEMA(int index, double *prices, double *EMAs, int timePeriod);
void calculateLogReturnEMA(int index, double *prices, double *EMAs, int timePeriod);
void createOneInputEntry(int index, double **inputs, double *prices, double *volumes, double *shortEMAs,
                         double *longEMAs, double *logEMAs);

#endif //ARM11_40_PREPROCESSING_H
