
#include "mdsip_connections.h"
#include <stdlib.h>

#ifndef HAVE_WINDOWS_H
void MdsDispatchEvent(int id)
#else
unsigned long WINAPI MdsDispatchEvent(int id)
#endif
{
  int status;
  Message  *m;
  if ((m = GetMdsMsg(id,&status)) != 0)  {
    if (status == 1 && m->h.msglen == (sizeof(MsgHdr) + sizeof(MdsEventInfo))) {
      MdsEventInfo *event = (MdsEventInfo *)m->bytes;
      (*event->astadr)(event->astprm, 12, event->data);
    }
    free(m);
  }
}
