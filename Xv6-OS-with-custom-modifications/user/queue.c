#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int open_queue() {
    int devQueueFd = open("dev/queue", O_RDWR);
    if (devQueueFd < 0) {
        printf(2, "Error: Unable to open dev/queue\n");
        exit();
    }
    return devQueueFd;
}

void read_queue(int read_bytes) {
    int devQueueFd = open_queue();
    char readBuffer[read_bytes];
    
    // Read from the queue
    int bytesRead = read(devQueueFd, readBuffer, read_bytes);
    if (bytesRead != read_bytes) {
        printf(2, "Error: Read failed or incomplete\n");
    } else {
        printf(1, "Read from /dev/queue: <%s>\n", readBuffer);
    }
    close(devQueueFd);
}

void write_queue(char *str) {
    int devQueueFd = open_queue();
    int writeSize = strlen(str);

    // Write to the queue
    int bytesWritten = write(devQueueFd, str, writeSize);
    if (bytesWritten != writeSize) {
        printf(2, "Error: Write failed or incomplete\n");
    } else {
        printf(1, "Write to /dev/queue succeeded: %d bytes written\n", bytesWritten);
    }
    close(devQueueFd);
}

int main(int argc, char *argv[]) {
    int read_bytes;
    
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-r") == 0) {
            if (i + 1 < argc) {
                read_bytes = atoi(argv[i + 1]);
                read_queue(read_bytes);
                i++;
            }
        } else if (strcmp(argv[i], "-w") == 0) {
            if (i + 1 < argc) {
                write_queue(argv[i+1]);
            }
        }
    }
    exit();
}
