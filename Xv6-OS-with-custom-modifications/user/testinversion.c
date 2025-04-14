// File: user/testinversion.c
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

// Declare syscall prototypes (correct signature)
int flock(int fd, int operation);
int funlock(int fd);

int lockfd;

// void low_process();
// void medium_process();
// void high_process();

int
main(void) {
  printf(1, "[MAIN] Priority inversion test.\n");

  // create the lock file
  lockfd = open("lockfile", O_CREATE | O_RDWR);
  if (lockfd < 0) {
    printf(2, "[ERROR] open lockfile failed.\n");
    exit();
  }

  int pid_low = fork();
  if (pid_low == 0) {
    nice(getpid(), 1); // low priority
    low_process();
    exit();
  }

//   sleep(5);

//   int pid_high = fork();
//   if (pid_high == 0) {
//     nice(getpid(), 9); // high priority
//     high_process();
//     exit();
//   }

//   sleep(5);

//   int pid_med = fork();
//   if (pid_med == 0) {
//     nice(getpid(), 5); // medium priority
//     medium_process();
//     exit();
//   }

//   for (int i = 0; i < 3; i++)
//     wait();

  printf(1, "[DONE]\n");
  exit();
}

void low_process() {
  flock(lockfd, 1);
  for (int i = 0; i < 20; i++) sleep(5);
  funlock(lockfd);
}

// void high_process() {
//   flock(lockfd, 1);
//   funlock(lockfd);
// }

// void medium_process() {
//   for (int i = 0; i < 50; i++) {
//     for (int j = 0; j < 100000; j++) {
//       asm volatile("");
//     }
//   }
// }