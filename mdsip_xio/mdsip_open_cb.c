#include "mdsip.h"
#include "globus_common.h"
#include "globus_xio_gsi.h"
#include "globus_gss_assist.h"

void mdsip_open_cb( globus_xio_handle_t xio_handle,
		    globus_result_t result,
		    void *user_arg)
{
  globus_result_t res;
  mdsip_client_t *ctx;
  mdsip_options_t *options = (mdsip_options_t *)user_arg;
  gss_cred_id_t                           credential;
  OM_uint32				  lifetime;
  char *error_str;
  ctx = mdsip_new_client(options);
  mdsip_test_result(xio_handle,result,ctx,"mdsip_open_cb");
  if (options->port_name == NULL)
  {
    if (options->security_level > 0 && options->delegation)
    {
      gss_buffer_desc buffer_desc = GSS_C_EMPTY_BUFFER;
      OM_uint32 minor_status;
      OM_uint32 major_status;
      /*Accept delegation for 500 seconds.  Delegated credential will be stored
       * in "credential"*/

      res = globus_xio_handle_cntl(xio_handle,
				   options->gsi_driver, 
				   GLOBUS_XIO_GSI_ACCEPT_DELEGATION,
				   &credential,
				   NULL,
				   NULL,
				   500,
				   &lifetime);
      mdsip_test_result(xio_handle,res,ctx,"mdsip_open_cb,ACCEPT_DELEGATION");
      major_status = gss_export_cred(&minor_status,credential,GSS_C_NO_OID,0,&buffer_desc);
      if(major_status != GSS_S_COMPLETE)
      {
        char *error_str;
        globus_gss_assist_display_status_str(&error_str,
                                             NULL,
                                             major_status,
                                             minor_status,
                                             0);
        printf("\nLINE %d ERROR: %s\n\n", __LINE__, error_str);
        globus_xio_register_close(xio_handle,NULL,mdsip_close_cb,ctx);
        return;
      }
      else
      {
        major_status = gss_release_buffer(&minor_status,&buffer_desc);
	if(major_status != GSS_S_COMPLETE)
	{
	  globus_gss_assist_display_status_str(&error_str,
                                             NULL,
                                             major_status,
                                             minor_status,
					       0);
	  printf("\nLINE %d ERROR: %s\n\n", __LINE__, error_str);
	  printf("\nLINE %d ERROR: %s\n\n", __LINE__, error_str);
	  globus_xio_register_close(xio_handle,NULL,mdsip_close_cb,ctx);
	  return;
	}
      }
    }
  }
  ctx->mdsip_read_cb=mdsip_authenticate_cb;
  res = globus_xio_register_read(xio_handle,(globus_byte_t *)&ctx->header, sizeof(mdsip_message_header_t), 
				 sizeof(mdsip_message_header_t),NULL,mdsip_read_header_cb,ctx);
  mdsip_test_result(xio_handle,res,ctx,"mdsip_open_cb,globus_xio_register_read");
}

