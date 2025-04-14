#include "types.h"
#include "stat.h"
#include "user.h"

#define LOW 10
#define MEDIUM 50
#define HIGH 90

int lock = 0; // simple spinlock flag
int high_started = 0;
int high_finished = 0;

// simple spinlock
void acquire_lock(volatile int *lock) {
  while(__sync_lock_test_and_set(lock, 1) != 0);
}
void release_lock(volatile int *lock) {
  __sync_lock_release(lock);
}

void low_proc() {
  // Set low priority
  set_priority(getpid(), LOW);
  acquire_lock(&lock);
  sleep(100);  // Hold lock long enough to demonstrate inversion
  release_lock(&lock);
  exit();
}

void high_proc() {
  // Set high priority
  set_priority(getpid(), HIGH);
  high_started = 1;
  acquire_lock(&lock);
  high_finished = 1;
  release_lock(&lock);
  exit();
}

void medium_proc() {
  // Set medium priority
  set_priority(getpid(), MEDIUM);
  int i;
  for (i = 0; i < 100000000; i++) {
    // burn CPU cycles to starve low_proc
    if (high_finished) break;  // Give high a chance
  }
  exit();
}

int main(void) {
  int pid;

  // Start low first
  pid = fork();
  if (pid == 0) low_proc();

  sleep(10);  // Let low grab the lock first

  // Start high next
  pid = fork();
  if (pid == 0) high_proc();

  sleep(10);  // Let high block on lock

  // Start medium last
  pid = fork();
  if (pid == 0) medium_proc();

  // Wait for all children
  wait();
  wait();
  wait();

  if (high_started && high_finished) {
    printf(1, "[TEST RESULT] Priority inversion DID NOT prevent high from running. TEST PASS\n");
  } else {
    printf(1, "[TEST RESULT] High priority process was blocked too long. TEST FAIL\n");
  }

  exit();
}
