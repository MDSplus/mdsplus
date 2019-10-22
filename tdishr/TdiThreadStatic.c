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
#include "tdithreadstatic.h"
#include <stdlib.h>
#include <mdsshr.h>
#include <strroutines.h>
#include <string.h>
/* Key for the thread-specific buffer */
static pthread_key_t buffer_key;

static inline ThreadStatic *ThreadStatic_create() {
  ThreadStatic *TdiThreadStatic_p = (ThreadStatic *) calloc(1,sizeof(ThreadStatic)); // zeros everything
  LibCreateVmZone(&TDI_VAR_PRIVATE.head_zone);
  LibCreateVmZone(&TDI_VAR_PRIVATE.data_zone);
  TDI_INTRINSIC_STAT = SsSUCCESS;
  TDI_INTRINSIC_MSG.dtype = DTYPE_T;
  TDI_INTRINSIC_MSG.class = CLASS_D;
  TDI_VAR_NEW_NARG_D.length = sizeof(int);
  TDI_VAR_NEW_NARG_D.dtype = DTYPE_L;
  TDI_VAR_NEW_NARG_D.class = CLASS_S;
  TDI_VAR_NEW_NARG_D.pointer = (char *)&TDI_VAR_NEW_NARG;
  for (; TDI_STACK_IDX<TDI_STACK_SIZE ; TDI_STACK_IDX++ ) {
    TDI_INDENT = 1;
    TDI_DECOMPILE_MAX = 0xffff;
  }
  TDI_STACK_IDX = 0;
  return TdiThreadStatic_p;
}
static void ThreadStatic_destroy(ThreadStatic *TdiThreadStatic_p) {
  if (!TdiThreadStatic_p) return;
  LibResetVmZone(&TDI_VAR_PRIVATE.head_zone);
  LibResetVmZone(&TDI_VAR_PRIVATE.data_zone);
  StrFree1Dx(&TDI_INTRINSIC_MSG);
  free(TdiThreadStatic_p);
}
static void buffer_destroy(void *buf){
  ThreadStatic_ref *ref = (ThreadStatic_ref *) buf;
  if (ref->free_me)
    ThreadStatic_destroy(ref->p);
  free(buf);
}
static void buffer_key_alloc(){
  pthread_key_create(&buffer_key, buffer_destroy);
}
/* Return the thread-specific buffer */
EXPORT ThreadStatic *TdiThreadStatic(ThreadStatic *in){
  RUN_FUNCTION_ONCE(buffer_key_alloc);
  ThreadStatic_ref *ref = (ThreadStatic_ref *) pthread_getspecific(buffer_key);
  if (ref && !in) return ref->p;
  if (!ref) ref = calloc(1,sizeof(ThreadStatic_ref));
  if (in) {
    ThreadStatic_destroy(ref->p);
    ref->p = in;
    ref->free_me = FALSE;
  } else if (!ref->p) {
    ref->p = ThreadStatic_create();
    ref->free_me = TRUE;
  }
  pthread_setspecific(buffer_key, (void *)ref);
  return ref->p;
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
    *initialized = TRUE;
  }
  pthread_mutex_unlock(&initMutex);
  pthread_mutex_lock(mutex);
}

void UnlockTdiMutex(pthread_mutex_t * mutex){
  pthread_mutex_unlock(mutex);
}
