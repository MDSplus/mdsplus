#include <config.h>
#include <status.h>
#include <libroutines.h>
#include <STATICdef.h>
#include "treethreadsafe.h"
#include <stdlib.h>
#include <mdsshr.h>
#include <strroutines.h>
#include <string.h>
/* Key for the thread-specific buffer */
STATIC_THREADSAFE pthread_key_t buffer_key;

/* Once-only initialisation of the key */
STATIC_THREADSAFE int is_init = B_FALSE;
STATIC_THREADSAFE pthread_rwlock_t buffer_lock = PTHREAD_RWLOCK_INITIALIZER;
#define WRLOCK_BUFFER pthread_rwlock_wrlock(&buffer_lock);
#define RDLOCK_BUFFER pthread_rwlock_rdlock(&buffer_lock);
#define UNLOCK_BUFFER pthread_rwlock_unlock(&buffer_lock);

/* Free the thread-specific buffer */
STATIC_ROUTINE void buffer_destroy(void *buf){
  if (buf) free(buf);
}

/* Return the thread-specific buffer */
TreeThreadStatic *TreeGetThreadStatic(){
  TreeThreadStatic *p;
  RDLOCK_BUFFER;
  if (!is_init) {
    UNLOCK_BUFFER;
    WRLOCK_BUFFER;
    pthread_key_create(&buffer_key, buffer_destroy);
    is_init = B_TRUE;
  }
  p = (TreeThreadStatic *) pthread_getspecific(buffer_key);
  if (p == NULL) {
    p = (TreeThreadStatic *) memset(malloc(sizeof(TreeThreadStatic)), 0, sizeof(TreeThreadStatic));
    pthread_setspecific(buffer_key, (void *)p);
  }
  UNLOCK_BUFFER;
  return p;
}

void *DBID = 0;

EXPORT void **TreeCtx(){
  TreeThreadStatic *p = TreeGetThreadStatic();
  return (p->privateCtx == 1) ? &p->DBID : &DBID;
}

EXPORT int TreeUsePrivateCtx(int onoff){
  TreeThreadStatic *p = TreeGetThreadStatic();
  int old = p->privateCtx;
  p->privateCtx = onoff;
  return old;
}

EXPORT int TreeUsingPrivateCtx(){
  TreeThreadStatic *p = TreeGetThreadStatic();
  return p->privateCtx;
}

void LockTreeMutex(pthread_mutex_t * mutex, int *initialized){
  if (!*initialized) {
#ifdef HAVE_PTHREADS_H
    pthread_mutexattr_t m_attr;
    pthread_mutexattr_init(&m_attr);
#ifndef xxxx__sun
    pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
#endif
    pthread_mutex_init(mutex, &m_attr);
    pthread_mutexattr_destroy(&m_attr);
#endif
    *initialized = 1;
  }
  pthread_mutex_lock(mutex);
}

void UnlockTreeMutex(pthread_mutex_t * mutex){
  pthread_mutex_unlock(mutex);
}
