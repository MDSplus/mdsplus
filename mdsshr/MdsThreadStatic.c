/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#define _GNU_SOURCE
#include <mdsplus/mdsconfig.h>

#include <stdlib.h>

#include <mdsshr.h>

#include <_mdsversion.h>
#include "mdsthreadstatic.h"

/* Key for the thread-specific buffer */
static pthread_key_t mts_key;
/* Free the thread-specific buffer */
static inline void free_buffer(buffer_t *buf)
{
  if (!buf)
    return;
  buf->free(buf->buffer);
  free(buf);
}

EXPORT MDSplusThreadStatic_t *newMDSplusThreadStatic()
{
  MDSplusThreadStatic_t *mts =
      (MDSplusThreadStatic_t *)malloc(sizeof(MDSplusThreadStatic_t));
  mts->is_owned = TRUE;
  mts->buffers = calloc(THREADSTATIC_SIZE, sizeof(buffer_t *));
  return mts;
}

EXPORT void destroyMDSplusThreadStatic(MDSplusThreadStatic_t *mts)
{
  if (mts->is_owned)
  {
    buffer_t **buf, **end = mts->buffers + THREADSTATIC_SIZE;
    for (buf = mts->buffers; buf < end; buf++)
      free_buffer(*buf);
    free(mts->buffers);
  }
  free(mts);
}

static void init_mts_key() { pthread_key_create(&mts_key, (void *)destroyMDSplusThreadStatic); }
EXPORT MDSplusThreadStatic_t *MDSplusThreadStatic(MDSplusThreadStatic_t *in)
{
  // mts = MDSplusThreadStatic(NULL) : get current thread's mts
  // MDSplusThreadStatic(parent_mts) : setup parent mts in thread ; should be
  // first command of thread
  RUN_FUNCTION_ONCE(init_mts_key);
  MDSplusThreadStatic_t *mts =
      (MDSplusThreadStatic_t *)pthread_getspecific(mts_key);
  if (in)
  {
    // duplicate in and replace
    MDSplusThreadStatic_t *old_mts = mts;
    mts = (MDSplusThreadStatic_t *)malloc(sizeof(MDSplusThreadStatic_t));
    mts->is_owned = FALSE;
    mts->buffers = in->buffers;
    pthread_setspecific(mts_key, (void *)mts);
    if (old_mts)
      destroyMDSplusThreadStatic(old_mts);
  }
  else
  {
    // create if NULL
    if (!mts)
    {
      mts = newMDSplusThreadStatic();
      pthread_setspecific(mts_key, (void *)mts);
    }
  }
  return mts;
}

static inline MDSTHREADSTATIC_TYPE *buffer_alloc()
{
  MDSTHREADSTATIC_ARG =
      (MDSTHREADSTATIC_TYPE *)calloc(1, sizeof(MDSTHREADSTATIC_TYPE));
  MDS_MDSGETMSG_DESC.dtype = DTYPE_T;
  MDS_MDSGETMSG_DESC.class = CLASS_S;
  MDS_MDSGETMSG_DESC.pointer = MDS_MDSGETMSG_CSTR;
  return MDSTHREADSTATIC_VAR;
}
static inline void buffer_free(MDSTHREADSTATIC_ARG)
{
  free(MDS_FIS_ERROR);
  free(MDSTHREADSTATIC_VAR);
}

IMPLEMENT_GETTHREADSTATIC(MDSTHREADSTATIC_TYPE, MdsGetThreadStatic,
                          THREADSTATIC_MDSSHR, buffer_alloc, buffer_free)

EXPORT void LockMdsShrMutex(pthread_mutex_t *mutex, int *initialized)
{
  static pthread_mutex_t initMutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&initMutex);
  if (!*initialized)
  {
    pthread_mutexattr_t m_attr;
    pthread_mutexattr_init(&m_attr);
    pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(mutex, &m_attr);
    *initialized = 1;
  }
  pthread_mutex_unlock(&initMutex);
  pthread_mutex_lock(mutex);
}

EXPORT void UnlockMdsShrMutex(pthread_mutex_t *mutex)
{
  pthread_mutex_unlock(mutex);
}

static pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
EXPORT void MdsGlobalUnlock() { pthread_mutex_unlock(&global_mutex); }

EXPORT void MdsGlobalLock() { pthread_mutex_lock(&global_mutex); }
