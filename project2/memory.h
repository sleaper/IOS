#ifndef MEM_H
#define MEM_H

#include <semaphore.h>
#include <stdio.h>

extern FILE *oFile;

// shared memory for data
extern int *line;
extern int *passengers;
extern int *curr_stop;
extern int *riders_at_stop;
extern int *coming;
extern int *blocked;
extern sem_t *stop_semaphores;
extern sem_t *stop_access_mutex;
extern int *riders_blocked_count;

int initialize_shared_memory(int skier_count, int stops_count);
void cleanup_shared_memory(int stops_count);

#endif
