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
#include "treeshrp.h"
#include <mds_stdarg.h>

static int (*Notify) (TreeshrHookType, char *, int, int);
static void load_Notify() {
  DESCRIPTOR(image, "TreeShrHooks");
  DESCRIPTOR(rtnname, "Notify");
  if IS_NOT_OK(LibFindImageSymbol(&image, &rtnname, &Notify))
    Notify = NULL;
}

int TreeCallHook(TreeshrHookType htype, TREE_INFO * info, int nid)
{
  RUN_FUNCTION_ONCE(load_Notify);
  if (Notify)
    return (*Notify) (htype, info->treenam, info->shot, nid);
  return 1;
}
