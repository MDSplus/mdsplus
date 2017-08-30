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
/* lock pthread_once */
STATIC_THREADSAFE pthread_mutex_t buffer_key_mutex = PTHREAD_MUTEX_INITIALIZER;
/* Free the thread-specific buffer */
STATIC_ROUTINE void buffer_destroy(void *buf){
  free(buf);
}
STATIC_ROUTINE void buffer_key_alloc(){
  pthread_key_create(&buffer_key, buffer_destroy);
}
/* Return the thread-specific buffer */
MdsShrThreadStatic *MdsShrGetThreadStatic(){
  pthread_mutex_lock(&buffer_key_mutex);
  pthread_once(&buffer_key_once, buffer_key_alloc);
  pthread_mutex_unlock(&buffer_key_mutex);
  MdsShrThreadStatic *p = (MdsShrThreadStatic *) pthread_getspecific(buffer_key);
  if (!p) {
    p = (MdsShrThreadStatic *) malloc(sizeof(MdsShrThreadStatic));
    p->MdsGetMsgDsc_tmp.dtype = DTYPE_T;
    p->MdsGetMsgDsc_tmp.class = CLASS_S;
    p->MdsGetMsgDsc_tmp.pointer = p->MdsGetMsg_text;
    pthread_setspecific(buffer_key, (void *)p);
  }
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
