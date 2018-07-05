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
/*      TdiGetShape.C
        Find the correct shape (vector/scalar) and make a new dtype array.
        Note, dat must be XDs of scalars or arrays.
        Cmode is -1 if all are scalars, 0 if array 0 is chosen, 1 if array 1, etc.
        With all scalars, we get scalar result.
        With one array, that array shape is used.
        With multiple arrays, shape of smallest is used.
        With equal elements, shape of first array is used.
        Used by TdiCvtDxDx TdiCvtArgs Tdi1Same Tdi1Scalar.

        Ken Klare LANL CTR-7    (c)1989,1990
        NEED CHECKS: Dimensions must match except first one and the smaller first dimension is used.
*/
#include <STATICdef.h>
#include <tdishr_messages.h>
#include <mdsdescrip.h>
#include <mdsdescrip.h>
#include "tdirefcat.h"
#include <mdsshr.h>



int TdiGetShape(int narg,
		struct descriptor_xd dat[1],
		unsigned short length,
		unsigned char dtype, int *cmode_ptr, struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  unsigned short ulen;
  int cmode = -1, count = 0x7fffffff, j, len, nelem;
  struct descriptor_a *aptr;

	/*******************************
        Find the smallest array, if any.
        *******************************/
  for (j = 0; j < narg; j++) {
    aptr = (struct descriptor_a *)dat[j].pointer;
    if (aptr)
      switch (aptr->class) {
      case CLASS_S:
      case CLASS_D:
	break;
      case CLASS_A:
	nelem = ((int)aptr->length > 0) ? (int)aptr->arsize / (int)aptr->length : 0;
	if (nelem < count) {
	  count = nelem;
	  cmode = j;
	}
	break;
      default:
	status = TdiINVCLADSC;
	break;
      }
  }

	/*****************************
        Get array or scalar as needed.
        *****************************/
  if STATUS_OK {
    if ((len = length) == 0 && dtype < TdiCAT_MAX)
      len = TdiREF_CAT[dtype].length;
    ulen = (unsigned short)len;
    if (cmode < 0)
      status = MdsGet1DxS(&ulen, &dtype, out_ptr);
    else
      status = MdsGet1DxA((struct descriptor_a *)dat[cmode].pointer, &ulen, &dtype, out_ptr);
  }
  *cmode_ptr = cmode;
  return status;
}
