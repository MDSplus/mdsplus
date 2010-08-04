#include <mdsdescrip.h>
#include "treeshrp.h"
#ifdef HAVE_WINDOWS_H
#ifndef NO_WINDOWS_H
#include <windows.h>
#endif
#define pthread_mutex_t HANDLE
#define pthread_once_t int
#define PTHREAD_ONCE_INIT 0
#else
#ifdef HAVE_VXWORKS_H
#define pthread_mutex_t int
#define pthread_key_t void *
#define pthread_once_t int
#define PTHREAD_ONCE_INIT 0
#else
#ifndef HAVE_VXWORKS_H
#include <pthread.h>
#endif
#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#endif
#endif
#endif


typedef struct _thread_static {
  void *DBID;
  int privateCtx;
  SEARCH_CONTEXT search_ctx[MAX_SEARCH_LEVELS];
  _int64 ViewDate;
  int nid_reference;
  int path_reference;
} TreeThreadStatic;

extern TreeThreadStatic *TreeGetThreadStatic();
extern void **TreeCtx();
extern int TreeUsePrivateCtx(int onoff);
#ifdef HAVE_VXWORKS_H
#define NULL 0
extern void LockTreeMutex(int *, int *);
extern void UnlockTreeMutex(int *);
#else
extern void LockTreeMutex(pthread_mutex_t *, int *);
extern void UnlockTreeMutex(pthread_mutex_t *);
#endif
