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
#ifdef DEBUG
#include <stdio.h>
#endif
#include "../mdsip_connections.h"
#include <status.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
//  GetAnswerInfo  /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

__attribute__((deprecated)) int GetAnswerInfo(int id, char *dtype,
                                              short *length, char *ndims,
                                              int *dims, int *numbytes,
                                              void **dptr)
{
  static void *m = 0;
  if (m)
  {
    free(m);
    m = NULL;
  }
  return GetAnswerInfoTS(id, dtype, length, ndims, dims, numbytes, dptr, &m);
}

////////////////////////////////////////////////////////////////////////////////
//  GetAnswerInfoTS  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int GetAnswerInfoTS(int id, char *dtype, short *length, char *ndims,
                    int *dims, int *numbytes, void **dptr, void **mout)
{
  return GetAnswerInfoTO(id, dtype, length, ndims, dims, numbytes, dptr, mout,
                         -1);
}

int GetAnswerInfoTO(int id, char *dtype, short *length, char *ndims, int *dims,
                    int *numbytes, void **dptr, void **mout, int timeout_msec)
{
  Connection *c = FindConnectionSending(id);
  if (!c)
    return MDSplusERROR;
  INIT_STATUS;
  int i;
  Message *m;
  m = GetMdsMsgTOC(c, &status, timeout_msec);
  UnlockConnection(c);
  if (!m && status == SsINTERNAL)
  {
    CloseConnection(id);
    status = MDSplusERROR;
  }
  if (STATUS_NOT_OK)
  {
    free(m);
    *mout = NULL;
    *dtype = 0;
    *length = 0;
    *ndims = 0;
    *numbytes = 0;
    *dptr = 0;
    return status;
  }
  int datalen = (int)(m->h.msglen - sizeof(MsgHdr));
  *numbytes = m->h.length;
  if (m->h.ndims)
  {
    for (i = 0; i < m->h.ndims; i++)
    {
#ifdef __CRAY
      dims[i] =
          i % 2 ? m->h.dims[i / 2] & 0xffffffff : (*m)->h.dims[i / 2] >> 32;
#else
      if (m->h.dims[i] || !datalen)
        dims[i] = m->h.dims[i];
      else
      {
        if (datalen > *numbytes && datalen % *numbytes)
        {
          dims[i] = datalen / *numbytes;
          m->h.ndims = i + 1;
        }
        else if (datalen == *numbytes && m->h.ndims == i + 1)
        {
          dims[i] = 1;
        }
        else
        {
          m->h.ndims = i;
          break; // dont mult numbytes
        }
      }
#endif
      *numbytes *= dims[i];
#ifdef DEBUG
      printf("dim[%d] = %d\n", i, dims[i]);
#endif
    }
    for (i = m->h.ndims; i < MAX_DIMS; i++)
      dims[i] = 0;
  }
  else
  {
    for (i = 0; i < MAX_DIMS; i++)
      dims[i] = 0;
  }
  if (*numbytes != datalen)
  {
    *numbytes = 0;
    if (m)
    {
      free(m);
      *mout = 0;
    }
    return MDSplusERROR;
  }
  *dtype = m->h.dtype;
  *length = m->h.length;
  *ndims = m->h.ndims;
  *dptr = m->bytes;
  *mout = m;
  return m->h.status;
}
