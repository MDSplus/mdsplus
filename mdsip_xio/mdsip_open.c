#include "mdsip.h"

int  mdsip_open(void *io_channel, char *tree, int shot)
{
  struct descrip treearg;
  struct descrip shotarg;
  struct descrip ansarg;
  char *expression = "TreeOpen($,$)";
  int status = mdsip_value(io_channel, expression, mdsip_make_descrip((struct descrip *)&treearg,DTYPE_CSTRING,0,0,0,tree), 
			      mdsip_make_descrip((struct descrip *)&shotarg,DTYPE_LONG,0,0,0,&shot),
			      (struct descrip *)&ansarg, (struct descrip *)NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}
