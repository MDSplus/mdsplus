#include "mdsip.h"

mdsip_message_t *mdsip_get_message(void *io_handle, int *status)
{
  mdsip_message_header_t header;
  globus_result_t result;
  mdsip_message_t *msg = NULL;
  int msglen = 0;
  globus_size_t nbytes;
  *status = 0;
  result = globus_xio_read(io_handle, (globus_byte_t *)&header, sizeof(header),sizeof(header),&nbytes, NULL);
  mdsip_test_status(io_handle,result,"mdsip_get_message, globus_xio_read");
  if (result != GLOBUS_SUCCESS) return NULL;
  if ( Endian(header.client_type) != Endian(mdsip_client_type()) )
    mdsip_flip_header(&header);
  if ( CType(header.client_type) > CRAY_CLIENT || header.ndims > MAX_DIMS || header.msglen < sizeof(header))
  {
    globus_xio_close(io_handle,NULL);
    fprintf(stderr,"ERROR: mdsip_get_message, bad message header - closing connection\n");
    return NULL;
  }
  msglen = header.msglen;
  msg = malloc(header.msglen);
  msg->h = header;
  if (msglen > sizeof(header))
  {
    result = globus_xio_read(io_handle,msg->bytes,msglen - sizeof(header),msglen - sizeof(header),&nbytes,NULL);
    mdsip_test_status(io_handle,result,"mdsip_get_message, globus_xio_read");
  }
  if (result != GLOBUS_SUCCESS)
  {
    free(msg);
    return NULL;
  }
  if (Endian(header.client_type) != Endian(mdsip_client_type()))
    mdsip_flip_data(msg);
  *status = 1;
  return msg;
}
