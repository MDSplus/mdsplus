#pragma once
//#define DEBUG
#ifdef DEBUG
#define DBG(...)                                                       \
  do                                                                   \
  {                                                                    \
    fprintf(stderr, "%s:%d %s()  ", __FILE__, __LINE__, __FUNCTION__); \
    fprintf(stderr, __VA_ARGS__);                                      \
  } while (0)
#else
#define DBG(...) \
  { /**/         \
  }
#endif

#define PID ((intptr_t)pthread_self())
#define IPADDRPRI "%d.%d.%d.%d"
#define IPADDRVAR(var) (int)(((uint8_t *)var)[0]), (int)(((uint8_t *)var)[1]), (int)(((uint8_t *)var)[2]), (int)(((uint8_t *)var)[3])
