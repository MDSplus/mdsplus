#include "mdsip.h"

void mdsip_test_status(globus_xio_handle_t xio_handle,globus_result_t res,char *msg)
{
  if(res != GLOBUS_SUCCESS)
  {
    fprintf(stderr, "ERROR:%s ---\n       %s\n", 
	    msg,globus_object_printable_to_string(globus_error_get(res)));
    if (xio_handle != NULL)
      globus_xio_close(xio_handle,NULL);
  }

}

