#ifndef HAVE_VXWORKS_H
#include <config.h>
#endif
#include <libroutines.h>
#include <STATICdef.h>
#include "mdsdclthreadsafe.h"
#include <stdlib.h>
#include <mdsshr.h>
#include <strroutines.h>
STATIC_THREADSAFE  pthread_mutex_t mutex;
/* Key for the thread-specific buffer */
STATIC_THREADSAFE pthread_key_t buffer_key;

/* Once-only initialisation of the key */
STATIC_THREADSAFE pthread_once_t buffer_key_once = PTHREAD_ONCE_INIT;

STATIC_ROUTINE void buffer_key_alloc();

/* Return the thread-specific buffer */
MdsdclThreadStatic *MdsdclGetThreadStatic()
{
  MdsdclThreadStatic *p;
  pthread_once(&buffer_key_once, buffer_key_alloc);
  p = (MdsdclThreadStatic *) pthread_getspecific(buffer_key);
  if (p == NULL)
  {
    p = (MdsdclThreadStatic *)memset(malloc(sizeof(MdsdclThreadStatic)),0,sizeof(MdsdclThreadStatic));
    pthread_setspecific(buffer_key,(void *)p);
  }
  return p;
}


/* Free the thread-specific buffer */
STATIC_ROUTINE void buffer_destroy(void * buf)
{
  if (buf != NULL)
  {
    MdsdclThreadStatic *ts = (MdsdclThreadStatic *)buf;
    free(buf);
  }
}

/* Allocate the key */
STATIC_ROUTINE void buffer_key_alloc()
{
   pthread_key_create(&buffer_key, buffer_destroy);
}

