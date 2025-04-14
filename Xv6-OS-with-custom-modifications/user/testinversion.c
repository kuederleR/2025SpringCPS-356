#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
static const int highPriority = 10;
static const int midPriority = 5;
int main(int argc, char *argv[])
{
  printf(2, "%s Test passes if low priority child gets lock, and\n", argv[0]);
  printf(2, "middle priority child completes before high priority gets lock.\n");
  nice(getpid(), highPriority); // Make self high priority
  printf(2, "%d started with priority: %d\n", getpid(), highPriority);
  int fd = open("lock", O_CREATE | O_RDWR);
  int lowChildPid = fork();
  if (0 != lowChildPid)
  {
    // In high priority parent
    sleep(1); // Give low priority child a chance to get lock
    int midChildPid = fork();
    if (0 != midChildPid)
    {
      // In high priority parent
      // give mid priority process a middle priority
      nice(midChildPid, midPriority);
      lock(fd); // Try to get lock low priority child already has
      printf(2, "%d high priority got lock!\n", getpid());
      unlock(fd);
    }
    else
    {
      // Run mid priority
      for (int i = 0; i < 10; i++)
      {
        printf(2, "%d mid priority running...\n", getpid());
        for (int j = 0; j < (1 << 20); j++)
        {
        }
      }
    }
  }
  else
  {
    lock(fd);
    printf(2, "%d low priority got lock!\n", getpid());
    for (int i = 0; i < 10; i++)
    {
      printf(2, "%d low priority running\n", getpid());
      for (int j = 0; j < (1 << 20); j++)
      {
      }
    }
    unlock(fd);
  }
  printf(2, "%d done!\n", getpid());
  wait();
  exit();
}