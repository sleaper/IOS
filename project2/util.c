#include "util.h"
#include "memory.h"
#include "semaphores.h"
#include <ctype.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern inline unsigned int random_range(int min, int max);
extern inline int min(int a, int b);
extern inline int max(int a, int b);

int is_number(char *s) {
  int is_digit = true;
  for (int i = 0, n = strlen(s); i < n; i++) {
    if (!isdigit(s[i])) {
      is_digit = false;
      break;
    }
  }

  if (is_digit) {
    return atoi(s);
  } else {
    return -1;
  }
}

void nice_print(const char *format, ...) {
  sem_wait(print);

  va_list args;
  va_start(args, format);

  fprintf(oFile, "%d: ", *line);
  (*line)++;
  vfprintf(oFile, format, args);
  fflush(oFile);

  va_end(args);

  sem_post(print);
}
