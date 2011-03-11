
#include "mdsip_connections.h"
#include <stdlib.h>

int  MdsEventAst(int id, char *eventnam, void (*astadr)(), void *astprm, int *eventid)
{

  struct descrip eventnamarg;
  struct descrip infoarg;
  struct descrip ansarg;
  MdsEventInfo info;
  int size = sizeof(info);
  int status;
  info.astadr = (void (*)(void *,int ,char *))astadr;
  info.astprm = astprm;
  ansarg.ptr = 0;
  status = MdsValue(id, EVENTASTREQUEST, MakeDescrip((struct descrip *)&eventnamarg,DTYPE_CSTRING,0,0,eventnam), 
			      MakeDescrip((struct descrip *)&infoarg,DTYPE_UCHAR,1,&size,&info),
			      (struct descrip *)&ansarg, (struct descrip *)NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) {
    *eventid = *(int *)ansarg.ptr;
	}
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}
