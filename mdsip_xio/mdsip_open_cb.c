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
  ctx = mdsip_new_client(options);
  mdsip_test_result(xio_handle,result,ctx,"mdsip_open_cb");
  if (options->port_name == NULL)
  {
    if (options->security_level > 0)
    {
      gss_buffer_desc buffer_desc = GSS_C_EMPTY_BUFFER;
      OM_uint32 minor_status;
      OM_uint32 major_status;
      res = globus_xio_handle_cntl(xio_handle,options->gsi_driver,
				   GLOBUS_XIO_GSI_GET_DELEGATED_CRED,&credential);
      mdsip_test_result(xio_handle,res,ctx,"mdsip_open_cb,GET_DELEGATED_CRED");
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
      }
      else
      {
        char cred_file_name[]="/tmp/x509pp_pXXXXXX";
        int fd=mkstemp(cred_file_name);
        if (fd != -1)
	{
          fchmod(fd,00600);
          write(fd,buffer_desc.value,buffer_desc.length);
          fchmod(fd,00400);
          close(fd);
          setenv("X509_USER_PROXY",cred_file_name,1);
        }
        else
          perror("Error creating proxy credential file");
        major_status = gss_release_buffer(&minor_status,&buffer_desc);
      }
    }
  }
  ctx->mdsip_read_cb=mdsip_authenticate_cb;
  res = globus_xio_register_read(xio_handle,(globus_byte_t *)&ctx->header, sizeof(mdsip_message_header_t), 
				 sizeof(mdsip_message_header_t),NULL,mdsip_read_header_cb,ctx);
  mdsip_test_result(xio_handle,res,ctx,"mdsip_open_cb,globus_xio_register_read");
}

