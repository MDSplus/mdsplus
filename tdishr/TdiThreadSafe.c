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
/* lock pthread_once */
STATIC_THREADSAFE pthread_mutex_t buffer_key_mutex = PTHREAD_MUTEX_INITIALIZER;
/* Free the thread-specific buffer */
STATIC_ROUTINE void buffer_destroy(void *buf){
  ThreadStatic *ts = (ThreadStatic *) buf;
  StrFree1Dx(&ts->TdiIntrinsic_message);
  LibResetVmZone(&ts->TdiVar_private.head_zone);
  LibResetVmZone(&ts->TdiVar_private.data_zone);
  free(buf);
}
STATIC_ROUTINE void buffer_key_alloc(){
  pthread_key_create(&buffer_key, buffer_destroy);
}
/* Return the thread-specific buffer */
ThreadStatic *TdiGetThreadStatic(){
  pthread_mutex_lock(&buffer_key_mutex);
  pthread_once(&buffer_key_once, buffer_key_alloc);
  pthread_mutex_unlock(&buffer_key_mutex);
  ThreadStatic *p = (ThreadStatic *) pthread_getspecific(buffer_key);
  if (!p) {
    p = (ThreadStatic *) calloc(1,sizeof(ThreadStatic));
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
    p->TdiIndent = 1;
    p->TdiDecompile_max = 0xffff;
    p->TdiOnError = 0;
  }
  return p;
}

void LockTdiMutex(pthread_mutex_t * mutex, int *initialized)
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

void UnlockTdiMutex(pthread_mutex_t * mutex){
  pthread_mutex_unlock(mutex);
}
