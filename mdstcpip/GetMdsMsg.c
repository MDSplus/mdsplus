#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <status.h>

#include "zlib/zlib.h"
#include "mdsip_connections.h"

static int GetBytes(int id, void *buffer, size_t bytes_to_recv){
  char *bptr = (char *)buffer;
  IoRoutines *io = GetConnectionIo(id);
  if (io) {
    int tries = 0;
    while (bytes_to_recv > 0 && (tries < 10)) {
      ssize_t bytes_recv;
      bytes_recv = io->recv(id, bptr, bytes_to_recv);
      if (bytes_recv <= 0) {
	if (errno != EINTR)
	  return MDSplusERROR;
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


////////////////////////////////////////////////////////////////////////////////
//  GetMdsMsg  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Message *GetMdsMsgTO(int id, int *status, float fsec __attribute__((unused))){
  MsgHdr header;
  Message *msg = 0;
  int msglen = 0;
  //MdsSetClientAddr(0);
  *status = GetBytes(id, (void *)&header, sizeof(MsgHdr));
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
    //int sec  = (int)(fsec);
    //int usec = (int)(fsec*1E6f) % 1000000;
    //GetConnectionIo(id)->settimeout(id,sec,usec);
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
  return GetMdsMsgTO(id, status, -1.f);
}

////////////////////////////////////////////////////////////////////////////////
//  GetMdsMsgOOB  //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Message *GetMdsMsgOOB(int id, int *status){
  return GetMdsMsg(id, status);
}
