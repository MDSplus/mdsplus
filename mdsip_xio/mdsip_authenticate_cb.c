#include "mdsip.h"
#include "globus_xio_tcp_driver.h"
#include "globus_xio_gsi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define __toupper(c) (((c) >= 'a' && (c) <= 'z') ? (c) & 0xDF : (c))

static int match_wild(char *candidate, char *pattern)
{
  char *cand1 = candidate;
  short cand1_len = strlen(candidate);
  char *cand2 = cand1;
  short cand2_len = 0;
  char *pat1 = pattern;
  short pat1_len = strlen(pattern);
  char *pat2 = pat1;
  char pc;
  int true;
  true = 1;

  while (true) {
    if (--pat1_len < 0)
    {
      if (cand1_len == 0)
        return 1;
      else 
      {
        if (--cand2_len < 0)
          return 0;
        else {
          cand2++;
          cand1 = cand2;
          cand1_len = cand2_len;
          pat1 = pat2;
        }
      }
    }
    else 
    {
      if ((pc = *pat1++) == '*')
      {
        if (pat1_len == 0) return 1;
        cand2 = cand1;
        cand2_len = cand1_len;
        pat2 = pat1;
      }
      else
      {
        if (--cand1_len < 0) return 0;
        if (__toupper(*cand1) != __toupper(pc))
        {
          cand1++;
          if (pc != '%')
          {
            if (--cand2_len < 0)
              return 0;
            else {
              cand2++;
              cand1 = cand2;
              cand1_len = cand2_len;
              pat1 = pat2;
            }
          }
        }
        else
          cand1++;
      } 
    }   
  }
  return 0;
}

int mdsip_find_user_mapping(char *hostfile, char *ipaddr, char *host, char *name, char **local_account)
{
  FILE *f = fopen(hostfile,"r");
  int ok = 0;
  if (f)
  {
    char line[1024];
    char *line_name;
    char *line_local_account;
    char *match_host = 0;
    char *match_ipaddr = 0;
    int i;
    match_ipaddr = strcpy(malloc((ipaddr ? strlen(ipaddr) : 0)+strlen(name)+2),name);
    if (ipaddr)
    {
      strcat(match_ipaddr,"@");
      strcat(match_ipaddr,ipaddr);
    }
    if (host)
    {
      match_host = strcpy(malloc(strlen(host)+strlen(name)+2),name);
      strcat(match_host,"@");
      strcat(match_host,host);
    }

    while (ok==0 && fgets(line,1023,f))
    {
      if (line[0] != '#')
      {
        size_t len=strcspn(line,"|");
        size_t wildlen=strcspn(line,"*%");
        if ((len < strlen(line)) && (ipaddr || (wildlen == strlen(line))))
	{
          line_name=line;
          line_name[len]=0;
          line_local_account=line+len+1;
          while(line_name[0] == 32 || line_name[0] == 8) line_name++;
          for (i=strlen(line_name);(i>0) && ((line_name[i-1] == 32) || (line_name[i-1] == 8)); line_name[i-1]=0,i--);
          if (i>0)
          {
            int match_type=1;
            int line_name_free=0;
            if (strcspn(line_name,"@")==strlen(line_name))
	    {
              line_name = strcpy(malloc(strlen(line_name)+3),line_name);
              strcat(line_name,"@*");
              line_name_free=1;
            }
            if (line_name[0] == '!')
	    {
              match_type = 2;
              line_name++;
            }
            if (match_wild(match_ipaddr,line_name) || (match_host && match_wild(match_host,line_name)))
            {
	      while(line_local_account[0] == 32 || line_local_account[0] == 8) line_local_account++;
	      for (i=strlen(line_local_account);i>0 && (line_local_account[i-1] == 32 || line_local_account[i-1] == 8); line_local_account[i-1]=0,i--);
              line_local_account[strlen(line_local_account)-1]=0;
	      *local_account = strcpy(malloc(strlen(line_local_account)+1),line_local_account);
              ok = match_type;
	    }
            if (line_name_free)
              free(line_name);
          }
        }
      }
    }
    free(match_ipaddr);
    free(match_host);
    fclose(f);
  }
  else
  {
    printf("Unable to open hostfile: %s\n",hostfile);
    ok=2;
  }
  return ok == 1;
}

void mdsip_authenticate_cb(
		globus_xio_handle_t xio_handle, 
		globus_result_t result,
                globus_byte_t *buffer,
		globus_size_t len,
		globus_size_t nbytes,
		globus_xio_data_descriptor_t data_desc,
		void *user_arg)
{
  mdsip_client_t *ctx = (mdsip_client_t *)user_arg;
  char *remote_user = strncpy(malloc(nbytes+1),buffer,nbytes);
  remote_user[nbytes]=0;

  mdsip_test_result(xio_handle,result,ctx,"mdsip_authenticate_cb");
  if (result == GLOBUS_SUCCESS)
  {
    int status;
    globus_result_t res;
    short dummy;
    mdsip_message_t *reply = (mdsip_message_t *)memset(malloc(sizeof(mdsip_message_t)),0,sizeof(mdsip_message_t));
    char *contact;
    res = globus_xio_handle_cntl(xio_handle, 
				 (globus_xio_driver_t)ctx->options->tcp_driver, 
				 GLOBUS_XIO_TCP_GET_REMOTE_CONTACT, 
				 &contact);
    mdsip_test_result(xio_handle,res,ctx,"mdsip_authenticate_cb,GET_REMOTE_CONTACT");
    ctx->host = strcpy(malloc(strlen(contact)+1),contact);
    ctx->host[strcspn(ctx->host,":")]=0;
    res = globus_xio_handle_cntl(xio_handle, 
				 (globus_xio_driver_t)ctx->options->tcp_driver, 
				 GLOBUS_XIO_TCP_GET_REMOTE_NUMERIC_CONTACT, 
				 &contact);
    mdsip_test_result(xio_handle,res,ctx,"mdsip_authenticate_cb,GETREMOTE_NUMERIC_CONTACT");
    ctx->ipaddr = strcpy(malloc(strlen(contact)+1),contact);
    ctx->ipaddr[strcspn(ctx->ipaddr,":")]=0;
    mdsip_host_to_ipaddr(ctx->host,&ctx->addr,&dummy);
    if (ctx->options->security_level > 0)
    {
      gss_buffer_desc                         buffer_desc = GSS_C_EMPTY_BUFFER;
      gss_buffer_t                            buffer = &buffer_desc;
      OM_uint32                               status;
      globus_xio_driver_t driver = (globus_xio_driver_t)ctx->options->gsi_driver;
      globus_result_t res;
      gss_name_t name;
      res = globus_xio_handle_cntl((globus_xio_handle_t)xio_handle, 
				   driver, 
				   GLOBUS_XIO_GSI_GET_PEER_NAME, 
				   &name);
      gss_export_name(&status, name, buffer);
      ctx->remote_user=strcpy(malloc(strlen((char *)buffer->value)+1),(char *)buffer->value);
    }
    else
    {
      ctx->remote_user=remote_user;
    }
    status = mdsip_find_user_mapping(ctx->options->hostfile, ctx->ipaddr,ctx->host,ctx->remote_user,&ctx->local_user);
    if (status & 1 && ctx->options->port_name == 0)
      status = mdsip_become_user(ctx->local_user,ctx->remote_user);
    reply->h.msglen = sizeof(mdsip_message_header_t);
    reply->h.status = status;
    if (status & 1)
    {
      if (ctx->options->security_level > 0)
      {
        fprintf(stdout,"%s, CONNECT - %s/%s@%s(%s)\n",mdsip_current_time(),ctx->remote_user,remote_user,ctx->host,ctx->ipaddr);
        free(remote_user);
      }
      else
        fprintf(stdout,"%s, CONNECT - %s@%s(%s)\n",mdsip_current_time(),ctx->remote_user,ctx->host,ctx->ipaddr);
    }
    else
    {
      fprintf(stdout,"%s, REJECT - %s@%s(%s)\n",mdsip_current_time(),ctx->remote_user,ctx->host,ctx->ipaddr);
    }
    free(ctx->message);
    mdsip_write((void *)xio_handle,ctx,reply,0);
    if (status & 1)
    {
      ctx->mdsip_read_cb = mdsip_do_message_cb;
      res = globus_xio_register_read(xio_handle,(globus_byte_t *)&ctx->header, sizeof(mdsip_message_header_t), sizeof(mdsip_message_header_t),
			       NULL,mdsip_read_header_cb,ctx);
      mdsip_test_result(xio_handle,res,ctx,"mdsip_authenticate_cb,globus_xio_register_read");
    }
    else
      globus_xio_register_close(xio_handle,NULL,mdsip_close_cb,ctx);
  }
  return;
}
