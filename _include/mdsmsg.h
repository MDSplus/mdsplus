#pragma once
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <processthreadsapi.h>
#define CURRENT_THREAD_ID() (long)GetCurrentThreadId()
#else
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/syscall.h>
#define CURRENT_THREAD_ID() (long)syscall(__NR_gettid)
#endif

#define MDSNOP(...) \
  do                \
  { /**/            \
  } while (0)

#define MSG_DEBUG 'D'
#define MSG_INFO 'I'
#define MSG_WARNING 'W'
#define MSG_ERROR 'E'
#define __MDSMSG(LV, ...)                                 \
  do                                                      \
  {                                                       \
    char msg[1024];                                       \
    int pos;                                              \
    struct timespec ts;                                   \
    clock_gettime(CLOCK_REALTIME, &ts);                   \
    pos = sprintf(msg, "%c, %lu, %u.%09u: %s:%d %s()  ", \
                  LV, CURRENT_THREAD_ID(),                \
                  (unsigned int)ts.tv_sec,                \
                  (unsigned int)ts.tv_nsec,               \
                  __FILE__, __LINE__, __FUNCTION__);      \
    pos += sprintf(msg + pos, __VA_ARGS__);               \
    if (LV == MSG_ERROR)                                  \
    {                                                     \
      perror(msg);                                        \
    }                                                     \
    else                                                  \
    {                                                     \
      strcpy(msg + pos, "\n");                            \
      fputs(msg, stderr);                                 \
    }                                                     \
  } while (0)

//#define DEBUG
//#undef DEBUG
#ifdef MDSDBG
#undef MDSDBG
#endif
#ifdef DEBUG
#define MDSDBG(...) __MDSMSG(MSG_DEBUG, __VA_ARGS__)
#else
#define MDSDBG(...) MDSNOP()
#endif
#define MDSMSG(...) __MDSMSG(MSG_INFO, __VA_ARGS__)
#define MDSWRN(...) __MDSMSG(MSG_WARNING, __VA_ARGS__)
#define MDSERR(...) __MDSMSG(MSG_ERROR, __VA_ARGS__)

#define IPADDRPRI "%d.%d.%d.%d"
#define IPADDRVAR(var) (int)(((uint8_t *)var)[0]), (int)(((uint8_t *)var)[1]), (int)(((uint8_t *)var)[2]), (int)(((uint8_t *)var)[3])
