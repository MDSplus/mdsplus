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
#include "mdsip_connections.h"
#include <ipdesc.h>
#include <stdlib.h>


////////////////////////////////////////////////////////////////////////////////
//  MdsOpen  ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


int MdsOpen(int id, char *tree, int shot)
{
  struct descrip treearg;
  struct descrip shotarg;
  struct descrip ansarg;
  STATIC_CONSTANT char *expression = "TreeOpen($,$)";
  int status =
      MdsValue(id, expression, MakeDescrip((struct descrip *)&treearg, DTYPE_CSTRING, 0, 0, tree),
	       MakeDescrip((struct descrip *)&shotarg, DTYPE_LONG, 0, 0, &shot),
	       (struct descrip *)&ansarg, (struct descrip *)NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG))
    status = *(int *)ansarg.ptr;
  if (ansarg.ptr)
    free(ansarg.ptr);
  return status;
}
