#include <stdio.h>
#include <time.h>

//#define DEBUG
//#undef DEBUG
#ifdef MDSDBG
#undef MDSDBG
#undef __MDSMSGTOFUN
#undef __MDSMSGPREFIX
#endif

#ifdef _WIN32
#include <processthreadsapi.h>
#define CURRENT_THREAD_ID() (long)GetCurrentThreadId()
#elif defined(__APPLE__) || defined(__MACH__)
#define CURRENT_THREAD_ID() (long)0
#else
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/syscall.h>
#define CURRENT_THREAD_ID() (long)syscall(__NR_gettid)
#endif

#if defined(WITH_DEBUG_SYMBOLS) && !defined(__APPLE__) && !defined(__MACH__)
#define __MDSMSGTOFUN 70
#define __MDSMSGPREFIX(LV) (                                  \
    {                                                         \
      pos = __FILE__;                                         \
      while (!strncmp(pos, "../", 3))                         \
        pos += 3;                                             \
      pos = msg + sprintf(msg, "%c, %u:%lu, %u.%09u: %s:%d ", \
                          LV, getpid(), CURRENT_THREAD_ID(),  \
                          (unsigned int)ts.tv_sec,            \
                          (unsigned int)ts.tv_nsec,           \
                          pos, __LINE__);                     \
    })
#else
#define __MDSMSGTOFUN 20
#define __MDSMSGPREFIX(LV)                \
  (msg + sprintf(msg, "%c, %u.%03u: ",    \
                 LV,                      \
                 (unsigned int)ts.tv_sec, \
                 (unsigned int)ts.tv_nsec / 1000000))
#endif

#define MDSNOP(...) \
  do                \
  { /**/            \
  } while (0)

#define MSG_DEBUG 'D'
#define MSG_INFO 'I'
#define MSG_WARNING 'W'
#define MSG_ERROR 'E'

#define __MDSMSGTOMSG (__MDSMSGTOFUN + 30)
#define __MDSMSG(LV, ...)                          \
  do                                               \
  {                                                \
    struct timespec ts;                            \
    clock_gettime(CLOCK_REALTIME, &ts);            \
    char msg[1024];                                \
    char *pos;                                     \
    pos = __MDSMSGPREFIX(LV);                      \
    if (pos < msg + __MDSMSGTOFUN)                 \
    {                                              \
      memset(pos, ' ', msg + __MDSMSGTOFUN - pos); \
      pos = msg + __MDSMSGTOFUN;                   \
    }                                              \
    pos += sprintf(pos, "%s() ", __FUNCTION__);    \
    if (pos < msg + __MDSMSGTOMSG)                 \
    {                                              \
      memset(pos, ' ', msg + __MDSMSGTOMSG - pos); \
      pos = msg + __MDSMSGTOMSG;                   \
    }                                              \
    pos += sprintf(pos, __VA_ARGS__);              \
    if (LV == MSG_ERROR)                           \
    {                                              \
      perror(msg);                                 \
    }                                              \
    else                                           \
    {                                              \
      strcpy(pos, "\n");                           \
      fputs(msg, stderr);                          \
    }                                              \
  } while (0)

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
