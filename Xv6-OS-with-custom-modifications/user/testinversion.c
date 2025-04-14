#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(void) {
  int fd = open("l", O_CREATE | O_RDWR); // very short filename
  if (fd < 0) exit();

  if (fork() == 0) {
    nice(getpid(), 1);
    flock(fd, 1);
    funlock(fd);
    exit();
  }

  wait();
  exit();
}