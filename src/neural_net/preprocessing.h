#ifndef ARM11_40_PREPROCESSING_H
#define ARM11_40_PREPROCESSING_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define NUMBER_OF_DAYS (2000)
#define NUMBER_OF_DOUBLES (6)
#define NUMBER_OF_INPUTS (6)
#define LINE_SIZE (80)
#define DATE_SIZE (12)
#define ROC_PERIODS (12)
#define SHORT_EMA_PERIOD (12)
#define LONG_EMA_PERIOD (26)
#define RSI_PERIOD (14)

int input_creator(void);
void free_array(double *array);
void free_2darray(char **array);
void parse_csv(char **dates, double *volumes, double *prices);
double log_return(int index, double *prices);
double rsi(int index, double *prices);
double macd(int index, double *shortEMAs, double *longEMAs);
double roc(int index, double *prices);
void calculate_ema(int index, double *prices, double *EMAs, int timePeriod);
void calculate_log_return_ema(int index, double *prices, double *EMAs, int timePeriod);
void create_one_input_entry(int index, double **inputs, double *prices, double *volumes, double *shortEMAs,
                            double *longEMAs, double *logEMAs);



#endif //ARM11_40_PREPROCESSING_H
