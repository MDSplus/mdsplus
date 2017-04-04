#include <config.h>
#include <status.h>
#include <libroutines.h>
#include <STATICdef.h>
#include "treethreadsafe.h"
#include <stdlib.h>
#include <mdsshr.h>
#include <strroutines.h>
#include <string.h>

/* Free the thread-specific buffer */
STATIC_ROUTINE void buffer_destroy(void *buf){
  if (buf) free(buf);
}

/* Return the thread-specific buffer */
TreeThreadStatic *TreeGetThreadStatic(){
  static pthread_key_t buffer_key;
  static int is_init = B_FALSE;
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  TreeThreadStatic *p;
  pthread_mutex_lock(&mutex);
  if (!is_init) {
    pthread_key_create(&buffer_key, buffer_destroy);
    is_init = B_TRUE;
  }
  p = (TreeThreadStatic *) pthread_getspecific(buffer_key);
  if (p == NULL) {
    p = (TreeThreadStatic *) memset(malloc(sizeof(TreeThreadStatic)), 0, sizeof(TreeThreadStatic));
    pthread_setspecific(buffer_key, (void *)p);
  }
  pthread_mutex_unlock(&mutex);
  return p;
}

void *DBID = NULL;

static pthread_rwlock_t treectx_lock = PTHREAD_RWLOCK_INITIALIZER;
EXPORT void **TreeCtx(){
  TreeThreadStatic *p = TreeGetThreadStatic();
  pthread_rwlock_rdlock(&treectx_lock);
  void **old_dbid = p->privateCtx ? &p->DBID : &DBID;
  pthread_rwlock_unlock(&treectx_lock);
  return old_dbid;
}

EXPORT void *TreeSwitchDbid(void *dbid){
  TreeThreadStatic *p = TreeGetThreadStatic();
  pthread_rwlock_wrlock(&treectx_lock);
  void *old_dbid = p->privateCtx ? p->DBID : DBID;
  if (p->privateCtx) p->DBID = dbid;
  else                  DBID = dbid;
  pthread_rwlock_unlock(&treectx_lock);
  return old_dbid;
}

EXPORT int TreeUsePrivateCtx(int onoff){
  TreeThreadStatic *p = TreeGetThreadStatic();
  int old = p->privateCtx;
  p->privateCtx = onoff!=0;
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
