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
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
//  MdsValue  //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int MdsValue(int id, char *expression, ...)
{
/**** NOTE: NULL terminated argument list expected ****/
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  unsigned char idx;
  int status = 1;
  struct descrip exparg;
  struct descrip *ans_arg;
  struct descrip *arg = &exparg;
#ifdef _USE_VARARGS
  va_start(incrmtr);
  id = va_arg(incrmtr, int);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
  for (a_count = 1; arg != NULL; a_count++) {
    ans_arg = arg;
    arg = va_arg(incrmtr, struct descrip *);
  }
#ifdef _USE_VARARGS
  va_start(incrmtr);
  id = va_arg(incrmtr, int);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
  nargs = a_count - 2;
  arg = MakeDescrip((struct descrip *)&exparg, DTYPE_CSTRING, 0, 0, expression);
  for (i = 1; i < a_count - 1 && (status & 1); i++) {
    idx = i - 1;
    status = SendArg(id, idx, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    arg = va_arg(incrmtr, struct descrip *);
  }
  va_end(incrmtr);
  if (status & 1) {
    short len;
    int numbytes;
    void *dptr;
    void *mem = 0;
    status =
	GetAnswerInfoTS(id, &ans_arg->dtype, &len, &ans_arg->ndims, ans_arg->dims, &numbytes, &dptr, &mem);
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
