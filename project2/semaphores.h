#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <semaphore.h>

#define RIDERS_FNAME "xspacpe00.ios.riders"
#define BOARDED_FNAME "xspacpe00.ios.boarded"
#define PRINT_FNAME "xspacpe00.ios.print"
#define GETOF_FNAME "xspacpe00.ios.getoff"
#define PASS_FNAME "xspacpe00.ios.passengers"
#define FINAL_FNAME "xspacpe00.ios.final_stop"
#define BLOCKED_FNAME "xspacpe00.ios.blocked"

// Semaphores
extern sem_t *riders_mutex;
extern sem_t *blocked_mutex;
extern sem_t *allBoard;
extern sem_t *print;
extern sem_t *pass_mutex;
extern sem_t *final_stop;
extern sem_t *get_of;

int semaphore_init(int skier_count, int stops_count);
void semaphore_clean(int stops_count);

#endif
