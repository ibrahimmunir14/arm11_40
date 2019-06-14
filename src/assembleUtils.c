#include "assembleUtils.h"

// used to REGEX regexMatch instructions
bool regexMatch(const char *string, const char *pattern) {
  regex_t matcher;
  if (regcomp(&matcher, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
    return false;
  }

  int status = regexec(&matcher, string, 0, NULL, 0);
  regfree(&matcher);

  return status == 0;
}

// return string with whitespace removed
char* trimWhiteSpace(char *string) {
  while (isspace(string[0])) {
    string++;
  }
  return string;
}

