#ifndef PTHREAD_PORT_H
#define PTHREAD_PORT_H
#define _GNU_SOURCE
#include <stdlib.h>
#include <status.h>
#include <STATICdef.h>
#ifdef _WIN32
#ifndef NO_WINDOWS_H
#include <windows.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else//HAVE_PTHREAD_H
#define pthread_mutex_t HANDLE
#define pthread_cond_t HANDLE
#define pthread_once_t int
typedef void *pthread_t;
#define PTHREAD_ONCE_INIT 0
#ifndef PTHREAD_MUTEX_RECURSIVE
#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif
//#ifndef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
//#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP {0x4000}
//#endif
#endif//HAVE_PTHREAD_H
#else//_WIN32
#include <pthread.h>
#endif//_WIN32

#define DEFAULT_STACKSIZE 0x800000

#if defined(PTHREAD_RECURSIVE_MUTEX_NP) && !defined(PTHREAD_RECURSIVE_MUTEX)
#define PTHREAD_RECURSIVE_MUTEX PTHREAD_RECURSIVE_MUTEX_NP
#endif
#if defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP) && !defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#endif

#if (defined(_DECTHREADS_) && (_DECTHREADS_ != 1)) || !defined(_DECTHREADS_)
#define pthread_attr_default NULL
#define pthread_mutexattr_default NULL
#define pthread_condattr_default NULL
#endif

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

typedef struct _Condition_p {
  pthread_cond_t  cond;
  pthread_mutex_t mutex;
  void*           value;
} Condition_p;

#define CONDITION_INITIALIZER {PTHREAD_COND_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,B_FALSE}

#define CONDITION_INIT(input){\
(input)->value = 0;\
pthread_cond_init(&(input)->cond, pthread_condattr_default);\
pthread_mutex_init(&(input)->mutex, pthread_mutexattr_default);\
}
#define _CONDITION_LOCK(input)   pthread_mutex_lock(&(input)->mutex)
#define _CONDITION_UNLOCK(input) pthread_mutex_unlock(&(input)->mutex)
#define _CONDITION_SIGNAL(input) pthread_cond_signal(&(input)->cond)
#define _CONDITION_WAIT_SET(input) while (!(input)->value) {pthread_cond_wait(&(input)->cond,&(input)->mutex);}
#define _CONDITION_WAIT_1SEC(input,status){\
struct timespec tp;\
clock_gettime(CLOCK_REALTIME, &tp);\
tp.tv_sec++;\
status pthread_cond_timedwait(&(input)->cond,&(input)->mutex,&tp);\
}
#define CONDITION_SET_TO(input,value_in){\
_CONDITION_LOCK(input);\
(input)->value = value_in;\
_CONDITION_SIGNAL(input);\
_CONDITION_UNLOCK(input);\
}
#define CONDITION_SET(input)   CONDITION_SET_TO(input,B_TRUE)
#define CONDITION_RESET(input){\
_CONDITION_LOCK(input);\
(input)->value = 0;\
_CONDITION_UNLOCK(input);\
}
#define CONDITION_WAIT_SET(input){\
_CONDITION_LOCK(input);\
_CONDITION_WAIT_SET(input);\
_CONDITION_UNLOCK(input);\
}
#define CONDITION_WAIT_1SEC(input){\
_CONDITION_LOCK(input);\
_CONDITION_WAIT_1SEC(input,);\
_CONDITION_UNLOCK(input);\
}
#define CONDITION_DESTROY(input){\
_CONDITION_LOCK(input);\
pthread_cond_destroy(&(input)->cond);\
_CONDITION_UNLOCK(input);\
pthread_mutex_destroy(&(input)->mutex);\
}
#define CREATE_DETACHED_THREAD(thread, stacksize, target, args)\
pthread_attr_t attr;\
pthread_attr_init(&attr);\
pthread_attr_setstacksize(&attr, DEFAULT_STACKSIZE stacksize);\
int c_status = pthread_create(&thread, &attr, (void *)target, args);\
pthread_attr_destroy(&attr);\
pthread_detach(thread);

#define CONDITION_START_THREAD(input, thread, stacksize, target, args){\
_CONDITION_LOCK(input);\
if (!(input)->value) {\
  CREATE_DETACHED_THREAD(thread, stacksize, target, args);\
  if (c_status) {\
    perror("Error creating pthread");\
    status = MDSplusERROR;\
  } else {\
    _CONDITION_WAIT_SET(input);\
    status = MDSplusSUCCESS;\
  }\
}\
_CONDITION_UNLOCK(input);\
}
//"
#if defined(__MACH__) || defined(_WIN32)
#define _ALLOC_HP struct hostent *hp;
#define _GETHOSTBYADDR(addr,type) gethostbyaddr(((void *)&addr),sizeof(addr),type)
#define _GETHOSTBYNAME(name)      gethostbyname(name)
#define _GETHOST(gethost) hp = gethost
#define FREE_HP
#else
#define _ALLOC_HP \
int memlen = 1024;\
struct hostent hostbuf, *hp;\
int herr;\
char *hp_mem = (char*)malloc(memlen)

#define _GETHOST(gethost_r) \
while ( hp_mem && (gethost_r == ERANGE) ) {\
  memlen *=2;\
  free(hp_mem);\
  hp_mem = (char*)malloc(memlen);\
}
#define _GETHOSTBYADDR(addr,type) gethostbyaddr_r(((void *)&addr),sizeof(addr),type,&hostbuf,hp_mem,memlen,&hp,&herr)
#define _GETHOSTBYNAME(name)      gethostbyname_r(name,&hostbuf,hp_mem,memlen,&hp,&herr)
#define FREE_HP if (hp_mem) free(hp_mem)
#endif

#define GETHOSTBYADDR(addr,type) \
_ALLOC_HP;\
_GETHOST(_GETHOSTBYADDR(addr,type))

#define GETHOSTBYNAME(name) \
_ALLOC_HP;\
_GETHOST(_GETHOSTBYNAME(name))

#define GETHOSTBYNAMEORADDR(name,addr) \
GETHOSTBYNAME(name);\
if (!hp){\
   addr = inet_addr(name);\
   if (addr != -1)  _GETHOST(_GETHOSTBYADDR(addr,AF_INET));\
}

#ifdef LOAD_INITIALIZESOCKETS
#ifndef _WIN32
#define INITIALIZESOCKETS
#else
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int sockets_initialized = B_FALSE;
#define INITIALIZESOCKETS {\
  pthread_mutex_lock(&mutex);\
  if (!sockets_initialized) {\
    WSADATA wsaData;\
    WORD wVersionRequested;\
    wVersionRequested = MAKEWORD(1, 1);\
    WSAStartup(wVersionRequested, &wsaData);\
    sockets_initialized = B_TRUE;\
  }\
  pthread_mutex_unlock(&mutex);\
}
#endif
#endif

#endif//nPTHREAD_PORT_H
