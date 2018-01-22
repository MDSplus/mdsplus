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
#include "mdsdclthreadsafe.h"
#include <stdlib.h>
#include <string.h>
/* Key for the thread-specific buffer */
STATIC_THREADSAFE pthread_key_t buffer_key;
/* Free the thread-specific buffer */
STATIC_ROUTINE void buffer_destroy(void *buf){
  ThreadStatic *ThreadStatic_p = (ThreadStatic *)buf;
  if (ThreadStatic_p) {
    if (PROMPT) free(PROMPT);
    if (DEF_FILE) free(DEF_FILE);
    dclDocListPtr next,dcl = DCLDOCS;
    for (;dcl;) {
      next = dcl->next;
      free(dcl);
      dcl=next;
    }
  }
  free(buf);
}
STATIC_ROUTINE void buffer_key_alloc(){
  pthread_key_create(&buffer_key, buffer_destroy);
}

/* Return the thread-specific buffer */
ThreadStatic *mdsdclGetThreadStatic(){
  RUN_FUNCTION_ONCE(buffer_key_alloc);
  void* p = pthread_getspecific(buffer_key);
  if (!p) {
    p = calloc(1, sizeof(ThreadStatic));
    pthread_setspecific(buffer_key, p);
  }
  return (ThreadStatic *)p;
}

EXPORT void mdsdclSetPrompt(const char *prompt){
  GET_THREADSTATIC_P;
  if (PROMPT) free(PROMPT);
  PROMPT = strdup(prompt);
}

EXPORT char *mdsdclGetPrompt(){
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

void mdsdclAllocDocDef(dclDocListPtr doc_l){
  GET_THREADSTATIC_P;
  dclDocListPtr doc_p = malloc(sizeof(dclDocList));
  doc_p->name = doc_l->name;
  doc_p->doc  = doc_l->doc;
  doc_p->next = DCLDOCS;
  DCLDOCS = doc_p;
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

