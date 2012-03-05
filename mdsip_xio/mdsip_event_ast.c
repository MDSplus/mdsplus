#include "mdsip.h"

int  mdsip_event_ast(void *io_channel, char *eventnam, void (*astadr)(), void *astprm, int *eventid)
{
  struct descrip eventnamarg;
  struct descrip infoarg;
  struct descrip ansarg;
  mdsip_event_info_t info;
  int size = sizeof(info);
  int status;
  info.astadr = (void (*)(void *,int ,char *))astadr;
  info.astprm = astprm;
  ansarg.ptr = 0;
  status = mdsip_value(io_channel, EVENTASTREQUEST, mdsip_make_descrip((struct descrip *)&eventnamarg,DTYPE_CSTRING,0,0,0,eventnam), 
			      mdsip_make_descrip((struct descrip *)&infoarg,DTYPE_UCHAR,0,1,&size,&info),
			      (struct descrip *)&ansarg, (struct descrip *)NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) {
    *eventid = *(int *)ansarg.ptr;
  }
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

