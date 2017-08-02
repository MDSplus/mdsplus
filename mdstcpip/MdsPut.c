
#include <STATICdef.h>
#include "mdsip_connections.h"
#include <stdlib.h>
#include <string.h>


////////////////////////////////////////////////////////////////////////////////
//  MdsPut  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



#ifdef _USE_VARARGS
int MdsPut(va_alist)
va_dcl
{
  int id;
  char *node;
  char *expression;
#else
int MdsPut(int id, char *node, char *expression, ...)
{
/**** NOTE: NULL terminated argument list expected ****/
#endif
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  unsigned char idx = 0;
  int status = 1;
  STATIC_CONSTANT char *putexpprefix = "TreePut(";
  STATIC_CONSTANT char *argplace = "$,";
  char *putexp;
  struct descrip putexparg;
  struct descrip exparg;
  struct descrip *arg;
#ifdef _USE_VARARGS
  va_start(incrmtr);
  id = va_arg(incrmtr, int);
  node = va_arg(incrmtr, char *);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
  for (a_count = 3; va_arg(incrmtr, struct descrip *); a_count++) ;
  putexp = malloc(strlen(putexpprefix) + (a_count - 1) * strlen(argplace) + 1);
  strcpy(putexp, putexpprefix);
  for (i = 0; i < a_count - 1; i++)
    strcat(putexp, argplace);
  putexp[strlen(putexp) - 1] = ')';
#ifdef _USE_VARARGS
  va_start(incrmtr);
  id = va_arg(incrmtr, int);
  node = va_arg(incrmtr, char *);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
  nargs = a_count;
  arg = MakeDescrip(&putexparg, DTYPE_CSTRING, 0, 0, putexp);
  status = SendArg(id, idx++, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
  free(putexp);
  arg = MakeDescrip(&exparg, DTYPE_CSTRING, 0, 0, node);
  status = SendArg(id, idx++, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
  arg = MakeDescrip(&exparg, DTYPE_CSTRING, 0, 0, expression);
  for (i = idx; i < a_count && (status & 1); i++) {
    status = SendArg(id, (char)i, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    arg = va_arg(incrmtr, struct descrip *);
  }
  va_end(incrmtr);
  if (status & 1) {
    char dtype;
    int dims[MAX_DIMS];
    char ndims;
    short len;
    int numbytes;
    void *dptr;
    void *mem = 0;
    status = GetAnswerInfoTS(id, &dtype, &len, &ndims, dims, &numbytes, &dptr, &mem, -1.f);
    if (status & 1 && dtype == DTYPE_LONG && ndims == 0 && numbytes == sizeof(int))
      memcpy(&status, dptr, numbytes);
    if (mem)
      free(mem);
  }
  return status;
}
