#include <stdlib.h>
#include <libroutines.h>

#include "mdsip_connections.h"

///
/// Process one pending message for the connection id at server side calling 
/// ProcessMessage function symbol in MdsIpSrvShr library. This at first waits 
/// for incoming message with GetMdsMsg() then calls ProcessMessage() that execute
/// the TDI expression, and finally returns the answer back to the client with 
/// a SendMdsMsg() call. If any error occurrs within transaction the connection
/// is closed.
/// 
/// \param id of the connection to use
/// \return the status of the Process message.
///
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

