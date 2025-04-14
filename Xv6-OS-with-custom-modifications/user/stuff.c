#include "stat.h"
#include "types.h"
#include "user.h"
#include "fcntl.h" // For O_ constants

int main(int argc, const char *argv[]) {
  int consoleLock = open("consoleLock", O_CREATE|O_RDWR);

  const char* message = "Stuff\n";
  for (int i = 0; i < 1000; ++i) {
    flock(consoleLock);
    for(int j = 0; j < 7; ++j) {
      write(1, &message[j], 1);
    }
    funlock(consoleLock);
  }
  close(consoleLock);
}