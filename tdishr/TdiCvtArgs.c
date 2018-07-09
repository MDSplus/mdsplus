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
/*      TdiCvtArgs.C
        Convert input and output categories to data types.
        Convert input data as needed. Called by
        Tdi1Build Tdi1Same Tdi1Scalar Tdi1Trans Tdi1Trim Tdi1RANGE Tdi1Vector

        Ken Klare, LANL CTR-7   (c)1989,1990
*/
#include <stdlib.h>
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include <tdishr_messages.h>
#include <mdsshr.h>
#include <STATICdef.h>

#define SIGNEDNESS      (TdiCAT_B ^ TdiCAT_BU)
#define FLOATMASK       (TdiCAT_FLOAT | TdiCAT_LENGTH)



extern int TdiConvert();
extern int TdiGetShape();
extern void UseNativeFloat();

int TdiCvtArgs(int narg, struct descriptor_xd dat[1], struct TdiCatStruct cats[1])
{
  INIT_STATUS;
  int j, cmode = -1;
  unsigned char jd;
  struct TdiCatStruct *cptr;

	/*************************
        Find data type if changed.
        Check output type also.
        Make wide F/D into G/H.
        Get length from table.
        Keep old text length.
        *************************/
  for (j = 0, cptr = cats; STATUS_OK && j <= narg; ++j, ++cptr) {
    if (cptr->out_cat != cptr->in_cat) {
      if (cptr->out_cat != TdiREF_CAT[DTYPE_T].cat) {
	if (TdiREF_CAT[cptr->out_dtype].cat != cptr->out_cat) {
	  if (cats[j].out_cat & 0x400) {
	    if (cats[j].out_cat & TdiCAT_COMPLEX) {
	      switch (cats[j].out_dtype) {
	      case DTYPE_F:
		cats[j].out_dtype = DTYPE_FC;
		break;
	      case DTYPE_FS:
		cats[j].out_dtype = DTYPE_FSC;
		break;
	      case DTYPE_D:
		cats[j].out_dtype = DTYPE_DC;
		break;
	      case DTYPE_G:
		cats[j].out_dtype = DTYPE_GC;
		break;
	      case DTYPE_FT:
		cats[j].out_dtype = DTYPE_FTC;
		break;
	      default:
		UseNativeFloat(cptr);
	      }
	    } else {
	      switch (cats[j].out_dtype) {
	      case DTYPE_FC:
		cats[j].out_dtype = DTYPE_F;
		break;
	      case DTYPE_FSC:
		cats[j].out_dtype = DTYPE_FS;
		break;
	      case DTYPE_DC:
		cats[j].out_dtype = DTYPE_D;
		break;
	      case DTYPE_GC:
		cats[j].out_dtype = DTYPE_G;
		break;
	      case DTYPE_FTC:
		cats[j].out_dtype = DTYPE_FT;
		break;
	      default:
		UseNativeFloat(cptr);
	      }
	    }
	  } else {
	    for (jd = 0; jd < TdiCAT_MAX && TdiREF_CAT[jd].cat != cptr->out_cat; ++jd) {;
	    }
	    if (jd >= TdiCAT_MAX)
	      status = TdiINVDTYDSC;
	    else
	      cptr->out_dtype = jd;
	  }
	}
	cptr->digits = TdiREF_CAT[cptr->out_dtype].length;
      } else
	cptr->out_dtype = DTYPE_T;
    } else if (cptr->out_cat != TdiREF_CAT[DTYPE_T].cat)
      cptr->digits = TdiREF_CAT[cptr->out_dtype].length;
  }

	/********************
        Do input conversions.
        ********************/
  for (j = 0, cptr = cats; STATUS_OK && j < narg; ++j, ++cptr) {
    struct descriptor_a *aptr = (struct descriptor_a *)dat[j].pointer;

    if (cptr->in_dtype == cptr->out_dtype && cptr->digits == aptr->length) {;
    }

		/********************************************
                Convert signed to unsigned by changing dtype.
                ********************************************/
    else if ((cptr->in_cat ^ cptr->out_cat) == SIGNEDNESS)
      aptr->dtype = cptr->out_dtype;

    else {
			/*****************************************
                        Convert overlay for same length.
                        For example: L into F.
                        Would need separate code of scalars.
                        NEED to revise if other array classes.
                        ASSUMES array-descriptor sized readable.
                        *****************************************/
      if (aptr->length == cptr->digits) {
	if (aptr->class == CLASS_A) {
	  struct descriptor_a arr = *aptr;
	  aptr->dtype = cptr->out_dtype;
	  status = TdiConvert(&arr, aptr MDS_END_ARG);
	} else {
	  struct descriptor d = *(struct descriptor *)aptr;
	  aptr->dtype = cptr->out_dtype;
	  status = TdiConvert(&d, aptr MDS_END_ARG);
	}
      }

			/**************************
                        Make new space and convert.
                        **************************/
      else {
	struct descriptor_xd xd = EMPTY_XD;
	status = TdiGetShape(1, &dat[j], cptr->digits, cptr->out_dtype, &cmode, &xd);
	if STATUS_OK
	  status = TdiConvert(aptr, xd.pointer MDS_END_ARG);
	MdsFree1Dx(&dat[j], NULL);
	dat[j] = xd;
      }
    }
  }
  return status;
}
