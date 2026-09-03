/* Copyright (c) 2026 MIT/GA. Apache-2.0.
 * In-tree reference TreeFileIo backend for tests, scheme "testmem://".
 * Backed by a real temp file (so read/write/lseek round-trip), but advertises
 * mmap_capable=0 and a no-op read lock to emulate a remote/URL backend. */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "../treefileio.h"

/* Value matches treeshr's TreeSUCCESS; kept local to avoid private headers. */
#ifndef TreeSUCCESS
#define TreeSUCCESS 1
#endif

/* Map "testmem://<name>" to a temp file path /tmp/mdstreefiletest-<name>. */
static int tm_open(const char *filename, int options, mode_t mode)
{
  const char *sep = strstr(filename, "://");
  const char *name = sep ? sep + 3 : filename;
  char path[512];
  snprintf(path, sizeof(path), "/tmp/mdstreefiletest-%s", name);
  return open(path, options, mode ? mode : 0644);
}
static int tm_close(int fd) { return close(fd); }
static ssize_t tm_read(int fd, void *b, size_t n) { return read(fd, b, n); }
static ssize_t tm_write(int fd, const void *b, size_t n)
{
  return write(fd, b, n);
}
static off_t tm_lseek(int fd, off_t off, int whence)
{
  return lseek(fd, off, whence);
}
/* No-op read lock (emulating an immutable remote source). */
static int tm_lock(int fd, off_t offset, size_t size, int mode_in, int *deleted)
{
  (void)fd; (void)offset; (void)size; (void)mode_in;
  if (deleted) *deleted = 0;
  return TreeSUCCESS;
}

static const TreeFileIo tm_io = {
    tm_open, tm_close, tm_read, tm_write, tm_lseek, tm_lock,
    /* .mmap_capable = */ 0};

__attribute__((visibility("default")))
const TreeFileIo *FileIo(void) { return &tm_io; }
