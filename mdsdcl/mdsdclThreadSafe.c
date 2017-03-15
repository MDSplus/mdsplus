#define _GNU_SOURCE
#include <config.h>
#include <STATICdef.h>
#include <mdsshr.h>
#include "mdsdclthreadsafe.h"
#include <stdlib.h>
#include <string.h>
/* Key for the thread-specific buffer */
STATIC_THREADSAFE int is_init = B_FALSE;
STATIC_THREADSAFE pthread_key_t buffer_key;
/* Once-only initialisation of the key */
STATIC_THREADSAFE pthread_rwlock_t buffer_lock   = PTHREAD_RWLOCK_INITIALIZER;
#define WRLOCK_BUFFER pthread_rwlock_wrlock(&buffer_lock);
#define RDLOCK_BUFFER pthread_rwlock_rdlock(&buffer_lock);
#define UNLOCK_BUFFER pthread_rwlock_unlock(&buffer_lock);

/* Free the thread-specific buffer */
STATIC_ROUTINE void buffer_destroy(void *buf){
  if (buf) {
    ThreadStatic *ThreadStatic_p = (ThreadStatic *)buf;
    if (ThreadStatic_p) {
      if (PROMPT) free(PROMPT);
      if (DEF_FILE) free(DEF_FILE);
    }
    free(buf);
  }
}
/* Return the thread-specific buffer */
ThreadStatic *mdsdclGetThreadStatic(){
  ThreadStatic *p;
  RDLOCK_BUFFER;
  if (!is_init) {
    UNLOCK_BUFFER;
    WRLOCK_BUFFER;
    pthread_key_create(&buffer_key, buffer_destroy);
    is_init = B_TRUE;
  }
  p = (ThreadStatic *) pthread_getspecific(buffer_key);
  if (p == NULL) {
    p = (ThreadStatic *) memset(calloc(1, sizeof(ThreadStatic)), 0, sizeof(ThreadStatic));
    pthread_setspecific(buffer_key, (void *)p);
  }
  UNLOCK_BUFFER;
  return p;
}

void mdsdclSetPrompt(const char *prompt){
  GET_THREADSTATIC_P;
  if (PROMPT) free(PROMPT);
  PROMPT = strdup(prompt);
}

char *mdsdclGetPrompt(){
  char *ans;
  GET_THREADSTATIC_P;
  if (!PROMPT)
    PROMPT = strdup("Command> ");
  ans = strdup(PROMPT);
  return ans;
}

void mdsdclSetDefFile(const char *deffile){
  GET_THREADSTATIC_P;
  if (DEF_FILE) free(DEF_FILE);
  if (deffile[0] == '*')
    DEF_FILE = strdup(deffile + 1);
  else
    DEF_FILE = strdup(deffile);
}

#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER
static pthread_mutex_t dcl_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
void dclLock()  {pthread_mutex_lock  (&dcl_mutex);}
#else
static pthread_mutex_t dcl_mutex;
void dclLock(){
  static pthread_mutex_t initMutex = PTHREAD_MUTEX_INITIALIZER;
  static int initialized = 0;
  pthread_mutex_lock(&initMutex);
  if (!initialized) {
    pthread_mutexattr_t m_attr;
    pthread_mutexattr_init(&m_attr);
    pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&dcl_mutex, &m_attr);
    pthread_mutexattr_destroy(&m_attr);
    initialized = 1;
  }
  pthread_mutex_unlock(&initMutex);
  pthread_mutex_lock(&dcl_mutex);
}
#endif
void dclUnlock(){
  pthread_mutex_unlock(&dcl_mutex);
}

