#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[]) {
    int devQueueFd = open("dev/queue", O_RDWR);
    if (devQueueFd < 0) {
        printf(2, "Error: Unable to open dev/queue\n");
        exit();
    }

    // Test data to enqueue
    char writeData[] = "Hello, queue!";
    int writeSize = sizeof(writeData);

    // Write to the queue
    int bytesWritten = write(devQueueFd, writeData, writeSize);
    if (bytesWritten != writeSize) {
        printf(2, "Error: Write failed or incomplete\n");
    } else {
        printf(1, "Write to /dev/queue succeeded: %d bytes written\n", bytesWritten);
    }

    // Buffer to read data back
    char readBuffer[writeSize];
    
    // Read from the queue
    int bytesRead = read(devQueueFd, readBuffer, writeSize);
    if (bytesRead != writeSize) {
        printf(2, "Error: Read failed or incomplete\n");
    } else {
        printf(1, "Read from /dev/queue: <%s>\n", readBuffer);
    }

    close(devQueueFd);
    exit();
}
