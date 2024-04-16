#include "proj2.h"
#include <ctype.h>
#include <fcntl.h> /* For O_* constants */
#include <semaphore.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

extern inline int random_range(int m, int n);
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

FILE *oFile;

// shared memory for data
int *waiting;
int *line;
int *curr_stop;
int *riders;

// Semaphores
sem_t *mutex_waiting;
sem_t *mutex_riders;
sem_t *bus;
sem_t *boarded;
sem_t *print;
sem_t *get_of;
sem_t *final_stop;

int semaphore_init() {
  sem_unlink(BUS_FNAME);
  sem_unlink(MUTEX_FNAME);
  sem_unlink(BOARDED_FNAME);
  sem_unlink(PRINT_FNAME);
  sem_unlink(GETOF_FNAME);
  sem_unlink(FINAL_FNAME);

  bus = sem_open(BUS_FNAME, O_CREAT, 0666, 0);
  if (bus == SEM_FAILED) {
    perror("sem_open/bus");
    return 1;
  }

  mutex_waiting = sem_open(MUTEX_FNAME, O_CREAT, 0666, 1);
  if (mutex_waiting == SEM_FAILED) {
    perror("sem_open/mutex_waiting");
    return 1;
  }

  mutex_riders = sem_open(RIDERS_FNAME, O_CREAT, 0666, 1);
  if (mutex_waiting == SEM_FAILED) {
    perror("sem_open/mutex_riders");
    return 1;
  }

  boarded = sem_open(BOARDED_FNAME, O_CREAT, 0666, 0);
  if (boarded == SEM_FAILED) {
    perror("sem_open/boarded");
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

  get_of = sem_open(GETOF_FNAME, O_CREAT, 0666, 0);
  if (get_of == SEM_FAILED) {
    perror("sem_open/getof");
    return 1;
  }

  oFile = fopen("proj2.out", "w");
  if (oFile == NULL) {
    fprintf(stderr, "File open failed\n");
    return 1;
  }

  waiting = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (waiting == MAP_FAILED) {
    perror("mmap/waiting");
    return 1;
  }
  *waiting = 0;

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

  riders = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (riders == MAP_FAILED) {
    perror("mmap/riders");
    return 1;
  }
  *riders = 0;

  return 0;
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

void semaphore_clean() {
  printf("cleaning\n");
  sem_close(bus);
  sem_close(boarded);
  sem_close(mutex_waiting);
  sem_close(print);
  sem_close(get_of);
  sem_close(final_stop);
  sem_close(mutex_riders);

  sem_unlink(BUS_FNAME);
  sem_unlink(PRINT_FNAME);
  sem_unlink(MUTEX_FNAME);
  sem_unlink(BOARDED_FNAME);
  sem_unlink(GETOF_FNAME);
  sem_unlink(FINAL_FNAME);
  sem_unlink(RIDERS_FNAME);

  munmap(waiting, sizeof(int));
  munmap(line, sizeof(int));
  munmap(curr_stop, sizeof(int));
  munmap(riders, sizeof(int));
}

void process_bus(int stops_count, int tb, int bus_cap) {
  nice_print("BUS: started\n");

  *curr_stop = 1;

  while (*curr_stop < stops_count) {
    usleep(random_range(0, tb)); // Driving to stop
    nice_print("BUS: arrived to %d\n", *curr_stop);

    sem_wait(mutex_waiting);
    int n = min(*waiting, bus_cap);

    for (int i = 0; i < n; i++) {
      sem_post(bus);
      sem_wait(boarded);
    }

    *waiting = max(*waiting - bus_cap, 0);
    sem_post(mutex_waiting);

    nice_print("BUS: leaving %d\n", *curr_stop);

    // Depart
    if (*curr_stop < stops_count) {
      (*curr_stop)++;
    } else {
      break;
    }
  }

  usleep(random_range(0, tb)); // Driving to final stop
  nice_print("BUS: arrived to final\n");

  for (int i = 0; i < *riders; i++) {
    sem_post(final_stop);
    sem_wait(get_of);
  }
  nice_print("BUS: leaving final\n");

  if (*waiting > 0) {
    process_bus(stops_count, tb, bus_cap);
  }

  nice_print("BUS: finish\n");
  exit(0);
}

void process_skier(int skier_id, int stop_id, int tl, int bus_cap) {
  nice_print("L %d: started\n", skier_id);
  usleep(random_range(0, tl)); // Breakfast

  nice_print("L %d: arrived to %d\n", skier_id, stop_id);
  sem_wait(mutex_waiting);
  *waiting += 1;
  sem_post(mutex_waiting);

  bool is_rider = false;
  while (!is_rider) {
    sem_wait(bus);
    // Board
    if (*curr_stop == stop_id && *riders < bus_cap) {
      (*riders)++;
      is_rider = true;
      nice_print("L %d: boarding\n", skier_id);
    } else {
      sem_post(boarded);

      sem_wait(mutex_waiting);
      *waiting += 1;
      sem_post(mutex_waiting);
    }
  }

  sem_post(boarded);

  sem_wait(final_stop);

  sem_wait(mutex_riders);
  (*riders)--;
  sem_post(mutex_riders);

  sem_post(get_of);

  nice_print("L %d: going to ski\n", skier_id);
}

int main(int argc, char *argv[]) {
  int skiers_count = 0;
  int stops_count = 0;
  int bus_capacity = 0;
  int skier_wait_time = 0;
  int bus_ride_time = 0;

  for (int i = 1; i < argc; i++) {
    int arg = is_number(argv[i]);
    if (arg == -1) {
      fprintf(stderr, "Invalid args format\n");
      exit(1);
    }

    switch (i) {
    case 1:
      // TODO: Maybe add lower bound?
      if (arg <= 0 || arg >= 20000) {
        fprintf(stderr, "Invalid skiers count\n");
        exit(1);
      }

      skiers_count = arg;
      break;
    case 2:
      if (0 >= arg || arg > 10) {
        fprintf(stderr, "Invalid stops count\n");
        exit(1);
      }

      stops_count = arg;
      break;
    case 3:
      if (arg < 10 || arg > 100) {
        fprintf(stderr, "Invalid bus capacity\n");
        exit(1);
      }

      bus_capacity = arg;
      break;

    case 4:
      if (0 > arg || arg > 10000) {
        fprintf(stderr, "Invalid TL\n");
        exit(1);
      }

      skier_wait_time = arg;
      break;
    case 5:
      if (0 > arg || arg > 1000) {
        fprintf(stderr, "Invalid TB\n");
        exit(1);
      }

      bus_ride_time = arg;
      break;
    }
  }

  if (semaphore_init() == 1) {
    semaphore_clean();
    return 1;
  }

  printf("forkis\n");
  pid_t bus = fork();
  if (bus == 0) {
    // child
    process_bus(stops_count, bus_ride_time, bus_capacity);
  } else if (bus < 0) {
    semaphore_clean();
    return 1;
  }

  // TODO: Create L processess of skiers
  srand(time(NULL));
  for (int i = 1; i <= skiers_count; i++) {

    printf("forkis skier %d\n", i);
    pid_t skier = fork();
    srand(getpid());
    int stop_id = random_range(1, stops_count);

    if (skier == 0) {
      printf("pid: %d i:%d stop_id: %d\n", getpid(), i, stop_id);
      // child
      process_skier(i, stop_id, skier_wait_time, bus_capacity);
    } else if (skier < 0) {
      semaphore_clean();
      return 1;
    }
  }

  while (wait(NULL) > 0)
    ;

  semaphore_clean();

  return 0;
}
