#include "globus_common.h"
#include "gssapi.h"
#include "globus_gss_assist.h"
#include "globus_gridmap_callout_error.h"
#include <stdlib.h>
#include "globus_xio.h"
#include "globus_xio_tcp_driver.h"
#include "globus_xio_gsi.h"
#include "globus_xio_http.h"
#define LINE_LEN 1024

static char *urlencode(char *in)
{
  static char *ans=0;
  int i;
  int j=0;
  int inlen=strlen(in);
  if (ans != 0) free(ans);
  ans=malloc(inlen*3);
  for(i=0,j=0;i<inlen;i++)
  {
    if ((in[i] >= '0' && in[i] <= '9') ||
	(in[i] >= 'A' && in[i] <= 'Z') ||
	(in[i] >= 'a' && in[i] <= 'z'))
    {
      ans[j++]=in[i];
    }
    else if (in[i] == ' ')
    {
      ans[j++]='+';
    }
    else
    {
      ans[j++]='%';
      sprintf(&ans[j],"%2X",(unsigned int)in[i]);
      j+=2;
    }
  }
  ans[j]=0;
  return ans;
}

int roam_check_access(char *host, int https, char *resource, char *permit, char *dn, char **aux)
{
  globus_xio_driver_t                     tcp_driver;
  globus_xio_driver_t                     gsi_driver;
  globus_xio_driver_t                     http_driver;
  globus_xio_stack_t                      stack;
  globus_xio_handle_t                     xio_handle;
  globus_xio_attr_t			  attr;
  globus_result_t                         res;
  char                                    line[LINE_LEN];
  globus_size_t                           nbytes;
  char                                    *url;
  char                                    *ans=0;
  int status=GLOBUS_FAILURE;

  url = malloc(strlen(host)+30+(strlen(resource)+strlen(permit)+strlen(dn))*3);
  strcpy(url,https ? "https://" : "http://");
  strcat(url,host);
  strcat(url,"/%2fcheck_access_aux.php?");
  strcat(url,"rname=");
  strcat(url,urlencode(resource));
  strcat(url,"&");
  strcat(url,"permission=");
  strcat(url,urlencode(permit));
  strcat(url,"&");
  strcat(url,"uname=");
  strcat(url,urlencode(dn));
  globus_module_activate(GLOBUS_XIO_MODULE);
  globus_xio_stack_init(&stack, NULL);
  globus_xio_driver_load("tcp", &tcp_driver);
  globus_xio_stack_push_driver(stack, tcp_driver);
  if (https)
  {
    globus_xio_driver_load("gsi", &gsi_driver);
    globus_xio_stack_push_driver(stack, gsi_driver);
  }
  globus_xio_driver_load("http", &http_driver);
  globus_xio_stack_push_driver(stack, http_driver);
  globus_xio_attr_init(&attr);
  if (https)
  {
    globus_xio_attr_cntl(attr, gsi_driver, GLOBUS_XIO_GSI_SET_SSL_COMPATIBLE, GLOBUS_TRUE);
  }    
  res = globus_xio_handle_create(&xio_handle, stack);
  //  test_res(res);
  res = globus_xio_open(xio_handle,url, attr);
  //test_res(res);
  while(res == GLOBUS_SUCCESS)
  {
    nbytes=0;
    res = globus_xio_read(xio_handle, line, LINE_LEN, 1, &nbytes, NULL);
    if (nbytes > 0)
    {
      if (ans)
      {
	int newlen=strlen(ans)+nbytes;
	ans=strncat(realloc(ans,newlen+1),line,nbytes);
	ans[newlen]=0;
      }
      else
      {
	ans=strncpy(malloc(nbytes+1),line,nbytes);
	ans[nbytes]=0;
      }
    }
    if (res)
      break;
  }
  if (ans && strlen(ans) > 4) 
  {
    if (strncmp(ans,"yes",3)==0)
    {
      status = GLOBUS_SUCCESS;
      *aux=malloc(strlen(ans)-4);
      if (strlen(ans) > 5)
	strncpy(*aux,&ans[4],strlen(ans)-4);
      (*aux)[strlen(ans)-5]=0;
    }	    
  }
  if (ans)
    free(ans);
  globus_xio_attr_destroy(attr);
  globus_xio_close(xio_handle, NULL);
  return status;
}
