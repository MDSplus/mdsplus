#include "mdsip.h"

int  mdsip_set_default(void *io_handle, char *node)
{
  struct descrip nodearg;
  struct descrip ansarg;
  char *expression = "TreeSetDefault($)";
  int status = mdsip_value(io_handle, expression, mdsip_make_descrip(&nodearg,DTYPE_CSTRING,0,0,0,node), &ansarg, NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}

