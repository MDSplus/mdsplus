#include "mdsip.h"
#include <stdlib.h>
#include <string.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

mdsip_client_t *mdsip_new_client(mdsip_options_t *options)
{
  mdsip_client_t *ctx = (mdsip_client_t *)malloc(sizeof(mdsip_client_t));
  memset(ctx,0,sizeof(mdsip_client_t));
  ctx->options = options;
  return ctx;
}

void mdsip_free_client(mdsip_client_t *ctx)
{
  MdsEventList *e,*n;
  if (ctx->local_user) free(ctx->local_user);
  if (ctx->remote_user) free(ctx->remote_user);
  if (ctx->host) free(ctx->host);
  if (ctx->ipaddr) free(ctx->ipaddr);
  for (e=ctx->event;e;e=n)
  {
    n=e->next;
    MDSEventCan(e->eventid);
    free(e);
  }
  free(ctx);
}
