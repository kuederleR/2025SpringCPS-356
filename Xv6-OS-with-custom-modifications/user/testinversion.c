// File: user/testinversion.c
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

// Declare syscall prototypes (correct signature)
int flock(int fd, int operation);
int funlock(int fd);

int lockfd;

void low_process();
void medium_process();
void high_process();

int
main(void)
{
  printf(1, "[MAIN] Starting priority inversion test.\n");

  // create the lock file
  lockfd = open("lockfile", O_CREATE | O_RDWR);
  if (lockfd < 0) {
    printf(2, "[ERROR] Could not create lockfile.\n");
    exit();
  }

  int pid_low = fork();
  if (pid_low == 0) {
    nice(getpid(), 1); // low priority
    low_process();
    exit();
  }

  sleep(10); // let low priority grab the lock

  int pid_high = fork();
  if (pid_high == 0) {
    nice(getpid(), 9); // high priority
    high_process();
    exit();
  }

  sleep(10); // allow high to block on the lock

  int pid_med = fork();
  if (pid_med == 0) {
    nice(getpid(), 5); // medium priority
    medium_process();
    exit();
  }

  for (int i = 0; i < 3; i++)
    wait();

  printf(1, "\n[TEST DONE] Priority inversion demonstration complete.\n");
  exit();
}

void low_process() {
  printf(1, "[LOW %d] Acquiring lock...\n", getpid());
  flock(lockfd, 1);
  printf(1, "[LOW %d] Holding lock, simulating work...\n", getpid());
  for (int i = 0; i < 50; i++) {
    sleep(10);
  }
  printf(1, "[LOW %d] Releasing lock.\n", getpid());
  funlock(lockfd);
}

void high_process() {
  printf(1, "[HIGH %d] Attempting to acquire lock...\n", getpid());
  flock(lockfd, 1); // Will block until LOW releases it
  printf(1, "[HIGH %d] Acquired lock!\n", getpid());
  funlock(lockfd);
}

void medium_process() {
  printf(1, "[MEDIUM %d] Running CPU-intensive loop.\n", getpid());
  for (int i = 0; i < 300; i++) {
    for (int j = 0; j < 1000000; j++) {
      asm volatile(""); // prevent optimization
    }
    if (i % 50 == 0)
      printf(1, "[MEDIUM %d] Still running...\n", getpid());
  }
  printf(1, "[MEDIUM %d] Done.\n", getpid());
}