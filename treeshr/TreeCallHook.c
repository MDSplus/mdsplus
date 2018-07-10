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
#include <STATICdef.h>
#include <libroutines.h>
#include <treeshr_hooks.h>
#include <ncidef.h>
#include <treeshr.h>
#include <mdsshr.h>
#include "treeshrp.h"
#include <tdishr_messages.h>
#include <mds_stdarg.h>
static int (*tdiExecute)();
static int (*Notify) (TreeshrHookType, char *, int, int);
static void load_Notify() {
  DESCRIPTOR(image, "TreeShrHooks");
  DESCRIPTOR(rtnname, "Notify");
  if IS_NOT_OK(LibFindImageSymbol(&image, &rtnname, &Notify))
    Notify = NULL;
  if (Notify == NULL) {
    DESCRIPTOR(image, "TdiShr");
    DESCRIPTOR(rtnname, "TdiExecute");
    if IS_NOT_OK(LibFindImageSymbol(&image, &rtnname, &tdiExecute))
      tdiExecute=NULL;
    else {
      DESCRIPTOR(expression,"TreeShrHook(-1,'',0,0)");
      EMPTYXD(ans_d);
      if ((*tdiExecute)(&expression, &ans_d, MdsEND_ARG) == TdiUNKNOWN_VAR)
	tdiExecute=NULL;
      MdsFree1Dx(&ans_d,NULL);
    }
  }
}

int TreeCallHook(PINO_DATABASE *dbid, TreeshrHookType htype, int nid)
{
  RUN_FUNCTION_ONCE(load_Notify);
  TREE_INFO *info = (dbid == NULL) ? NULL : dbid->tree_info;
  if (info == NULL)
    return 1;
  if (Notify)
    return (*Notify) (htype, info->treenam, info->shot, nid);
  if (tdiExecute) {
    char expression[128];
    struct descriptor expression_d = {0, DTYPE_T, CLASS_S, expression};
    snprintf(expression, sizeof(expression), "TreeShrHook(%d,'%s',%d,%d)",
	     htype, info->treenam, info->shot, nid);
    expression_d.length=strlen(expression);
    void *old_dbid=TreeSwitchDbid(dbid);
    EMPTYXD(ans_d);
    (*tdiExecute) (&expression_d, &ans_d,  MdsEND_ARG);
    old_dbid = TreeSwitchDbid(old_dbid);
    if (ans_d.pointer != NULL) {
      int ans = 1;
      struct descriptor *d = (struct descriptor *)ans_d.pointer;
      if ((d->dtype == DTYPE_L) && (d->pointer != NULL))
	ans = *(int *)d->pointer;
      MdsFree1Dx(&ans_d,NULL);
      return ans;
    }
  }
  return 1;
}
