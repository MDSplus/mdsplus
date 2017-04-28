#define _GNU_SOURCE
#include <config.h>
#include <STATICdef.h>
#include <mdsshr.h>
#include "mdsshrthreadsafe.h"
#include <stdlib.h>

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
MdsShrThreadStatic *MdsShrGetThreadStatic(){
  MdsShrThreadStatic *p;
  RDLOCK_BUFFER;
  if (!is_init) {
    UNLOCK_BUFFER;
    WRLOCK_BUFFER;
    pthread_key_create(&buffer_key, buffer_destroy);
    is_init = B_TRUE;
  }
  p = (MdsShrThreadStatic *) pthread_getspecific(buffer_key);
  if (!p) {
    p = (MdsShrThreadStatic *) malloc(sizeof(MdsShrThreadStatic));
    p->MdsGetMsgDsc_tmp.dtype = DTYPE_T;
    p->MdsGetMsgDsc_tmp.class = CLASS_S;
    p->MdsGetMsgDsc_tmp.pointer = p->MdsGetMsg_text;
    pthread_setspecific(buffer_key, (void *)p);
  }
  UNLOCK_BUFFER;
  return p;
}

STATIC_THREADSAFE pthread_mutex_t initMutex = PTHREAD_MUTEX_INITIALIZER;
void LockMdsShrMutex(pthread_mutex_t * mutex, int *initialized)
{
  pthread_mutex_lock(&initMutex);
  if (!*initialized) {
    pthread_mutexattr_t m_attr;
    pthread_mutexattr_init(&m_attr);
    pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(mutex, &m_attr);
    *initialized = 1;
  }
  pthread_mutex_unlock(&initMutex);
  pthread_mutex_lock(mutex);
}

void UnlockMdsShrMutex(pthread_mutex_t * mutex){
  pthread_mutex_unlock(mutex);
}

STATIC_THREADSAFE pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
EXPORT void MdsGlobalUnlock(){
  pthread_mutex_unlock(&global_mutex);

}

EXPORT void MdsGlobalLock(){
  pthread_mutex_lock(&global_mutex);
}
