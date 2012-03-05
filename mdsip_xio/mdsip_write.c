#include "mdsip.h"
#include "globus_xio_tcp_driver.h"

static void mdsip_write_cb( globus_xio_handle_t xio_handle,
			    globus_result_t result,
			    globus_byte_t *buffer,
			    globus_size_t len,
			    globus_size_t nbytes,
			    globus_xio_data_descriptor_t data_desc,
			    void *user_arg)
{
  globus_result_t res;
  mdsip_client_t *ctx = (mdsip_client_t *)user_arg;
  mdsip_test_result(xio_handle,result,ctx,"mdsip_write_cb");
  if (result != GLOBUS_SUCCESS)
  {
    free(ctx->send_buffer);
  }
  else
  {
    if (nbytes < len)
    {
      res = globus_xio_register_write(xio_handle,buffer+nbytes,len-nbytes,len-nbytes,
				NULL,mdsip_write_cb,user_arg);
      mdsip_test_result(xio_handle,result,ctx,"mdsip_write_cb,globux_xio_register_write");
    }
    else
      free(ctx->send_buffer);
  }
}

int mdsip_write(void *io_handle, mdsip_client_t *ctx, mdsip_message_t *m, int oob)
{
  globus_result_t res;
  globus_xio_data_descriptor_t desc;
  ctx->send_buffer=(char *)m;
  m->h.client_type = ctx->options->server_type;
  globus_xio_data_descriptor_init(&desc,io_handle);
  if (oob)
    globus_xio_data_descriptor_cntl(desc,ctx->options->tcp_driver,GLOBUS_XIO_TCP_SET_SEND_FLAGS,GLOBUS_XIO_TCP_SEND_OOB);
  res=globus_xio_register_write((globus_xio_handle_t)io_handle,(globus_byte_t *)m,m->h.msglen,
				m->h.msglen,desc,mdsip_write_cb,ctx);
  globus_xio_data_descriptor_destroy(desc);
  mdsip_test_result((globus_xio_handle_t)io_handle,res,ctx,"mdsip_write,globus_xio_register_write");
  return res=GLOBUS_SUCCESS;
}
  
