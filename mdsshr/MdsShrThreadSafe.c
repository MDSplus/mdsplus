#include <config.h>
#include <STATICdef.h>
#include "mdsshrthreadsafe.h"

/* Key for the thread-specific buffer */
STATIC_THREADSAFE pthread_key_t buffer_key;

/* Once-only initialisation of the key */
STATIC_THREADSAFE pthread_once_t buffer_key_once = PTHREAD_ONCE_INIT;

STATIC_ROUTINE void buffer_key_alloc();

#ifdef HAVE_WINDOWS_H

void pthread_once(pthread_once_t *one_time,void (*key_alloc)())
{
  if (*one_time == PTHREAD_ONCE_INIT)
  {
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
  TlsSetValue(buffer_key,p);
}

void pthread_key_create(pthread_key_t *buffer_key,void *d2){*buffer_key=TlsAlloc();}
extern void pthread_mutex_init(HANDLE *mutex);
extern void pthread_mutex_lock(HANDLE *mutex);
extern void pthread_mutex_unlock(HANDLE *mutex);
#endif
#ifdef HAVE_VXWORKS_H
#define HANDLE void*
void pthread_once(pthread_once_t *one_time,void (*key_alloc)())
{
  if (*one_time == PTHREAD_ONCE_INIT)
  {
    *one_time = 1;
    key_alloc();
  }
}

static int bufferKeys[1024];
static void *pthreadKeys[1024];
static int numKeys;

static void TlsSetValue(int bufferKey, void *p)
{
	int i;
	for(i = 0; i < numKeys; i++)
	{
		if(bufferKeys[i] == bufferKey)
			break;
	}
	if(i == numKeys)
		numKeys++;
	bufferKeys[i] = bufferKey;
	pthreadKeys[i] = p;
}

static void *TlsGetValue(int bufferKey)
{
	int i;
	for(i = 0; i < numKeys; i++)
	{
		if(bufferKeys[i] == bufferKey)
			break;
	}
	return pthreadKeys[i];
}




void *pthread_getspecific(pthread_key_t buffer_key)
{
  return TlsGetValue(buffer_key);
}

void pthread_setspecific(pthread_key_t buffer_key, void *p)
{
  TlsSetValue(buffer_key,p);
}

//void pthread_key_create(pthread_key_t *buffer_key,void *d2){*buffer_key=TlsAlloc();}
void pthread_key_create(pthread_key_t *buffer_key,void *d2){*buffer_key=malloc(sizeof(int *));}
extern void pthread_mutex_init(HANDLE *mutex);
extern void pthread_mutex_lock(HANDLE *mutex);
extern void pthread_mutex_unlock(HANDLE *mutex);
#endif

/* Return the thread-specific buffer */
MdsShrThreadStatic *MdsShrGetThreadStatic()
{
  MdsShrThreadStatic *p;
  pthread_once(&buffer_key_once, buffer_key_alloc);
  p = (MdsShrThreadStatic *) pthread_getspecific(buffer_key);
  if (p == NULL)
  {
    p = (MdsShrThreadStatic *)malloc(sizeof(MdsShrThreadStatic));
    p->MdsGetMsgDsc_tmp.dtype = DTYPE_T;
    p->MdsGetMsgDsc_tmp.class = CLASS_S;
    p->MdsGetMsgDsc_tmp.pointer = p->MdsGetMsg_text;
    pthread_setspecific(buffer_key,(void *)p);
  }
  return p;
}

/* Free the thread-specific buffer */
STATIC_ROUTINE void buffer_destroy(void * buf)
{
  if (buf != NULL)
  {
    MdsShrThreadStatic *ts = (MdsShrThreadStatic *)buf;
    free(buf);
  }
}

/* Allocate the key */
STATIC_ROUTINE void buffer_key_alloc()
{
   pthread_key_create(&buffer_key, buffer_destroy);
}

void LockMdsShrMutex(pthread_mutex_t *mutex,int *initialized)
{
  if(!*initialized)
  {
#ifndef HAVE_WINDOWS_H
#ifndef HAVE_VXWORKS_H
    pthread_mutexattr_t m_attr;
    pthread_mutexattr_init(&m_attr);
#if !defined(PTHREAD_MUTEX_RECURSIVE)
#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif
#ifndef __sun
#ifdef HAVE_PTHREAD_MUTEXATTR_SETKIND_NP
    pthread_mutexattr_setkind_np(&m_attr,PTHREAD_MUTEX_RECURSIVE);
#else
    pthread_mutexattr_settype(&m_attr,PTHREAD_MUTEX_RECURSIVE);
#endif
#endif
    pthread_mutex_init(mutex,&m_attr);
#else
    pthread_mutex_init(mutex);
#endif
#endif
    *initialized = 1;
  }
  pthread_mutex_lock(mutex);
}

void UnlockMdsShrMutex(pthread_mutex_t *mutex)
{
  pthread_mutex_unlock(mutex);
}


