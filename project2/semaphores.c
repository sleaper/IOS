#include "semaphores.h"
#include "memory.h"
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

sem_t *riders_mutex;
sem_t *allBoard;
sem_t *print;
sem_t *pass_mutex;
sem_t *final_stop;
sem_t *get_of;
sem_t *blocked_mutex;

int semaphore_init(int skier_count, int stops_count) {
  initialize_shared_memory(skier_count, stops_count);

  sem_unlink(BLOCKED_FNAME);
  sem_unlink(PRINT_FNAME);
  sem_unlink(BOARDED_FNAME);
  sem_unlink(RIDERS_FNAME);
  sem_unlink(GETOF_FNAME);
  sem_unlink(FINAL_FNAME);
  sem_unlink(PASS_FNAME);

  char semaphore_name[256];
  for (int i = 0; i < stops_count; i++) {
    sprintf(semaphore_name, "/stop_semaphore_%d", i);
    sem_unlink(semaphore_name);
    stop_semaphores[i] = *sem_open(semaphore_name, O_CREAT | O_EXCL, 0666, 0);
    if (&stop_semaphores[i] == SEM_FAILED) {
      perror("sem_open/stop semaphore failed");
      return 1;
    }
  }

  char stop_sems_name[256];
  for (int i = 0; i < stops_count; i++) {
    sprintf(stop_sems_name, "/stop_access_mutex_%d", i);
    sem_unlink(stop_sems_name);
    stop_access_mutex[i] = *sem_open(stop_sems_name, O_CREAT | O_EXCL, 0666, 0);
    if (&stop_access_mutex[i] == SEM_FAILED) {
      perror("sem_open/stop_access_mutex");
      return 1;
    }
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

  sem_close(blocked_mutex);
  sem_close(print);
  sem_close(riders_mutex);
  sem_close(pass_mutex);
  sem_close(final_stop);
  sem_close(allBoard);
  sem_close(get_of);

  sem_unlink(BLOCKED_FNAME);
  sem_unlink(PRINT_FNAME);
  sem_unlink(BOARDED_FNAME);
  sem_unlink(RIDERS_FNAME);
  sem_unlink(GETOF_FNAME);
  sem_unlink(FINAL_FNAME);
  sem_unlink(PASS_FNAME);

  char semaphore_name[256];
  for (int i = 0; i < stops_count; i++) {
    sem_close(&stop_semaphores[i]);
    sprintf(semaphore_name, "/stop_semaphore_%d", i);
    sem_unlink(semaphore_name);
  }

  char stop_access_mutex_name[256];
  for (int i = 0; i < stops_count; i++) {
    sem_close(&stop_access_mutex[i]);
    sprintf(stop_access_mutex_name, "/stop_access_mutex_%d", i);
    sem_unlink(stop_access_mutex_name);
  }
}
