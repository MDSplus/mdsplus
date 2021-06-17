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
#include "../zlib/zlib.h"
#include <errno.h>
#include <pthread_port.h>
#include <status.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tdishr_messages.h>
#include <unistd.h>
//#define DEBUG
#include <mdsmsg.h>

static int get_bytes_to(Connection *c, void *buffer, size_t bytes_to_recv, int to_msec)
{
  char *bptr = (char *)buffer;
  if (!c || !c->io)
    return MDSplusERROR;
  MDSDBG(CON_PRI " awaiting %ld bytes", CON_VAR(c), (long)bytes_to_recv);
  while (bytes_to_recv > 0)
  {
    ssize_t ans;
    errno = 0;
    // don't use timeout if not available or requested
    if (c->io->recv_to && to_msec >= 0)
      ans = c->io->recv_to(c, bptr, bytes_to_recv, to_msec);
    else
      ans = c->io->recv(c, bptr, bytes_to_recv);
    if (ans > 0)
    {
      bytes_to_recv -= ans;
      bptr += ans;
      continue;
    } // only exception from here on
    ssize_t received = bptr - (char *)buffer;
    if (ans < 0)
    {
      MDSERR(CON_PRI " error %ld/%ld", CON_VAR(c),
             (long)received, (long)(received + bytes_to_recv));
      if (errno == ETIMEDOUT)
        return TdiTIMEOUT;
      if (errno == EINTR)
        return MDSplusERROR;
      if (errno == EINVAL)
        return SsINTERNAL;
    }
    else
    {
      MDSDBG(CON_PRI " closed %ld/%ld", CON_VAR(c),
             (long)received, (long)(received + bytes_to_recv));
    }
    return SsINTERNAL;
  }
  MDSDBG(CON_PRI "got all bytes", CON_VAR(c));
  return MDSplusSUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
//  GetMdsMsg  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Message *GetMdsMsgTOC(Connection *c, int *status, int to_msec)
{
  MsgHdr header;
  Message *msg = NULL;
  *status = get_bytes_to(c, (void *)&header, sizeof(MsgHdr), to_msec);
  if (*status == SsINTERNAL)
    return NULL;
  if (IS_OK(*status))
  {
    if (Endian(header.client_type) != Endian(ClientType()))
      FlipHeader(&header);
    MDSDBG("Message(msglen = %d, status = %d, length = %d, nargs = %d, "
           "descriptor_idx = %d, message_id = %d, dtype = %d, "
           "client_type = %d, header.ndims = %d)",
           header.msglen, header.status, header.length, header.nargs,
           header.descriptor_idx, header.message_id, header.dtype,
           header.client_type, header.ndims);
    uint32_t msglen = (uint32_t)header.msglen;
    if (msglen < sizeof(MsgHdr) || CType(header.client_type) > CRAY_CLIENT ||
        header.ndims > MAX_DIMS)
    {
      fprintf(stderr,
              "\nGetMdsMsg shutdown connection %d: bad msg header, "
              "header.ndims=%d, client_type=%d\n",
              c->id, header.ndims, CType(header.client_type));
      *status = SsINTERNAL;
      return NULL;
    }
    unsigned long dlen;
    msg = malloc(msglen);
    msg->h = header;
    *status = get_bytes_to(c, msg->bytes, msglen - sizeof(MsgHdr), 1000);
    if (IS_OK(*status) && IsCompressed(header.client_type))
    {
      Message *m;
      memcpy(&msglen, msg->bytes, 4);
      dlen = msglen - sizeof(MsgHdr);
      if (Endian(header.client_type) != Endian(ClientType()))
        FlipBytes(4, (char *)&msglen);
      m = malloc(msglen);
      m->h = header;
      *status = uncompress((unsigned char *)m->bytes, &dlen,
                           (unsigned char *)msg->bytes + 4, dlen - 4) == 0;
      if (IS_OK(*status))
      {
        m->h.msglen = msglen;
        free(msg);
        msg = m;
      }
      else
        free(m);
    }
    if (IS_OK(*status) &&
        (Endian(header.client_type) != Endian(ClientType())))
      FlipData(msg);
  }
  return msg;
}

Message *GetMdsMsgTO(int id, int *status, int to_msec)
{
  Connection *c = FindConnectionWithLock(id, CON_RECV);
  Message *msg = GetMdsMsgTOC(c, status, to_msec);
  UnlockConnection(c);
  if (!msg && *status == SsINTERNAL)
  {
    // not for ETIMEDOUT or EINTR like exceptions
    CloseConnection(id);
    *status = MDSplusERROR;
  }
  return msg;
}

Message *GetMdsMsg(int id, int *status) { return GetMdsMsgTO(id, status, -1); }

////////////////////////////////////////////////////////////////////////////////
//  GetMdsMsgOOB  //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Message *GetMdsMsgOOB(int id, int *status) { return GetMdsMsg(id, status); }
