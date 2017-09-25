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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "zlib/zlib.h"

#include "mdsip_connections.h"


static int SendBytes(int id, void *buffer, size_t bytes_to_send, int options)
{
  char *bptr = (char *)buffer;
  IoRoutines *io = GetConnectionIo(id);
  if (io) {
    int tries = 0;
    while ((bytes_to_send > 0) && (tries < 10)) {
      ssize_t bytes_sent;
      bytes_sent = io->send(id, bptr, bytes_to_send, options);
      if (bytes_sent <= 0) {
	if (errno != EINTR)
	  return 0;
	tries++;
      } else {
	bytes_to_send -= bytes_sent;
	bptr += bytes_sent;
	tries = 0;
      }
    }
    if (tries >= 10) {
      char msg[256];
      sprintf(msg, "\rsend failed, shutting down connection %d", id);
      perror(msg);
      DisconnectConnection(id);
      return 0;
    }
    return 1;
  }
  return 0;
}





int SendMdsMsg(int id, Message * m, int msg_options)
{
  unsigned long len = m->h.msglen - sizeof(m->h);
  unsigned long clength = 0;
  Message *cm = 0;
  int status;
  int do_swap = 0;		/*Added to handle byte swapping with compression */

  if (len > 0 && GetCompressionLevel() > 0 && m->h.client_type != SENDCAPABILITIES) {
    clength = len;
    cm = (Message *) malloc(m->h.msglen + 4);
  }
  if (!msg_options)
    FlushConnection(id);
  if (m->h.client_type == SENDCAPABILITIES)
    m->h.status = GetCompressionLevel();
  if ((m->h.client_type & SwapEndianOnServer) != 0) {
    if (Endian(m->h.client_type) != Endian(ClientType())) {
      FlipData(m);
      FlipHeader(&m->h);
      do_swap = 1;		/* Recall that the header field msglen needs to be swapped */
    }
  } else
    m->h.client_type = ClientType();
  if (clength
      && compress2((unsigned char *)cm->bytes + 4, &clength, (unsigned char *)m->bytes, len,
		   GetCompressionLevel()) == 0 && clength < len) {
    cm->h = m->h;
    cm->h.client_type |= COMPRESSED;
    memcpy(cm->bytes, &cm->h.msglen, 4);
    cm->h.msglen = clength + 4 + sizeof(MsgHdr);
/*If byte swapping required, swap msglen */
    if (do_swap)
      FlipBytes(4, (char *)&cm->h.msglen);
    /* status = SendBytes(id, (char *)cm, cm->h.msglen, oob); */
/* now msglen is swapped, and cannot be used as byte counter */
    status = SendBytes(id, (char *)cm, clength + 4 + sizeof(MsgHdr), msg_options);
  } else
    status = SendBytes(id, (char *)m, len + sizeof(MsgHdr), msg_options);
  if (clength)
    free(cm);
  return status;
}
