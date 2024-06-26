#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>

inline unsigned int random_range(int min, int max) {
  return rand() % (max + 1 - min) + min;
}

inline int min(int a, int b) { return ((a) < (b) ? a : b); }
inline int max(int a, int b) { return ((a) > (b) ? a : b); }

void nice_print(const char *format, ...);
int is_number(char *s);

#endif /* ifndef UTIL_H */
