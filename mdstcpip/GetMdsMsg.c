#include "mdsip_connections.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static int GetBytes(int id, void *buffer, size_t bytes_to_recv) {
  char *bptr = (char *)buffer;
  IoRoutines *io = GetConnectionIo(id);
  if (io) {
    int tries = 0;
    while (bytes_to_recv > 0 && (tries < 10)) {
      ssize_t bytes_recv;
      bytes_recv = io->recv(id, bptr, bytes_to_recv);
      if (bytes_recv <= 0) {
	if (errno != EINTR)
	  return 0;
	tries++;
      }
      else {
	tries = 0;
	bytes_to_recv -= bytes_recv;
	bptr += bytes_recv;
      }
    }
    if (tries >= 10) {
      DisconnectConnection(id);
      fprintf(stderr,"\rrecv failed for connection %d: too many EINTR's",id);
      return 0;
    }
    return 1;
  }
  return 0;
}

Message *GetMdsMsg(int id, int *status) {
  MsgHdr header;
  Message *msg = 0;
  int msglen = 0;
  MdsSetClientAddr(0);
  *status = 0;
  *status = GetBytes(id, (void *)&header, sizeof(MsgHdr));
  if (*status &1){
    if ( Endian(header.client_type) != Endian(ClientType()) ) FlipHeader(&header);
#ifdef DEBUG
    printf("msglen = %d\nstatus = %d\nlength = %d\nnargs = %d\ndescriptor_idx = %d\nmessage_id = %d\ndtype = %d\n",
               header.msglen,header.status,header.length,header.nargs,header.descriptor_idx,header.message_id,header.dtype);
    printf("client_type = %d\nndims = %d\n",header.client_type,header.ndims);
#endif
    if (CType(header.client_type) > CRAY_CLIENT || header.ndims > MAX_DIMS)
    {
      DisconnectConnection(id);
      fprintf(stderr,"\rGetMdsMsg shutdown connection %d: bad msg header, header.ndims=%d, client_type=%d\n",id,header.ndims,CType(header.client_type));
      *status = 0;
      return 0;
    }  
    msglen = header.msglen;
    msg = malloc(header.msglen);
    msg->h = header;
    *status = GetBytes(id, msg->bytes, msglen - sizeof(MsgHdr));
    if (*status & 1 && IsCompressed(header.client_type))
    {
      Message *m;
      unsigned long dlen;
      memcpy(&msglen, msg->bytes, 4);
      if (Endian(header.client_type) != Endian(ClientType()))
        FlipBytes(4,(char *)&msglen);
      m = malloc(msglen);
      m->h = header;
      dlen = msglen - sizeof(MsgHdr);
      *status = uncompress(m->bytes, &dlen, msg->bytes + 4, header.msglen - sizeof(MsgHdr) - 4) == 0;
      if (*status & 1)
      {
	m->h.msglen = msglen;
        free(msg);
	msg = m;
      }
      else
	free(m);
    }
    if (*status & 1 && (Endian(header.client_type) != Endian(ClientType())))
      FlipData(msg);
  }
  return msg;
}
