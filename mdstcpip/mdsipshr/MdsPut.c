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
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
//  MdsPut  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int MdsPut(int id, char *node, char *expression, ...)
{
  /**** NOTE: NULL terminated argument list expected ****/
  int i;
  int status = 1;
  int nargs;
  struct descrip dsc[3];
  struct descrip *arglist[265];
  VA_LIST_NULL(arglist, nargs, 3, -1, expression); // -1: dont count putexpr
  char *putexpr = malloc(8 + nargs * 2 + 1);
  strcpy(putexpr, "TreePut(");
  for (i = 0; i < nargs - 1; i++)
    strcat(putexpr, "$,");
  strcat(putexpr, "$)");
  arglist[0] = MakeDescrip(&dsc[0], DTYPE_CSTRING, 0, 0, putexpr);
  arglist[1] = MakeDescrip(&dsc[1], DTYPE_CSTRING, 0, 0, node);
  arglist[2] = MakeDescrip(&dsc[2], DTYPE_CSTRING, 0, 0, expression);
  for (i = 0; i <= nargs && STATUS_OK; i++)
    status = SendArg(id, i, arglist[i]->dtype, nargs, ArgLen(arglist[i]),
                     arglist[i]->ndims, arglist[i]->dims, arglist[i]->ptr);
  free(putexpr);
  if (STATUS_OK)
  {
    char dtype;
    int dims[MAX_DIMS];
    char ndims;
    short len;
    int numbytes;
    void *dptr;
    void *mem = 0;
    status = GetAnswerInfoTS(id, &dtype, &len, &ndims, dims, &numbytes, &dptr,
                             &mem);
    if (STATUS_OK && dtype == DTYPE_LONG && ndims == 0 &&
        numbytes == sizeof(int))
      memcpy(&status, dptr, numbytes);
    free(mem);
  }
  return status;
}
