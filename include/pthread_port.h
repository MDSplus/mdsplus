#ifndef PTHREAD_PORT_H
#define PTHREAD_PORT_H
#include <status.h>
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else//HAVE_PTHREAD_H
#define pthread_mutex_t HANDLE
#define pthread_once_t int
typedef void *pthread_t;
#define PTHREAD_ONCE_INIT 0
#endif//HAVE_PTHREAD_H
#ifdef _WIN32
#ifndef NO_WINDOWS_H
#include <windows.h>
#endif
#else//_WIN32
#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#endif
#endif//_WIN32
#include <time.h>
#include <sys/time.h>
#if defined(__MACH__) && !defined(CLOCK_REALTIME)
#define CLOCK_REALTIME 0
// clock_gettime is not implemented on older versions of OS X (< 10.12).
// If implemented, CLOCK_REALTIME will have already been defined.
#define clock_gettime(clk_id_unused, timespec) {\
struct timeval now;\
int rv = gettimeofday(&now, NULL);\
(timespec)->tv_sec  = rv ? 0 : now.tv_sec;\
(timespec)->tv_nsec = rv ? 0 : now.tv_usec * 1000;\
}
#endif

typedef struct _Condition {
  pthread_cond_t  cond;
  pthread_mutex_t mutex;
  int             value;
} Condition;

#define CONDITION_INITIALIZER {PTHREAD_COND_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,B_FALSE}

#define CONDITION_INIT(input){\
Condition *c = (Condition*)input; \
c->value = B_FALSE; \
pthread_cond_init(&c->cond, pthread_condattr_default); \
pthread_mutex_init(&c->mutex, pthread_mutexattr_default); \
}
#define CONDITION_RESET(input){\
Condition *c = (Condition*)input; \
pthread_mutex_lock(&c->mutex); \
c->value = B_FALSE; \
pthread_mutex_unlock(&c->mutex); \
}
#define CONDITION_SIGNAL(input){\
Condition *c = (Condition*)input; \
pthread_mutex_lock(&c->mutex); \
c->value = B_TRUE; \
pthread_cond_signal(&c->cond); \
pthread_mutex_unlock(&c->mutex); \
}
#define CONDITION_WAIT(input){\
Condition *c = (Condition*)input; \
pthread_mutex_lock(&c->mutex); \
while (!c->value) \
  pthread_cond_wait(&c->cond,&c->mutex); \
pthread_mutex_unlock(&c->mutex); \
}
#define CONDITION_WAIT_1SEC(input){\
Condition *c = (Condition*)input; \
pthread_mutex_lock(&c->mutex); \
if (!c->value){ \
  struct timespec tp; \
  clock_gettime(CLOCK_REALTIME, &tp); \
  tp.tv_sec++; \
  pthread_cond_timedwait(&c->cond,&c->mutex,&tp); \
} \
pthread_mutex_unlock(&c->mutex); \
}
#define CONDITION_DESTROY(input){\
Condition *c = (Condition*)input; \
pthread_mutex_lock(&c->mutex); \
pthread_cond_destroy(&c->cond); \
pthread_mutex_unlock(&c->mutex); \
pthread_mutex_destroy(&c->mutex); \
}
#define CREATE_DETACHED_THREAD(thread, stacksize, target, args)\
size_t ssize;\
pthread_attr_t attr;\
pthread_attr_init(&attr);\
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);\
pthread_attr_getstacksize(&attr, &ssize);\
pthread_attr_setstacksize(&attr, ssize stacksize);\
int c_status = pthread_create(&thread, &attr, (void *)target, args);\
pthread_attr_destroy(&attr);

#define CONDITION_START_THREAD(input, thread, stacksize, target, args){\
Condition *c = (Condition*)&input; \
pthread_mutex_lock(&c->mutex); \
if (!c->value) { \
  CREATE_DETACHED_THREAD(thread, stacksize, target, args); \
  if (c_status) { \
    perror("Error creating pthread"); \
    status = MDSplusERROR; \
  } else { \
    pthread_cond_wait(&c->cond,&c->mutex); \
    status = MDSplusSUCCESS; \
  } \
} \
pthread_mutex_unlock(&c->mutex); \
}
#endif//nPTHREAD_PORT_H
