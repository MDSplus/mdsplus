#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else
#define pthread_mutex_t HANDLE
#define pthread_once_t int
#define PTHREAD_ONCE_INIT 0
#endif
#ifdef _WIN32
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

