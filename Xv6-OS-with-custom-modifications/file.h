#ifndef C0B8A0F5_BF34_4081_94FE_4F6887BECE9D
#define C0B8A0F5_BF34_4081_94FE_4F6887BECE9D
#include "fs.h"
#include "sleeplock.h"

struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe;
  struct inode *ip;
  uint32_t off;
};

// in-memory copy of an inode
struct inode {
  uint32_t dev;          // Device number
  uint32_t inum;         // Inode number
  int ref;               // Reference count
  struct sleeplock flock; // protects everything below hereNDIRECT
  struct sleeplock lock; // protects everything below hereNDIRECT
  int valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint32_t size;
  uint32_t addrs[NDIRECT + 1];
};

// table mapping major device number to
// device functions
struct devsw {
  int (*read)(struct inode *, char *, int);
  int (*write)(struct inode *, char *, int);
};

extern struct devsw devsw[];

// Device major numbers
#define CONSOLE 1
#define HELLO 7

#endif /* C0B8A0F5_BF34_4081_94FE_4F6887BECE9D */
