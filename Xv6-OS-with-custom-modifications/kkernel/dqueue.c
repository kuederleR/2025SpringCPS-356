#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "buf.h"

#define QUEUE_SIZE 8192

struct dev_queue {
  char buffer[QUEUE_SIZE];
  int read_pos;
  int write_pos;
  int count;
  struct sleeplock lock;
};

static struct dev_queue queue;

void devqueue_init(void) {
  initsleeplock(&queue.lock, "queue");
  queue.read_pos = 0;
  queue.write_pos = 0;
  queue.count = 0;
}

int devqueue_write(struct inode *ip, char *buf, int n) {
  int i;
  iunlock(ip);
  acquiresleep(&queue.lock);
  
  for (i = 0; i < n; i++) {
    while (queue.count == QUEUE_SIZE) {
      sleep(&queue, &queue.lock.lk);
    }
    
    queue.buffer[queue.write_pos] = buf[i];
    queue.write_pos = (queue.write_pos + 1) % QUEUE_SIZE;
    queue.count++;
    wakeup(&queue);
  }

  releasesleep(&queue.lock);
  ilock(ip);
  return n;
}

int devqueue_read(struct inode *ip, char *buf, int n) {
  int i;
  iunlock(ip);
  acquiresleep(&queue.lock);
  
  for (i = 0; i < n; i++) {
    while (queue.count == 0) {
      sleep(&queue, &queue.lock.lk);
    }
    
    buf[i] = queue.buffer[queue.read_pos];
    queue.read_pos = (queue.read_pos + 1) % QUEUE_SIZE;
    queue.count--;
    wakeup(&queue);
  }

  releasesleep(&queue.lock);
  ilock(ip);
  return n;
}

void queueinit(void) {
  devsw[DQUEUE].write = devqueue_write;
  devsw[DQUEUE].read = devqueue_read;
  devqueue_init();
}
