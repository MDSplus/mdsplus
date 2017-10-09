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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <status.h>
#include "zlib/zlib.h"
#include "mdsip_connections.h"
#include <pthread_port.h>
#include <tdishr_messages.h>

static int GetBytesTO(int id, void *buffer, size_t bytes_to_recv, int to_msec){
  char *bptr = (char *)buffer;
  IoRoutines *io = GetConnectionIo(id);
  if (io) {
    int tries = 0;
    while (bytes_to_recv > 0 && (tries < 10)) {
      ssize_t bytes_recv;
      if (io->recv_to && to_msec>=0) // don't use timeout if not available or requested
        bytes_recv = io->recv_to(id, bptr, bytes_to_recv, to_msec);
      else
        bytes_recv = io->recv(id, bptr, bytes_to_recv);
      if (bytes_recv <= 0) {
        if (errno != ETIMEDOUT) return TdiTIMEOUT;
	if (errno != EINTR)     return MDSplusERROR;
	tries++;
      } else {
	tries = 0;
	bytes_to_recv -= bytes_recv;
	bptr += bytes_recv;
      }
    }
    if (tries >= 10) {
      DisconnectConnection(id);
      fprintf(stderr, "\rrecv failed for connection %d: too many EINTR's", id);
      return MDSplusERROR;
    }
    return MDSplusSUCCESS;
  }
  return MDSplusERROR;
}

static int GetBytes(int id, void* buffer, size_t bytes_to_recv){
  return GetBytesTO(id, buffer, bytes_to_recv, -1);
}

////////////////////////////////////////////////////////////////////////////////
//  GetMdsMsg  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Message *GetMdsMsgTO(int id, int *status, int to_msec){
  MsgHdr header;
  Message *msg = 0;
  int msglen = 0;
  //MdsSetClientAddr(0);
  *status = GetBytesTO(id, (void *)&header, sizeof(MsgHdr), to_msec);
  if IS_OK(*status) {
    if (Endian(header.client_type) != Endian(ClientType()))
      FlipHeader(&header);
#ifdef DEBUG
    printf
	("msglen = %d\nstatus = %d\nlength = %d\nnargs = %d\ndescriptor_idx = %d\nmessage_id = %d\ndtype = %d\n",
	 header.msglen, header.status, header.length, header.nargs, header.descriptor_idx,
	 header.message_id, header.dtype);
    printf("client_type = %d\nndims = %d\n", header.client_type, header.ndims);
#endif
    if (CType(header.client_type) > CRAY_CLIENT || header.ndims > MAX_DIMS) {
      DisconnectConnection(id);
      fprintf(stderr,
	      "\rGetMdsMsg shutdown connection %d: bad msg header, header.ndims=%d, client_type=%d\n",
	      id, header.ndims, CType(header.client_type));
      *status = MDSplusERROR;
      return NULL;
    }
    msglen = header.msglen;
    msg = malloc(header.msglen);
    msg->h = header;
    *status = GetBytes(id, msg->bytes, msglen - sizeof(MsgHdr));
    if (IS_OK(*status) && IsCompressed(header.client_type)) {
      Message *m;
      unsigned long dlen;
      memcpy(&msglen, msg->bytes, 4);
      if (Endian(header.client_type) != Endian(ClientType()))
	FlipBytes(4, (char *)&msglen);
      m = malloc(msglen);
      m->h = header;
      dlen = msglen - sizeof(MsgHdr);
      *status =
	  uncompress((unsigned char *)m->bytes, &dlen, (unsigned char *)msg->bytes + 4,
		     header.msglen - sizeof(MsgHdr) - 4) == 0;
      if IS_OK(*status) {
	m->h.msglen = msglen;
	free(msg);
	msg = m;
      } else
	free(m);
    }
    if (IS_OK(*status) && (Endian(header.client_type) != Endian(ClientType())))
      FlipData(msg);
  }
  return msg;
}

Message *GetMdsMsg(int id, int *status){
  return GetMdsMsgTO(id, status, -1);
}

////////////////////////////////////////////////////////////////////////////////
//  GetMdsMsgOOB  //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Message *GetMdsMsgOOB(int id, int *status){
  return GetMdsMsg(id, status);
}
