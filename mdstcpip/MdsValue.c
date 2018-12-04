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

#include "mdsip_connections.h"
#include <mdsshr.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <status.h>

////////////////////////////////////////////////////////////////////////////////
//  MdsValue  //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
# define DBG(...) fprintf(stderr, __VA_ARGS__)
#else
# define DBG(...)
#endif
EXPORT int _MdsValue(int id, int nargs, struct descrip** arglist, struct descrip* ans_arg) {
  DBG("mdstcpip.MdsValue> '%s'\n", (char*)(**arglist).ptr);
  int i,status = 1;
  for (i = 0; i < nargs && STATUS_OK; i++)
    status = SendArg(id, i, arglist[i]->dtype, nargs, ArgLen(arglist[i]), arglist[i]->ndims, arglist[i]->dims, arglist[i]->ptr);
  if STATUS_OK {
    short len;
    int numbytes;
    void *dptr;
    void *mem = 0;
    status = GetAnswerInfoTS(id, &ans_arg->dtype, &len, &ans_arg->ndims, ans_arg->dims, &numbytes, &dptr, &mem);
    ans_arg->length = len;
    if (numbytes) {
      if (ans_arg->dtype == DTYPE_CSTRING) {
        ans_arg->ptr = malloc(numbytes + 1);
        ((char *)ans_arg->ptr)[numbytes] = 0;
      } else if (numbytes > 0)
        ans_arg->ptr = malloc(numbytes);
      if (numbytes > 0)
        memcpy(ans_arg->ptr, dptr, numbytes);
    } else
      ans_arg->ptr = NULL;
    if (mem)
      free(mem);
  } else
    ans_arg->ptr = NULL;
  return status;
}

EXPORT int _MdsValueDsc(int id, int nargs, struct descrip** arglist_in, struct descriptor_xd* ans_ptr) {
  DBG("mdstcpip.MdsValueDsc> '%s'\n", (char*)(**arglist_in).ptr);
  struct descrip** arglist = malloc((++nargs)*sizeof(struct descrip*));
  int i;
  char* ser = malloc(2 * nargs + 128);
  strcpy(ser,"_=*;MdsShr->MdsSerializeDscOut(xd(execute($");
  for ( i=1 ; i<nargs ; i++) arglist[i] = arglist_in[i-1];
  for ( i=2 ; i<nargs ; i++) strcat(ser,",$"); //add remaining inputs
  strcat(ser,")),xd(_));execute('deallocate(\"_\");`_')");
  struct descrip ser_dsc = {DTYPE_CSTRING,0,{0},0,ser};
  arglist[0] = &ser_dsc;
  struct descrip ans_tmp = {0};
  int status = _MdsValue(id,nargs,arglist,&ans_tmp);
  if (STATUS_OK && ans_tmp.dtype==DTYPE_B)
    status = MdsSerializeDscIn((char*)ans_tmp.ptr,ans_ptr);
  if (ans_tmp.ptr) MdsIpFree(ans_tmp.ptr);
  free(arglist);
  free(ser);
  return status;
}

int MdsValue(int id, char *expression, ...){
/**** NOTE: NULL terminated argument list expected ****/
  int nargs;
  struct descrip* arglist[265];
  VA_LIST_NULL(arglist,nargs,1,-1,expression);
  struct descrip exparg = {DTYPE_CSTRING,0,{0},0,(char*)expression};
  arglist[0] = &exparg;
  return _MdsValue(id,nargs,arglist,arglist[nargs]);
}

EXPORT int MdsValueDsc(int id, const char *expression, ...){
  int nargs;
  struct descrip* arglist[265];
  VA_LIST_NULL(arglist,nargs,1,-1,expression);
  struct descrip exp_dsc = {DTYPE_CSTRING,0,{0},0,(char*)expression};
  arglist[0] = &exp_dsc;
  struct descriptor_xd *ans_ptr = (struct descriptor_xd *)arglist[nargs];
  return _MdsValueDsc(id,nargs,arglist,ans_ptr);
}
