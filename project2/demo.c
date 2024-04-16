#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

void swap(int *aa, int *bb) {
  int tmp = *aa;
  usleep(1);
  *aa = *bb;
  *bb = tmp;
}

int *c;
sem_t *m;

int main() {
  c = mmap(NULL, sizeof(int) * 2, PROT_READ | PROT_WRITE,
           MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  m = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,
           MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if ((c == MAP_FAILED) || (m == MAP_FAILED)) {
    printf("mmap faild\n");
    exit(1);
  }

  c[0] = 1;
  c[1] = 2;
  int res;
  sem_init(m, 1, 1);

  res = fork();
  if (res < 0) {
    return 1;
  }
  for (int i = 0; i < 100; i++) {
    sem_wait(m);
    swap(&(c[0]), &(c[1]));
    sem_post(m);
    printf(">>> %i %i\n", c[0], c[1]);
  }
  if (res > 0) {
    wait(NULL);
    sem_destroy(m);
  }
  printf("konec\n");
  munmap(c, sizeof(int) * 2);
  munmap(m, sizeof(sem_t));
  return 0;
}
