#include "semaphores.h"
#include "memory.h"
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

sem_t *riders_mutex;
sem_t *stop_mutex;
sem_t *bus;
sem_t *allBoard;
sem_t *print;
sem_t *pass_mutex;
sem_t *final_stop;
sem_t *get_of;
sem_t *blocked_mutex;
// sem_t *stop_semaphores;

int semaphore_init(int skier_count, int stops_count) {
  initialize_shared_memory(skier_count, stops_count);

  sem_unlink(BUS_FNAME);
  sem_unlink(BLOCKED_FNAME);
  sem_unlink(PRINT_FNAME);
  sem_unlink(BOARDED_FNAME);
  sem_unlink(RIDERS_FNAME);
  sem_unlink(GETOF_FNAME);
  sem_unlink(FINAL_FNAME);
  sem_unlink(STOP_FNAME);
  sem_unlink(PASS_FNAME);

  bus = sem_open(BUS_FNAME, O_CREAT, 0666, 0);
  if (bus == SEM_FAILED) {
    perror("sem_open/bus");
    return 1;
  }

  blocked_mutex = sem_open(BLOCKED_FNAME, O_CREAT, 0666, 1);
  if (blocked_mutex == SEM_FAILED) {
    perror("sem_open/blocked_mutex");
    return 1;
  }

  riders_mutex = sem_open(RIDERS_FNAME, O_CREAT, 0666, 1);
  if (riders_mutex == SEM_FAILED) {
    perror("sem_open/rider_mutex");
    return 1;
  }

  stop_mutex = sem_open(STOP_FNAME, O_CREAT, 0666, 0);
  if (stop_mutex == SEM_FAILED) {
    perror("sem_open/multiplex");
    return 1;
  }

  allBoard = sem_open(BOARDED_FNAME, O_CREAT, 0666, 0);
  if (allBoard == SEM_FAILED) {
    perror("sem_open/allBoard");
    return 1;
  }

  print = sem_open(PRINT_FNAME, O_CREAT, 0666, 1);
  if (print == SEM_FAILED) {
    perror("sem_open/print");
    return 1;
  }

  final_stop = sem_open(FINAL_FNAME, O_CREAT, 0666, 0);
  if (final_stop == SEM_FAILED) {
    perror("sem_open/final_stop");
    return 1;
  }

  pass_mutex = sem_open(PASS_FNAME, O_CREAT, 0666, 1);
  if (pass_mutex == SEM_FAILED) {
    perror("sem_open/pass_mutex");
    return 1;
  }

  get_of = sem_open(GETOF_FNAME, O_CREAT, 0666, 0);
  if (pass_mutex == SEM_FAILED) {
    perror("sem_open/get_off");
    return 1;
  }

  return 0;
}

void semaphore_clean(int stops_count) {
  fclose(oFile);
  cleanup_shared_memory(stops_count);

  sem_close(bus);
  sem_close(blocked_mutex);
  sem_close(print);
  sem_close(stop_mutex);
  sem_close(riders_mutex);
  sem_close(pass_mutex);
  sem_close(final_stop);
  sem_close(allBoard);
  sem_close(get_of);

  sem_unlink(BUS_FNAME);
  sem_unlink(BLOCKED_FNAME);
  sem_unlink(PRINT_FNAME);
  sem_unlink(BOARDED_FNAME);
  sem_unlink(RIDERS_FNAME);
  sem_unlink(GETOF_FNAME);
  sem_unlink(FINAL_FNAME);
  sem_unlink(STOP_FNAME);
  sem_unlink(PASS_FNAME);

  munmap(passengers, sizeof(int));
  munmap(line, sizeof(int));
  munmap(curr_stop, sizeof(int));
  munmap(coming, sizeof(int));
  munmap(riders_at_stop, sizeof(int) * stops_count);
}
