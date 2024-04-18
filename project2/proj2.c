#include "proj2.h"
#include <ctype.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
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
int *riders;
int *line;

int *passengers;
int *curr_stop;
int *riders_at_stop;
int *coming;

// Semaphores
sem_t *rider_mutex;
sem_t *in_stop;
sem_t *bus;
sem_t *allBoard;
sem_t *print;
sem_t *pass_mutex;
sem_t *final_stop;
sem_t *get_of;

int semaphore_init(int skier_count, int multiplex_default, int stops_count) {
  ((void)multiplex_default);
  sem_unlink(BUS_FNAME);
  sem_unlink(PRINT_FNAME);
  sem_unlink(BOARDED_FNAME);
  sem_unlink(MUTEX_FNAME);
  sem_unlink(GETOF_FNAME);
  sem_unlink(FINAL_FNAME);
  sem_unlink(RIDERS_FNAME);
  sem_unlink(PASS_FNAME);

  bus = sem_open(BUS_FNAME, O_CREAT, 0666, 0);
  if (bus == SEM_FAILED) {
    perror("sem_open/bus");
    return 1;
  }

  rider_mutex = sem_open(MUTEX_FNAME, O_CREAT, 0666, 1);
  if (rider_mutex == SEM_FAILED) {
    perror("sem_open/rider_mutex");
    return 1;
  }

  in_stop = sem_open(RIDERS_FNAME, O_CREAT, 0666, 0);
  if (in_stop == SEM_FAILED) {
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

  oFile = fopen("proj2.out", "w");
  if (oFile == NULL) {
    fprintf(stderr, "File open failed\n");
    return 1;
  }

  passengers = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (passengers == MAP_FAILED) {
    perror("mmap/bus_cap");
    return 1;
  }
  *passengers = 0;

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

  // NOTE: I do not whant to recalculate the zero index all the time
  riders_at_stop =
      mmap(NULL, sizeof(int) * stops_count + 1, PROT_READ | PROT_WRITE,
           MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (riders_at_stop == MAP_FAILED) {
    perror("mmap/riders_at_stop");
    return 1;
  }
  for (int i = 1; i < stops_count; i++) {
    riders[i] = 0;
  }

  coming = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (coming == MAP_FAILED) {
    perror("mmap/comming");
    return 1;
  }
  *coming = skier_count;
  // printf("skier count: %d\n", skier_count);

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

void semaphore_clean(int stop_count) {
  fclose(oFile);

  sem_close(bus);
  sem_close(print);
  sem_close(in_stop);
  sem_close(rider_mutex);
  sem_close(pass_mutex);
  sem_close(final_stop);
  sem_close(allBoard);
  sem_close(get_of);

  sem_unlink(BUS_FNAME);
  sem_unlink(PRINT_FNAME);
  sem_unlink(BOARDED_FNAME);
  sem_unlink(MUTEX_FNAME);
  sem_unlink(GETOF_FNAME);
  sem_unlink(FINAL_FNAME);
  sem_unlink(RIDERS_FNAME);
  sem_unlink(PASS_FNAME);

  munmap(passengers, sizeof(int));
  munmap(line, sizeof(int));
  munmap(curr_stop, sizeof(int));
  munmap(riders, sizeof(int));
  munmap(coming, sizeof(int));
  munmap(riders_at_stop, sizeof(int) * stop_count);
  printf("clean\n");
}

void process_bus(int stops_count, int tb) {
  nice_print("BUS: started\n");

again:
  (*curr_stop) = 1;

  while ((*curr_stop) <= stops_count) {
    usleep(random_range(0, tb));

    nice_print("BUS: arrived to %d\n", *curr_stop);

    // Let skiers board
    sem_wait(rider_mutex);
    int tmp = riders_at_stop[*curr_stop];
    sem_post(rider_mutex);

    if (tmp > 0) {
      sem_post(bus);
      sem_wait(allBoard);
    }

    nice_print("BUS: leaving %d\n", *curr_stop);
    // Depart
    (*curr_stop)++;
  }

  usleep(random_range(0, tb));

  nice_print("BUS: arrived to final\n");

  sem_wait(pass_mutex);
  int tmp = *passengers;
  sem_post(pass_mutex);

  for (int i = 0; i < tmp; i++) {
    sem_post(final_stop);
    sem_wait(get_of);
  }

  nice_print("BUS: leaving final\n");

  bool still_waiting = false;
  for (int i = 1; i < stops_count + 1; i++) {
    sem_wait(rider_mutex);
    if (riders_at_stop[i] > 0) {
      still_waiting = true;
    }
    sem_post(rider_mutex);
  }

  if (still_waiting || *coming > 0) {
    // If there are still waiting skiers or coming skiers, restart the bus loop
    goto again;
  } else {
    nice_print("BUS: finish\n");
    exit(0);
  }
}

void process_skier(int skier_id, int stop_id, int tl, int bus_cap) {
  nice_print("L %d: started\n", skier_id);

  usleep(random_range(0, tl));

  sem_wait(rider_mutex);
  riders_at_stop[stop_id] += 1;
  (*coming) -= 1;
  sem_post(rider_mutex);

  nice_print("L %d: arrived to %d\n", skier_id, stop_id);

  // Waiting on BUS
waiting_again:
  sem_wait(bus);

  // BoardBUS()
  if (riders_at_stop[*curr_stop] > 0 && *passengers < bus_cap) {

    nice_print("L %d: boarding \n", skier_id);

    sem_wait(pass_mutex);
    sem_wait(rider_mutex);
    (*passengers)++;
    riders_at_stop[*curr_stop] -= 1;
    sem_post(rider_mutex);
    sem_post(pass_mutex);

    sem_wait(rider_mutex);
    int tmp = riders_at_stop[*curr_stop];
    sem_post(rider_mutex);

    if (tmp == 0) {
      sem_post(allBoard);
    } else {
      sem_post(bus);
    }
  } else {
    sem_post(allBoard);
    goto waiting_again;
  }

  sem_wait(final_stop);
  sem_wait(pass_mutex);
  (*passengers)--;
  sem_post(pass_mutex);
  sem_post(get_of);

  nice_print("L %d: going to ski\n", skier_id);

  exit(0);
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
      return 1;
    }

    switch (i) {
    case 1:
      if (arg <= 0 || arg >= 20000) {
        fprintf(stderr, "Invalid skiers count\n");
        exit(1);
      }

      skiers_count = arg;
      break;
    case 2:
      if (0 >= arg || arg > 10) {
        fprintf(stderr, "Invalid stops count\n");
        return 1;
      }

      stops_count = arg;
      break;
    case 3:
      if (arg < 10 || arg > 100) {
        fprintf(stderr, "Invalid bus capacity\n");
        return 1;
      }

      bus_capacity = arg;
      break;

    case 4:
      if (0 > arg || arg > 10000) {
        fprintf(stderr, "Invalid TL\n");
        return 1;
      }

      skier_wait_time = arg;
      break;
    case 5:
      if (0 > arg || arg > 1000) {
        fprintf(stderr, "Invalid TB\n");
        return 1;
      }

      bus_ride_time = arg;
      break;
    }
  }

  if (semaphore_init(skiers_count, bus_capacity, stops_count) == 1) {
    semaphore_clean(stops_count);
    return 1;
  }

  // NOTE: Random numbers
  srand(getpid());

  pid_t bus = fork();
  if (bus == 0) {
    // NOTE: child
    process_bus(stops_count, bus_ride_time);
  } else if (bus < 0) {
    semaphore_clean(stops_count);
    return 1;
  }

  // NOTE: Creatig skiers processes
  for (int i = 1; i <= skiers_count; i++) {
    pid_t skier = fork();
    int stop_id = random_range(1, stops_count);

    if (skier == 0) {
      // NOTE: child
      process_skier(i, stop_id, skier_wait_time, bus_capacity);
    } else if (skier < 0) {
      semaphore_clean(stops_count);
      return 1;
    }
  }

  while (wait(NULL) > 0)
    ;

  semaphore_clean(stops_count);

  return 0;
}
