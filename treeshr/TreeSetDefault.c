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
#include <treeshr.h>
#include "treeshrp.h"

extern void **TreeCtx();

int TreeSetDefault(char *path, int *nid)
{
  return _TreeSetDefault(*TreeCtx(), path, nid);
}

int TreeSetDefaultNid(int nid_in)
{
  return _TreeSetDefaultNid(*TreeCtx(), nid_in);
}

int TreeGetDefaultNid(int *nid_in)
{
  return _TreeGetDefaultNid(*TreeCtx(), nid_in);
}

int _TreeSetDefault(void *dbid, char *path, int *nid)
{
  int status;
  status = _TreeFindNode(dbid, path, nid);
  if (status & 1)
    status = _TreeSetDefaultNid(dbid, *nid);
  return status;
}

int _TreeSetDefaultNid(void *dbid, int nid_in)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid = (NID *) & nid_in;
  if (IS_OPEN(dblist)) {
    NODE *node_ptr;
    if (dblist->remote)
      return SetDefaultNidRemote(dblist, nid_in);
    node_ptr = nid_to_node(dblist, nid);
    if (node_ptr) {
      dblist->default_node = node_ptr;
      return TreeNORMAL;
    } else
      return TreeNNF;
  } else
    return TreeNOT_OPEN;
}

int _TreeGetDefaultNid(void *dbid, int *nid_in)
{
  PINO_DATABASE *dblist = (PINO_DATABASE *) dbid;
  NID *nid = (NID *) nid_in;
  if (!(IS_OPEN(dblist)))
    return TreeNOT_OPEN;
  if (dblist->remote)
    return GetDefaultNidRemote(dblist, nid_in);
  node_to_nid(dblist, (dblist->default_node), nid);
  return TreeNORMAL;
}
