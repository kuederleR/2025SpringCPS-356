// user/testscheduler.c
#include "types.h"
#include "stat.h"
#include "user.h"

void test_starvation();
void test_round_robin();

int
main(int argc, char *argv[])
{
  printf(1, "=== Running Priority Scheduler Tests ===\n");

  if(argc < 2){
    printf(2, "Usage: testscheduler [starvation|roundrobin]\n");
    exit();
  }

  if(strcmp(argv[1], "starvation") == 0){
    test_starvation();
  } else if(strcmp(argv[1], "roundrobin") == 0){
    test_round_robin();
  } else {
    printf(2, "Unknown test %s\n", argv[1]);
  }

  exit();
}

void test_starvation() {
  for (int i = 0; i < 3; i++) {
    if (fork() == 0) {
      setpriority(9);
      while (1) {
        printf(1, "[HIGH-%d] running\n", getpid());
        sleep(50);
      }
    }
  }
  if (fork() == 0) {
    setpriority(1);
    while (1) {
      printf(1, "[LOW-%d] running\n", getpid());
      sleep(50);
    }
  }
  wait();
}

void test_round_robin() {
  for (int i = 0; i < 3; i++) {
    if (fork() == 0) {
      setpriority(5);
      while (1) {
        printf(1, "[EQUAL-%d] running\n", getpid());
        sleep(50);
      }
    }
  }
  wait();
}
