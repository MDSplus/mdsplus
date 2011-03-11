
#include <STATICdef.h>
#include <ipdesc.h>
#include <stdlib.h>
int  MdsOpen(int id, char *tree, int shot)
{
  struct descrip treearg;
  struct descrip shotarg;
  struct descrip ansarg;
  STATIC_CONSTANT char *expression = "TreeOpen($,$)";
  int status = MdsValue(id, expression, MakeDescrip((struct descrip *)&treearg,DTYPE_CSTRING,0,0,tree), 
			      MakeDescrip((struct descrip *)&shotarg,DTYPE_LONG,0,0,&shot),
			      (struct descrip *)&ansarg, (struct descrip *)NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}
