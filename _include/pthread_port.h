#ifndef PTHREAD_PORT_H
#define PTHREAD_PORT_H
#define NOP() \
  do          \
  {           \
  } while (0)

#define _GNU_SOURCE
#include <STATICdef.h>
#include <status.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <pthread.h>

#define DEFAULT_STACKSIZE 0x800000

// helper for reentrant mutex
#ifndef PTHREAD_MUTEX_RECURSIVE
#ifdef PTHREAD_MUTEX_RECURSIVE_NP
#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif
#endif
#ifndef PTHREAD_RECURSIVE_MUTEX_INITIALIZER
#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER \
  PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#endif
#endif
#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER
#define STATIC_PTHREAD_RECURSIVE_MUTEX_DEF(name) \
  static pthread_mutex_t name = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#define STATIC_PTHREAD_RECURSIVE_MUTEX_INIT(name)
#else
#define STATIC_PTHREAD_RECURSIVE_MUTEX_DEF(name)                   \
  static pthread_mutex_t name;                                     \
  static pthread_once_t __##name##__once = PTHREAD_ONCE_INIT;      \
  static void init__##name##__once()                               \
  {                                                                \
    pthread_mutexattr_t attr;                                      \
    if (pthread_mutexattr_init(&attr))                             \
      abort();                                                     \
    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) \
      abort();                                                     \
    pthread_mutex_init(&name, &attr);                              \
    pthread_mutexattr_destroy(&attr);                              \
  }
#define STATIC_PTHREAD_RECURSIVE_MUTEX_INIT(name) \
  pthread_once(&__##name##__once, init__##name##__once);
#endif

#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#endif

#include <sys/time.h>
#include <time.h>

#if defined(__MACH__) && !defined(CLOCK_REALTIME)
#define CLOCK_REALTIME 0
// clock_gettime is not implemented on older versions of OS X (< 10.12).
// If implemented, CLOCK_REALTIME will have already been defined.
#define clock_gettime(clk_id_unused, timespec)         \
  {                                                    \
    struct timeval now;                                \
    int rv = gettimeofday(&now, NULL);                 \
    (timespec)->tv_sec = rv ? 0 : now.tv_sec;          \
    (timespec)->tv_nsec = rv ? 0 : now.tv_usec * 1000; \
  }
#endif

// FREE
static void __attribute__((unused)) free_if(void *ptr) { free(*(void **)ptr); }
#define FREE_ON_EXIT(ptr) pthread_cleanup_push(free_if, (void *)&ptr)
#define FREE_IF(ptr, c) pthread_cleanup_pop(c);
#define FREE_NOW(ptr) FREE_IF(ptr, 1)
#define FREE_CANCEL(ptr) FREE_IF(ptr, 0)
#define INIT_AND_FREE_ON_EXIT(type, ptr) \
  type ptr = NULL;                       \
  FREE_ON_EXIT(ptr)
#define INIT_AS_AND_FREE_ON_EXIT(type, ptr, value) \
  INIT_AND_FREE_ON_EXIT(type, ptr);                \
  ptr = (type)value;

// FCLOSE
#include <stdio.h>
static void __attribute__((unused)) fclose_if(void *ptr)
{
  if (*(FILE **)ptr)
    fclose(*(FILE **)ptr);
}
#define FCLOSE_ON_EXIT(ptr) pthread_cleanup_push(fclose_if, (void *)&ptr)
#define FCLOSE_IF(ptr, c) pthread_cleanup_pop(c)
#define FCLOSE_NOW(ptr) FCLOSE_IF(ptr, 1)
#define FCLOSE_CANCEL(ptr) FCLOSE_IF(ptr, 0)
#define INIT_AS_AND_FCLOSE_ON_EXIT(ptr, value) \
  FILE *ptr = value;                           \
  FCLOSE_ON_EXIT(ptr)
#define INIT_AND_FCLOSE_ON_EXIT(ptr) INIT_AS_AND_FCLOSE_ON_EXIT(ptr, NULL)

#define INIT_SHARED_FUNCTION_ONCE(fun) \
  static pthread_once_t __##fun##__once = PTHREAD_ONCE_INIT
#define RUN_SHARED_FUNCTION_ONCE(fun) \
  pthread_once(&__##fun##__once, (void *)fun)
#define RUN_FUNCTION_ONCE(fun)      \
  do                                \
  {                                 \
    INIT_SHARED_FUNCTION_ONCE(fun); \
    RUN_SHARED_FUNCTION_ONCE(fun);  \
  } while (0)

#endif // PTHREAD_PORT_H
