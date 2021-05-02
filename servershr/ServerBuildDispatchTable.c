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
/*------------------------------------------------------------------------------

                Name:   ServerBuildDispatchTable

                Type:   C function

                Author:	TOM FREDIAN

                Date:   17-APR-1992

                Purpose: Build dispatch table

------------------------------------------------------------------------------

        Call sequence:

int ServerBuildDispatchTable( )

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <ipdesc.h>
#include <usagedef.h>
#include <dbidef.h>
#include <stdio.h>
#include <stdlib.h>
#include <strroutines.h>
#include <string.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <libroutines.h>
#include <servershr.h>
#include <mds_stdarg.h>
#include "servershrp.h"
extern int TdiDeallocate();
extern int TdiDispatchOf();
extern int TdiExecute();
extern int TdiGetLong();
extern int TdiData();
extern int TdiGetNci();

static int compare_actions(ActionInfo *a, ActionInfo *b)
{
  return a->on == b->on ? (a->phase == b->phase ? a->sequence - b->sequence
                                                : a->phase - b->phase)
                        : b->on - a->on;
}

#define MAX_ACTIONS 10000

typedef struct ctx_s
{
  ActionInfo *actions;
  int num_actions;
  int idx;
} ctx_t;

static int if_add_reference(int *nid, const ctx_t *const ctx)
{
  int i, j;
  for (i = 0; i < ctx->num_actions; i++)
  {
    if (ctx->actions[i].nid == *nid)
    {
      if (ctx->actions[i].num_references == 0)
        ctx->actions[i].referenced_by = (int *)malloc(sizeof(int));
      else
      {
        for (j = 0; j < ctx->actions[i].num_references; j++)
          if (ctx->actions[i].referenced_by[j] == ctx->idx)
            return B_TRUE;
        ctx->actions[i].referenced_by =
            (int *)realloc((char *)ctx->actions[i].referenced_by,
                           sizeof(int) * (ctx->actions[i].num_references + 1));
      }
      ctx->actions[i].referenced_by[ctx->actions[i].num_references] = ctx->idx;
      ctx->actions[i].num_references++;
      return B_TRUE;
    }
  }
  return B_FALSE;
}

static int fixup_nid(int *nid, const ctx_t *const ctx, mdsdsc_d_t *path_out)
{
  INIT_STATUS;
  static DESCRIPTOR(dtype_str, "DTYPE");
  static int dtype;
  static DESCRIPTOR_LONG(dtype_dsc, &dtype);
  DESCRIPTOR_NID(niddsc, 0);
  niddsc.pointer = (char *)nid;
  status = TdiGetNci(&niddsc, &dtype_str, &dtype_dsc MDS_END_ARG);
  if (STATUS_OK && dtype == DTYPE_ACTION && if_add_reference(nid, ctx))
  {
    char ident[64];
    char tmp[64];
    mdsdsc_t ident_dsc = {0, DTYPE_T, CLASS_S, 0};
    EMPTYXD(xd);
    sprintf(ident, "_ACTION_%08X", *nid);
    ident_dsc.length = strlen(ident);
    ident_dsc.pointer = ident;
    StrCopyDx((mdsdsc_t *)path_out, (mdsdsc_t *)&ident_dsc);
    strcpy(tmp, ident);
    strcpy(ident, "public ");
    strcat(ident, tmp);
    strcat(ident, "=compile('_$_$_$_UNDEFINED')");
    ident_dsc.length = strlen(ident);
    TdiExecute(&ident_dsc, &xd MDS_END_ARG);
    MdsFree1Dx(&xd, NULL);
    return B_TRUE;
  }
  return B_FALSE;
}

inline static int fixup_path(mdsdsc_t *path_in, const ctx_t *const ctx, mdsdsc_d_t *path_out)
{
  char *path = strncpy(
      (char *)malloc(path_in->length + 1), path_in->pointer, path_in->length);
  int nid;
  int flag = B_FALSE;
  path[path_in->length] = 0;
  if (IS_OK(TreeFindNode(path, &nid)))
    flag = fixup_nid(&nid, ctx, path_out);
  free(path);
  return flag;
}

inline static int make_idents(mdsdsc_t *path_in,
                              int idx __attribute__((unused)),
                              mdsdsc_t *path_out __attribute__((unused)))
{
  if (path_in && path_in->pointer && path_in->pointer[0] == '_')
    path_in->dtype = DTYPE_IDENT;
  return B_FALSE;
}

inline static void link_conditions(ctx_t *const ctx)
{
  // (*fixup_nid)(in.pointer, fixup_nid_arg, &path))
  int i;
  for (i = 0; i < ctx->num_actions; i++)
  {
    if (ctx->actions[i].condition)
    {
      EMPTYXD(xd);
      ctx->idx = i;
      MdsCopyDxXdZ(ctx->actions[i].condition, &xd, 0,
                   fixup_nid, ctx, fixup_path, ctx);
      MdsCopyDxXdZ((mdsdsc_t *)&xd, (mdsdsc_xd_t *)ctx->actions[i].condition,
                   0, 0, 0, make_idents, (void *)(intptr_t)i);
      MdsFree1Dx(&xd, 0);
    }
  }
}

#define ALL_NODES "***"
EXPORT int ServerBuildDispatchTable(char *wildcard, char *monitor_name,
                                    void **table)
{
  if (*table)
    ServerFreeDispatchTable(*table);
  DispatchTable **table_ptr = (DispatchTable **)table;
  void *fnwctx = 0;
  int i;
  char *nodespec = wildcard ? wildcard : ALL_NODES;
  int mask = 1 << TreeUSAGE_ACTION;
  int status = 1;
  int *nids;
  int *nidptr;
  char tree[12];
  int shot = -1;
  char *cptr;
  DBI_ITM itmlst[] = {
      {sizeof(tree), DbiNAME, 0, 0},
      {sizeof(shot), DbiSHOTID, 0, 0},
      {0, 0, 0, 0},
  };
  itmlst[0].pointer = &tree;
  itmlst[1].pointer = &shot;
  memset(tree, ' ', sizeof(tree));
  TreeGetDbi(itmlst);
  for (i = 0, cptr = tree; i < 12; i++)
    if (cptr[i] == ' ')
      break;
    else
      tree[i] = cptr[i];
  tree[i] = 0;
  if (shot == -1)
    return ServerINVSHOT;
  nids = (int *)malloc(MAX_ACTIONS * sizeof(int));
  ctx_t ctx = {NULL, 0, 0};
  while ((TreeFindNodeWild(nodespec, &nids[ctx.num_actions], &fnwctx, mask) & 1) &&
         (ctx.num_actions < MAX_ACTIONS))
    ctx.num_actions++;
  TreeFindNodeEnd(&fnwctx);
  if (ctx.num_actions)
  {
    int table_size = sizeof(DispatchTable) + (ctx.num_actions - 1) * sizeof(ActionInfo);
    *table_ptr = (DispatchTable *)calloc(1, table_size);
    ctx.actions = (*table_ptr)->actions;
    (*table_ptr)->shot = shot;
    strcpy((*table_ptr)->tree, tree);
    EMPTYXD(xd);
    for (i = 0, nidptr = nids; i < ctx.num_actions; nidptr++, i++)
    {
      mdsdsc_d_t event_name = {0, DTYPE_T, CLASS_D, 0};
      mdsdsc_t niddsc = {4, DTYPE_NID, CLASS_S, 0};
      niddsc.pointer = (char *)nidptr;
      ctx.actions[i].nid = *nidptr;
      ctx.actions[i].on = TreeIsOn(*nidptr) & 1;
      ctx.actions[i].num_references = 0;
      ctx.actions[i].referenced_by = 0;
      ctx.actions[i].status = 0;
      ctx.actions[i].dispatched = 0;
      ctx.actions[i].closed = 0;
      ctx.actions[i].condition = 0;
      if (ctx.actions[i].on)
      {
        if (TdiDispatchOf(&niddsc, &xd MDS_END_ARG) & 1)
        {
          struct descriptor_dispatch *dispatch =
              (struct descriptor_dispatch *)xd.pointer;
          if (dispatch->pointer && (dispatch->pointer[0] == TreeSCHED_SEQ))
          {
            mdsdsc_t server = {sizeof(ctx.actions->server), DTYPE_T,
                               CLASS_S, 0};
            DESCRIPTOR(phase_lookup, "PHASE_NUMBER_LOOKUP($)");
            mdsdsc_t phase_d = {sizeof(int), DTYPE_L, CLASS_S, 0};
            server.pointer = ctx.actions[i].server;
            phase_d.pointer = (char *)&ctx.actions[i].phase;
            if (IS_NOT_OK(TdiExecute(&phase_lookup, dispatch->phase, &phase_d MDS_END_ARG)))
              ctx.actions[i].phase = 0x10000001;
            if (IS_NOT_OK(TdiGetLong(dispatch->when, &ctx.actions[i].sequence)))
            {
              static const EMPTYXD(emptyxd);
              ctx.actions[i].sequence = 0;
              ctx.actions[i].condition = (mdsdsc_t *)memcpy(
                  malloc(sizeof(emptyxd)), &emptyxd, sizeof(emptyxd));
              MdsCopyDxXd((mdsdsc_t *)dispatch->when,
                          (mdsdsc_xd_t *)ctx.actions[i].condition);
              int zero = 0;
              ctx.actions[i].path = TreeGetMinimumPath(&zero, ctx.actions[i].nid);
            }
            else if (ctx.actions[i].sequence <= 0)
              ctx.actions[i].phase = 0x10000002;
            else
            {
              int zero = 0;
              ctx.actions[i].path = TreeGetMinimumPath(&zero, ctx.actions[i].nid);
            }
            if (IS_NOT_OK(TdiData(dispatch->ident, &server MDS_END_ARG)))
              ctx.actions[i].phase = 0x10000003;
          }
          else
            ctx.actions[i].phase = 0x10000004;
          if (TdiData(dispatch->completion, &event_name MDS_END_ARG) & 1 &&
              event_name.length)
          {
            ctx.actions[i].event = strncpy((char *)malloc(event_name.length + 1),
                                           event_name.pointer, event_name.length);
            ctx.actions[i].event[event_name.length] = 0;
            StrFree1Dx(&event_name);
          }
        }
        else
          ctx.actions[i].phase = 0x10000005;
      }
    }
    MdsFree1Dx(&xd, NULL);
    qsort(ctx.actions, ctx.num_actions, sizeof(ActionInfo),
          (int (*)(const void *, const void *))compare_actions);
    for (i = 0; i < ctx.num_actions && ctx.actions[i].on; i++)
      ;
    (*table_ptr)->num = i;
    link_conditions(&ctx);
    if (monitor_name)
    {
      char tree[13];
      char *cptr;
      for (i = 0, cptr = (*table_ptr)->tree; i < 12; i++)
        if (cptr[i] == ' ')
          break;
        else
          tree[i] = cptr[i];
      tree[i] = 0;
      char *server = "";
#define SKIP_OTHERS
#define SEND(i, mode)                                                             \
  ServerSendMonitor(monitor_name, tree, (*table_ptr)->shot, ctx.actions[i].phase, \
                    ctx.actions[i].nid, ctx.actions[i].on, mode, server,          \
                    ctx.actions[i].status)
      if (IS_OK(SEND(0, MonitorBuildBegin)))
      { // send begin
#ifdef SKIP_OTHERS
        if (ctx.num_actions > 1)
#else
        for (i = 1; i < num_actions - 1; i++)
          IS_NOT_OK(SEND(i, MonitorBuild))
        break; // send others
        if (i == num_actions - 1)
#endif
          SEND(ctx.num_actions - 1, MonitorBuildEnd); // send last
      }
    }
  }
  else
    status = TreeNNF;
  free(nids);
  return status;
}
