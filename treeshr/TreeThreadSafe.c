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
#include <mdsplus/mdsconfig.h>
#include <status.h>
#include <libroutines.h>
#include <STATICdef.h>
#include "treethreadsafe.h"
#include <stdlib.h>
#include <mdsshr.h>
#include <strroutines.h>
#include <string.h>

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
TreeThreadStatic *TreeGetThreadStatic(){
  RUN_FUNCTION_ONCE(buffer_key_alloc);
  TreeThreadStatic* p = (TreeThreadStatic*)pthread_getspecific(buffer_key);
  if (!p) {
    p = (TreeThreadStatic*)calloc(1,sizeof(TreeThreadStatic));
    pthread_setspecific(buffer_key, (void*)p);
  }
  return p;
}

void *DBID = NULL;

static pthread_rwlock_t treectx_lock = PTHREAD_RWLOCK_INITIALIZER;
EXPORT void **TreeCtx(){
  TreeThreadStatic *p = TreeGetThreadStatic();
  pthread_rwlock_rdlock(&treectx_lock);
  void **old_dbid = p->privateCtx ? &p->DBID : &DBID;
  pthread_rwlock_unlock(&treectx_lock);
  return old_dbid;
}

EXPORT void *TreeDbid(){
  return *TreeCtx();
}

EXPORT void *_TreeDbid(void **dbid){
  return *dbid;
}

EXPORT void *TreeSwitchDbid(void *dbid){
  TreeThreadStatic *p = TreeGetThreadStatic();
  pthread_rwlock_wrlock(&treectx_lock);
  void *old_dbid = p->privateCtx ? p->DBID : DBID;
  if (p->privateCtx) p->DBID = dbid;
  else                  DBID = dbid;
  pthread_rwlock_unlock(&treectx_lock);
  return old_dbid;
}

EXPORT int TreeUsePrivateCtx(int onoff){
  TreeThreadStatic *p = TreeGetThreadStatic();
  int old = p->privateCtx;
  p->privateCtx = onoff!=0;
  return old;
}

EXPORT int TreeUsingPrivateCtx(){
  TreeThreadStatic *p = TreeGetThreadStatic();
  return p->privateCtx;
}

struct private_ctx{
  void *dbid;
  int  upc;
} PRIVATE_CTX;

EXPORT void* TreeSavePrivateCtx(void** ctx){
  struct private_ctx* pctx = (struct private_ctx*)malloc(sizeof(struct private_ctx));
  pctx->dbid = TreeDbid();
  pctx->upc  = TreeUsePrivateCtx(1);
  TreeSwitchDbid(*ctx);
  return pctx;
}

EXPORT void TreeRestorePrivateCtx(void* _pctx){
  struct private_ctx* pctx = (struct private_ctx*)_pctx;
  TreeSwitchDbid(pctx->dbid);
  TreeUsePrivateCtx(pctx->upc);
  free(pctx);
}

/*
EXPORT int _Call(void** ctx, struct descriptor* dsc){
  if (!dsc) return TreeNormal;
  void* pctx = TreeSavePrivateCtx(ctx);
  pthread_cleanup_push(RestorePrivateCtx,pctx);
  // do stuff that requires ctx but does not accept ctx as input
  pthread_cleanup_pop(1);
}

EXPORT int TreeDeRef(struct descript* dsc){
  return _TreeDeRef(TreeCtx(), dsc);
}
*/
