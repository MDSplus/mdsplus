#include <stdlib.h>
#include <libroutines.h>

#include "mdsip_connections.h"

////////////////////////////////////////////////////////////////////////////////
//  DoMessage  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


int DoMessage(int id)
{
  int status = 0;
  static Message *(*processMessage) (Connection *, Message *) = 0;
  Connection *c = FindConnection(id, 0);
  if (processMessage == 0) {
    DESCRIPTOR(MdsIpSrvShr, "MdsIpSrvShr");
    DESCRIPTOR(procmsg, "ProcessMessage");
    status = LibFindImageSymbol(&MdsIpSrvShr, &procmsg, &processMessage);
  }
  if (c && processMessage) {
    Message *msgptr = GetMdsMsg(id, &status);
    Message *ans = 0;
    if (status & 1) {
      ans = (*processMessage) (c, msgptr);
      if (ans) {
          // NOTE: [Andrea] this status is not actually tested for errors //
	status = SendMdsMsg(id, ans, 0);
	free(ans);
      }
    } else
      CloseConnection(id);
    if (msgptr)
      free(msgptr);
  }
  return status;
}

