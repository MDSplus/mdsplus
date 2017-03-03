#define _GNU_SOURCE
#include <config.h>
#include <libroutines.h>
#include <STATICdef.h>
#include "tdithreadsafe.h"
#include <stdlib.h>
#include <mdsshr.h>
#include <strroutines.h>
#include <string.h>
/* Key for the thread-specific buffer */
STATIC_THREADSAFE pthread_key_t buffer_key;

/* Once-only initialisation of the key */
STATIC_THREADSAFE pthread_once_t buffer_key_once = PTHREAD_ONCE_INIT;
STATIC_THREADSAFE pthread_rwlock_t buffer_key_mutex = PTHREAD_RWLOCK_INITIALIZER;

void lock_buffer_key() {
  pthread_rwlock_wrlock(&buffer_key_mutex);
}

void unlock_buffer_key() {
  pthread_rwlock_unlock(&buffer_key_mutex);
}


STATIC_ROUTINE void buffer_key_alloc();

/* Return the thread-specific buffer */
ThreadStatic *TdiThreadStatic()
{
  ThreadStatic *p;
  lock_buffer_key();
  pthread_once(&buffer_key_once, buffer_key_alloc);
  p = (ThreadStatic *) pthread_getspecific(buffer_key);
  if (p == NULL) {
    p = (ThreadStatic *) memset(malloc(sizeof(ThreadStatic)), 0, sizeof(ThreadStatic));
    p->TdiGetData_recursion_count = 0;
    p->TdiIntrinsic_mess_stat = -1;
    p->TdiIntrinsic_recursion_count = 0;
    p->TdiIntrinsic_message.length = 0;
    p->TdiIntrinsic_message.dtype = DTYPE_T;
    p->TdiIntrinsic_message.class = CLASS_D;
    p->TdiIntrinsic_message.pointer = 0;
    p->TdiVar_private.head = 0;
    LibCreateVmZone(&p->TdiVar_private.head_zone);
    LibCreateVmZone(&p->TdiVar_private.data_zone);
    p->TdiVar_private.public = 0;
    p->TdiVar_new_narg = 0;
    p->TdiVar_new_narg_d.length = sizeof(int);
    p->TdiVar_new_narg_d.dtype = DTYPE_L;
    p->TdiVar_new_narg_d.class = CLASS_S;
    p->TdiVar_new_narg_d.pointer = (char *)&p->TdiVar_new_narg;
    p->compiler_recursing = 0;
    pthread_setspecific(buffer_key, (void *)p);
  }
  unlock_buffer_key();
  return p;
}

/* Free the thread-specific buffer */
STATIC_ROUTINE void buffer_destroy(void *buf)
{
  if (buf != NULL) {
    ThreadStatic *ts = (ThreadStatic *) buf;
    StrFree1Dx(&ts->TdiIntrinsic_message);
    LibResetVmZone(&ts->TdiVar_private.head_zone);
    LibResetVmZone(&ts->TdiVar_private.data_zone);
    free(buf);
  }
}

/* Allocate the key */
STATIC_ROUTINE void buffer_key_alloc()
{
  pthread_key_create(&buffer_key, buffer_destroy);
}

void LockTdiMutex(pthread_mutex_t * mutex, int *initialized)
{
  if (!initialized || !*initialized) {
#ifdef HAVE_PTHREAD_H
    pthread_mutexattr_t m_attr;
    pthread_mutexattr_init(&m_attr);
#if !defined(PTHREAD_MUTEX_RECURSIVE)
#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif
#ifndef __sun
    pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
#endif
    pthread_mutex_init(mutex, &m_attr);
#else
    pthread_mutex_init(mutex);
#endif
    if (initialized)
      *initialized = 1;
  }
  pthread_mutex_lock(mutex);
}

void UnlockTdiMutex(pthread_mutex_t * mutex)
{
  pthread_mutex_unlock(mutex);
}
