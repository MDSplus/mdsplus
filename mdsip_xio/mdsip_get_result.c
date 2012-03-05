#include "mdsip.h"

int mdsip_get_result(void *io_handle, char *dtype, short *length, char *ndims, 
		     int *dims, int *numbytes, void * *dptr, void **mem)
{
  int status;
  int i;
  mdsip_message_t **m = (mdsip_message_t **)mem;
  *m = 0;
  *numbytes = 0;
  *m = mdsip_get_message(io_handle, &status);
  if (status != 1)
  {
    *dtype = 0;
    *length = 0;
    *ndims = 0;
    *numbytes = 0;
    *dptr = 0;
    if (*m) 
    {
      free(*m);
      *m=0;
    }
    return 0;
  }
  if ((*m)->h.ndims)
  {
    *numbytes = (*m)->h.length;
    for (i=0;i<(*m)->h.ndims;i++)
    {
      dims[i] = (*m)->h.dims[i];
      *numbytes *= dims[i];
    }
    for (i=(*m)->h.ndims;i < MAX_DIMS; i++)
      dims[i] = 0;
  }
  else
  {
    *numbytes = (*m)->h.length;
    for (i=0;i<MAX_DIMS;i++)
      dims[i] = 0;
  }
  if ((int)(sizeof(mdsip_message_header_t) + *numbytes) != (*m)->h.msglen)
  {
    *numbytes = 0;
    if (*m) {
      	free(*m);
        *m=0;
    }
    return 0;
  }
  *dtype = (*m)->h.dtype;
  *length = (*m)->h.length;
  *ndims = (*m)->h.ndims;
  *dptr = (*m)->bytes;
  return (*m)->h.status;
}
