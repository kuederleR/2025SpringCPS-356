#include "types.h"
#include "user.h"
#include "fcntl.h"

#define BUFFER_SIZE 32

int main() {
  int fd = open("/dev/ticks", O_RDONLY);
  if (fd < 0) {
    printf(1, "Failed to open /dev/ticks\n");
    exit();
  }

  char buf[BUFFER_SIZE];
  int bytes_read = read(fd, buf, BUFFER_SIZE);

  if (bytes_read < 0) {
    printf(1, "Test failed: /dev/ticks read error\n");
    close(fd);
    exit();
  }

  // Ensure the last character is '\0'
  if (buf[bytes_read - 1] != '\0') {
    printf(1, "Test failed: /dev/ticks did not return a null-terminated string\n");
    close(fd);
    exit();
  }

  printf(1, "Test passed: /dev/ticks returned \"%s\" with %d bytes\n", buf, bytes_read);

  close(fd);
  exit();
}
