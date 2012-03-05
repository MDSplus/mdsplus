#include "mdsip.h"

int mdsip_send_message(void *io_handle, mdsip_message_t *m, int oob)
{
  globus_size_t nbytes;
  globus_result_t result;
  if ((m->h.client_type & SwapEndianOnServer) != 0)
  {
    if (Endian(m->h.client_type) != Endian(mdsip_client_type()))
    {
      mdsip_flip_data(m);
      mdsip_flip_header(&m->h);
    }
  }
  else
    m->h.client_type = mdsip_client_type();
  result = globus_xio_write((globus_xio_handle_t)io_handle, (globus_byte_t *)m, m->h.msglen, m->h.msglen, &nbytes, NULL);
  mdsip_test_status(0,result,"mdsip_send_message globus_xio_write");
  return result == GLOBUS_SUCCESS;
}

