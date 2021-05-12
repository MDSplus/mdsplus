#pragma once
#include <stdio.h>
#include <time.h>

#define MDSNOP(...) \
  do                \
  { /**/            \
  } while (0)

#define MSG_DEBUG 'D'
#define MSG_INFO 'I'
#define MSG_WARNING 'W'
#define MSG_ERROR 'E'
#define __MDSMSG(LV, ...)                                      \
  do                                                           \
  {                                                            \
    struct timespec ts;                                        \
    clock_gettime(CLOCK_REALTIME, &ts);                        \
    fprintf(stderr, "%c, %u.%09u: %s:%d %s()  ", LV,           \
            (unsigned int)ts.tv_sec, (unsigned int)ts.tv_nsec, \
            __FILE__, __LINE__, __FUNCTION__);                 \
    fprintf(stderr, __VA_ARGS__);                              \
    (LV == MSG_ERROR) ? perror(" ") : fprintf(stderr, "\n");   \
  } while (0)

//#define DEBUG
#ifdef DEBUG
#define MDSDBG(...) __MDSMSG(MSG_DEBUG, __VA_ARGS__)
#else
#define MDSDBG(...) MDSNOP()
#endif
#define MDSMSG(...) __MDSMSG(MSG_INFO, __VA_ARGS__)
#define MDSWRN(...) __MDSMSG(MSG_WARNING, __VA_ARGS__)
#define MDSERR(...) __MDSMSG(MSG_ERROR, __VA_ARGS__)

#ifdef _WIN32
#include <processthreadsapi.h>
#define CURRENT_THREAD_ID() (long)GetCurrentThreadId()
#else
#include <sys/syscall.h>
#define CURRENT_THREAD_ID() syscall(__NR_gettid)
#endif

#define IPADDRPRI "%d.%d.%d.%d"
#define IPADDRVAR(var) (int)(((uint8_t *)var)[0]), (int)(((uint8_t *)var)[1]), (int)(((uint8_t *)var)[2]), (int)(((uint8_t *)var)[3])
