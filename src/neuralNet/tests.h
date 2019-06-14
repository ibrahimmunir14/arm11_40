#ifndef ARM11_40_TESTS_H
#define ARM11_40_TESTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loader.h"

double **makeFakeDataArray(void);
void printMappings(dataMapping_t *dataMapping, int numOfEntries);
dataMapping_t *makeFakeData(void);

#endif //ARM11_40_TESTS_H
