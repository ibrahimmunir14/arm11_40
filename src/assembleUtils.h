#ifndef ARM11_40_SRC_ASSEMBLERUTILS_H_
#define ARM11_40_SRC_ASSEMBLERUTILS_H

#include <stdlib.h>
#include <regex.h>
#include <ctype.h>
#include <stdbool.h>

// check if string matches a regex pattern
bool regexMatch(const char *string, const char *pattern);
// return string with leading whitespace characters removed
char* trimWhiteSpace(char *string);

#endif //ARM11_40_SRC_ASSEMBLERUTILS_H_
