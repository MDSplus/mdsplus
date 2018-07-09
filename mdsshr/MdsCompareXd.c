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

       Name:	MdsCompareXd

       Type:	C function

       Author:	Josh Stillerman
	       MIT Plasma Fusion Center

       Date:	7-SEP-1988

       Purpose: Routine to compare two things pased by descriptor.

       Call sequence:
	       struct descriptor *dsc1_ptr;
	       struct descriptor *dsc2_ptr;

	       isequal = MdsCompareXd(dsc1_ptr, dsc2_ptr);
       returns:
	      0 - dsc1_ptr not equiv to dsc2_ptr
	     1 - dsc1_ptr equiv to dsc2_ptr
------------------------------------------------------------------------------
       Copyright (c) 1988
       Property of Massachusetts Institute of Technology, Cambridge MA 02139.
       This program cannot be copied or distributed in any form for non-MIT
       use without specific written approval of MIT Plasma Fusion Center
       Management.

-----------------------------------------------------------------------------*/
#include <string.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

EXPORT int MdsCompareXd(const struct descriptor *dsc1_ptr, const struct descriptor *dsc2_ptr)
{
  const struct descriptor *d1 = dsc1_ptr;
  const struct descriptor *d2 = dsc2_ptr;
  int isequal = 0;
/**************************
  Strip off any extrainous
  descriptors from both of
  the inputs.
***************************/
  while (d1 && d1->dtype == DTYPE_DSC)
    d1 = (const struct descriptor *)d1->pointer;
  while (d2 && d2->dtype == DTYPE_DSC)
    d2 = (const struct descriptor *)d2->pointer;
  if (d1 && d2) {
    if (d1->dtype == d2->dtype) {
      switch (d1->class) {
      case CLASS_S:
      case CLASS_D:
	if (((d2->class == CLASS_S) || (d2->class == CLASS_D)) && (d1->length == d2->length))
	  isequal = memcmp(d1->pointer, d2->pointer, d1->length) == 0;
	break;

      case CLASS_XD:
      case CLASS_XS:
	if ((d2->class == CLASS_XS) || (d2->class == CLASS_XD)) {
	  struct descriptor_xd *xd1 = (struct descriptor_xd *)d1;
	  struct descriptor_xd *xd2 = (struct descriptor_xd *)d2;
	  if (xd1->l_length == xd2->l_length)
	    isequal = memcmp(xd1->pointer, xd2->pointer, xd1->l_length) == 0;
	}
	break;

      case CLASS_R:
	if (d2->class == CLASS_R) {
	  struct descriptor_r *r1 = (struct descriptor_r *)d1;
	  struct descriptor_r *r2 = (struct descriptor_r *)d2;
	  int i;
	  isequal = (r1->length == r2->length) && (r1->ndesc == r2->ndesc);
	  if (isequal) {
	    if (r1->length) {
	      isequal = memcmp(r1->pointer, r2->pointer, r1->length) == 0;
	    }
	    if (isequal) {
	      for (i = 0; isequal && (i < r1->ndesc); i++) {
		isequal = MdsCompareXd(r1->dscptrs[i], r2->dscptrs[i]) & 1;
	      }
	    }
	  }
	}
	break;

      case CLASS_A:
      case CLASS_CA:
      case CLASS_APD:
	if (d2->class == d1->class) {
	  array_bounds *a1 = (array_bounds *) d1;
	  array_bounds *a2 = (array_bounds *) d2;
	  isequal = (a1->length == a2->length) &&
	    (a1->arsize == a2->arsize) &&
	    (a1->scale == a2->scale) &&
	    (a1->digits == a2->digits) &&
	    (a1->dimct == a2->dimct) &&
	    (a1->aflags.binscale == a2->aflags.binscale) &&
	    (a1->aflags.redim == a2->aflags.redim) &&
	    (a1->aflags.column == a2->aflags.column) &&
	    (a1->aflags.coeff == a2->aflags.coeff) &&
	    (a1->aflags.bounds == a2->aflags.bounds);
	  if (isequal ) {
	    if (a1->aflags.coeff) {
	      isequal = (a1->pointer - a1->a0 == a2->pointer - a2->a0) &&
		(memcmp(a1->m, a2->m, sizeof(a1->m[0]) * a1->dimct) == 0);
	      if (isequal && a1->aflags.bounds) {
		isequal = memcmp(a1->m + a1->dimct, a2->m + a2->dimct,
				sizeof(a1->bounds[0]) * a1->dimct) == 0;
	      }
	    }
	    if (isequal) {
	      switch (a1->class) {
	      case CLASS_A:
		isequal = memcmp(a1->pointer, a2->pointer, a1->arsize) == 0;
		break;
	      case CLASS_CA:
		isequal = MdsCompareXd((struct descriptor *)a1->pointer,
				      (struct descriptor *)a2->pointer);
		break;
	      case CLASS_APD:
		{
		  unsigned int i, nelts = a1->arsize / a1->length;
		  struct descriptor *ptr1 = (struct descriptor *)a1->pointer;
		  struct descriptor *ptr2 = (struct descriptor *)a2->pointer;
		  for (i = 0; isequal && (i < nelts);) {
		    isequal = MdsCompareXd(ptr1, ptr2);
		    ptr1++;
		    ptr2++;
		  }
		}
	      }
	    }
	  }
	}
	break;
      }
    }
  } else {
    isequal = !(d1 || d2);
  }
  return isequal;
}
