#include "mdsip.h"

void mdsip_dispatch_event(void *io_handle)
{
  int status;
  mdsip_message_t  *m;
  if ((m = mdsip_get_message(io_handle,&status)) != 0)
  {
    if (status == 1 && m->h.msglen == (sizeof(mdsip_message_header_t) + sizeof(mdsip_event_info_t)))
    {
      mdsip_event_info_t *event = (mdsip_event_info_t *)m->bytes;
      (*event->astadr)(event->astprm, event->eventid, event->data);
    }
    free(m);
  }
}

