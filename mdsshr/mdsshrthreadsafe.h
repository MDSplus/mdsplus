#include <mdstypes.h>
#include <mdsdescrip.h>
#ifdef HAVE_WINDOWS_H
#ifndef NO_WINDOWS_H
#include <windows.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else
#define pthread_mutex_t HANDLE
#define pthread_once_t int
#define PTHREAD_ONCE_INIT 0
#endif
#else
#include <pthread.h>
#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#endif
#endif

typedef struct _thread_static {
  char MdsGetMsg_text[1024];
  char MdsMsg_text[1024];
  struct descriptor MdsGetMsgDsc_tmp;
} MdsShrThreadStatic;

extern MdsShrThreadStatic *MdsShrGetThreadStatic();
EXPORT  void LockMdsShrMutex(pthread_mutex_t *, int *);
EXPORT void UnlockMdsShrMutex(pthread_mutex_t *);
