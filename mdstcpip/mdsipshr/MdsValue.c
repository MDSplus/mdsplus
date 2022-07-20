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

#include "../mdsip_connections.h"
#include "../mdsIo.h"
#include <mdsshr.h>
#include <status.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
//  MdsValue  //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <mdsmsg.h>

#define SERIAL
#define DEF_SERIAL_IN                          \
  "public fun __si(in _i)"                     \
  "{"                                          \
  "_o=*;"                                      \
  "MdsShr->MdsSerializeDscIn(ref(_i),xd(_o));" \
  "return(_o);"                                \
  "};"
#define DEF_SERIAL_OUT                         \
  "public fun __so(optional in _i)"            \
  "{"                                          \
  "_o=*;"                                      \
  "MdsShr->MdsSerializeDscOut(xd(_i),xd(_o));" \
  "return(_o);"                                \
  "};"

EXPORT int MdsIpGetDescriptor(int id, const char *expression, int nargs,
                              mdsdsc_t **arglist_in,
                              mdsdsc_xd_t *ans_ptr)
{
  int dim = 0;
  int i, status;
  int version = MdsIpGetConnectionVersion(id);
  const int expect_serial = version >= MDSIP_VERSION_DSC_ARGS;
  if (expect_serial)
  {
    DESCRIPTOR_FROM_CSTRING(exp_dsc, expression);
    status = SendDsc(id, 0, ++nargs, &exp_dsc);
    for (i = 1; i < nargs && STATUS_OK; i++)
      status = SendDsc(id, i, nargs, arglist_in[i - 1]);
  }
  else
  {
    EMPTYXD(xd);
    char *newexp = malloc(16 + 8 * 8 + sizeof(DEF_SERIAL_OUT) + sizeof(DEF_SERIAL_IN));
    strcpy(newexp, DEF_SERIAL_IN DEF_SERIAL_OUT "__so(execute($");
    for (i = 0; i < nargs; i++)
      strcat(newexp, ",__si($)");
    strcat(newexp, "))");
    nargs += 2;
    MDSDBG(newexp);
    status = SendArg(id, 0, DTYPE_CSTRING, nargs, strlen(newexp), 0, &dim,
                     (char *)newexp);
    free(newexp);
    MDSDBG(expression);
    struct descriptor_a *arr;
    status = SendArg(id, 1, DTYPE_CSTRING, nargs, strlen(expression), 0, &dim,
                     (char *)expression);
    for (i = 2; i < nargs && STATUS_OK; i++)
    {
      status = MdsSerializeDscOut(arglist_in[i - 2], &xd);
      arr = (mdsdsc_a_t *)xd.pointer;
      if (STATUS_OK)
        status = SendArg(id, i, arr->dtype, nargs, arr->length, 1,
                         (int *)&arr->arsize, arr->pointer);
    }
    MdsFree1Dx(&xd, NULL);
  }
  if (STATUS_OK)
  {
    char ndims;
    void *mem = 0;
    int dims[MAX_DIMS] = {0};
    mdsdsc_a_t ser = {0};
    status = GetAnswerInfoTS(id, (char *)&ser.dtype, (short int *)&ser.length,
                             &ndims, dims, (int *)&ser.arsize,
                             (void **)&ser.pointer, &mem);
    if (STATUS_OK)
    {
      ser.class = CLASS_A;
      if ((expect_serial && ser.dtype == DTYPE_SERIAL) || ser.dtype == DTYPE_B)
        status = MdsSerializeDscIn(ser.pointer, ans_ptr);
      else
        status = MdsCopyDxXd((mdsdsc_t *)&ser, ans_ptr);
      free(mem);
    }
  }
  return status;
}

EXPORT int _MdsValue(int id, int nargs, struct descrip **arglist,
                     struct descrip *ans_arg)
{
  MDSDBG("mdstcpip.MdsValue> '%s'", (char *)(**arglist).ptr);
  int i, status = 1;
  for (i = 0; i < nargs && STATUS_OK; i++)
    status = SendArg(id, i, arglist[i]->dtype, nargs, ArgLen(arglist[i]),
                     arglist[i]->ndims, arglist[i]->dims, arglist[i]->ptr);
  if (STATUS_OK)
  {
    short len;
    int numbytes;
    void *dptr;
    void *mem = 0;
    status = GetAnswerInfoTS(id, &ans_arg->dtype, &len, &ans_arg->ndims,
                             ans_arg->dims, &numbytes, &dptr, &mem);
    ans_arg->length = len;
    if (numbytes)
    {
      if (ans_arg->dtype == DTYPE_CSTRING)
      {
        ans_arg->ptr = malloc(numbytes + 1);
        ((char *)ans_arg->ptr)[numbytes] = 0;
      }
      else if (numbytes > 0)
        ans_arg->ptr = malloc(numbytes);
      if (numbytes > 0)
        memcpy(ans_arg->ptr, dptr, numbytes);
    }
    else
      ans_arg->ptr = NULL;
    free(mem);
  }
  else
    ans_arg->ptr = NULL;
  return status;
}

EXPORT int MdsValue(int id, char *expression, ...)
{
  /**** NOTE: NULL terminated argument list expected ****/
  int nargs;
  struct descrip *arglist[265];
  VA_LIST_NULL(arglist, nargs, 1, -1, expression);
  struct descrip exparg = {DTYPE_CSTRING, 0, {0}, 0, (char *)expression};
  arglist[0] = &exparg;
  return _MdsValue(id, nargs, arglist, arglist[nargs]);
}

EXPORT int MdsValueDsc(int id, const char *expression, ...)
{
  /**** NOTE: MDS_END_ARG terminated argument list expected ****/
  int nargs;
  struct descriptor *arglist[265];
  VA_LIST_NULL(arglist, nargs, 0, -1, expression);
  struct descriptor_xd *ans_ptr = (struct descriptor_xd *)arglist[nargs];
  return MdsIpGetDescriptor(id, expression, nargs, arglist, ans_ptr);
}

EXPORT void MdsIpFree(void *ptr)
{
  /* for compatibility malloc memory should be freed by same module/lib
   * use MdsIpFree to free ans.ptr returned by MdsValue
   */
  free(ptr);
}

EXPORT void MdsIpFreeDsc(struct descriptor_xd *xd)
{
  /* for compatibility malloc memory should be freed by same module/lib
   * use MdsIpFreeDsc to free xd returned by MdsValueDsc
   */
  if (xd)
    free_xd(xd);
}
