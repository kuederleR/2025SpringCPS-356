// user/nice.c
#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if(argc != 3){
    printf(2, "Usage: nice <pid> <priority>\n");
    exit();
  }

  int pid = atoi(argv[1]);
  int prio = atoi(argv[2]);

  int r = nice(pid, prio);
  if(r < 0)
    printf(2, "nice: failed to set priority for pid %d\n", pid);
  else
    printf(1, "nice: priority of pid %d set to %d\n", pid, prio);

  exit();
}
