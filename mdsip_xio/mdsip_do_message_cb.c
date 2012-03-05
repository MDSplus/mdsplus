#include "mdsip.h"

void mdsip_do_message_cb(
		globus_xio_handle_t xio_handle, 
		globus_result_t result,
                globus_byte_t *buffer,
		globus_size_t len,
		globus_size_t nbytes,
		globus_xio_data_descriptor_t data_desc,
		void *user_arg)
{
  mdsip_client_t *ctx = (mdsip_client_t *)user_arg;
  mdsip_test_result(xio_handle,result,ctx,"mdsip_do_message_cb");
  if (result == GLOBUS_SUCCESS)
  {
    mdsip_process_message((void *)xio_handle,ctx,ctx->message);
    free(ctx->message);
    globus_result_t res = globus_xio_register_read(xio_handle,(globus_byte_t *)&ctx->header, 
						     sizeof(mdsip_message_header_t), sizeof(mdsip_message_header_t), NULL,mdsip_read_header_cb,ctx);
    mdsip_test_result(xio_handle,res,ctx,"mdsip_do_message_cb,globus_xio_register_read");
  }
  return;
}
