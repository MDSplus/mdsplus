#include "mdsip.h"

int mdsip_put(void *io_handle, char *node, char *expression, ...)  /**** NOTE: NULL terminated argument list expected ****/
{
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  unsigned char idx = 0;
  int status = 1;
  char *putexpprefix = "TreePut(";
  char *argplace = "$,";
  char *putexp;
  struct descrip putexparg;
  struct descrip exparg;
  struct descrip *arg;
  va_start(incrmtr, expression);
  for (a_count = 3; va_arg(incrmtr, struct descrip *); a_count++);
  putexp = malloc(strlen(putexpprefix) + (a_count - 1) * strlen(argplace) + 1);
  strcpy(putexp,putexpprefix);
  for (i=0;i<a_count - 1;i++) strcat(putexp,argplace);
  putexp[strlen(putexp)-1] = ')';
  va_start(incrmtr, expression);
  nargs = a_count;
  arg = mdsip_make_descrip(&putexparg,DTYPE_CSTRING,0,0,0,putexp);
  status = mdsip_send_arg(io_handle, idx++, arg->dtype, nargs, mdsip_arglen(arg), arg->ndims, arg->dims, arg->ptr);
  free(putexp);
  arg = mdsip_make_descrip(&exparg,DTYPE_CSTRING,0,0,0,node);
  status = mdsip_send_arg(io_handle, idx++, arg->dtype, nargs, mdsip_arglen(arg), arg->ndims, arg->dims, arg->ptr);
  arg = mdsip_make_descrip(&exparg,DTYPE_CSTRING,0,0,0,expression);
  for (i=idx;i<a_count && (status & 1);i++)
  {
    status = mdsip_send_arg(io_handle, (char)i, arg->dtype, nargs, mdsip_arglen(arg), arg->ndims, arg->dims, arg->ptr);
    arg = va_arg(incrmtr, struct descrip *);
  }
  va_end(incrmtr);
  if (status & 1)
  {
    char dtype;
    int dims[MAX_DIMS];
    char ndims;
    short len;
    int numbytes;
    void *dptr;
    void *mem = 0;
    status = mdsip_get_result(io_handle, &dtype, &len, &ndims, dims, &numbytes, &dptr, (void *)&mem);
    if (status & 1 && dtype == DTYPE_LONG && ndims == 0 && numbytes == sizeof(int))
      memcpy(&status,dptr,numbytes);
    if (mem) free(mem);
  }
  return status;
}

