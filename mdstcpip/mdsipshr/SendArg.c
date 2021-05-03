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
#include <stdlib.h>
#include <string.h>

int SendDsc(int id, unsigned char idx, unsigned char nargs,
            struct descriptor *dsc)
{
  int status;
  INIT_AND_FREEXD_ON_EXIT(out);
  status = MdsSerializeDscOut(dsc, &out);
  if (STATUS_OK)
  {
    struct descriptor_a *array = (struct descriptor_a *)out.pointer;
    int dims[MAX_DIMS] = {0};
    dims[0] = array->arsize;
    status = SendArg(id, idx, DTYPE_SERIAL, nargs, 1, 1, dims,
                     (char *)array->pointer);
  }
  FREEXD_NOW(out);
  return status;
}

int SendArg(int id, unsigned char idx, char dtype, unsigned char nargs,
            unsigned short length, char ndims, int *dims, char *bytes)
{
  Connection *c = (idx == 0 || idx > nargs)
                      ? FindConnectionWithLock(id, CON_REQUEST)
                      : FindConnectionSending(id);
  if (!c)
    return MDSplusERROR; // both methods will leave connection id unlocked

  int msglen;
  int i;
  int nbytes = length;
  Message *m;

  // MESSAGE ID //
  // * if this is the first argument sent, increments connection message id   //
  // * get the connection message_id and store it inside message              //
  if (idx > nargs)
  {
    /**** Special I/O message ****/
    if (idx == MDS_IO_OPEN_ONE_K && c->version < MDSIP_VERSION_OPEN_ONE)
    {
      UnlockConnection(c);
      return MDSplusFATAL;
    }
    nbytes = dims[0];
  }
  else
  {
    for (i = 0; i < ndims; i++)
      nbytes *= dims[i];
  }
  if (!bytes)
    nbytes = 0;
  msglen = sizeof(MsgHdr) + nbytes;
  m = malloc(msglen);
  memset(&m->h, 0, sizeof(m->h));
  m->h.msglen = msglen;
  m->h.descriptor_idx = idx;
  m->h.dtype = dtype;
  m->h.nargs = nargs;
  m->h.length = length;
  m->h.ndims = ndims;
#ifdef __CRAY
  for (i = 0; i < 4; i++)
    m->h.dims[i] = ((ndims > i * 2) ? (dims[i * 2] << 32) : 0) |
                   ((ndims > (i * 2 + 1)) ? (dims[i * 2 + 1]) : 0);
#else
  for (i = 0; i < MAX_DIMS; i++)
    m->h.dims[i] = i < ndims ? dims[i] : 0;
#endif
  if (nbytes > 0)
    memcpy(m->bytes, bytes, nbytes);
  m->h.message_id = (idx == 0 || nargs == 0) ? ConnectionIncMessageId(c)
                                             : c->message_id;
  int status = m->h.message_id ? SendMdsMsgC(c, m, 0) : MDSplusERROR;
  free(m);
  if (STATUS_NOT_OK)
    UnlockConnection(c);
  return status;
}
