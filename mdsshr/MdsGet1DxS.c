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
#include <status.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <STATICdef.h>

#define align(bytes,size) ((((bytes) + (size) - 1)/(size)) * (size))

EXPORT int MdsGet1DxS(unsigned short const *length_ptr, unsigned char const *dtype_ptr,
	       struct descriptor_xd *out_dsc_ptr)
{

  int status;
  unsigned int dsc_size = (unsigned int)sizeof(struct descriptor);
  unsigned int align_size = (*dtype_ptr == DTYPE_T) ? 1 : *length_ptr;
  unsigned int length;
  STATIC_CONSTANT unsigned char dsc_dtype = DTYPE_DSC;
  dsc_size = align(dsc_size, align_size);
  length = dsc_size + *length_ptr;
  status = MdsGet1Dx(&length, &dsc_dtype, out_dsc_ptr, NULL);
  if STATUS_OK {
    out_dsc_ptr->pointer->length = *length_ptr;
    out_dsc_ptr->pointer->dtype = *dtype_ptr;
    out_dsc_ptr->pointer->class = CLASS_S;
    out_dsc_ptr->pointer->pointer = (char *)out_dsc_ptr->pointer + dsc_size;
  }
  return status;
}
