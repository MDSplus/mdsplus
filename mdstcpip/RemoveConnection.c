#include "mdsip_connections.h"
#include <stdlib.h>
#include <treeshr.h>

int RemoveConnection(int conid) {
  int status = 0;
  Connection *c=FindConnection(conid,0);
  if (c) {
    void *tdi_context[6];
    MdsEventList *e,*nexte;
    FreeDescriptors(c);
    for (e=c->event; e; e=nexte) {
      nexte = e->next;
    /**/
#ifndef HAVE_VXWORKS_H
      MDSEventCan(e->eventid);
#endif
    /**/
      if (e->info_len > 0) free(e->info);
      free(e);
    }
    if (c->context.tree) {
      void  *old_context;
      old_context = TreeSwitchDbid(c->context.tree);
      TreeClose(0,0);
      c->context.tree = TreeSwitchDbid(old_context);
    }
    TdiSaveContext(tdi_context);
    TdiDeleteContext(c->tdicontext);
    TdiRestoreContext(tdi_context);
    status = DisconnectConnection(conid);
  }
  return status;
}
