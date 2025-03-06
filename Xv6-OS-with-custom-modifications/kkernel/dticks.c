#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "file.h"

// External tick counter from xv6 kernel
extern uint32_t ticks;
extern struct spinlock tickslock;

// Read from /dev/ticks: Return tick count as a NUL-terminated string
int devticks_read(struct inode *ip, char *buf, int n) {
  char tick_str[16]; // Buffer to hold tick count as a string
  int tick_count;
  
  acquire(&tickslock);  // Ensure atomic read of the global ticks variable
  tick_count = ticks;
  release(&tickslock);

  // Convert tick_count to a string
  snprintf(tick_str, sizeof(tick_str), "%d", tick_count);

  // Ensure NUL termination
  int len = strlen(tick_str);
  tick_str[len] = '\0';
  len++;  // Include the '\0' in the output

  // Copy the tick string to user buffer, respecting `n`
  if (n < len) {
    return -1; // Fail if buffer is too small
  }
  
  memmove(buf, tick_str, len);
  return len;  // Return the number of bytes read (including '\0')
}

// Write is not supported (read-only device)
int devticks_write(struct inode *ip, char *buf, int n) {
  return -1; // Return an error (operation not permitted)
}

// Register the device
void ticksinit(void) {
  devsw[DTICKS].read = devticks_read;
  devsw[DTICKS].write = devticks_write;
}
