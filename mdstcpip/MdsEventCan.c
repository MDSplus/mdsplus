
#include "mdsip_connections.h"
#include <stdlib.h>

int  MdsEventCan(int id, int eventid) {
  struct descrip eventarg;
  struct descrip ansarg = {0,0,0,0};
  int status = MdsValue(id, EVENTCANREQUEST, MakeDescrip((struct descrip *)&eventarg,DTYPE_LONG,0,0,&eventid), 
			      (struct descrip *)&ansarg, (struct descrip *)NULL);
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

