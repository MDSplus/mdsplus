#include <mdsdescrip.h>
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else
#define pthread_mutex_t HANDLE
#define pthread_once_t int
#define PTHREAD_ONCE_INIT 0
#endif
#ifdef HAVE_WINDOWS_H
#ifndef NO_WINDOWS_H
#include <windows.h>
#endif
#else
#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#endif
#endif


typedef struct _thread_static {
  int TdiGetData_recursion_count;
  int TdiIntrinsic_mess_stat;
  int TdiIntrinsic_recursion_count;
  struct descriptor_d TdiIntrinsic_message;
  struct { 
    void *head;
    void *head_zone;
    void *data_zone;
    int   public;
  } TdiVar_private;
  int TdiVar_new_narg;
  struct descriptor TdiVar_new_narg_d;
  int compiler_recursing;
  struct descriptor *TdiRANGE_PTRS[3];
  struct descriptor_xd *TdiSELF_PTR;
} ThreadStatic;

extern ThreadStatic *TdiThreadStatic();
extern void LockTdiMutex(pthread_mutex_t *, int *);
extern void UnlockTdiMutex(pthread_mutex_t *);
