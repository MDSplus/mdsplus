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
#include <string.h>

#include "mdsip_connections.h"

short ArgLen(struct descrip *d)
{
  short len = 0;
  switch (d->dtype) {
  case DTYPE_CSTRING:
    len = d->length ? d->length : (short)(d->ptr ? strlen(d->ptr) : 0);
    break;
  case DTYPE_UCHAR:
  case DTYPE_CHAR:
    len = sizeof(char);
    break;
  case DTYPE_USHORT:
  case DTYPE_SHORT:
    len = sizeof(short);
    break;
  case DTYPE_ULONG:
  case DTYPE_LONG:
    len = sizeof(int);
    break;
  case DTYPE_FLOAT:
    len = sizeof(float);
    break;
  case DTYPE_DOUBLE:
    len = sizeof(double);
    break;
  case DTYPE_COMPLEX:
    len = sizeof(float) * 2;
    break;
  case DTYPE_COMPLEX_DOUBLE:
    len = sizeof(double) * 2;
    break;
  case DTYPE_ULONGLONG:
  case DTYPE_LONGLONG:
    len = 8;
    break;
  }
  return len;
}
