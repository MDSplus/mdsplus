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
#include <stdio.h>
#include <treeshr_hooks.h>
extern char *TreeGetPath(int);
int Notify(TreeshrHookType htype, char *tree, int shot, int nid)
{
  char *name;
  char def_name[50];
  char *na = "N/A";
  char *path = na;
  sprintf(def_name, "Hook number %d", htype);
  name = def_name;
  switch (htype) {
  case OpenTree:
    name = "OpenTree";
    break;
  case OpenTreeEdit:
    name = "OpenTreeEdit";
    break;
  case RetrieveTree:
    return 0;
  case WriteTree:
    name = "WriteTree";
    break;
  case CloseTree:
    name = "CloseTree";
    break;
  case OpenNCIFileWrite:
    return 1;
  case OpenDataFileWrite:
    return 1;
  case GetData:
    name = "GetData";
    path = TreeGetPath(nid);
    break;
  case GetNci:
    name = "GetNci";
    path = TreeGetPath(nid);
    break;
  case PutData:
    name = "PutData";
    path = TreeGetPath(nid);
    break;
  case PutNci:
    name = "PutNci";
    path = TreeGetPath(nid);
    break;
  }
  printf("%s hook called for tree=%s, shot=%d, node=%s\n", name, tree, shot, path);
  if (path != na && path != (char *)0)
    free(path);
  return 1;
}
