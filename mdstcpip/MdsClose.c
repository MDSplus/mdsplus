
#include <STATICdef.h>
#include <stdlib.h>
#include "mdsip_connections.h"

int  MdsClose(int id)
{
  struct descrip ansarg;
  STATIC_CONSTANT char *expression = "TreeClose()";
  int status = MdsValue(id, expression, &ansarg, NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}
