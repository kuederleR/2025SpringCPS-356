#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "string.h"

void low_process(int fd);
void medium_process(int fd);
void print(int i, char *msg, char *process);

char* process_ = "[MAIN]";

int main() {
  int fd = open("lock", O_CREATE|O_RDWR);
  if (fd < 0) {
    printf(1, "Failed to open lock file\n");
    exit();
  } else {
    printf(1, "Lock file opened successfully\n");
    printf(1, "lockFd is %d\n", fd);
  }

  int pid_low = fork();
  print(1, "Setting priority for low-priority process\n", process_);
  nice(pid_low, 1); // Set low priority
  if (pid_low == 0) {
    process_ = "[LOW]";
    low_process(fd);
  }

  int pid_medium = fork();
  print(1, "Setting priority for medium-priority process\n", process_);
  nice(pid_medium, 5); // Set medium priority
  if (pid_medium == 0) {
    process_ = "[MEDIUM]";
    medium_process(fd);
  }


  wait();
  wait();


  // int pid_low = fork();
  // if (pid_low == 0) {
  //   nice(getpid(), 1); // Low priority
  //   flock(fd);
  //   printf(1, "Low-priority process acquired lock\n");
  //   sleep(200); // Hold lock for a while
  //   funlock(fd);
  //   printf(1, "Low-priority process released lock\n");
  //   exit();
  // }

  // sleep(10); // Ensure low-priority process acquires lock

  // int pid_high = fork();
  // if (pid_high == 0) {
  //   nice(getpid(), 10); // High priority
  //   printf(1, "High-priority process attempting to acquire lock\n");
  //   flock(fd);
  //   printf(1, "High-priority process acquired lock\n");
  //   funlock(fd);
  //   printf(1, "High-priority process released lock\n");
  //   exit();
  // }

  // sleep(10); // Ensure high-priority process is waiting on lock

  // int pid_medium = fork();
  // if (pid_medium == 0) {
  //   nice(getpid(), 5); // Medium priority
  //   for (int i = 0; i < 100000000; i++) {
  //     // Busy work
  //   }
  //   printf(1, "Medium-priority process completed work\n");
  //   exit();
  // }

  // wait(); // Wait for low-priority process
  // wait(); // Wait for high-priority process
  // wait(); // Wait for medium-priority process

  // close(fd);
  // unlink("lockfile");
  exit();
}

void low_process(int fd) {
  print(1, "[LOW] Entering low-priority process.\n", process_);
  flock(fd);
  print(1, "[LOW] Acquired lock in low-priority process.\n", process_);
  sleep(200); // Hold lock for a while
  funlock(fd);
  exit();
}

void medium_process(int fd) {
  print(1, "Entering medium-priority process.\n", process_);
  for (int i = 0; i < 100000000; i++) {
    // Busy work
  }
  print(1, "Completed work in medium-priority process.\n", process_);
  exit();
}

void print(int i, char *msg, char *process) {
  strcat(msg, " ");
  strcat(process, msg);
  write(i, process, strlen(process));
}
