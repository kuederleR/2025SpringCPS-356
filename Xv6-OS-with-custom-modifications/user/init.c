// init: The initial user-level program

#include "fcntl.h"
#include "stat.h"
#include "types.h"
#include "user.h"

char *argv[] = {"sh", 0};

int main(void) {
  int pid, wpid;

  mkdir("dev");

  if (open("dev/console", O_RDWR) < 0) {
    mknod("dev/console", 1, 1);
    open("dev/console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

  // Only 10 device major numbers are allowed by param.h
  int hellofd = open("dev/hello", O_RDWR);
  if(hellofd < 0){
    mknod("dev/hello", 7, 1); // 7 is major number, 1 is minor number 
    hellofd = open("dev/hello", O_RDWR);
  }
  close(hellofd);

  for (;;) {
    printf(1, "init: starting sh\n");
    pid = fork();
    if (pid < 0) {
      printf(1, "init: fork failed\n");
      exit();
    }
    if (pid == 0) {
      exec("sh", argv);
      printf(1, "init: exec sh failed\n");
      exit();
    }
    while ((wpid = wait()) >= 0 && wpid != pid) {
      printf(1, "zombie!\n");
    }
  }
}
