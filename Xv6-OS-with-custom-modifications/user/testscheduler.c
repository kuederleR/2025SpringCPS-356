// user/testscheduler.c
#include "types.h"
#include "stat.h"
#include "user.h"

void test_starvation();
void test_round_robin();

int
main(int argc, char *argv[])
{
  if(argc < 2){
    printf(2, "Usage: testscheduler [starvation|roundrobin]\n");
    exit();
  }

  if(strcmp(argv[1], "starvation") == 0){
    test_starvation();
  } else if(strcmp(argv[1], "roundrobin") == 0){
    test_round_robin();
  } else {
    printf(2, "Unknown test type.\n");
  }

  exit();
}

void test_starvation() {
    int pid;
    int num_high = 10;
    for (int i = 0; i < num_high; i++) {
      pid = fork();
      if (pid == 0) {
        nice(getpid(), 9);
        for (int j = 0; j < 20; j++) {
          printf(1, "[HIGH %d] running\n", getpid());
          sleep(10);
        }
        exit();
      }
    }
  
    pid = fork();
    if (pid == 0) {
      nice(getpid(), 1);
      for (int j = 0; j < 5; j++) {
        printf(1, "!!! LOW %d running (should NOT run much)\n", getpid());
        sleep(200);
      }
      exit();
    }
  
    for (int i = 0; i < 1 + num_high; i++)
      wait();
  
    printf(1, "\n[TEST DONE] Starvation test complete.\n");
}
  
  

void test_round_robin() {
    int num_children = 3;
  
    for (int i = 0; i < num_children; i++) {
      int pid = fork();
      if (pid == 0) {
        nice(getpid(), 5); // Equal priority
        for (int j = 0; j < 10; j++) {
          printf(1, "[ROUND %d] running (iteration %d)\n", getpid(), j);
          sleep(50);
        }
        exit();
      }
    }
  
    for (int i = 0; i < num_children; i++)
      wait();
  
    printf(1, "\n[TEST DONE] Round-robin test complete.\n");
    printf(1, "All equal-priority processes should have run alternately.\n");
}
  
  