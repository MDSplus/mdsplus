
#include "mdsip_connections.h"
#include <stdlib.h>

void MdsDispatchEvent(int id)
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
