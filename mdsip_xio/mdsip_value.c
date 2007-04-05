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
  struct descrip *ans_arg;
  struct descrip *arg = &exparg;
  va_start(incrmtr, expression);
  for (a_count = 1; arg != NULL; a_count++)
  {
    ans_arg=arg;
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
    status = mdsip_get_result(io_handle, &ans_arg->dtype, &len, &ans_arg->ndims, ans_arg->dims, &numbytes, &dptr, &mem);
    ans_arg->length = len;
    if (numbytes)
    {
      if (ans_arg->dtype == DTYPE_CSTRING)
      {
        ans_arg->ptr = malloc(numbytes+1);
        ((char *)ans_arg->ptr)[numbytes] = 0;
      }
      else if (numbytes > 0)
        ans_arg->ptr = malloc(numbytes);
      if (numbytes > 0)
        memcpy(ans_arg->ptr,dptr,numbytes);
    }
    else
      ans_arg->ptr = NULL;
    if (mem) free(mem);
  }
  else
    ans_arg->ptr = NULL;
  return status;
}

