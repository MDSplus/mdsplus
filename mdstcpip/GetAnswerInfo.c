
#include "mdsip_connections.h"
#include <stdlib.h>


///
/// Waits for the server reply as \ref Message holding a descriptor.
/// Then it fills the descriptor info such as dtype, length, ndims and numbytes passed
/// as a reference to the arguments. Usually this function is called by client that
/// waits for the answer of an expression evaluation passed using SendArg()
/// 
/// \note The output message is kept statically inside function so this code is 
/// **NOT thread safe**.
/// 
/// \param id the id of the connection to use
/// \param dtype pointer to store the descriptor dtype info
/// \param length pointer to store the descriptor length
/// \param ndims pointer to store the descriptor dimensions number
/// \param dims pointer to store the descriptor dimensions
/// \param numbytes pointer to store the descriptor total number of bytes
/// \param dptr pointer to store the descriptor data
/// \return the function returns the status held by the answered descriptor.
///
int GetAnswerInfo(int id, char *dtype, short *length, char *ndims, int *dims, int *numbytes,
		  void * *dptr)
{
  static void *m = 0;
  if (m) {
    free(m);
    m = 0;
  }
  return GetAnswerInfoTS(id, dtype, length, ndims, dims, numbytes, dptr, &m);
}



/// Thread Safe version of GetAnswerInfo(),
/// waits for the server reply as \ref Message holding a descriptor.
/// Then it fills the descriptor info such as dtype, length, ndims and numbytes passed
/// as a reference to the arguments. Usually this function is called by client that
/// waits for the answer of an expression evaluation passed using SendArg()
/// 
/// \param id the id of the connection to use
/// \param dtype pointer to store the descriptor dtype info
/// \param length pointer to store the descriptor length
/// \param ndims pointer to store the descriptor dimensions number
/// \param dims pointer to store the descriptor dimensions
/// \param numbytes pointer to store the descriptor total number of bytes
/// \param dptr pointer to store the descriptor data
/// \param mout a pointer to the Message structure to store the incoming message.
/// \return the function returns the status held by the answered descriptor.
///
int GetAnswerInfoTS(int id, char *dtype, short *length, char *ndims, int *dims, int *numbytes,
		    void * *dptr, void **mout)
{
  int status;
  int i;
  Message *m;
  *mout = 0;
  *numbytes = 0;
  m = GetMdsMsg(id, &status);
  if (status != 1) {
    *dtype = 0;
    *length = 0;
    *ndims = 0;
    *numbytes = 0;
    *dptr = 0;
    if (m) {
      free(m);
      *mout = 0;
    }
    return 0;
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
    return 0;
  }
  *dtype = m->h.dtype;
  *length = m->h.length;
  *ndims = m->h.ndims;
  *dptr = m->bytes;
  *mout = m;
  return m->h.status;
}
