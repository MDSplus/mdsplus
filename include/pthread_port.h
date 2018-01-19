#ifndef PTHREAD_PORT_H
#define PTHREAD_PORT_H
#define _GNU_SOURCE
#include <stdlib.h>
#include <status.h>
#include <STATICdef.h>
#ifdef _WIN32
 #ifndef NO_WINDOWS_H
  #ifdef LOAD_INITIALIZESOCKETS
   #include <winsock2.h>
  #endif
  #include <windows.h>
 #endif
#endif
#include <pthread.h>

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
#ifdef DEF_FREEBEGIN
 static void __attribute__((unused)) freebegin(void* ptr){
   if (((struct TdiZoneStruct*)ptr)->a_begin) {
     free(((struct TdiZoneStruct*)ptr)->a_begin);
     ((struct TdiZoneStruct*)ptr)->a_begin=NULL;
   }
 }
  #define FREEBEGIN_ON_EXIT() pthread_cleanup_push(freebegin,&TdiRefZone)
  #define FREEBEGIN_NOW()     pthread_cleanup_pop(1)
#endif

#ifdef DEF_FREED
 #include <strroutines.h>
 static void __attribute__((unused)) free_d(void *ptr){
   StrFree1Dx((struct descriptor_d*)ptr);
 }
  #define FREED_ON_EXIT(ptr) pthread_cleanup_push(free_d, ptr)
  #define FREED_IF(ptr,c)    pthread_cleanup_pop(c)
  #define FREED_NOW(ptr)     pthread_cleanup_pop(1)
 #define INIT_AS_AND_FREED_ON_EXIT(var,value) struct descriptor_d var=value;FREED_ON_EXIT(&var);
 #define INIT_AND_FREED_ON_EXIT(dtype,var)    INIT_AS_AND_FREED_ON_EXIT(var, ((struct descriptor_d){ 0, dtype, CLASS_D, 0 }))
#endif
#ifdef DEF_FREEXD
 #include <mdsshr.h>
 static void __attribute__((unused)) free_xd(void *ptr){
   MdsFree1Dx((struct descriptor_xd*)ptr, NULL);
 }
  #define FREEXD_ON_EXIT(ptr) pthread_cleanup_push(free_xd, ptr)
  #define FREEXD_IF(ptr,c)    pthread_cleanup_pop(c)
  #define FREEXD_NOW(ptr)     pthread_cleanup_pop(1)
 #define INIT_AND_FREEXD_ON_EXIT(xd) EMPTYXD(xd);FREEXD_ON_EXIT(&xd);
#endif
static void __attribute__((unused)) free_if(void *ptr){
  if (*(void**)ptr) free(*(void**)ptr);
}
 #define FREE_ON_EXIT(ptr)   pthread_cleanup_push(free_if, (void*)&ptr)
 #define FREE_IF(ptr,c)      pthread_cleanup_pop(c)
 #define FREE_NOW(ptr)       pthread_cleanup_pop(1)
 #define FREE_CANCEL(ptr)    pthread_cleanup_pop(0)
#define INIT_AS_AND_FREE_ON_EXIT(type,ptr,value) type ptr = value;FREE_ON_EXIT(ptr)
#define INIT_AND_FREE_ON_EXIT(type,ptr) INIT_AS_AND_FREE_ON_EXIT(type,ptr,NULL)

#define CONDITION_INITIALIZER {PTHREAD_COND_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,B_FALSE}

#define CONDITION_INIT(input){\
(input)->value = 0;\
pthread_cond_init(&(input)->cond, pthread_condattr_default);\
pthread_mutex_init(&(input)->mutex, pthread_mutexattr_default);\
}
#define _CONDITION_LOCK(input)   pthread_mutex_lock(&(input)->mutex)
#define _CONDITION_UNLOCK(input) pthread_mutex_unlock(&(input)->mutex)
#define _CONDITION_SIGNAL(input) pthread_cond_signal(&(input)->cond)
#define _CONDITION_WAIT(input)   pthread_cond_wait(&(input)->cond,&(input)->mutex)
#define _CONDITION_WAIT_SET(input)   while (!(input)->value) _CONDITION_WAIT(input)
#define _CONDITION_WAIT_RESET(input) while ( (input)->value) _CONDITION_WAIT(input)
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
#define CONDITION_RESET(input) CONDITION_SET_TO(input,0)
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
#ifdef LOAD_INITIALIZESOCKETS
 #ifndef _WIN32
  #define INITIALIZESOCKETS
 #else
  static pthread_once_t InitializeSockets_once = PTHREAD_ONCE_INIT;
  static void InitializeSockets() {
    WSADATA wsaData;
    WORD wVersionRequested;
    wVersionRequested = MAKEWORD(1, 1);
    WSAStartup(wVersionRequested, &wsaData);
  }
  #define INITIALIZESOCKETS pthread_once(&InitializeSockets_once,InitializeSockets)
 #endif
#endif

#ifdef LOAD_GETUSERNAME
#define GETUSERNAME(user_p) GETUSERNAME_BEGIN(user_p);GETUSERNAME_END;

#define GETUSERNAME_BEGIN(user_p) {\
static pthread_mutex_t username_mutex = PTHREAD_MUTEX_INITIALIZER;\
pthread_mutex_lock(&username_mutex);\
if (!user_p) {\
  user_p = _getUserName()

#define GETUSERNAME_END }\
pthread_mutex_unlock(&username_mutex);\
}
static char* _getUserName(){
  char *user_p;
#ifdef _WIN32
    static char user[128];
    DWORD bsize = 128;
    user_p = GetUserName(user, &bsize) ? user : "Windows User";
#elif __MWERKS__
    ans.pointer = "Macintosh User";
#else
    static char user[256];
    struct passwd *pwd;
    pwd = getpwuid(geteuid());
    if (pwd) {
      strcpy(user,pwd->pw_name);
      user_p = user;
    } else
#ifdef __APPLE__
      user_p = "Apple User";
#else
    {
      user_p = getlogin();
      if (user_p && strlen(user_p)>0){
        strcpy(user,user_p);
        user_p = user;
      } else
        user_p = "Linux User";
    }
#endif
#endif
  return user_p;
}
#endif

#define RUN_FUNCTION_ONCE(fun) do{ \
  static pthread_once_t RUN_FUNCTION_once = PTHREAD_ONCE_INIT; \
  static pthread_mutex_t RUN_FUNCTION_lock = PTHREAD_MUTEX_INITIALIZER; \
  pthread_mutex_lock(&RUN_FUNCTION_lock); \
  pthread_cleanup_push((void*)pthread_mutex_unlock,&RUN_FUNCTION_lock); \
  pthread_once(&RUN_FUNCTION_once,fun); \
  pthread_cleanup_pop(1); \
}while(0)


#endif//nPTHREAD_PORT_H
