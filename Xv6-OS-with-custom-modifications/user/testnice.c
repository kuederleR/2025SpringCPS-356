#include "user.h"

int
main(int argc, char *argv[])
{
  if(argc != 3){
    printf("Usage: nice pid priority\n");
    exit();
  }
  int pid = atoi(argv[1]);
  int prio = atoi(argv[2]);
  if (nice(pid, prio) < 0)
    printf("Failed to set priority\n");
  exit();
}
