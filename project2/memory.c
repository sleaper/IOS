#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

FILE *oFile;

// shared memory for data
int *line;
int *passengers;
int *curr_stop;
int *riders_at_stop;
int *coming;
int *blocked;
int *riders_blocked_count;
sem_t *stop_semaphores;
sem_t *stop_access_mutex;

void cleanup_shared_memory(int stops_count) {
  munmap(passengers, sizeof(int));
  munmap(line, sizeof(int));
  munmap(curr_stop, sizeof(int));
  munmap(coming, sizeof(int));
  munmap(blocked, sizeof(int));
  munmap(riders_blocked_count, sizeof(int));
  munmap(riders_at_stop, sizeof(int) * stops_count);
  munmap(stop_semaphores, sizeof(sem_t) * stops_count);
  munmap(stop_access_mutex, sizeof(sem_t) * stops_count);
}

int initialize_shared_memory(int skier_count, int stops_count) {
  cleanup_shared_memory(stops_count);

  oFile = fopen("proj2.out", "w");
  if (oFile == NULL) {
    fprintf(stderr, "File open failed\n");
    return 1;
  }

  stop_semaphores =
      mmap(NULL, sizeof(sem_t) * stops_count, PROT_READ | PROT_WRITE,
           MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (stop_semaphores == MAP_FAILED) {
    perror("mmap/stop_semaphores");
    return 1;
  }

  stop_access_mutex =
      mmap(NULL, sizeof(sem_t) * stops_count, PROT_READ | PROT_WRITE,
           MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (stop_access_mutex == MAP_FAILED) {
    perror("mmap/stop_access_mutex");
    return 1;
  }

  passengers = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (passengers == MAP_FAILED) {
    perror("mmap/bus_cap");
    return 1;
  }
  *passengers = 0;

  blocked = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (blocked == MAP_FAILED) {
    perror("mmap/blocked");
    return 1;
  }
  *blocked = 0;

  riders_blocked_count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (riders_blocked_count == MAP_FAILED) {
    perror("mmap/riders_blocked_count");
    return 1;
  }
  *riders_blocked_count = 0;

  line = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
              MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (line == MAP_FAILED) {
    perror("mmap/line");
    return 1;
  }
  *line = 1;

  curr_stop = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (curr_stop == MAP_FAILED) {
    perror("mmap/curr_stop");
    return 1;
  }
  *curr_stop = 1;

  // NOTE: I do not whant to recalculate the zero index all the time
  riders_at_stop =
      mmap(NULL, sizeof(int) * stops_count + 1, PROT_READ | PROT_WRITE,
           MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (riders_at_stop == MAP_FAILED) {
    perror("mmap/riders_at_stop");
    return 1;
  }
  for (int i = 1; i < stops_count; i++) {
    riders_at_stop[i] = 0;
  }

  coming = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (coming == MAP_FAILED) {
    perror("mmap/comming");
    return 1;
  }
  *coming = skier_count;

  return 0;
}
