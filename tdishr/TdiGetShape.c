/*	TdiGetShape.C
	Find the correct shape (vector/scalar) and make a new dtype array.
	Note, dat must be XDs of scalars or arrays.
	Cmode is -1 if all are scalars, 0 if array 0 is chosen, 1 if array 1, etc.
	With all scalars, we get scalar result.
	With one array, that array shape is used.
	With multiple arrays, shape of smallest is used.
	With equal elements, shape of first array is used.
	Used by TdiCvtDxDx TdiCvtArgs Tdi1Same Tdi1Scalar.

	Ken Klare LANL CTR-7	(c)1989,1990
	NEED CHECKS: Dimensions must match except first one and the smaller first dimension is used.
*/
#include <tdimessages.h>
#include <mdsdescrip.h>
#include <mdsdescrip.h>
#include "tdirefcat.h"
#include <mdsshr.h>

int				TdiGetShape(
int				narg,
struct descriptor_xd	dat[1],
unsigned short			length,
unsigned char			dtype,
int				*cmode_ptr,
struct descriptor_xd	*out_ptr)
{
int	cmode = -1, status = 1, count = 0x7fffffff, j, len, nelem;
struct descriptor_a		*aptr;

	/*******************************
	Find the smallest array, if any.
	*******************************/
	for (j = 0; j < narg; j++) {
		aptr = (struct descriptor_a *)dat[j].pointer;
		if (aptr) switch (aptr->class) {
		case CLASS_S :
		case CLASS_D :
			break;
		case CLASS_A :
			nelem = (int)aptr->arsize / (int)aptr->length;
			if (nelem < count) {
				count = nelem;
				cmode = j;
			}
			break;
		default :
			status = TdiINVCLADSC;
			break;
		}
	}

	/*****************************
	Get array or scalar as needed.
	*****************************/
	if (status & 1) {
		if ((len = length) == 0 && dtype < TdiCAT_MAX) len = TdiREF_CAT[dtype].length;
		if (cmode < 0) status = MdsGet1DxS((unsigned short *)&len, &dtype, out_ptr);
		else status = MdsGet1DxA((struct descriptor_a *)dat[cmode].pointer, (unsigned short *)&len, &dtype, 
                                             out_ptr);
	}
	*cmode_ptr = cmode;
	return status;
}
