#include "mdsip.h"

int  mdsip_event_can(void *io_channel, int eventid)
{
  struct descrip eventarg;
  struct descrip ansarg = {0,0,{0},0,0};
  int status = mdsip_value(io_channel, EVENTCANREQUEST, mdsip_make_descrip((struct descrip *)&eventarg,DTYPE_LONG,0,0,0,&eventid), 
			      (struct descrip *)&ansarg, (struct descrip *)NULL);
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}
