#include "mdsip.h"
#include "globus_xio_tcp_driver.h"
#include "globus_xio_gsi.h"

void  mdsip_initialize_io(mdsip_options_t *options)
{
  globus_xio_driver_t                     gsi_driver;
  globus_xio_driver_t                     tcp_driver = NULL;
  globus_xio_stack_t                      stack;
  globus_xio_handle_t                     xio_handle;
  globus_xio_server_t                     server_handle;
  globus_result_t                         res;
  globus_xio_attr_t		    	  server_attr;
  


  char *local_account;

  if (options->port_name && mdsip_find_user_mapping(options->hostfile,0,0,"multi@localhost",&local_account))
  {
    mdsip_client_t *c = mdsip_new_client(0);
    c->local_user = local_account;    
    mdsip_become_user(c);
    free(c);
  }

  globus_module_activate(GLOBUS_XIO_MODULE);

  globus_xio_stack_init(&stack, NULL);
    
  globus_xio_driver_load("tcp", &tcp_driver);
  options->tcp_driver=tcp_driver;
  globus_xio_stack_push_driver(stack, tcp_driver);


  if (options->security_level > 0)
  {
    globus_xio_driver_load("gsi", &gsi_driver);
    options->gsi_driver=gsi_driver;
    globus_xio_stack_push_driver(stack, gsi_driver);
  }

  globus_xio_attr_init(&server_attr);

  if (options->port_name == 0)
  {
    /*start listening on connection already on stdin (from inetd)*/

    globus_xio_attr_cntl(server_attr,
			 tcp_driver,
			 GLOBUS_XIO_TCP_SET_HANDLE,
			 STDIN_FILENO);
  }
  else
  {
    MdsSetServerPortname(options->port_name);
    res = globus_xio_attr_cntl(server_attr,
			       tcp_driver,
			       GLOBUS_XIO_TCP_SET_PORT,
			 options->port);
    res = globus_xio_attr_cntl(server_attr,
			       tcp_driver,
			       GLOBUS_XIO_TCP_SET_RCVBUF,
			       options->rcvbuf);

    res = globus_xio_attr_cntl(server_attr,
			       tcp_driver,
			       GLOBUS_XIO_TCP_SET_SNDBUF,
			       options->sndbuf);
    res = globus_xio_attr_cntl(server_attr,
			       tcp_driver,
			       GLOBUS_XIO_TCP_SET_NODELAY,
			       GLOBUS_TRUE);
    res = globus_xio_attr_cntl(server_attr,
			       tcp_driver,
			       GLOBUS_XIO_TCP_SET_KEEPALIVE,
			       GLOBUS_TRUE);

  }

  if (options->security_level > 0)
  {
    int level;
    switch (options->security_level)
    {
      case 1: level=GLOBUS_XIO_GSI_PROTECTION_LEVEL_NONE; break;
      case 2: level=GLOBUS_XIO_GSI_PROTECTION_LEVEL_INTEGRITY; break;
      case 3: level=GLOBUS_XIO_GSI_PROTECTION_LEVEL_PRIVACY; break;
    }
    globus_xio_attr_cntl(server_attr, 
		    	 gsi_driver, 
			 GLOBUS_XIO_GSI_SET_PROTECTION_LEVEL, 
			 level);

    globus_xio_attr_cntl(server_attr, 
		         gsi_driver, 
			 GLOBUS_XIO_GSI_SET_DELEGATION_MODE, 
			 GLOBUS_XIO_GSI_DELEGATION_MODE_FULL);
  }

  if (options->port_name == 0)
  {
    if (options->security_level > 0)
      globus_xio_attr_cntl(server_attr, gsi_driver, GLOBUS_XIO_GSI_FORCE_SERVER_MODE, GLOBUS_TRUE);

    globus_xio_handle_create(&xio_handle, stack);
    res = globus_xio_register_open(xio_handle, NULL, server_attr, mdsip_open_cb, options);
    mdsip_test_result(0,res,0,"mdsip_initialize_io,globus_xio_register_open");
  }
  else
  {
    res = globus_xio_server_create(&server_handle, server_attr, stack);
    mdsip_test_result(0,res,0,"mdsip_initialize_io,globus_xio_server_create");
    if (res == GLOBUS_SUCCESS)
    {
      res = globus_xio_server_register_accept(server_handle,mdsip_accept_cb,options);
      mdsip_test_result(0,res,0,"mdsip_initialize_io,globus_xio_server_register_accept");
    }
  }
  if (res != GLOBUS_SUCCESS)
    exit(0);
  return;
}
