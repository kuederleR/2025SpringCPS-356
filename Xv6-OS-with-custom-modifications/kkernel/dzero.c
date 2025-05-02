#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "file.h"

// Read from /dev/zero: Always return '\0'
int devzero_read(struct inode *ip, char *buf, int n) {
  int i;
  for (i = 0; i < n; i++) {
    buf[i] = '\0';
  }
  return n;
}

// Write is not supported (read-only device)
int devzero_write(struct inode *ip, char *buf, int n) {
  return -1; // Return an error (operation not permitted)
}

// Register the device
void zeroinit(void) {
  devsw[DZERO].read = devzero_read;
  devsw[DZERO].write = devzero_write;
}
