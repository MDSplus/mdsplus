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
/*------------------------------------------------------------------------------

		Name:   GETNIDARRAY   

		Type:   C function

     		Author:	JOSH STILLERMAN

		Date:   21-AUG-1990

    		Purpose: Return a C array of nids given a descriptor of a
                      nid of a node which contains a vector of nids.
                      (i.e. contains AS_IS([\A, \B, \C,...])
                      uses malloc to get the space to hold the nids. 

------------------------------------------------------------------------------

	Call sequence: 

int GetNidArray(struct dsc$descriptor *list, int **nids, int *num_nids)

------------------------------------------------------------------------------
   Copyright (c) 1990
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <stdlib.h>

extern unsigned short OpcVector;

EXPORT int GetNidArray(struct descriptor *list, int **nids, int *num_nids)
{

  struct descriptor *ptr;
  int arg;
  int *nidarray;

  for (ptr = list; (ptr && (ptr->dtype == DTYPE_DSC)); ptr = (struct descriptor *)ptr->pointer) ;
  if ((ptr->class == CLASS_R) &&
      (ptr->dtype == DTYPE_FUNCTION) && (*(unsigned short *)ptr->pointer == OpcVector)) {
    struct descriptor_r *r_ptr = (struct descriptor_r *)ptr;
    *num_nids = r_ptr->ndesc;
    *nids = (int *)malloc(sizeof(int) * *num_nids);
    nidarray = *nids;
    for (arg = 0; arg < *num_nids; arg++) {
      struct descriptor *ptr;
      for (ptr = r_ptr->dscptrs[arg]; (ptr && (ptr->dtype == DTYPE_DSC));
	   ptr = (struct descriptor *)ptr->pointer) ;
      if (ptr && (ptr->dtype == DTYPE_NID))
	nidarray[arg] = *(int *)ptr->pointer;
      else
	nidarray[arg] = 0;
    }
  } else {
    *num_nids = 0;
    *nids = 0;
    return 0;
  }
  return 1;
}
