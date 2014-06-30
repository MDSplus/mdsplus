#include <config.h>
#include <mdstypes.h>
#include <mdsdescrip.h>

#if HAVE_PTHREAD_H
#include <pthread.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#ifndef HAVE_PTHREAD_H
#define pthread_mutex_t HANDLE
#define pthread_once_t int
#define PTHREAD_ONCE_INIT 0
#endif
#endif
#include "mdsdcl.h"

typedef struct _thread_static {
  struct _mdsdcl_ctrl  ctrl;
} MdsdclThreadStatic;

extern MdsdclThreadStatic *MdsdclGetThreadStatic();
