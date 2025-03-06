#include "types.h"
#include "user.h"
#include "fcntl.h"

#define TEST_SIZE 100

int main() {
  int fd = open("/dev/null", O_WRONLY);
  if (fd < 0) {
    printf(1, "Failed to open /dev/null\n");
    exit();
  }

  char buf[TEST_SIZE];
  for (int i = 0; i < TEST_SIZE; i++) {
    buf[i] = 'A'; // Fill buffer with dummy data
  }

  int written = write(fd, buf, TEST_SIZE);
  if (written != TEST_SIZE) {
    printf(1, "Test failed: /dev/null did not accept all bytes\n");
    close(fd);
    exit();
  }

  printf(1, "Test passed: /dev/null accepted all writes and discarded data.\n");

  close(fd);

  // Now test read behavior
  fd = open("/dev/null", O_RDONLY);
  if (fd >= 0) {
    char read_buf[10];
    int read_bytes = read(fd, read_buf, 10);
    if (read_bytes != -1) {
      printf(1, "Test failed: /dev/null should not allow reads.\n");
      close(fd);
      exit();
    }
    close(fd);
  }

  printf(1, "Test passed: /dev/null correctly rejects reads.\n");

  exit();
}
