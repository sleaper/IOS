#include "processes.h"
#include "memory.h"
#include "semaphores.h"
#include "util.h"
#include <stdbool.h>
#include <unistd.h>

void process_bus(int stops_count, int tb) {
  nice_print("BUS: started\n");

  while (true) {
    (*curr_stop) = 1;

    while ((*curr_stop) <= stops_count) {
      usleep(random_range(0, tb));

      nice_print("BUS: arrived to %d\n", *curr_stop);

      // Let skiers board
      sem_wait(riders_mutex);
      int tmp_riders = riders_at_stop[*curr_stop];
      sem_post(riders_mutex);

      if (tmp_riders > 0) {
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
      sem_wait(riders_mutex);
      if (riders_at_stop[i] > 0) {
        still_waiting = true;
      }
      sem_post(riders_mutex);
    }

    if (!still_waiting && *coming <= 0) {
      // If there are still waiting skiers or coming skiers, restart the bus
      break;
    }
  }

  DEBUG_PRINT("exiting bus\n");
  nice_print("BUS: finish\n");
  exit(0);
}

void process_skier(int skier_id, int stop_id, int tl, int bus_cap) {
  nice_print("L %d: started\n", skier_id);

  usleep(random_range(0, tl));

  sem_wait(riders_mutex);
  riders_at_stop[stop_id] += 1;
  (*coming) -= 1;
  sem_post(riders_mutex);

  nice_print("L %d: arrived to %d\n", skier_id, stop_id);

  // Waiting on BUS
  while (true) {
    sem_wait(bus);

    // BoardBUS()
    if (riders_at_stop[*curr_stop] > 0 && *passengers < bus_cap) {

      nice_print("L %d: boarding\n", skier_id);

      sem_wait(pass_mutex);
      sem_wait(riders_mutex);
      (*passengers)++;
      riders_at_stop[*curr_stop] -= 1;
      sem_post(riders_mutex);
      sem_post(pass_mutex);

      sem_wait(riders_mutex);
      int tmp = riders_at_stop[*curr_stop];
      sem_post(riders_mutex);

      if (tmp == 0) {
        sem_post(allBoard);
      } else {
        sem_post(bus);
      }

      break;
    } else {
      sem_post(allBoard);
    }
  }

  sem_wait(final_stop);
  sem_wait(pass_mutex);
  (*passengers)--;
  sem_post(pass_mutex);
  sem_post(get_of);

  nice_print("L %d: going to ski\n", skier_id);
  exit(0);
}
