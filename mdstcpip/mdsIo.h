
#ifndef _MDSIO_H
#define _MDSIO_H
#ifndef DOXYGEN // hide this part from documentation

typedef enum
{
  MDS_IO_OPEN_K = 1,
  MDS_IO_CLOSE_K,
  MDS_IO_LSEEK_K,
  MDS_IO_READ_K,
  MDS_IO_WRITE_K,
  MDS_IO_LOCK_K,
  MDS_IO_EXISTS_K,
  MDS_IO_REMOVE_K,
  MDS_IO_RENAME_K,
  MDS_IO_READ_X_K,
  MDS_IO_PARALLEL_K,
  MDS_IO_OPEN_ONE_K
} mds_io_mode;

#define MDS_IO_O_CREAT 0x00000040
#define MDS_IO_O_TRUNC 0x00000200
#define MDS_IO_O_EXCL 0x00000080
#define MDS_IO_O_WRONLY 0x00000001
#define MDS_IO_O_RDONLY 0x00004000
#define MDS_IO_O_RDWR 0x00000002

#define MDS_IO_LOCK_RD 0x01
#define MDS_IO_LOCK_WRT 0x02
#define MDS_IO_LOCK_MASK 0x03
#define MDS_IO_LOCK_NONE 0x00
#define MDS_IO_LOCK_NOWAIT 0x08

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef O_RANDOM
#define O_RANDOM 0
#endif

#define MDSIP_VERSION_DSC_ARGS 1
#define MDSIP_VERSION_OPEN_ONE 2
#define MDSIP_VERSION_DSC_ANS 3
#define MDSIP_VERSION MDSIP_VERSION_DSC_ANS

#define MAX_DIMS 8

typedef union {
  int dims[MAX_DIMS];
  uint32_t length;
  struct __attribute__((__packed__))
  {
    uint32_t length;
    int options;
    int mode;
  } open;
  struct __attribute__((__packed__))
  {
    uint32_t length;
    int fd;
  } close;
  struct __attribute__((__packed__))
  {
    uint32_t length;
    int fd;
    int64_t offset;
    int whence;
  } lseek;
  struct __attribute__((__packed__))
  {
    uint32_t length;
    int fd;
    uint32_t count;
  } read;
  struct __attribute__((__packed__))
  {
    uint32_t count;
    int fd;
  } write;
  struct __attribute__((__packed__))
  {
    uint32_t length;
    int fd;
    int64_t offset;
    uint32_t size;
    int mode;
  } lock;
  struct __attribute__((__packed__))
  {
    uint32_t length;
  } exists;
  struct __attribute__((__packed__))
  {
    uint32_t length;
  } remove;
  struct __attribute__((__packed__))
  {
    uint32_t length;
  } rename;
  struct __attribute__((__packed__))
  {
    uint32_t length;
    int fd;
    int64_t offset;
    uint32_t count;
  } read_x;
  struct __attribute__((__packed__))
  {
    uint32_t length;
    int shot;
    int type;
    int new;
    int edit;
  } open_one;
} mdsio_t;

#ifdef WORDS_BIGENDIAN
#define SWAP_INT_IF_BIGENDIAN(pointer)         \
  {                                            \
    int *ptr = (int *)(pointer), tmp = ptr[0]; \
    ptr[0] = ptr[1];                           \
    ptr[0] = tmp;                              \
  }
#else
#define SWAP_INT_IF_BIGENDIAN(...) \
  { /**/                           \
  }
#endif
#endif // DOXYGEN end of hidden code
#endif // ifndef _MDSIO_H
