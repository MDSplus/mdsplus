
#include "mdsip_connections.h"

////////////////////////////////////////////////////////////////////////////////
//  MakeDescripWithLength  /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


struct descrip *MakeDescripWithLength(struct descrip *in_descrip, char dtype, int length,
				      char ndims, int *dims, void *ptr)
{
  int i;
  in_descrip->dtype = dtype;
  in_descrip->ndims = ndims;
  in_descrip->length = length;
  for (i = 0; i < ndims; i++)
    in_descrip->dims[i] = dims[i];
  for (i = ndims; i < MAX_DIMS; i++)
    in_descrip->dims[i] = 0;
  in_descrip->ptr = ptr;
  return in_descrip;
}



////////////////////////////////////////////////////////////////////////////////
//  MakeSecrip  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct descrip *MakeDescrip(struct descrip *in_descrip, char dtype, char ndims, int *dims,
			    void *ptr)
{
  int i;
  in_descrip->dtype = dtype;
  in_descrip->ndims = ndims;
  in_descrip->length = 0;
  for (i = 0; i < ndims; i++)
    in_descrip->dims[i] = dims[i];
  for (i = ndims; i < MAX_DIMS; i++)
    in_descrip->dims[i] = 0;
  in_descrip->ptr = ptr;
  return in_descrip;
}
