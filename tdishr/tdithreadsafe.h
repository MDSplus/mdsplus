#include <mdsdescrip.h>
#ifdef HAVE_WINDOWS_H
#ifndef NO_WINDOWS_H
#include <windows.h>
#endif
#define pthread_mutex_t HANDLE
#define pthread_key_t void *
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
} ThreadStatic;

#ifdef HAVE_VXWORKS_H
#define NULL 0
extern ThreadStatic *TdiThreadStatic();
extern void LockTdiMutex(int *, int *);
extern void UnlockTdiMutex(int *);
#else
extern ThreadStatic *TdiThreadStatic();
extern void LockTdiMutex(pthread_mutex_t *, int *);
extern void UnlockTdiMutex(pthread_mutex_t *);
#endif