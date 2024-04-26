#include "processes.h"
#include "memory.h"
#include "semaphores.h"
#include "util.h"
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>

void process_bus(int stops_count, int tb, int bus_cap) {
  (void)bus_cap;
  nice_print("BUS: started\n");

  while (true) {
    (*curr_stop) = 1;

    while ((*curr_stop) <= stops_count) {
      usleep(random_range(0, tb));

      nice_print("BUS: BLOCK STOP %d\n", *curr_stop);
      sem_wait(blocked_mutex);
      (*blocked) = 1;
      sem_post(blocked_mutex);

      nice_print("BUS: arrived to %d\n", *curr_stop);

      // Let skiers board
      sem_wait(riders_mutex);
      int tmp_riders = riders_at_stop[*curr_stop];
      sem_post(riders_mutex);

      if (tmp_riders > 0) {
        for (int i = 0; i < tmp_riders; i++) {
          nice_print("BUS: post at %d\n", *curr_stop - 1);
          sem_post(&stop_semaphores[*curr_stop - 1]);
          nice_print("BUS: waiting\n");
          sem_wait(allBoard);
        }
      }

      nice_print("BUS: leaving %d\n", *curr_stop);

      sem_wait(blocked_mutex);
      (*blocked) = 0;
      int tmp = (*riders_blocked_count);
      sem_post(blocked_mutex);

      for (int i = 0; i < tmp; i++) {
        sem_post(&stop_access_mutex[*curr_stop - 1]);
      }

      // Depart
      (*curr_stop)++;
    }

    usleep(random_range(0, tb));

    nice_print("BUS: arrived to final\n");

    sem_wait(pass_mutex);
    int tmp = *passengers;
    sem_post(pass_mutex);

    for (int i = 0; i < tmp; i++) {
      nice_print("BUS: go\n");
      sem_post(final_stop);
      nice_print("BUS: waiting on get of\n");
      sem_wait(get_of);
      nice_print("BUS: another round\n");
    }

    nice_print("BUS: leaving final\n");

    bool still_waiting = false;
    for (int i = 1; i <= stops_count; i++) {
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

  nice_print("BUS: finish\n");
  exit(0);
}

void process_skier(int skier_id, int stop_id, int tl, int bus_cap) {
  nice_print("L %d: started\n", skier_id);

  usleep(random_range(0, tl));

  nice_print("L %d: stop unblock %d\n", skier_id, stop_id);
  sem_wait(blocked_mutex);
  int pepa = *blocked;
  sem_post(blocked_mutex);
  if (pepa && *curr_stop == stop_id) {
    (*riders_blocked_count)++;
    sem_wait(&stop_access_mutex[stop_id - 1]);
  }

  sem_wait(riders_mutex);
  riders_at_stop[stop_id] += 1;
  (*coming) -= 1;
  sem_post(riders_mutex);

  nice_print("L %d: arrived to %d\n", skier_id, stop_id);

  // Waiting on BUS
  while (true) {
    sem_wait(&stop_semaphores[stop_id - 1]);
    nice_print("L %d: goging inside\n", skier_id);

    if (*curr_stop != stop_id) {
      nice_print("L %d: huh my: %d, curr: %d\n", skier_id, stop_id, *curr_stop);
      continue;
    }

    if (*passengers >= bus_cap) {
      sem_post(allBoard);
      continue;
    }

    // BoardBUS()
    nice_print("L %d: passengers: %d, buscap: %d\n", skier_id, *passengers,
               bus_cap);
    if (*passengers < bus_cap) {
      nice_print("L %d: boarding\n", skier_id);

      sem_wait(pass_mutex);
      sem_wait(riders_mutex);
      (*passengers)++;
      riders_at_stop[*curr_stop] -= 1;
      sem_post(riders_mutex);
      sem_post(pass_mutex);

      sem_post(allBoard);

      break;
    } else {
      nice_print("L %d: BUS CAN GO\n", skier_id);
      sem_post(allBoard);
    }
  }

  sem_wait(final_stop);
  sem_wait(pass_mutex);
  (*passengers)--;
  sem_post(pass_mutex);
  sem_post(get_of);

  exit(0);
}
