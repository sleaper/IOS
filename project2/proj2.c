#include "processes.h"
#include "semaphores.h"
#include "util.h"
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int skiers_count = 0;
  int stops_count = 0;
  int bus_capacity = 0;
  int skier_wait_time = 0;
  int bus_ride_time = 0;

  if (argc != 6) {
    fprintf(stderr, "Invalid args count: %d instead of %d\n", argc - 1, 5);
    return 1;
  }

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
        return 1;
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

  if (semaphore_init(skiers_count, stops_count) == 1) {
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

  // Waiting for children
  while (wait(NULL) > 0)
    ;

  semaphore_clean(stops_count);

  DEBUG_PRINT("exiting main\n");
  return 0;
}
