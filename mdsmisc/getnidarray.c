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


int GetNidArray(struct descriptor *list, int **nids, int *num_nids)
{

  struct descriptor *ptr;
  int arg;
  int *nidarray;

  for(ptr=list; (ptr && (ptr->dtype==DTYPE_DSC)); ptr= (struct descriptor *)ptr->pointer);
  if ((ptr->class == CLASS_R) && 
      (ptr->dtype == DTYPE_FUNCTION) &&
      (*(unsigned short *)ptr->pointer == OpcVector)) {
   struct descriptor_r *r_ptr= (struct descriptor_r *)ptr;
   *num_nids = r_ptr->ndesc;
   *nids = (int *)malloc(sizeof(int) * *num_nids);
   nidarray = *nids;
   for (arg=0; arg<*num_nids; arg++) {
     struct descriptor *ptr;
     for (ptr=r_ptr->dscptrs[arg]; (ptr&&(ptr->dtype==DTYPE_DSC)); 
          ptr = (struct descriptor *)ptr->pointer);
     if (ptr && (ptr->dtype==DTYPE_NID))
       nidarray[arg] = *(int *)ptr->pointer;
     else
       nidarray[arg] = 0;
   }
  }
  else {
    *num_nids = 0;
    *nids = 0;
    return 0;
  }
  return 1;
}
