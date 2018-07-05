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


void FlipData(Message * m)
{
  int num = 1;
  int i;
  char *ptr;
  int dims[MAX_DIMS_R];
  for (i = 0; i < MAX_DIMS_R; i++) {
#ifdef __CRAY
    dims[i] = i % 2 ? m->h.dims[i / 2] & 0xffffffff : m->h.dims[i / 2] >> 32;
#else
    dims[i] = m->h.dims[i];
#endif
  }
  if (m->h.ndims)
    for (i = 0; i < m->h.ndims; i++)
      num *= dims[i];
#ifdef DEBUG
  printf("num to flip = %d\n", num);
#endif
  switch (m->h.dtype) {
#ifndef __CRAY
  case DTYPE_COMPLEX:
  case DTYPE_COMPLEX_DOUBLE:
    for (i = 0, ptr = m->bytes; i < (num * 2); i++, ptr += m->h.length / 2)
      FlipBytes(m->h.length / 2, ptr);
    break;
  case DTYPE_FLOAT:
  case DTYPE_DOUBLE:
#endif
  case DTYPE_LONGLONG:
  case DTYPE_ULONGLONG:
  case DTYPE_USHORT:
  case DTYPE_SHORT:
  case DTYPE_ULONG:
  case DTYPE_LONG:
    for (i = 0, ptr = m->bytes; i < num; i++, ptr += m->h.length)
      FlipBytes(m->h.length, ptr);
    break;
  }
}
