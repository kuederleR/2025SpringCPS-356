#ifndef F2A88E5D_2F16_4549_BD3C_4EB0A7AB1FA4
#define F2A88E5D_2F16_4549_BD3C_4EB0A7AB1FA4
#include "types.h"

struct buf;
struct context;
struct file;
struct inode;
struct pipe;
struct proc;
struct rtcdate;
struct spinlock;
struct sleeplock;
struct stat;
struct superblock;

// bio.c
void binit(void);
struct buf *bread(uint32_t, uint32_t);
void brelse(struct buf *);
void bwrite(struct buf *);

// console.c
void consoleinit(void);
void cprintf(char *, ...);
void consoleintr(int (*)(void));
void panic(char *) __attribute__((noreturn));

// dhello.c
void helloinit(void);

// exec.c
int exec(char *, char **);

// file.c
struct file *filealloc(void);
void fileclose(struct file *);
struct file *filedup(struct file *);
void fileinit(void);
int fileread(struct file *, char *, int n);
int filestat(struct file *, struct stat *);
int filewrite(struct file *, char *, int n);

// fs.c
void readsb(int dev, struct superblock *sb);
int dirlink(struct inode *, char *, uint32_t);
struct inode *dirlookup(struct inode *, char *, uint32_t *);
struct inode *ialloc(uint32_t, short);
struct inode *idup(struct inode *);
void iinit(int dev);
void ilock(struct inode *);
void iput(struct inode *);
void iunlock(struct inode *);
void iunlockput(struct inode *);
void iupdate(struct inode *);
int namecmp(const char *, const char *);
struct inode *namei(char *);
struct inode *nameiparent(char *, char *);
int readi(struct inode *, char *, uint32_t, uint32_t);
void stati(struct inode *, struct stat *);
int writei(struct inode *, char *, uint32_t, uint32_t);

// ide.c
void ideinit(void);
void ideintr(void);
void iderw(struct buf *);

// ioapic.c
void ioapicenable(int irq, int cpu);
extern uint8_t ioapicid;
void ioapicinit(void);

// kalloc.c
char *kalloc(void);
void kfree(char *);
void kinit1(void *, void *);
void kinit2(void *, void *);

// kbd.c
void kbdintr(void);

// lapic.c
void cmostime(struct rtcdate *r);
int lapicid(void);
extern volatile uint32_t *lapic;
void lapiceoi(void);
void lapicinit(void);
void lapicstartap(uint8_t, uint32_t);
void microdelay(int);

// log.c
void initlog(int dev);
void log_write(struct buf *);
void begin_op();
void end_op();

// mp.c
extern int ismp;
void mpinit(void);

// picirq.c
void picenable(int);
void picinit(void);

// pipe.c
int pipealloc(struct file **, struct file **);
void pipeclose(struct pipe *, int);
int piperead(struct pipe *, char *, int);
int pipewrite(struct pipe *, char *, int);

// PAGEBREAK: 16
//  proc.c
int cpuid(void);
void exit(void);
int fork(void);
int growproc(int);
int kill(int);
struct cpu *mycpu(void);
struct proc *myproc();
void pinit(void);
void procdump(void);
void scheduler(void) __attribute__((noreturn));
void sched(void);
void setproc(struct proc *);
void sleep(void *, struct spinlock *);
void userinit(void);
int wait(void);
void wakeup(void *);
void yield(void);

// swtch.S
void swtch(struct context **, struct context *);

// spinlock.c
void acquire(struct spinlock *);
void getcallerpcs(void *, uint32_t *);
int holding(struct spinlock *);
void initlock(struct spinlock *, char *);
void release(struct spinlock *);
void pushcli(void);
void popcli(void);

// sleeplock.c
void acquiresleep(struct sleeplock *);
void releasesleep(struct sleeplock *);
int holdingsleep(struct sleeplock *);
void initsleeplock(struct sleeplock *, char *);

// string.c
int memcmp(const void *, const void *, uint32_t);
void *memmove(void *, const void *, uint32_t);
void *memset(void *, int, uint32_t);
char *safestrcpy(char *, const char *, int);
int strlen(const char *);
int strncmp(const char *, const char *, uint32_t);
char *strncpy(char *, const char *, int);

// syscall.c
int argint(int, int *);
int argptr(int, char **, int);
int argstr(int, char **);
int fetchint(uint32_t, int *);
int fetchstr(uint32_t, char **);
void syscall(void);

// timer.c
void timerinit(void);

// trap.c
void idtinit(void);
extern uint32_t ticks;
void tvinit(void);
extern struct spinlock tickslock;

// uart.c
void uartinit(void);
void uartintr(void);
void uartputc(int);

// vm.c
void seginit(void);
void kvmalloc(void);
pde_t *setupkvm(void);
char *uva2ka(pde_t *, char *);
int allocuvm(pde_t *, uint32_t, uint32_t);
int deallocuvm(pde_t *, uint32_t, uint32_t);
void freevm(pde_t *);
void inituvm(pde_t *, char *, uint32_t);
int loaduvm(pde_t *, char *, struct inode *, uint32_t, uint32_t);
pde_t *copyuvm(pde_t *, uint32_t);
void switchuvm(struct proc *);
void switchkvm(void);
int copyout(pde_t *, uint32_t, void *, uint32_t);
void clearpteu(pde_t *pgdir, char *uva);

// kshutdown.c
void            shutdown(void);

/// @brief This fuction acquires a sleep lock for the given file.
/// @param fp a valid file pointer
/// @return 0 upon success and -1 upon error
int             filelock(struct file* fp);

/// @brief This fuction releases a sleep lock for the given file.
/// @param fp a valid file pointer
/// @return 0 upon success and -1 upon error
int             fileunlock(struct file* fp);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))

#endif /* F2A88E5D_2F16_4549_BD3C_4EB0A7AB1FA4 */
