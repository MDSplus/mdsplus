#include "mdsip.h"

void mdsip_accept_cb(
		     globus_xio_server_t server,
		     globus_xio_handle_t xio_handle,
		     globus_result_t     result,
		     void *options)
{
  globus_result_t res;
  res = globus_xio_register_open(xio_handle, NULL, NULL, mdsip_open_cb, options);
  mdsip_test_result(0,res,0,"mdsip_accept_cb, error in globus_xio_register_open");
  res = globus_xio_server_register_accept(server,mdsip_accept_cb,options);
  mdsip_test_result(0,res,0,"mdsip_accept_cb, error in globus_xio_server_register_accept");
  if (res != GLOBUS_SUCCESS)
    exit(0);
}
