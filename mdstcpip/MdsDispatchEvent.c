
#include "mdsip_connections.h"
#include <stdlib.h>
#include <status.h>

////////////////////////////////////////////////////////////////////////////////
//  MdsDispachEvent  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void MdsDispatchEvent(int id)
{
  INIT_STATUS;
  Message *m;
  if ((m = GetMdsMsg(id, &status)) != 0) {
    if (STATUS_OK && m->h.msglen == (sizeof(MsgHdr) + sizeof(MdsEventInfo))) {
      MdsEventInfo *event = (MdsEventInfo *) m->bytes;
      (*event->astadr) (event->astprm, 12, event->data);
    }
    free(m);
  }
}
