#include "mdsip.h"

void mdsip_close_cb(globus_xio_handle_t xio_handle, globus_result_t result, void *user_arg)
{
  mdsip_client_t *ctx = (mdsip_client_t *)user_arg;
  int doexit=ctx->options->port_name == NULL;
  fprintf(stdout,"%s, DISCONN - %s@%s(%s)\n",mdsip_current_time(),ctx->remote_user,ctx->host,ctx->ipaddr);
  fflush(stdout);
  mdsip_free_client((mdsip_client_t *)ctx);
  if (doexit)
  {
    struct stat statbuf;
    char *proxy_file=getenv("X509_USER_PROXY");
    if (proxy_file && (stat(proxy_file,&statbuf)==0))
      remove(proxy_file);
    exit(0);
  }
}
