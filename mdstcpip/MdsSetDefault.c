
#include <STATICdef.h>
#include "mdsip_connections.h"
#include <stdlib.h>

int  MdsSetDefault(int id, char *node)
{
  struct descrip nodearg;
  struct descrip ansarg;
  STATIC_CONSTANT char *expression = "TreeSetDefault($)";
  int status = MdsValue(id, expression, MakeDescrip(&nodearg,DTYPE_CSTRING,0,0,node), &ansarg, NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}
