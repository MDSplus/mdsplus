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
#include <STATICdef.h>
#include <mdsshr.h>
#include "mdsshrthreadsafe.h"
#include <stdlib.h>

/* Key for the thread-specific buffer */
STATIC_THREADSAFE pthread_key_t buffer_key;
/* Free the thread-specific buffer */
STATIC_ROUTINE void buffer_destroy(void *buf){
  free(buf);
}
STATIC_ROUTINE void buffer_key_alloc(){
  pthread_key_create(&buffer_key, buffer_destroy);
}
/* Return the thread-specific buffer */
MdsShrThreadStatic *MdsShrGetThreadStatic(){
  RUN_FUNCTION_ONCE(buffer_key_alloc);
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

void LockMdsShrMutex(pthread_mutex_t * mutex, int *initialized)
{
  static pthread_mutex_t initMutex = PTHREAD_MUTEX_INITIALIZER;
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
