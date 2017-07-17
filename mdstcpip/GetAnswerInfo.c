#include "mdsip_connections.h"
#include <stdlib.h>
#include <status.h>

////////////////////////////////////////////////////////////////////////////////
//  GetAnswerInfo  /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

__attribute__((deprecated))
int GetAnswerInfo(int id, char *dtype, short *length, char *ndims, int *dims, int *numbytes,
		  void **dptr, int timeout_sec){
  static void *m = 0;
  if (m) {
    free(m);
    m=NULL;
  }
  return GetAnswerInfoTS(id, dtype, length, ndims, dims, numbytes, dptr, &m, timeout_sec);
}


////////////////////////////////////////////////////////////////////////////////
//  GetAnswerInfoTS  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int GetAnswerInfoTS(int id, char *dtype, short *length, char *ndims, int *dims, int *numbytes,
		    void **dptr, void **mout, int timeout_sec){
  INIT_STATUS;
  int i;
  Message *m;
  *mout = 0;
  *numbytes = 0;
  m = GetMdsMsgTO(id, &status,timeout_sec);
  if STATUS_NOT_OK {
    *dtype = 0;
    *length = 0;
    *ndims = 0;
    *numbytes = 0;
    *dptr = 0;
    if (m) {
      free(m);
      *mout = 0;
    }
    return MDSplusERROR;
  }
  if (m->h.ndims) {
    *numbytes = m->h.length;
    for (i = 0; i < m->h.ndims; i++) {
#ifdef __CRAY
      dims[i] = i % 2 ? m->h.dims[i / 2] & 0xffffffff : (*m)->h.dims[i / 2] >> 32;
#else
      dims[i] = m->h.dims[i];
#endif
      *numbytes *= dims[i];
#ifdef DEBUG
      printf("dim[%d] = %d\n", i, dims[i]);
#endif
    }
    for (i = m->h.ndims; i < MAX_DIMS; i++)
      dims[i] = 0;
  } else {
    *numbytes = m->h.length;
    for (i = 0; i < MAX_DIMS; i++)
      dims[i] = 0;
  }
  if ((int)(sizeof(MsgHdr) + *numbytes) != m->h.msglen) {
    *numbytes = 0;
    if (m) {
      free(m);
      *mout = 0;
    }
    return MDSplusERROR;
  }
  *dtype = m->h.dtype;
  *length = m->h.length;
  *ndims = m->h.ndims;
  *dptr = m->bytes;
  *mout = m;
  return m->h.status;
}
