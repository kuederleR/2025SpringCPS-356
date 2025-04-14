#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h" // For O_CREATE

int
main(int argc, char *argv[])
{
    int lockFd = open("consoleLock", O_CREATE|O_RDWR);
    printf(1, "lockFd is %d\n", lockFd);
    flock(lockFd);
    flock(lockFd);
    funlock(lockFd);
    exit();
}
