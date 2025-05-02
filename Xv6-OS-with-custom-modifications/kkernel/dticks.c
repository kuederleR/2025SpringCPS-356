#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "file.h"

// External tick counter from xv6 kernel
extern uint32_t ticks;
extern struct spinlock tickslock;

int int_to_string(char *buf, int n, uint32_t num) {
  int i = 0;
  if (num == 0) {
    buf[i++] = '0';
  } else {
    while (num > 0 && i < n - 1) {
      buf[i++] = '0' + num % 10;
      num /= 10;
    }
  }
  buf[i] = '\0';

  // Reverse the string
  for (int j = 0, k = i - 1; j < k; j++, k--) {
    char temp = buf[j];
    buf[j] = buf[k];
    buf[k] = temp;
  }

  return i;
}

// Read from /dev/ticks: Return tick count as a NUL-terminated string
int devticks_read(struct inode *ip, char *buf, int n) {
  char tick_str[16] = {'\0'}; // Buffer to hold tick count as a string, initialized to null characters
  int tick_count;
  
  acquire(&tickslock);  // Ensure atomic read of the global ticks variable
  tick_count = ticks;
  release(&tickslock);

  // Convert tick_count to a string
  int len = int_to_string(tick_str, 16, tick_count);
  
  memmove(buf, tick_str, len + 1);
  return len + 1;  // Return the number of bytes read (including '\0')
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
