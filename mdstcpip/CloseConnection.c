#include <stdlib.h>
#include <libroutines.h>
#include "mdsip_connections.h"

int CloseConnection(int id) {
  int status=0;
  static int (*removeConnection)(int) = 0;
  Connection *c=FindConnection(id,0);
  if (c) {
    if (removeConnection == 0) {
      DESCRIPTOR(MdsIpSrvShr,"MdsIpSrvShr");
      DESCRIPTOR(rmcon,"RemoveConnection");
      status = LibFindImageSymbol(&MdsIpSrvShr,&rmcon,&removeConnection);
    } else
      status=1;
    if (status & 1) {
      status = (*removeConnection)(id);
    }
  }
  return status;
}
