#include "mdsip.h"

void mdsip_read_header_cb(
		globus_xio_handle_t xio_handle, 
		globus_result_t result,
                globus_byte_t *buffer,
		globus_size_t len,
		globus_size_t nbytes,
		globus_xio_data_descriptor_t data_desc,
		void *user_arg)
{
  globus_result_t res;
  mdsip_client_t *ctx = (mdsip_client_t *)user_arg;
  if (result == GLOBUS_SUCCESS)
  {
    if ( Endian(ctx->header.client_type) != Endian(ctx->options->server_type) ) mdsip_flip_header(&ctx->header);
#ifdef DEBUG
    printf("msglen = %d\nstatus = %d\nlength = %d\nnargs = %d\ndescriptor_idx = %d\nmessage_id = %d\ndtype = %d\n",
               ctx->header.msglen,ctx->header.status,ctx->header.length,ctx->header.nargs,ctx->header.descriptor_idx,ctx->header.message_id,ctx->header.dtype);
    printf("client_type = %d\nndims = %d\n",ctx->header.client_type,ctx->header.ndims);
#endif
    if (CType(ctx->header.client_type) > CRAY_CLIENT || ctx->header.ndims > MAX_DIMS)
    {
      fprintf(stderr,"\rGetMdsMsg shutdown: bad msg header, header.ndims=%d, client_type=%d\n",ctx->header.ndims,CType(ctx->header.client_type));
      globus_xio_register_close(xio_handle,NULL,mdsip_close_cb,ctx);
    }  
    ctx->message = malloc(ctx->header.msglen);
    ctx->message->h = ctx->header;
    res = globus_xio_register_read(xio_handle, (globus_byte_t *)ctx->message->bytes,
				   ctx->header.msglen-sizeof(mdsip_message_header_t),
				   ctx->header.msglen-sizeof(mdsip_message_header_t),NULL,ctx->mdsip_read_cb,ctx);
    mdsip_test_result(xio_handle,res,ctx,"mdsip_read_header_cb,globus_xio_register_read");
  }
  else
  {
    if (globus_error_get_type(globus_error_peek(result)) != GLOBUS_XIO_ERROR_EOF)
      fprintf(stderr, "ERROR: %s\n", globus_object_printable_to_string(globus_error_get(result)));
    globus_xio_register_close(xio_handle,NULL,mdsip_close_cb, user_arg);
  }
  return;
}

