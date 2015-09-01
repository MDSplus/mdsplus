#include <string.h>

#include "mdsip_connections.h"



short ArgLen(struct descrip *d)
{
  short len;
  switch (d->dtype) {
  case DTYPE_CSTRING:
    len = d->length ? d->length : (short)(d->ptr ? strlen(d->ptr) : 0);
    break;
  case DTYPE_UCHAR:
  case DTYPE_CHAR:
    len = sizeof(char);
    break;
  case DTYPE_USHORT:
  case DTYPE_SHORT:
    len = sizeof(short);
    break;
  case DTYPE_ULONG:
  case DTYPE_LONG:
    len = sizeof(int);
    break;
  case DTYPE_FLOAT:
    len = sizeof(float);
    break;
  case DTYPE_DOUBLE:
    len = sizeof(double);
    break;
  case DTYPE_COMPLEX:
    len = sizeof(float) * 2;
    break;
  case DTYPE_COMPLEX_DOUBLE:
    len = sizeof(double) * 2;
    break;
  case DTYPE_ULONGLONG:
  case DTYPE_LONGLONG:
    len = 8;
    break;
  }
  return len;
}
