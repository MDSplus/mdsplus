#include "mdsip.h"
#include "globus_xio_tcp_driver.h"
#include "globus_xio_gsi.h"

static int MDSIP_SNDBUF = 32768;
static int MDSIP_RCVBUF = 32768;

void *mdsip_connect(char *host)
{
  static int activated=0;
  int status;
  static globus_xio_stack_t stack_tcp;
  static globus_xio_stack_t stack_gsi;
  static globus_xio_driver_t tcp_driver;
  static globus_xio_driver_t gsi_driver;
  globus_result_t result;
  globus_xio_handle_t xio_handle;
  globus_xio_attr_t attr;
  char *contact_string;
  int is_gsi;
  mdsip_message_t *m;

  if (activated == 0)
  {
    result = globus_module_activate(GLOBUS_XIO_MODULE);
    mdsip_test_status(0,result,"mdsip_connect globus_module_activate");
    if (result != GLOBUS_SUCCESS) return 0;
    result = globus_xio_driver_load("tcp",&tcp_driver);
    mdsip_test_status(0,result,"mdsip_connect load tcp driver");
    if (result != GLOBUS_SUCCESS) return 0;
    result = globus_xio_driver_load("gsi",&gsi_driver);
    mdsip_test_status(0,result,"mdsip_connect load gsi driver");
    if (result != GLOBUS_SUCCESS) return 0;
    result = globus_xio_stack_init(&stack_tcp, NULL);
    mdsip_test_status(0,result,"mdsip_connect globus_xio_stack_init");
    if (result != GLOBUS_SUCCESS) return 0;
    result = globus_xio_stack_init(&stack_gsi, NULL);
    mdsip_test_status(0,result,"mdsip_connect globus_xio_stack_inite");
    if (result != GLOBUS_SUCCESS) return 0;
    result = globus_xio_stack_push_driver(stack_tcp, tcp_driver);
    mdsip_test_status(0,result,"mdsip_connect globus_xio_stack_push_driver");
    if (result != GLOBUS_SUCCESS) return 0;
    result = globus_xio_stack_push_driver(stack_gsi, tcp_driver);
    mdsip_test_status(0,result,"mdsip_connect globus_xio_stack_push_driver");
    if (result != GLOBUS_SUCCESS) return 0;
    result = globus_xio_stack_push_driver(stack_gsi, gsi_driver);
    mdsip_test_status(0,result,"mdsip_connect globus_xio_stack_push_driver");
    if (result != GLOBUS_SUCCESS) return 0;
    activated = 1;
  }

  if (host == NULL || strlen(host) == 0)
    return 0;
  is_gsi = host[0] == '_';
  result = globus_xio_handle_create(&xio_handle, is_gsi ? stack_gsi : stack_tcp);
  mdsip_test_status(0,result,"mdsip_connect globus_xio_handle_create");
  if (result != GLOBUS_SUCCESS) return 0;

  contact_string = strcpy((char *)malloc(strlen(host)+10),&host[is_gsi ? 1 : 0]);
  
  if (strstr(contact_string,":") == NULL)
    strcat(contact_string,is_gsi ? ":8200" : ":8000");
  result = globus_xio_attr_init(&attr);
  mdsip_test_status(0,result,"mdsip_connect globus_xio_attr_init");
  if (result != GLOBUS_SUCCESS) return 0;
  if (is_gsi)
  {
    result = globus_xio_attr_cntl(attr,gsi_driver,GLOBUS_XIO_GSI_SET_DELEGATION_MODE,
				  GLOBUS_XIO_GSI_DELEGATION_MODE_FULL);
    mdsip_test_status(0,result,"mdsip_connect globus_xio_attr_cntl");
    if (result != GLOBUS_SUCCESS) return 0;
  }
  result = globus_xio_attr_cntl(attr,tcp_driver,GLOBUS_XIO_TCP_SET_SNDBUF,MDSIP_SNDBUF);
  mdsip_test_status(0,result,"mdsip_connect SET_SNDBUF");
  if (result != GLOBUS_SUCCESS) return 0;
  result = globus_xio_attr_cntl(attr,tcp_driver,GLOBUS_XIO_TCP_SET_RCVBUF,MDSIP_RCVBUF);
  mdsip_test_status(0,result,"mdsip_connect SET_RCVBUF");
  if (result != GLOBUS_SUCCESS) return 0;
  result = globus_xio_attr_cntl(attr,tcp_driver,GLOBUS_XIO_TCP_SET_NODELAY,GLOBUS_TRUE);
  mdsip_test_status(0,result,"mdsip_connect SET_NODELAY");
  if (result != GLOBUS_SUCCESS) return 0;
  result = globus_xio_attr_cntl(attr,tcp_driver,GLOBUS_XIO_TCP_SET_KEEPALIVE,GLOBUS_TRUE);
  mdsip_test_status(0,result,"mdsip_connect SET_KEEPALIVE");
  if (result != GLOBUS_SUCCESS) return 0;
  result = globus_xio_open(xio_handle, contact_string, attr);
  mdsip_test_status(0,result,"mdsip_connect globus_xio_open");
  if (result != GLOBUS_SUCCESS) 
    xio_handle = 0;
  else
  {
#ifdef _WIN32
    static char user[128];
    int bsize=128;
    char *user_p = GetUserName(user,&bsize) ? user : "Windows User";
#elif __MWERKS__
    static char user[128];
    int bsize=128;
    char *user_p = "Macintosh User";
#elif __APPLE__
    char *user_p;
    struct passwd *pwd;
    pwd = getpwuid(geteuid());
    user_p = pwd->pw_name;
#else
    char *user_p;
#ifdef HAVE_VXWORKS_H
    user_p = "vxWorks";
#else
    user_p = (getpwuid(geteuid()))->pw_name;
#endif
#endif
    m = malloc(sizeof(mdsip_message_header_t)+strlen(user_p)+1);
    memset(m,0,sizeof(mdsip_message_header_t));
    m->h.length=strlen(user_p);
    m->h.msglen=sizeof(mdsip_message_header_t)+m->h.length;
    m->h.dtype=DTYPE_CSTRING;
    m->h.status=0;
    m->h.ndims=0;
    strcpy(m->bytes,user_p);
    mdsip_send_message(xio_handle,m,0);
    free(m);
    m = NULL;
    m = mdsip_get_message(xio_handle,&status);
    if (!(status & 1))
    {
      fprintf(stderr,"ERROR: mdsip_connect\n");
      return NULL;
    }
    else
    {
      if (!(m->h.status & 1))
      {
        fprintf(stderr,"ERROR: mdsip_connect - Access Denied\n");
        globus_xio_close(xio_handle,NULL);
        return NULL;
      }
    }
    if (m != NULL)
      free(m);
  }
  return (void *)xio_handle;
}

void mdsip_set_bufsize(int sndbuf, int rcvbuf)
{
  MDSIP_SNDBUF=sndbuf;
  MDSIP_RCVBUF=rcvbuf;
}
