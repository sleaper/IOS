#ifndef PROJ2_H
#define PROJ2_H

#include <stdlib.h>
#define BUS_FNAME "xspacpe00.ios.bus"
#define MUTEX_FNAME "xspacpe00.ios.mutex"
#define BOARDED_FNAME "xspacpe00.ios.boarded"
#define PRINT_FNAME "xspacpe00.ios.print"
#define GETOF_FNAME "xspacpe00.ios.getoff"
#define PASS_FNAME "xspacpe00.ios.passengers"
#define FINAL_FNAME "xspacpe00.ios.final_stop"
#define RIDERS_FNAME "xspacpe00.ios.riders"

static inline int random_range(int m, int n) {
  return m + rand() / (RAND_MAX / (n - m + 1) + 1);
}

static inline int min(int a, int b) { return ((a) < (b) ? a : b); }
static inline int max(int a, int b) { return ((a) > (b) ? a : b); }

#endif /* ifndef PROJ2_H */
