/*  CMS REPLACEMENT HISTORY, Element MdsGet1DxS.C */
/*  *10    2-AUG-1996 08:16:21 TWF "Use int instead of long" */
/*  *9    23-OCT-1995 08:14:57 TWF "Use sizeof descriptor" */
/*  *8    19-OCT-1995 15:00:57 TWF "Add mdsxdroutines.c" */
/*  *7    18-OCT-1994 16:38:42 TWF "No longer support VAXC" */
/*  *6     1-MAR-1993 08:41:42 TWF "Use standard indentation" */
/*  *5    26-FEB-1993 16:49:36 TWF "Use MDSSHR" */
/*  *4     8-FEB-1993 12:29:09 TWF "Use DECC" */
/*  *3     3-FEB-1993 11:11:33 TWF "Make it portable" */
/*  *2    30-DEC-1992 11:34:49 TWF "Make it portable" */
/*  *1    28-DEC-1989 10:40:41 TWF "Get 1 XD pointing to S descriptor" */
/*  CMS REPLACEMENT HISTORY, Element MdsGet1DxS.C */
/*------------------------------------------------------------------------------

		Name: MdsGet1DxS

		Type:   C function

		Author:	Thomas W. Fredian
			MIT Plasma Fusion Center

		Date:    6-SEP-1988

		Purpose: Get an XD descriptor pointing to an S descriptor

------------------------------------------------------------------------------

	Call sequence: status = MdsGet1DxS(length_ptr,dtype_ptr,out_dsc_ptr)

------------------------------------------------------------------------------
   Copyright (c) 1988
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

+-----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <STATICdef.h>

#define align(bytes,size) ((((bytes) + (size) - 1)/(size)) * (size))

EXPORT int MdsGet1DxS(unsigned short const *length_ptr, unsigned char const *dtype_ptr,
	       struct descriptor_xd *out_dsc_ptr)
{

  int status;
  int dsc_size = sizeof(struct descriptor);
  int align_size = (*dtype_ptr == DTYPE_T) ? 1 : *length_ptr;
  unsigned int length;
  STATIC_CONSTANT unsigned char dsc_dtype = DTYPE_DSC;
  dsc_size = align(dsc_size, align_size);
  length = dsc_size + *length_ptr;
  status = MdsGet1Dx(&length, &dsc_dtype, out_dsc_ptr, NULL);
  if (status & 1) {
    out_dsc_ptr->pointer->length = *length_ptr;
    out_dsc_ptr->pointer->dtype = *dtype_ptr;
    out_dsc_ptr->pointer->class = CLASS_S;
    out_dsc_ptr->pointer->pointer = (char *)out_dsc_ptr->pointer + dsc_size;
  }
  return status;
}
