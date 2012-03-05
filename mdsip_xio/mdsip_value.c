#include "mdsip.h"

int mdsip_value(void *io_handle, char *expression, ...)  /**** NOTE: NULL terminated argument list expected ****/
{
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  unsigned char idx;
  int status = 1;
  struct descrip exparg;
  struct descrip *arg = &exparg;
  va_start(incrmtr, expression);
  for (a_count = 1; arg != NULL; a_count++)
  {
    arg = va_arg(incrmtr, struct descrip *);
  }
  va_start(incrmtr, expression);
  nargs = a_count - 2;
  arg = mdsip_make_descrip((struct descrip *)&exparg,DTYPE_CSTRING,0,0,0,expression);
  for (i=1;i<a_count-1 && (status & 1);i++)
  {
    idx = i - 1;
    status = mdsip_send_arg(io_handle, idx, arg->dtype, nargs, mdsip_arglen(arg), arg->ndims, arg->dims, arg->ptr);
    arg = va_arg(incrmtr, struct descrip *);
  }
  va_end(incrmtr);
  if (status & 1)
  {
    short len;
    int numbytes;
    void *dptr;
    void *mem = 0;
    status = mdsip_get_result(io_handle, &arg->dtype, &len, &arg->ndims, arg->dims, &numbytes, &dptr, &mem);
    arg->length = len;
    if (numbytes)
    {
      if (arg->dtype == DTYPE_CSTRING)
      {
        arg->ptr = malloc(numbytes+1);
        ((char *)arg->ptr)[numbytes] = 0;
      }
      else if (numbytes > 0)
        arg->ptr = malloc(numbytes);
      if (numbytes > 0)
        memcpy(arg->ptr,dptr,numbytes);
    }
    else
      arg->ptr = NULL;
    if (mem) free(mem);
  }
  else
    arg->ptr = NULL;
  return status;
}

