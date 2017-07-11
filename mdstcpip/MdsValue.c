
#include "mdsip_connections.h"
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
//  MdsValue  //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int MdsValue(int id, char *expression, ...)
{
/**** NOTE: NULL terminated argument list expected ****/
  va_list incrmtr;
  int a_count;
  int i;
  unsigned char nargs;
  unsigned char idx;
  int status = 1;
  struct descrip exparg;
  struct descrip *ans_arg;
  struct descrip *arg = &exparg;
#ifdef _USE_VARARGS
  va_start(incrmtr);
  id = va_arg(incrmtr, int);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
  for (a_count = 1; arg != NULL; a_count++) {
    ans_arg = arg;
    arg = va_arg(incrmtr, struct descrip *);
  }
#ifdef _USE_VARARGS
  va_start(incrmtr);
  id = va_arg(incrmtr, int);
  expression = va_arg(incrmtr, char *);
#else
  va_start(incrmtr, expression);
#endif
  nargs = a_count - 2;
  arg = MakeDescrip((struct descrip *)&exparg, DTYPE_CSTRING, 0, 0, expression);
  for (i = 1; i < a_count - 1 && (status & 1); i++) {
    idx = i - 1;
    status = SendArg(id, idx, arg->dtype, nargs, ArgLen(arg), arg->ndims, arg->dims, arg->ptr);
    arg = va_arg(incrmtr, struct descrip *);
  }
  va_end(incrmtr);
  if (status & 1) {
    short len;
    int numbytes;
    void *dptr;
    void *mem = 0;
    status =
	GetAnswerInfoTS(id, &ans_arg->dtype, &len, &ans_arg->ndims, ans_arg->dims, &numbytes, &dptr,
			&mem, -1.f);
    ans_arg->length = len;
    if (numbytes) {
      if (ans_arg->dtype == DTYPE_CSTRING) {
	ans_arg->ptr = malloc(numbytes + 1);
	((char *)ans_arg->ptr)[numbytes] = 0;
      } else if (numbytes > 0)
	ans_arg->ptr = malloc(numbytes);
      if (numbytes > 0)
	memcpy(ans_arg->ptr, dptr, numbytes);
    } else
      ans_arg->ptr = NULL;
    if (mem)
      free(mem);
  } else
    ans_arg->ptr = NULL;
  return status;
}
