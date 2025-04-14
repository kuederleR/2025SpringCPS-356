// File: user/testinversion.c
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

// Lock file descriptor
int lockfd;

// Function declarations
void low_process();
void high_process();
void medium_process();

int main(void)
{
  printf(1, "[MAIN] Starting priority inversion test.\n");

  // Open or create a file to be used as a lock
  lockfd = open("lockfile", O_CREATE | O_RDWR);
  if (lockfd < 0) {
    printf(2, "[ERROR] Could not create lockfile.\n");
    exit();
  }

  // Low priority process: acquires lock first
  int pid_low = fork();
  if (pid_low == 0) {
    nice(getpid(), 1); // low priority
    low_process();
    exit();
  }

  sleep(10); // Let low acquire the lock

  // High priority process: will block on lock
  int pid_high = fork();
  if (pid_high == 0) {
    nice(getpid(), 9); // high priority
    high_process();
    exit();
  }

  sleep(10); // Let high block on lock

  // Medium priority process: does not need lock, but hogs CPU
  int pid_med = fork();
  if (pid_med == 0) {
    nice(getpid(), 5); // medium priority
    medium_process();
    exit();
  }

  // Parent waits for all children to finish
  for (int i = 0; i < 3; i++)
    wait();

  printf(1, "\n[TEST DONE] Priority inversion demonstration complete.\n");
  exit();
}

void low_process() {
  printf(1, "[LOW %d] Acquiring lock...\n", getpid());
  flock(lockfd);
  printf(1, "[LOW %d] Holding lock, simulating work...\n", getpid());
  for (int i = 0; i < 50; i++) {
    sleep(10); // simulate long work
  }
  printf(1, "[LOW %d] Releasing lock.\n", getpid());
  funlock(lockfd);
}

void high_process() {
  printf(1, "[HIGH %d] Attempting to acquire lock...\n", getpid());
  flock(lockfd); // Will block until LOW releases
  printf(1, "[HIGH %d] Acquired lock! (after LOW releases it)\n", getpid());
  funlock(lockfd);
}

void medium_process() {
  printf(1, "[MEDIUM %d] Running CPU-intensive loop.\n", getpid());
  for (int i = 0; i < 300; i++) {
    for (int j = 0; j < 1000000; j++) {
      asm volatile(""); // prevent loop optimization
    }
    if (i % 50 == 0)
      printf(1, "[MEDIUM %d] Still running...\n", getpid());
  }
  printf(1, "[MEDIUM %d] Done.\n", getpid());
}
