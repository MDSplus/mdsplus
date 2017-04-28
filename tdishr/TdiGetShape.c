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
