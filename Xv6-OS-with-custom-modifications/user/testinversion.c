#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int lockfd;

void low() {
  flock(lockfd);
  printf(1, "[LOW] Got lock, doing work...\n");
  sleep(200);
  funlock(lockfd);
  printf(1, "[LOW] Released lock.\n");
  exit();
}

void high() {
  sleep(10);
  printf(1, "[HIGH] Attempting to acquire lock...\n");
  flock(lockfd); // Will block
  printf(1, "[HIGH] Acquired lock!\n");
  funlock(lockfd);
  exit();
}

void medium() {
  while (1) {
    printf(1, "[MEDIUM] Running...\n");
    sleep(10);
  }
}

int main() {
  lockfd = open("lockfile", O_CREATE | O_RDWR);
  if (lockfd < 0) {
    printf(2, "[ERROR] open lockfile failed.\n");
    exit();
  }

  int pid_low = fork();
  if (pid_low == 0) low();
  sleep(5);

  int pid_high = fork();
  if (pid_high == 0) high();
  sleep(5);

  int pid_medium = fork();
  if (pid_medium == 0) medium();
  sleep(5);

  // Set priorities
  nice(pid_low, 0);      // Low
  nice(pid_high, 10);    // High
  nice(pid_medium, 5);   // Medium

  wait(); wait(); wait();
  exit();
}
