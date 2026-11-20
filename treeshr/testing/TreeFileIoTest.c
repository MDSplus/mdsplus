/* Copyright (c) 2026 MIT/GA. Apache-2.0.
 * Dependency-free test of the TreeFileIo backend seam. Requires the
 * libMdsTreeFileTESTMEM backend (built alongside) on the image search path. */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "testing.h"

extern int     MDS_IO_OPEN(char *, int, mode_t);
extern int     MDS_IO_CLOSE(int);
extern ssize_t MDS_IO_READ(int, void *, size_t);
extern ssize_t MDS_IO_WRITE(int, void *, size_t);
extern off_t   MDS_IO_LSEEK(int, off_t, int);
extern int     MDS_IO_MMAP_CAPABLE(int);

int main(void)
{
  BEGIN_TESTING(TreeFileIo backend seam);

  char hello[] = "hello";       /* writable buffers: MDS_IO_WRITE takes void* */
  char abcd[]  = "abcdABCD";
  char buf[16] = {0};

  /* 1. Default path: a plain (no-scheme) path round-trips via default backend. */
  char plain[] = "/tmp/mdstreefiletest-plain";
  unlink(plain);
  int fd = MDS_IO_OPEN(plain, O_RDWR | O_CREAT, 0644);
  TEST1(fd >= 0);
  TEST1(MDS_IO_WRITE(fd, hello, 5) == 5);
  TEST1(MDS_IO_LSEEK(fd, 0, SEEK_SET) == 0);
  TEST1(MDS_IO_READ(fd, buf, 5) == 5);
  TEST1(memcmp(buf, "hello", 5) == 0);
  TEST1(MDS_IO_MMAP_CAPABLE(fd) == 1);
  TEST1(MDS_IO_CLOSE(fd) == 0);

  /* 2. Scheme dispatch: testmem:// routes to the loaded backend and round-trips. */
  unlink("/tmp/mdstreefiletest-A");
  int a = MDS_IO_OPEN("testmem://A", O_RDWR | O_CREAT, 0644);
  TEST1(a >= 0);
  TEST1(MDS_IO_MMAP_CAPABLE(a) == 0);
  TEST1(MDS_IO_WRITE(a, abcd, 8) == 8);
  TEST1(MDS_IO_LSEEK(a, 0, SEEK_SET) == 0);
  memset(buf, 0, sizeof(buf));
  /* 3. Short read then EOF=0. */
  TEST1(MDS_IO_READ(a, buf, 4) == 4);
  TEST1(MDS_IO_READ(a, buf, 4) == 4);
  TEST1(MDS_IO_READ(a, buf, 4) == 0);
  TEST1(MDS_IO_READ(a, buf, 4) == 0);
  TEST1(MDS_IO_CLOSE(a) == 0);

  /* 2b. Cache: a second testmem open succeeds (backend loaded once, reused). */
  int a2 = MDS_IO_OPEN("testmem://A", O_RDONLY, 0);
  TEST1(a2 >= 0);
  TEST1(MDS_IO_CLOSE(a2) == 0);

  /* 4. Fail-closed: an unregistered scheme fails the open. */
  int bad = MDS_IO_OPEN("nosuch://whatever", O_RDONLY, 0);
  TEST1(bad < 0);

  END_TESTING;
  return 0;
}
