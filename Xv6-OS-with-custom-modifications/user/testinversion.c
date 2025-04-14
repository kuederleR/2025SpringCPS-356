#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[]) {
  sleep(10);
  fprint(1, "Hello from testinversion!\n");
  exit();
}