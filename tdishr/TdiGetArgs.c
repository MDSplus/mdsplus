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
/*      TdiGetArgs.C
        Fetches signal, data, and category of each input.
        Guesses input conversion and output category.
        Empty XD if not a signal.
        Empty XD or null units if not with_units.
        Permits BUILD_UNITS(BUILD_SIGNAL(data),units) or BUILD_SIGNAL(BUILD_UNITS(data,units)) and others.
        Internal routine called by
                Tdi1Build       Tdi1Same        Tdi1Scalar      Tdi1Trans       Tdi1Trim
                Tdi1RANGE       Tdi1SetRange    Tdi1Vector

        Ken Klare, LANL CTR-7   (c)1989,1990
*/
#include <STATICdef.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <tdishr_messages.h>
#include <mdsshr.h>



#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefstandard.h"
#include "tdithreadsafe.h"

extern int TdiGetData();
extern int TdiData();
extern int TdiUnits();
extern int Tdi2Keep();
extern int Tdi2Long2();
extern int Tdi2Any();
extern int Tdi2Cmplx();

int TdiGetSignalUnitsData(struct descriptor *in_ptr,
			  struct descriptor_xd *signal_ptr,
			  struct descriptor_xd *units_ptr, struct descriptor_xd *data_ptr)
{
  STATIC_CONSTANT unsigned char omitsu[] = { DTYPE_SIGNAL, DTYPE_WITH_UNITS, 0 };
  STATIC_CONSTANT unsigned char omits[] = { DTYPE_SIGNAL, 0 };
  STATIC_CONSTANT unsigned char omitu[] = { DTYPE_WITH_UNITS, 0 };
  struct descriptor_xd tmp, *keep;
  INIT_STATUS;
  GET_TDITHREADSTATIC_P;
  MdsFree1Dx(signal_ptr, NULL);
  status = TdiGetData(omitsu, in_ptr, data_ptr);
  if STATUS_OK
    switch (data_ptr->pointer->dtype) {
    case DTYPE_SIGNAL:
      *signal_ptr = *data_ptr;
      *data_ptr = EMPTY_XD;
      keep = TdiThreadStatic_p->TdiSELF_PTR;
      TdiThreadStatic_p->TdiSELF_PTR = (struct descriptor_xd *)signal_ptr->pointer;
      status = TdiGetData(omitu, ((struct descriptor_signal *)signal_ptr->pointer)->data, data_ptr);
      if STATUS_OK
	switch (data_ptr->pointer->dtype) {
	case DTYPE_WITH_UNITS:
	  tmp = *data_ptr;
	  *data_ptr = EMPTY_XD;
	  status =
	      TdiData(((struct descriptor_with_units *)tmp.pointer)->units, units_ptr MDS_END_ARG);
	  if STATUS_OK
	    status =
		TdiData(((struct descriptor_with_units *)tmp.pointer)->data, data_ptr MDS_END_ARG);
	  MdsFree1Dx(&tmp, NULL);
	  break;
	default:
	  MdsFree1Dx(units_ptr, NULL);
	  break;
	}
      TdiThreadStatic_p->TdiSELF_PTR = keep;
      break;
    case DTYPE_WITH_UNITS:
      tmp = *data_ptr;
      *data_ptr = EMPTY_XD;
      status = TdiUnits(tmp.pointer, units_ptr MDS_END_ARG);
      if STATUS_OK
	status = TdiGetData(omits, tmp.pointer, data_ptr);
      if STATUS_OK
	switch (data_ptr->pointer->dtype) {
	case DTYPE_SIGNAL:
	  *signal_ptr = *data_ptr;
	  *data_ptr = EMPTY_XD;
	  status = TdiData(signal_ptr->pointer, data_ptr MDS_END_ARG);
	  break;
	default:
	  break;
	}
      MdsFree1Dx(&tmp, NULL);
      break;
    default:
      MdsFree1Dx(units_ptr, NULL);
      break;
    }
  return status;
}

void UseNativeFloat(struct TdiCatStruct *cat)
{
  unsigned char k;
  for (k = 0; k < TdiCAT_MAX; k++)
    if (((TdiREF_CAT[k].cat & ~(0x800)) == (cat->out_cat & ~(0x800))) &&
	(k == DTYPE_NATIVE_FLOAT ||
	 k == DTYPE_NATIVE_DOUBLE || k == DTYPE_FLOAT_COMPLEX || k == DTYPE_DOUBLE_COMPLEX)) {
      cat->out_dtype = k;
      cat->out_cat = TdiREF_CAT[k].cat;
    }
}

/*-------------------------------------------------------------------*/

int TdiGetArgs(int opcode,
	       int narg,
	       struct descriptor *list[],
	       struct descriptor_xd sig[],
	       struct descriptor_xd uni[], struct descriptor_xd dat[], struct TdiCatStruct cats[])
{
  INIT_STATUS;
  struct TdiCatStruct *cptr;
  struct TdiFunctionStruct *fun_ptr = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
  int nc = 0, j;
  unsigned short i1 = TdiREF_CAT[fun_ptr->i1].cat,
      i2 = TdiREF_CAT[fun_ptr->i2].cat,
      o1 = TdiREF_CAT[fun_ptr->o1].cat, o2 = TdiREF_CAT[fun_ptr->o2].cat;
  unsigned char nd = 0, jd;
  int use_native = (fun_ptr->f2 != Tdi2Keep && fun_ptr->f2 != Tdi2Long2
		    && fun_ptr->f2 != Tdi2Any && fun_ptr->f2 != Tdi2Cmplx);
	/***************************
        Get signal, units, and data.
        ***************************/
  for (j = 0; j < narg; ++j) {
    sig[j] = uni[j] = dat[j] = EMPTY_XD;
    if STATUS_OK
      status = TdiGetSignalUnitsData(list[j], &sig[j], &uni[j], &dat[j]);
  }

	/******************************
        Find category of data type.
        Adjust out in f2 routine.
        Make in into out in CVT_ARGS.
        ******************************/
  if STATUS_OK
    for (nd = 0, nc = 0, j = 0; j < narg; ++j) {
      struct descriptor *dat_ptr = dat[j].pointer;

      cptr = &cats[j];
      cptr->out_dtype = cptr->in_dtype = jd = dat_ptr->dtype;
      if (jd > nd && (nd = jd) >= TdiCAT_MAX) {
	status = TdiINVDTYDSC;
	break;
      }
      cptr->out_cat = (unsigned short)(((cptr->in_cat = TdiREF_CAT[jd].cat) | i1) & i2);
      if (jd != DTYPE_MISSING)
	nc |= cptr->out_cat;
      if ((cptr->digits = TdiREF_CAT[jd].digits) == 0)
	cptr->digits = dat_ptr->length;
    }

  for (j = 0; j < narg; j++) {
    if (fun_ptr->i1 == fun_ptr->i2) {
      cats[j].out_dtype = fun_ptr->i2;
      cats[j].out_cat = i2;
    } else if (cats[j].out_cat & 0x400 && use_native)
      UseNativeFloat(&cats[j]);
  }

	/***********************************************
        Output cat and dtype are guesses, checked later.
        ***********************************************/
  if STATUS_OK {
    cptr = &cats[narg];
    cptr->in_dtype = nd;
    cptr->out_dtype = nd;
    cptr->in_cat = TdiREF_CAT[nd].cat;
    cptr->out_cat = (unsigned short)((nc | o1) & o2);
    cptr->digits = TdiREF_CAT[nd].digits;
    if (fun_ptr->o1 == fun_ptr->o2) {
      cptr->out_dtype = cptr->in_dtype = fun_ptr->o2;
      cptr->out_cat = cptr->in_cat = o2;
    } else if (cptr->out_cat & 0x400 && use_native)
      UseNativeFloat(cptr);
  }
  return status;
}
