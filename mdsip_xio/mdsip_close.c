#include "mdsip.h"

int  mdsip_close(void *io_handle)
{
  struct descrip ansarg;
  char *expression = "TreeClose()";
  int status = mdsip_value(io_handle, expression, &ansarg, NULL);
  if ((status & 1) && (ansarg.dtype == DTYPE_LONG)) status = *(int *)ansarg.ptr;
  if (ansarg.ptr) free(ansarg.ptr);
  return status;
}
