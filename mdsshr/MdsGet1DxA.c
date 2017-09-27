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
/*  CMS REPLACEMENT HISTORY, Element MdsGet1DxA.C */
/*  *13    5-AUG-1996 14:38:14 TWF "Fix copy_dx for unpacked descriptors" */
/*  *12    2-AUG-1996 12:28:46 TWF "fix alignment" */
/*  *11    2-AUG-1996 08:16:18 TWF "Use int instead of long" */
/*  *10   19-OCT-1995 15:01:01 TWF "Add mdsxdroutines.c" */
/*  *9    18-OCT-1994 16:38:38 TWF "No longer support VAXC" */
/*  *8     1-MAR-1993 08:41:36 TWF "Use standard indentation" */
/*  *7    26-FEB-1993 16:49:33 TWF "Use MDSSHR" */
/*  *6     9-FEB-1993 09:56:18 TWF "Use DECC" */
/*  *5     8-FEB-1993 16:16:54 TWF "Use DECC" */
/*  *4     3-FEB-1993 11:11:29 TWF "Make it portable" */
/*  *3    30-DEC-1992 11:34:45 TWF "Make it portable" */
/*  *2     1-MAY-1991 19:46:52 KLARE "Handle CLASS_CA's input, fix A0" */
/*  *1    28-DEC-1989 10:39:01 TWF "Get 1 XD pointing to an array" */
/*  CMS REPLACEMENT HISTORY, Element MdsGet1DxA.C */
/*------------------------------------------------------------------------------

		Name: MdsGet1DxA

		Type:   C function

		Author:	Thomas W. Fredian
			MIT Plasma Fusion Center

		Date:    8-SEP-1988

		Purpose: Get an XD pointing to an array descriptor
			 Used by CVT_DX_DX_A.

------------------------------------------------------------------------------

	Call sequence:

    status = MdsGet1DxA(in_dsc,length_ptr,dtype_ptr,out_xd);

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

EXPORT int MdsGet1DxA(struct descriptor_a const *in_ptr, unsigned short const *length_ptr, unsigned char const *dtype_ptr,
	       struct descriptor_xd *out_xd)
{
  array_coeff *in_dsc = (array_coeff *) in_ptr;
  unsigned int new_arsize;
  unsigned int dsc_size;
  unsigned int new_size;
  int status;
  int i;
  unsigned int align_size;
  array_coeff *out_dsc;
  unsigned char dsc_dtype = DTYPE_DSC;
  if ((in_dsc->length == 0) || (*length_ptr == 0))
    new_arsize = 0;
  else
    new_arsize = (in_dsc->arsize / in_dsc->length) * (*length_ptr);
  dsc_size = (unsigned int)(sizeof(struct descriptor_a) + ((in_dsc->aflags.coeff || (new_arsize == 0))? sizeof(char *) +
					    sizeof(int) * in_dsc->dimct : 0) +
      (in_dsc->aflags.bounds ? sizeof(int) * (size_t)(in_dsc->dimct * 2)
       : 0));
  align_size = (*dtype_ptr == DTYPE_T) ? 1 : *length_ptr;
  dsc_size = align(dsc_size, align_size);
  new_size = dsc_size + new_arsize;
  status = MdsGet1Dx(&new_size, &dsc_dtype, out_xd, NULL);
  if (status & 1) {
    out_dsc = (array_coeff *) out_xd->pointer;
    *(struct descriptor_a *)out_dsc = *(struct descriptor_a *)in_dsc;
    out_dsc->length = *length_ptr;
    out_dsc->dtype = *dtype_ptr;
    out_dsc->pointer = (char *)out_dsc + align(dsc_size, align_size);
    out_dsc->arsize = new_arsize;
    if (out_dsc->aflags.coeff || (new_arsize == 0)) {
      if (out_dsc->class == CLASS_CA) {
	int64_t offset =
	    ((int64_t) out_dsc->length) * ((in_dsc->a0 - (char *)0) /
						((int64_t) in_dsc->length));
	out_dsc->a0 = out_dsc->pointer + offset;
      } else {
	int64_t offset = ((int64_t) out_dsc->length) *
	  ((in_dsc->a0 - in_dsc->pointer) / ((int64_t) (in_dsc->length > 0 ? in_dsc ->length : 1)));
	out_dsc->a0 = out_dsc->pointer + offset;
      }
      for (i = 0; i < out_dsc->dimct; i++)
	out_dsc->m[i] = in_dsc->m[i];
      if (new_arsize == 0 && out_dsc->dimct == 0) {
	out_dsc->dimct = 1;
	out_dsc->m[0]=in_dsc->arsize;
      }
      if (in_dsc->aflags.bounds) {
	struct bound {
	  int l;
	  int u;
	};
	struct bound *new_bound_ptr = (struct bound *)&out_dsc->m[out_dsc->dimct];
	struct bound *a_bound_ptr = (struct bound *)&in_dsc->m[in_dsc->dimct];
	for (i = 0; i < out_dsc->dimct; i++)
	  new_bound_ptr[i] = a_bound_ptr[i];
      }

    }
    out_dsc->class = CLASS_A;
  }
  return status;
}
