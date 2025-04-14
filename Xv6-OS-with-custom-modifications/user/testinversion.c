// File: testinversion.c
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int lockfd;

void low_process() {
  flock(lockfd); // Acquires lock
  printf(1, "[LOW] Got lock, working...\n");
  sleep(100);    // Simulate work
  funlock(lockfd);
  printf(1, "[LOW] Released lock.\n");
  exit();
}

void high_process() {
  sleep(10); // Ensure low grabs the lock first
  printf(1, "[HIGH] Trying to acquire lock...\n");
  flock(lockfd); // Will block here
  printf(1, "[HIGH] Got the lock!\n");
  funlock(lockfd);
  exit();
}

void medium_process() {
  sleep(20); // Ensure high is waiting on the lock
  printf(1, "[MEDIUM] Starting CPU work...\n");
  for (volatile int i = 0; i < 100000000; i++); // Burn CPU
  printf(1, "[MEDIUM] Done with work.\n");
  exit();
}

int
main(void) {
  printf(1, "[MAIN] Priority inversion test.\n");

  // Create lockfile
  lockfd = open("lockfile", O_CREATE | O_RDWR);
  if (lockfd < 0) {
    printf(2, "[ERROR] open lockfile failed.\n");
    exit();
  }

  int pid_low = fork();
  if (pid_low == 0) {
    nice(getpid(), 1);
    low_process();
  }

  int pid_high = fork();
  if (pid_high == 0) {
    nice(getpid(), 10);
    high_process();
  }

  int pid_med = fork();
  if (pid_med == 0) {
    nice(getpid(), 5);
    medium_process();
  }

  wait(); wait(); wait(); // Wait for all children
  printf(1, "[MAIN] Test complete.\n");
  exit();
}
