#include "types.h"
#include "user.h"
#include "fcntl.h"

#define TEST_SIZE 100

int main() {
  int fd = open("/dev/zero", O_RDONLY);
  if (fd < 0) {
    printf(1, "Failed to open /dev/zero\n");
    exit();
  }

  char buf[TEST_SIZE];
  read(fd, buf, TEST_SIZE);

  // Verify that the buffer is filled with '\0'
  for (int i = 0; i < TEST_SIZE; i++) {
    if (buf[i] != '\0') {
      printf(1, "Test failed: /dev/zero returned nonzero byte at %d\n", i);
      close(fd);
      exit();
    }
  }

  printf(1, "Test passed: /dev/zero returned only NUL bytes.\n");

  close(fd);
  exit();
}
