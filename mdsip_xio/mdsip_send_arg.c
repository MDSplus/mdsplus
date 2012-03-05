#include "mdsip.h"

int MDSIP_MESSAGE_ID = 1;

int  mdsip_send_arg(void *io_handle, unsigned char idx, char dtype, unsigned char nargs, short length, char ndims,
int *dims, char *bytes)
{
  int status;
  int msglen;
  int i;
  int nbytes = length;
  mdsip_message_t *m;
  if (idx > nargs)
  {
    /**** Special I/O message ****/ 
    nbytes = dims[0];
  }
  else
  {
    for (i=0;i<ndims;i++)
      nbytes *= dims[i];
  }
  msglen = sizeof(mdsip_message_header_t) + nbytes;
  m = malloc(msglen);
  m->h.client_type = 0;
  m->h.msglen = msglen;
  m->h.message_id = MDSIP_MESSAGE_ID;
  m->h.descriptor_idx = idx;
  m->h.dtype = dtype;
  m->h.nargs = nargs;
  m->h.length = length;
  m->h.ndims = ndims;
#ifdef __CRAY
  for (i=0;i<4;i++)
    m->h.dims[i] = ((ndims > i * 2) ? (dims[i * 2] << 32) : 0) | ((ndims > (i * 2 + 1)) ? (dims[i * 2 + 1]) : 0); 
#else
  for (i=0;i<MAX_DIMS;i++) m->h.dims[i] = i < ndims ? dims[i] : 0;
#endif
  memcpy(m->bytes,bytes,nbytes);
  status = mdsip_send_message(io_handle, m, 0);
  if (idx > nargs || idx == (nargs -1)) MDSIP_MESSAGE_ID++;
  if (MDSIP_MESSAGE_ID == 0) MDSIP_MESSAGE_ID = 1;
  free(m);
  return status;
}

