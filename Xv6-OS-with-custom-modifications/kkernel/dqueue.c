#include "types.h"
#include "defs.h"
#include "param.h"    // for NDEV
#include "traps.h"
#include "spinlock.h" // Use spinlock instead of sleeplock
#include "fs.h"
#include "file.h"     // for struct devsw

#define QUEUE_SIZE 8192

struct dev_queue {
    char buffer[QUEUE_SIZE];
    int read_pos;
    int write_pos;
    int count;
    struct spinlock lock;
};

static struct dev_queue queue;

void devqueue_init(void) {
    initlock(&queue.lock, "queue");
    queue.read_pos = 0;
    queue.write_pos = 0;
    queue.count = 0;
}

int queueread(struct inode *ip, char *dst, int n) {
    int i = 0;
    if(n <= 0)
        return 0;
    
    // Unlock inode before blocking on our queue.
    iunlock(ip);
    acquire(&queue.lock);
    
    while(i < n) {
        // If there's no data available:
        if(queue.count == 0) {
            // If we've already read some data, break out and return what we have.
            if(i > 0)
                break;
            // Otherwise, sleep waiting for data.
            sleep(&queue, &queue.lock);
            continue;
        }
        
        // Dequeue one byte.
        dst[i] = queue.buffer[queue.read_pos];
        queue.read_pos = (queue.read_pos + 1) % QUEUE_SIZE;
        queue.count--;
        i++;
        
        // Wake up any writers waiting for space.
        wakeup(&queue);
    }
    
    release(&queue.lock);
    ilock(ip);
    return i;
}

int queuewrite(struct inode *ip, char *buf, int n) {
    int i;
    if(n <= 0)
        return 0;
    
    iunlock(ip);
    acquire(&queue.lock);
    
    for (i = 0; i < n; i++) {
        // Wait until there is space in the queue.
        while(queue.count == QUEUE_SIZE) {
            sleep(&queue, &queue.lock);
        }
        
        // Enqueue one byte.
        queue.buffer[queue.write_pos] = buf[i];
        queue.write_pos = (queue.write_pos + 1) % QUEUE_SIZE;
        queue.count++;
        
        // Wake up any readers waiting for data.
        wakeup(&queue);
    }
    
    release(&queue.lock);
    ilock(ip);
    return n;
}

void queueinit(void) {
    devsw[DQUEUE].write = queuewrite;
    devsw[DQUEUE].read = queueread;
}
