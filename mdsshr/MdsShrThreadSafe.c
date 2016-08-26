#define _GNU_SOURCE
#include <config.h>
#include <STATICdef.h>
#include <mdsshr.h>
#include "mdsshrthreadsafe.h"
#include <stdlib.h>

/* Key for the thread-specific buffer */
STATIC_THREADSAFE pthread_key_t buffer_key;

/* Once-only initialisation of the key */
STATIC_THREADSAFE pthread_once_t buffer_key_once = PTHREAD_ONCE_INIT;

STATIC_ROUTINE void buffer_key_alloc();

#if defined _MSC_VER
// Emulate pthreads for Visual C applications
#include <process.h>
void pthread_detach(HANDLE * thread)
{
  return;
}

int pthread_cond_init(HANDLE * cond, void *def)
{
  *cond = CreateEvent(NULL, TRUE, FALSE, NULL);
  return (*cond == NULL);
}

BOOL pthread_cond_destroy(HANDLE * cond)
{
  return CloseHandle(*cond);
}

int pthread_cond_signal(HANDLE * cond)
{
  BOOL status;
#ifdef ___DEBUG_IT
  printf("signalling event %p\n", *cond);
#endif
  status = SetEvent(*cond);
#ifdef ___DEBUG_IT
  printf("SetEvent on %p completed with status = %d\n", *cond, status);
#endif
  if (status)
    status = ResetEvent(*cond);
#ifdef ___DEBUG_IT
  printf("ResetEvent on %p completed with status = %d\n", *cond, status);
#endif
  return status == 0;
}

int pthread_cond_wait(HANDLE * cond, HANDLE * mutex)
{
  int status;
#ifdef ___DEBUG_IT
  printf("waiting for condition %p\n", *cond);
#endif
  pthread_mutex_unlock(mutex);
  status = WaitForSingleObject(*cond, INFINITE);
  pthread_mutex_lock(mutex);
#ifdef ___DEBUG_IT
  printf("got condition %p\n", *cond);
#endif
  return (status == WAIT_FAILED);
}

#define ETIMEDOUT 138
int pthread_cond_timedwait(HANDLE * cond, HANDLE * mutex, int msec)
{
  int status;
  pthread_mutex_unlock(mutex);
  status = WaitForSingleObject(*cond, msec);
  pthread_mutex_lock(mutex);
  if (status == WAIT_TIMEOUT)
    status = ETIMEDOUT;
  else
    status = 0;
  return status;
}

int pthread_mutex_init(HANDLE * mutex, void *dummy)
{
  *mutex = CreateMutex(0, FALSE, NULL);
  return (*mutex == NULL);
}

BOOL pthread_mutex_destroy(HANDLE * mutex)
{
  return CloseHandle(*mutex);
}

int pthread_exit(int status)
{
  return status;
}

int pthread_create(pthread_t * thread, void *dummy, void *(*rtn) (void *), void *rtn_param)
{
  *thread = (pthread_t) _beginthread((void (*)(void *))rtn, 0, rtn_param);
  return *thread == 0;
}

void pthread_cleanup_pop()
{
}

void pthread_cleanup_push()
{
}

int pthread_mutex_lock(HANDLE * mutex)
{
  int status;
#ifdef ___DEBUG_IT
  printf("Trying to lock mutex %p\n", *mutex);
#endif
  status = WaitForSingleObject(*mutex, INFINITE);
#ifdef ___DEBUG_IT
  printf("Locked mutex %p\n", *mutex);
#endif
  return status;
}

int pthread_mutex_unlock(HANDLE * mutex)
{
  int status;
#ifdef ___DEBUG_IT
  printf("Unlocking mutex %p\n", *mutex);
#endif
  status = ReleaseMutex(*mutex);
#ifdef ___DEBUG_IT
  printf("Unlocked mutex %p with status=%d\n", *mutex, status);
#endif
  return status;
}

void pthread_cancel(HANDLE thread)
{
  printf("Abort not supported");
}

void pthread_once(pthread_once_t * one_time, void (*key_alloc) ())
{
  if (*one_time == PTHREAD_ONCE_INIT) {
    *one_time = 1;
    key_alloc();
  }
}

void *pthread_getspecific(pthread_key_t buffer_key)
{
  return TlsGetValue(buffer_key);
}

void pthread_setspecific(pthread_key_t buffer_key, void *p)
{
  TlsSetValue(buffer_key, p);
}

void pthread_key_create(pthread_key_t * buffer_key, void *d2)
{
  *buffer_key = TlsAlloc();
}
#endif

/* Return the thread-specific buffer */
MdsShrThreadStatic *MdsShrGetThreadStatic()
{
  MdsShrThreadStatic *p;
  pthread_once(&buffer_key_once, buffer_key_alloc);
  p = (MdsShrThreadStatic *) pthread_getspecific(buffer_key);
  if (p == NULL) {
    p = (MdsShrThreadStatic *) malloc(sizeof(MdsShrThreadStatic));
    p->MdsGetMsgDsc_tmp.dtype = DTYPE_T;
    p->MdsGetMsgDsc_tmp.class = CLASS_S;
    p->MdsGetMsgDsc_tmp.pointer = p->MdsGetMsg_text;
    pthread_setspecific(buffer_key, (void *)p);
  }
  return p;
}

/* Free the thread-specific buffer */
STATIC_ROUTINE void buffer_destroy(void *buf)
{
  if (buf != NULL) {
    //MdsShrThreadStatic *ts = (MdsShrThreadStatic *) buf;
    free(buf);
  }
}

/* Allocate the key */
STATIC_ROUTINE void buffer_key_alloc()
{
  pthread_key_create(&buffer_key, buffer_destroy);
}

#ifndef _MSC_VER
pthread_mutex_t initMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void LockMdsShrMutex(pthread_mutex_t * mutex, int *initialized)
{
#ifndef _MSC_VER
  pthread_mutex_lock(&initMutex);
#endif
  if (!*initialized) {
#ifndef _MSC_VER
    pthread_mutexattr_t m_attr;
    pthread_mutexattr_init(&m_attr);
#if !defined(PTHREAD_MUTEX_RECURSIVE)
#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif
#ifndef xxxx__sun
    pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
#endif
    pthread_mutex_init(mutex, &m_attr);
#else
    pthread_mutex_init(mutex, NULL);
#endif
    *initialized = 1;
  }
#ifndef _MSC_VER
  pthread_mutex_unlock(&initMutex);
#endif
  pthread_mutex_lock(mutex);

}

void UnlockMdsShrMutex(pthread_mutex_t * mutex)
{
  pthread_mutex_unlock(mutex);
}

#ifdef _MSC_VER
STATIC_THREADSAFE HANDLE global_mutex = NULL;
#else
STATIC_THREADSAFE pthread_mutex_t global_mutex;
#endif

STATIC_THREADSAFE int global_mutex_initialized = 0;
EXPORT void MdsGlobalUnlock()
{
  if (!global_mutex_initialized) {
    global_mutex_initialized = 1;
    pthread_mutex_init(&global_mutex, NULL);
  }
  pthread_mutex_unlock(&global_mutex);

}

EXPORT void MdsGlobalLock()
{
  if (!global_mutex_initialized) {
    global_mutex_initialized = 1;
    pthread_mutex_init(&global_mutex, NULL);
  }
  pthread_mutex_lock(&global_mutex);
}
