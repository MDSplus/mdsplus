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

#include <stdlib.h>
#include <string.h>

#include <libroutines.h>
#include <mdsshr.h>
#include <status.h>
#include <strroutines.h>
#include <treeshr.h>
#include <_mdsshr.h>

#include <_mdsversion.h>
#include "treethreadstatic.h"

// #define DEBUG
#include <mdsmsg.h>

extern int _TreeNewDbid(void **dblist);
static pthread_rwlock_t treectx_lock = PTHREAD_RWLOCK_INITIALIZER;
static void *DBID = NULL, *G_DBID = NULL;

void destroy_host(Host *host)
{
  MDSSHR_LOAD_LIBROUTINE_LOCAL(MdsIpShr, DisconnectFromMds, abort(), void, (int));
  MDSDBG(HOST_PRI, HOST_VAR(host));
  DisconnectFromMds(host->conid);
  free(host);
}

static void buffer_free(TREETHREADSTATIC_ARG)
{
  if (TREE_DBID)
  {
    PINO_DATABASE *g_dbid, *p_dbid;
    pthread_rwlock_rdlock(&treectx_lock);
    for (g_dbid = G_DBID; g_dbid && g_dbid != TREE_DBID; g_dbid = g_dbid->next)
      ;
    if (g_dbid)
    {
      pthread_rwlock_unlock(&treectx_lock);
      perror("privateCtx share globalCtx on Threadexit! -> memory leak");
    }
    else
    {
      for (p_dbid = TREE_DBID; p_dbid->next; p_dbid = p_dbid->next)
      {
        if (p_dbid->next == G_DBID)
        {
          // clip private context if extension of global
          p_dbid->next = NULL;
          break;
        }
      }
      pthread_rwlock_unlock(&treectx_lock);
      TreeFreeDbid(TREE_DBID);
    }
  }
  Host *host;
  while (TREE_HOSTLIST)
  {
    host = TREE_HOSTLIST;
    TREE_HOSTLIST = TREE_HOSTLIST->next;
    destroy_host(host);
  }
  free(TREETHREADSTATIC_VAR);
}
static inline TREETHREADSTATIC_TYPE *buffer_alloc()
{
  TREETHREADSTATIC_ARG =
      (TREETHREADSTATIC_TYPE *)calloc(1, sizeof(TREETHREADSTATIC_TYPE));
  return TREETHREADSTATIC_VAR;
}

IMPLEMENT_GETTHREADSTATIC(TREETHREADSTATIC_TYPE, TreeGetThreadStatic,
                          THREADSTATIC_TREESHR, buffer_alloc, buffer_free)

EXPORT void **TreeCtx()
{
  TREETHREADSTATIC_INIT;
  void **ctx;
  if (TREE_PRIVATECTX)
  {
    if (!TREE_DBID)
      _TreeNewDbid(&TREE_DBID);
    ctx = &TREE_DBID;
  }
  else
  {
    pthread_rwlock_wrlock(&treectx_lock);
    if (!DBID)
    {
      if (!G_DBID)
        _TreeNewDbid(&G_DBID);
      DBID = G_DBID;
    }
    ctx = &DBID;
    pthread_rwlock_unlock(&treectx_lock);
  }
  return ctx;
}

EXPORT void *TreeDbid() { return *TreeCtx(); }

EXPORT void *_TreeDbid(void **dbid) { return *dbid; }

EXPORT void *TreeSwitchDbid(void *dbid)
{
  TREETHREADSTATIC_INIT;
  void *old_dbid;
  if (TREE_PRIVATECTX)
  {
    old_dbid = TREE_DBID;
    TREE_DBID = dbid;
  }
  else
  {
    pthread_rwlock_wrlock(&treectx_lock);
    old_dbid = DBID;
    DBID = dbid;
    pthread_rwlock_unlock(&treectx_lock);
  }
  return old_dbid;
}

EXPORT int TreeUsePrivateCtx(int onoff)
{
  TREETHREADSTATIC_INIT;
  int old = TREE_PRIVATECTX;
  TREE_PRIVATECTX = onoff != 0;
  return old;
}

EXPORT int TreeUsingPrivateCtx()
{
  TREETHREADSTATIC_INIT;
  return TREE_PRIVATECTX;
}

typedef struct
{
  void *dbid;
  void **ctx;
  int priv;
} push_ctx_t;
EXPORT void *TreeCtxPush(void **ctx)
{
  /* switch to private context and use dbid
   * SHOULD be used with pthread_cleanup or similar constucts
   * DONT USE this from tdi; will cause memory violation
   */
  TREETHREADSTATIC_INIT;
  void *ps = malloc(sizeof(push_ctx_t));
  ((push_ctx_t *)ps)->priv = TREE_PRIVATECTX;
  ((push_ctx_t *)ps)->dbid = TREE_DBID;
  ((push_ctx_t *)ps)->ctx = ctx;
  TREE_PRIVATECTX = TRUE;
  TREE_DBID = *ctx;
  return ps;
}

EXPORT void TreeCtxPop(void *ps)
{
  /* done using dbid in private context, restore state
   */
  TREETHREADSTATIC_INIT;
  *((push_ctx_t *)ps)->ctx = TREE_DBID;
  TREE_PRIVATECTX = ((push_ctx_t *)ps)->priv;
  TREE_DBID = ((push_ctx_t *)ps)->dbid;
  free(ps);
}

typedef struct
{
  void *dbid;
  int priv;
} push_dbid_t;
EXPORT void *TreeDbidPush(void *dbid)
{
  /* switch to private context and use dbid
   * DONT USE with pthread_cleanup
   * Useful when called from tdi
   */
  TREETHREADSTATIC_INIT;
  void *ps = malloc(sizeof(push_dbid_t));
  ((push_dbid_t *)ps)->priv = TREE_PRIVATECTX;
  ((push_dbid_t *)ps)->dbid = TREE_DBID;
  TREE_PRIVATECTX = TRUE;
  TREE_DBID = dbid;
  return ps;
}

EXPORT void *TreeDbidPop(void *ps)
{
  /* done using dbid in private context, restore state
   */
  TREETHREADSTATIC_INIT;
  void *dbid = TREE_DBID;
  TREE_PRIVATECTX = ((push_dbid_t *)ps)->priv;
  TREE_DBID = ((push_dbid_t *)ps)->dbid;
  free(ps);
  return dbid;
}
