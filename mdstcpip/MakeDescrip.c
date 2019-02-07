/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
