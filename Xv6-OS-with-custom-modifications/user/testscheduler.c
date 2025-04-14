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
    int high_pids[3];
    int low_pid;
  
    // Fork 3 high-priority processes
    for(int i = 0; i < 3; i++) {
      int pid = fork();
      if(pid == 0){
        while(1){
          printf(1, "[HIGH %d] running\n", getpid());
          sleep(100);
        }
      } else {
        high_pids[i] = pid;
      }
    }
  
    // Fork a low-priority process
    int pid = fork();
    if(pid == 0){
      while(1){
        printf(1, "!!! LOW %d running (should be rare or never)\n", getpid());
        sleep(100);
      }
    } else {
      low_pid = pid;
    }
  
    // Parent sets priorities
    for(int i = 0; i < 3; i++)
      nice(high_pids[i], 9);  // very high priority
    nice(low_pid, 1);         // very low priority
  
    // Wait for children
    for(int i = 0; i < 4; i++) wait();
  }

  void test_round_robin() {
    int pids[3];
  
    for(int i = 0; i < 3; i++) {
      int pid = fork();
      if(pid == 0){
        while(1){
          printf(1, "[EQUAL %d] running\n", getpid());
          sleep(100);
        }
      } else {
        pids[i] = pid;
      }
    }
  
    // All at same priority
    for(int i = 0; i < 3; i++)
      nice(pids[i], 7);
  
    for(int i = 0; i < 3; i++) wait();
  }
  