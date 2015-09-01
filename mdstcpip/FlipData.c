
#include "mdsip_connections.h"


void FlipData(Message * m)
{
  int num = 1;
  int i;
  char *ptr;
  int dims[MAX_DIMS];
  for (i = 0; i < MAX_DIMS; i++) {
#ifdef __CRAY
    dims[i] = i % 2 ? m->h.dims[i / 2] & 0xffffffff : m->h.dims[i / 2] >> 32;
#else
    dims[i] = m->h.dims[i];
#endif
  }
  if (m->h.ndims)
    for (i = 0; i < m->h.ndims; i++)
      num *= dims[i];
#ifdef DEBUG
  printf("num to flip = %d\n", num);
#endif
  switch (m->h.dtype) {
#ifndef __CRAY
  case DTYPE_COMPLEX:
  case DTYPE_COMPLEX_DOUBLE:
    for (i = 0, ptr = m->bytes; i < (num * 2); i++, ptr += m->h.length / 2)
      FlipBytes(m->h.length / 2, ptr);
    break;
  case DTYPE_FLOAT:
  case DTYPE_DOUBLE:
#endif
  case DTYPE_LONGLONG:
  case DTYPE_ULONGLONG:
  case DTYPE_USHORT:
  case DTYPE_SHORT:
  case DTYPE_ULONG:
  case DTYPE_LONG:
    for (i = 0, ptr = m->bytes; i < num; i++, ptr += m->h.length)
      FlipBytes(m->h.length, ptr);
    break;
  }
}
