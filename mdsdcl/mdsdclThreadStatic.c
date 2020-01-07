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
#include <string.h>

#include <mdsshr.h>

#include "mdsdclthreadstatic.h"

static inline DCLTHREADSTATIC_TYPE *buffer_alloc() {
  return (DCLTHREADSTATIC_TYPE *)calloc(1, sizeof(DCLTHREADSTATIC_TYPE));
}
static void buffer_free(DCLTHREADSTATIC_ARG) {
  free(DCL_PROMPT);
  free(DCL_DEFFILE);
  dclDocListPtr next,dcl;
  for (dcl = DCL_DOCS ; dcl ; dcl = next) {
    next = dcl->next;
    free(dcl);
  }
  free(DCLTHREADSTATIC_VAR);
}

IMPLEMENT_GETTHREADSTATIC(DCLTHREADSTATIC_TYPE,DclGetThreadStatic,THREADSTATIC_DCLSHR,buffer_alloc,buffer_free)

EXPORT void mdsdclSetPrompt(const char *prompt){
  DCLTHREADSTATIC_INIT;
  free(DCL_PROMPT);
  DCL_PROMPT = strdup(prompt);
}

EXPORT char *mdsdclGetPrompt(){
  char *ans;
  DCLTHREADSTATIC_INIT;
  if (!DCL_PROMPT)
    DCL_PROMPT = strdup("Command> ");
  ans = strdup(DCL_PROMPT);
  return ans;
}

void mdsdclSetDefFile(const char *deffile){
  DCLTHREADSTATIC_INIT;
  free(DCL_DEFFILE);
  if (deffile[0] == '*')
    DCL_DEFFILE = strdup(deffile + 1);
  else
    DCL_DEFFILE = strdup(deffile);
}

void mdsdclAllocDocDef(dclDocListPtr doc_l){
  DCLTHREADSTATIC_INIT;
  dclDocListPtr doc_p = malloc(sizeof(dclDocList));
  doc_p->name = doc_l->name;
  doc_p->doc  = doc_l->doc;
  doc_p->next = DCL_DOCS;
  DCL_DOCS = doc_p;
}


STATIC_PTHREAD_RECURSIVE_MUTEX_DEF(dcl_lock)
void dclLock() {
  STATIC_PTHREAD_RECURSIVE_MUTEX_INIT(dcl_lock);
  pthread_mutex_lock(&dcl_lock);
}
void dclUnlock() {
  pthread_mutex_unlock(&dcl_lock);
}

