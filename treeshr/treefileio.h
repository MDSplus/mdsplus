/*
Copyright (c) 2026, Massachusetts Institute of Technology and General Atomics.
All rights reserved. Licensed under the Apache License, Version 2.0.
*/

/* Pluggable backend for local MDSplus tree-file IO.
 *
 * Mirrors the mdsip transport-plugin pattern (IoRoutines / LoadIo in
 * mdstcpip/): a tree-file path with a URL scheme (e.g. "https://...") is
 * served by a backend loaded by image name "MdsTreeFile<SCHEME>" exporting a
 * function symbol "FileIo" of type FileIoFn. A path with no scheme uses the
 * built-in default backend (plain open/read/write/lseek/close). */
#ifndef TREEFILEIO_H
#define TREEFILEIO_H

#include <sys/types.h>   /* off_t, ssize_t, mode_t */

/* Backend plugin contract
 * -----------------------
 * To serve tree-file paths with URL scheme "x://", provide a shared library
 * named "libMdsTreeFileX" (scheme uppercased; the loader adds the platform
 * "lib" prefix and ".so"/".dll" suffix) that exports a function:
 *
 *     const TreeFileIo *FileIo(void);
 *
 * The exported FileIo symbol MUST be visible to the dynamic loader: annotate it
 * with __attribute__((visibility("default"))) on Unix when the library is built
 * with -fvisibility=hidden, or __declspec(dllexport) on Windows; otherwise the
 * loader's symbol lookup (dlsym / GetProcAddress) will not find it.
 *
 * The backend is discovered lazily on the first MDS_IO_OPEN of an "x://" path
 * via LibFindImageSymbol_C and cached per scheme, mirroring how mdsip loads
 * transport backends ("MdsIp<PROTOCOL>" exporting "Io"; see mdstcpip/).
 *
 * Backend semantics:
 *   - Set mmap_capable = 0 for sources whose fds are not real mmap-able kernel
 *     descriptors (e.g. HTTP/remote object stores); MapFile() then uses the
 *     calloc+read path instead of mmap(). Set 1 only for real local files.
 *   - lock() should succeed (no-op) for read locks on an immutable/read-only
 *     source and fail for write/edit locks, so editing such a tree is refused.
 *   - An unknown or unloadable scheme makes GetTreeFileIo() return NULL and the
 *     open fail (fail-closed) rather than treating "x://..." as a local path.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tree_file_io
{
  int     (*open)(const char *filename, int options, mode_t mode);
  int     (*close)(int fd);
  ssize_t (*read)(int fd, void *buff, size_t count);
  ssize_t (*write)(int fd, const void *buff, size_t count);
  off_t   (*lseek)(int fd, off_t offset, int whence);
  /* lock: same contract as the legacy io_lock_local fcntl helper.
   * mode_in uses MDS_IO_LOCK_* flags; *deleted (if non-NULL) is set to
   * whether the file has been unlinked. Returns TreeSUCCESS / TreeLOCK_FAILURE. */
  int     (*lock)(int fd, off_t offset, size_t size, int mode_in, int *deleted);
  /* mmap_capable: 1 if fds from open() are real kernel descriptors that
   * MapFile() may mmap(); 0 if MapFile() must use the calloc+read path. */
  int     mmap_capable;
} TreeFileIo;

/* Exported-symbol signature a backend plugin must provide under name "FileIo". */
typedef const TreeFileIo *(*FileIoFn)(void);

/* Returns the backend for `filename`:
 *   - no "scheme://" prefix  -> built-in default backend (never NULL)
 *   - known/loadable scheme  -> plugin backend
 *   - unknown/unloadable     -> NULL  (caller must fail the open: fail-closed) */
const TreeFileIo *GetTreeFileIo(const char *filename);

#ifdef __cplusplus
}
#endif

#endif /* TREEFILEIO_H */
