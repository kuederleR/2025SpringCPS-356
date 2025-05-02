#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "file.h"

// Read from /dev/null: Always return an error (nothing to read)
int devnull_read(struct inode *ip, char *buf, int n) {
  return -1; // Read is not allowed
}

// Write to /dev/null: Accepts input but discards it
int devnull_write(struct inode *ip, char *buf, int n) {
  return n; // Pretend the write was successful, but discard data
}

// Register the device
void nullinit(void) {
  devsw[DNULL].read = devnull_read;
  devsw[DNULL].write = devnull_write;
}
