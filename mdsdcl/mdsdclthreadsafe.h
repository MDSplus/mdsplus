#include <mdsdescrip.h>
#include "mdsdcl.h"
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
  struct _mdsdcl_ctrl  ctrl;
} MdsdclThreadStatic;

extern MdsdclThreadStatic *MdsdclGetThreadStatic();
