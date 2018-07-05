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
#include <status.h>



int SendArg(int id, unsigned char idx, char dtype, unsigned char nargs, unsigned short length, char ndims,
	    int *dims, char *bytes){
  INIT_STATUS_AS MDSplusERROR;
  int msglen;
  int i;
  int nbytes = length;
  Message *m;

  // MESSAGE ID //
  // * if this is the first argument sent, increments connection message id   //
  // * get the connection message_id and store it inside message              //

  if (idx > nargs) {
    /**** Special I/O message ****/
    nbytes = dims[0];
  } else {
    for (i = 0; i < ndims; i++)
      nbytes *= dims[i];
  }
  msglen = sizeof(MsgHdr) + nbytes;
  m = memset(malloc(msglen), 0, msglen);
  m->h.client_type = 0;
  m->h.msglen = msglen;
  m->h.descriptor_idx = idx;
  m->h.dtype = dtype;
  m->h.nargs = nargs;
  m->h.length = length;
  m->h.ndims = ndims;
#ifdef __CRAY
  for (i = 0; i < 4; i++)
    m->h.dims[i] =
	((ndims > i * 2) ? (dims[i * 2] << 32) : 0) | ((ndims >
							(i * 2 + 1)) ? (dims[i * 2 + 1]) : 0);
#else
  for (i = 0; i < MAX_DIMS_R; i++)
    m->h.dims[i] = i < ndims ? dims[i] : 0;
#endif
  memcpy(m->bytes, bytes, nbytes);
  Connection* c = idx==0 ? FindConnectionWithLock(id, CON_SENDARG) : FindConnection(id, NULL);
  m->h.message_id = (idx == 0 || nargs == 0)
            ? IncrementConnectionMessageIdC(c)
            : c->message_id;
  status = m->h.message_id ? SendMdsMsgC(c, m, 0) : MDSplusERROR;
  free(m);
  if STATUS_NOT_OK UnlockConnection(c);
  return status;
}
