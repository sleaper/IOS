#ifndef PROJ2_H
#define PROJ2_H

#include <stdlib.h>
#define BUS_FNAME "xspacpe00.ios.bus"
#define RIDERS_FNAME "xspacpe00.ios.riders"
#define BOARDED_FNAME "xspacpe00.ios.boarded"
#define PRINT_FNAME "xspacpe00.ios.print"
#define GETOF_FNAME "xspacpe00.ios.getoff"
#define PASS_FNAME "xspacpe00.ios.passengers"
#define FINAL_FNAME "xspacpe00.ios.final_stop"
#define STOP_FNAME "xspacpe00.ios.stop"

#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x)                                                         \
  do {                                                                         \
  } while (0)
#endif

inline unsigned int random_range(int min, int max) {
  return rand() % (max + 1 - min) + min;
}

inline int min(int a, int b) { return ((a) < (b) ? a : b); }
inline int max(int a, int b) { return ((a) > (b) ? a : b); }

#endif /* ifndef PROJ2_H */
