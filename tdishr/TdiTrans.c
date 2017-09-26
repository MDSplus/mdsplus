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
/*      Tdi1Trans.C
        Generic transformation function possibly using a mask or specifying a dimension.
        Also includes code for C routines: ALL ANY COUNT FIRSTLOC LASTLOC REPLICATE SPREAD.
        Note: DIM begins at zero, but F90 begins at one.
        Note: MASK may be any logical of sufficient length or is converted.

                result = xx(MASK, [DIM])
        MASK1:  ALL ANY COUNT
        type logical output, scalar for no DIM or vector input, else rank (n-1) with DIM-th dimension missing.

                result = xx(ARRAY, [DIM], [MASK])
        MASK3:  MAXVAL MEAN MINVAL PRODUCT RMS STD_DEV SUM
        type numeric output, scalar for no DIM or vector input, else rank (n-1) with DIM-th dimension missing.

                result = xx(ARRAY, [DIM], [MASK])
        MASK3:  ACCUMULATE
        type numeric output of input shape, rank n.

                rank-n = xx(MASK, [DIM])
        MASK1:  FIRSTLOC LASTLOC
        type logical output of input shape, rank n.

                rank-n = xx(ARRAY, DIM, [const])
        SIGN:   DERIVATIVE, INTEGRAL, RC_DROOP
        type numeric output of input shape, rank n, mask set to signal axis.

                rank-n = REPLICATE(ARRAY, DIM, NCOPIES)
        SIGN:   REPLICATE
        type same as ARRAY, rank n, replicates DIM-th dimension by a factor.

                rank-(n+1) = SPREAD(ARRAY, DIM, NCOPIES)
        SIGN:   SPREAD
        type same as ARRAY, rank n+1, spreads and jams in a dimension before DIM-th, DIM = 0..rank.

                rank1 = xx(ARRAY, [MASK])
        MASK2:  MAXLOC MINLOC
        vector offsets from first element.

        For logical array V[3,4,5], steps in unit sizes:
        loop    count @step     no DIM  DIM=0   optim   DIM=1   DIM=2
        outer   aft             1 @xx   20 @3   1 @xx   5 @12   1 @60   DIM dimension and number of results
        middle  bef             1 @1    1 @3    20 @3   3 @1    12 @1   product of dims after DIM
        inner   dim             60 @1   3 @1    3 @1    4 @3    5 @12   product of dims before DIM
        Example, ALL(V,0) chooses elements: [0,1,2] [3,4,5] ...
        Example, ALL(V,1) chooses elements: [0,3,6,9] [1,4,7,10] [2,5,8,11] / [12,15,18,21] ...
        Example, ALL(V,2) chooses elements: [0,12,24,36,48] [1,13...], ... [11,...]

        Ken Klare, LANL P-4     (c)1989,1990,1991
        NEED units for DERIVATIVE INTEGRAL.
        NEED to know about REPLICATE and SPREAD with subscript ranges.
*/

#define _MOVC3(a,b,c) memcpy(c,b,a)
#include <STATICdef.h>
#ifdef WORDS_BIGENDIAN
#define MaskTrue (pi0[leni-1] & 1)
#else
#define MaskTrue (pi0[0] & 1)
#endif



extern unsigned short
 OpcAccumulate, OpcFirstLoc, OpcLastLoc, OpcProduct, OpcReplicate, OpcSpread;

#include "tdinelements.h"
#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefstandard.h"
#include <tdishr_messages.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <string.h>

extern int Tdi2Mask2();
extern int Tdi2Mask3();
extern int Tdi2Sign();
extern int TdiGetArgs();
extern int TdiCvtArgs();
extern int TdiGetLong();
extern int TdiConvert();
extern int TdiMasterData();

STATIC_CONSTANT unsigned char zero_val = 0;
STATIC_CONSTANT unsigned char one_val = 1;
STATIC_CONSTANT struct descriptor zero = { sizeof(zero_val), DTYPE_BU, CLASS_S, (char *)&zero_val };
STATIC_CONSTANT struct descriptor one = { sizeof(one_val), DTYPE_BU, CLASS_S, (char *)&one_val };

int Tdi1Trans(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor *pmask = &one;
  struct descriptor_signal *psig;
  signal_maxdim tmpsig;
  array_bounds arr, *pa, *pd;
  struct descriptor_xd sig[3], uni[3], dat[3];
  struct TdiCatStruct cats[4];
  struct TdiFunctionStruct *pfun = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
  int cmode = -1, dim = -1, j, mul = 0, ncopies = 0, rank = 0, ndim;
  int step_bef = 0, step_dim = 0, step_aft = 0;
  int count_bef = 1, count_dim = 1, count_aft = 1;
  unsigned short digits, head;
  unsigned char out_dtype;

	/******************************************
        Fetch signals and data and data's category.
        ******************************************/
  status = TdiGetArgs(opcode, narg, list, sig, uni, dat, cats);

	/******************************************
        Adjust categories need to match data types.
        ******************************************/
  if STATUS_OK
    status = (*pfun->f2) (narg, uni, dat, cats, 0);

	/******************************
        Do the needed type conversions.
        ******************************/
  if STATUS_OK
    status = TdiCvtArgs(narg, dat, cats);

	/********************
        Get dimension number.
        Get mask pointer.
        ********************/
  if (pfun->f2 == Tdi2Mask2) {
    if (narg > 1 && cats[1].in_dtype != DTYPE_MISSING)
      pmask = dat[1].pointer;
  } else if (narg > 1 && cats[1].in_dtype != DTYPE_MISSING) {
    if STATUS_OK
      status = TdiGetLong(dat[1].pointer, &dim);
    if (STATUS_OK && dim < 0)
      status = TdiBAD_INDEX;
  } else if (pfun->f2 == Tdi2Sign)
    status = TdiBAD_INDEX;

  if (pfun->f2 == Tdi2Mask3) {
    if (narg > 2 && cats[2].in_dtype != DTYPE_MISSING)
      pmask = dat[2].pointer;
  } else if (opcode == OpcReplicate || opcode == OpcSpread) {
    if STATUS_OK
      status = TdiGetLong(dat[2].pointer, &ncopies);
    if (ncopies < 0)
      ncopies = 0;
  }

	/**************************
        Get rank,counts, and steps.
        **************************/
  pa = (array_bounds *) dat[0].pointer;
  if STATUS_OK
    switch (pa->class) {
    case CLASS_A:
      rank = pa->aflags.coeff ? pa->dimct : 1;
      if (rank > MAXDIM)
	status = TdiNDIM_OVER;
		/** Whole array. Unrestricted. Product of multipliers == size, unchecked **/
      else if (dim < 0) {
	count_dim = (int)pa->arsize / (int)pa->length;
      }
		/** Array with multipliers **/
      else if (pa->aflags.coeff) {
	if (dim < rank || (dim <= rank && opcode == OpcSpread)) {
	  for (j = 0; j < dim; ++j)
	    count_bef *= pa->m[j];
	  if (j < rank)
	    count_dim = pa->m[j];
	  for (; ++j < rank;)
	    count_aft *= pa->m[j];
	} else
	  status = TdiBAD_INDEX;
      }
		/** Simple vector. A mistake for some? **/
      else {
	j = (int)pa->arsize / (int)pa->length;
	if (dim == 1 && opcode == OpcSpread)
	  count_bef = j;
	else if (dim > 0)
	  status = TdiBAD_INDEX;
	else
	  count_dim = j;
      }
      break;
    case CLASS_S:
    case CLASS_D:
      rank = 0;
      if (dim > 0 || (dim > 1 && opcode == OpcSpread)) {
	status = TdiBAD_INDEX;
      }
      break;
    default:
      status = TdiINVCLADSC;
      break;
    }

  if STATUS_OK {
    N_ELEMENTS(pmask, mul);
  }
  if STATUS_OK {
    if (pmask->class == CLASS_A && mul < count_bef * count_dim * count_aft)
      status = TdiMISMATCH;
    step_dim = count_bef;
    step_aft = count_dim * count_bef;
		/************
                Optimization.
                ************/
    if (dim == 0 && opcode != OpcReplicate && opcode != OpcSpread) {
      count_bef = count_aft;
      count_aft = 1;
      step_bef = step_aft;
    } else
      step_bef = 1;
  }

	/******************
        Find correct shape.
        ******************/
  if (STATUS_NOT_OK)
    goto err;
  head = (unsigned short)(sizeof(struct descriptor_a) +
			  (pa->aflags.coeff ? sizeof(char *) + rank * sizeof(int) : 0) +
			  (pa->aflags.bounds ? 2 * rank * sizeof(int) : 0));
  digits = cats[narg].digits;
  out_dtype = cats[narg].out_dtype;

	/****************************
        Same shape as input, cleared.
        ****************************/
  psig = (struct descriptor_signal *)sig[0].pointer;
  if (psig)
    ndim = psig->ndesc - 2;
  else
    ndim = -1;
  if (opcode == OpcFirstLoc || opcode == OpcLastLoc) {
    status = MdsGet1DxA((struct descriptor_a *)pa, &digits, &out_dtype, out_ptr);
    if STATUS_OK
      status = TdiConvert(&zero, out_ptr->pointer MDS_END_ARG);
  }
	/***************************
        Shape multiplied for DIM-th.
        ***************************/
  else if (opcode == OpcReplicate) {
    if (dim < ndim)
      psig->dimensions[dim] = 0;
    pmask = (struct descriptor *)&ncopies;
		/** scalar to simple vector **/
    if (rank == 0)
      _MOVC3(head, (char *)pa, (char *)&arr);
		/** simple and coefficient vector **/
    else {
      _MOVC3(head, (char *)pa, (char *)&arr);
      arr.m[dim] *= ncopies;
    }
    arr.arsize *= ncopies;
    status = MdsGet1DxA((struct descriptor_a *)&arr, &digits, &out_dtype, out_ptr);
  }
	/*************************************
        Shape gets new dimension after DIM-th.
        *************************************/
  else if (opcode == OpcSpread) {
    if (ndim > dim) {
      EMPTYXD(tmpxd);
      *(struct descriptor_signal *)&tmpsig = *psig;
      ++tmpsig.ndesc;
      for (j = ndim; --j >= dim;)
	tmpsig.dimensions[j + 1] = psig->dimensions[j];
      tmpsig.dimensions[dim] = 0;
      for (j = dim; --j >= 0;)
	tmpsig.dimensions[j] = psig->dimensions[j];
      tmpsig.raw = 0;
      tmpsig.data = 0;
      status = MdsCopyDxXd((struct descriptor *)&tmpsig, &tmpxd);
      status = MdsCopyDxXd((struct descriptor *)&tmpxd, &sig[0]);
      MdsFree1Dx(&tmpxd, NULL);
      if (STATUS_NOT_OK)
	goto err;
    }
    pmask = (struct descriptor *)&ncopies;
		/** scalar to simple vector **/
    if (rank == 0)
      _MOVC3(head, (char *)pa, (char *)&arr);
    else if (rank >= MAXDIM)
      status = TdiNDIM_OVER;
		/** coefficient vector **/
    else if (pa->aflags.coeff) {
      _MOVC3(head, (char *)pa, (char *)&arr);
      _MOVC3((short)(sizeof(int) * (rank - dim) * 2),
	     (char *)&arr.m[rank + 2 * dim], (char *)&arr.m[rank + 2 * dim + 3]);
      _MOVC3((short)(sizeof(int) * (rank - dim)), (char *)&arr.m[dim], (char *)&arr.m[dim + 1]);
      arr.m[dim] = ncopies;
      arr.m[rank + 2 * dim + 1] = 0;
      arr.m[rank + 2 * dim + 2] = ncopies - 1;
      arr.dimct++;
    }
		/** simple vector to 2-D **/
    else {
      _MOVC3(head, (char *)pa, (char *)&arr);
      arr.dimct = 2;
      arr.aflags.coeff = 1;
      arr.m[1] = arr.m[0] = (int)pa->arsize / (int)pa->length;
      arr.m[dim] = ncopies;
    }
    arr.arsize *= ncopies;
    if STATUS_OK
      status = MdsGet1DxA((struct descriptor_a *)&arr, &digits, &out_dtype, out_ptr);
  }
	/***************
        Overwrite input.
        ***************/
  else if (opcode == OpcAccumulate) {
    MdsFree1Dx(out_ptr, NULL);
    *out_ptr = dat[0];
  }
	/*******************
        Same shape as input.
        *******************/
  else if (pfun->f2 == Tdi2Sign) {
    if (psig && rank <= ndim)
      pmask = psig->dimensions[dim];
    else
      pmask = 0;
    status = MdsGet1DxA((struct descriptor_a *)pa, &digits, &out_dtype, out_ptr);
  }
	/****************
        Subscript vector.
        ****************/
  else if (pfun->f2 == Tdi2Mask2) {
    psig = 0;
    _MOVC3(head, (char *)pa, (char *)&arr);
    arr.arsize = pa->length * rank;
    status = MdsGet1DxA((struct descriptor_a *)&arr, &digits, &out_dtype, out_ptr);
  }
	/*******************
        Rank reduced by one.
        *******************/
  else if (dim >= 0 && pa->aflags.coeff && rank > 1) {
    if (dim < ndim)
      --psig->ndesc;
    for (j = dim; ++j < ndim;)
      psig->dimensions[j - 1] = psig->dimensions[j];
    _MOVC3(head, (char *)pa, (char *)&arr);
    arr.arsize /= arr.m[dim];
    _MOVC3((short)(sizeof(int) * (rank - dim - 1)), (char *)&arr.m[dim + 1], (char *)&arr.m[dim]);
    _MOVC3((short)(sizeof(int) * (rank - dim - 1) * 2),
	   (char *)&arr.m[rank + 2 * dim + 2], (char *)&arr.m[rank + 2 * dim - 1]);
    --arr.dimct;
    status = MdsGet1DxA((struct descriptor_a *)&arr, &digits, &out_dtype, out_ptr);
  }
	/*************
        Scalar result.
        *************/
  else {
    psig = 0;
    status = MdsGet1DxS(&digits, &out_dtype, out_ptr);
  }
  if (STATUS_NOT_OK)
    goto err;

	/****************************************
        Adjust offset and test if we need bounds.
        Simple array if 1D and no bounds.
        ****************************************/
  pd = (array_bounds *) out_ptr->pointer;
  if (pd->class == CLASS_A && pd->aflags.coeff) {
    if (pd->aflags.bounds) {
      int a0 = 0, keep_bound = 0, dimct = pd->dimct, mul = pd->length;
      for (j = 0; j < dimct; ++j) {
	keep_bound |= pd->m[dimct + 2 * j];
	a0 += pd->m[dimct + 2 * j] * mul;
	mul *= pd->m[j];
      }
      pd->a0 = pd->pointer - a0;
      pd->aflags.bounds = (unsigned char)(keep_bound != 0);
    }
    pd->aflags.coeff = (unsigned char)(pd->dimct > 1 || pd->aflags.bounds);
  }

	/***********
        Act on data.
        ***********/
  status =
      (*pfun->f3) (pa, pmask, pd, count_dim, count_bef, count_aft, step_dim, step_bef, step_aft);
  if (STATUS_NOT_OK)
    goto err;

	/*******************************
        Offset list from overall offset.
        *******************************/
  if (pfun->f2 == Tdi2Mask2 && rank > 1) {
    int *pout = (int *)pd->pointer;
    int *pin = (int *)&pa->m[0];
    for (mul = *pout, j = rank; --j >= 0; ++pin) {
      if (*pin > 0) {
	*pout++ = mul % *pin;
	mul /= *pin;
      } else
	*pout++ = 0;
    }
  } else if (opcode == OpcAccumulate)
    dat[0] = EMPTY_XD;
  else if (opcode == OpcProduct)
    MdsFree1Dx(&uni[0], NULL);

  status = TdiMasterData(psig != 0, sig, uni, &cmode, out_ptr);

 err:
	/********************
        Free all temporaries.
        ********************/
  for (j = narg; --j >= 0;) {
    if (sig[j].pointer)
      MdsFree1Dx(&sig[j], NULL);
    if (uni[j].pointer)
      MdsFree1Dx(&uni[j], NULL);
    if (dat[j].pointer)
      MdsFree1Dx(&dat[j], NULL);
  }
  return status;
}

/*---------------------------------------------------------------------------
        F90 transformation of logical test of all true.
                logical = ALL(mask, [dim])
*/
int Tdi3All(struct descriptor *in_ptr,
	    struct descriptor *pmask __attribute__ ((unused)),
	    struct descriptor *out_ptr,
	    int count0, int count1, int count2, int step0, int step1, int step2)
{
  char *pi0, *pi1, *pi2, *pout = out_ptr->pointer;
  int j0, j1, j2;
  int leni = in_ptr->length, stepi0 = leni * step0, stepi1 = leni * step1, stepi2 = leni * step2;

  for (pi2 = in_ptr->pointer, j2 = count2; --j2 >= 0; pi2 += stepi2) {
    for (pi1 = pi2, j1 = count1; --j1 >= 0; pi1 += stepi1) {
      for (pi0 = pi1, j0 = count0; --j0 >= 0; pi0 += stepi0)
	if (!MaskTrue)
	  break;
      *pout++ = (char)(j0 < 0);
    }
  }
  return 1;
}

/*---------------------------------------------------------------------------
        F90 transformation of logical test of ANY true.
                logical = ANY(mask, [dim])
*/
int Tdi3Any(struct descriptor *in_ptr,
	    struct descriptor *pmask __attribute__ ((unused)),
	    struct descriptor *out_ptr,
	    int count0, int count1, int count2, int step0, int step1, int step2)
{
  char *pi0, *pi1, *pi2, *pout = out_ptr->pointer;
  int j0, j1, j2;
  int leni = in_ptr->length, stepi0 = leni * step0, stepi1 = leni * step1, stepi2 = leni * step2;

  for (pi2 = in_ptr->pointer, j2 = count2; --j2 >= 0; pi2 += stepi2) {
    for (pi1 = pi2, j1 = count1; --j1 >= 0; pi1 += stepi1) {
      for (pi0 = pi1, j0 = count0; --j0 >= 0; pi0 += stepi0)
	if (MaskTrue)
	  break;
      *pout++ = (char)(j0 >= 0);
    }
  }
  return 1;
}

/*---------------------------------------------------------------------------
        F90 transformation of number of trues to list.
                scalar-long = COUNT(mask)
                vector-long = COUNT(mask, dim)
*/
int Tdi3Count(struct descriptor *in_ptr,
	      struct descriptor *pmask __attribute__ ((unused)),
	      struct descriptor *out_ptr,
	      int count0, int count1, int count2, int step0, int step1, int step2)
{
  int result, *pout = (int *)out_ptr->pointer;
  char *pi0, *pi1, *pi2;
  int j0, j1, j2;
  int leni = in_ptr->length, stepi0 = leni * step0, stepi1 = leni * step1, stepi2 = leni * step2;

  for (pi2 = in_ptr->pointer, j2 = count2; --j2 >= 0; pi2 += stepi2) {
    for (pi1 = pi2, j1 = count1; --j1 >= 0; pi1 += stepi1) {
      result = 0;
      for (pi0 = pi1, j0 = count0; --j0 >= 0; pi0 += stepi0)
	if (MaskTrue)
	  ++result;
      *pout++ = result;
    }
  }
  return 1;
}

/*---------------------------------------------------------------------------
        F8X-extension transformation to set first true of array or DIM reduced.
                array-logical = FIRSTLOC(mask, [dim])
*/
int Tdi3FirstLoc(struct descriptor *in_ptr,
		 struct descriptor *pmask __attribute__ ((unused)),
		 struct descriptor *out_ptr,
		 int count0, int count1, int count2, int step0, int step1, int step2)
{
  char *pi0, *pi1, *pi2, *pin = in_ptr->pointer, *pout = out_ptr->pointer;
  int j0, j1, j2;
  int leni = in_ptr->length, stepi0 = leni * step0, stepi1 = leni * step1, stepi2 = leni * step2;

  for (pi2 = pin, j2 = count2; --j2 >= 0; pi2 += stepi2) {
    for (pi1 = pi2, j1 = count1; --j1 >= 0; pi1 += stepi1) {
      for (pi0 = pi1, j0 = count0; --j0 >= 0; pi0 += stepi0)
	if (MaskTrue)
	  break;
      if (j0 >= 0)
	*(pout + (pi0 - pin) / leni) = 1;
    }
  }
  return 1;
}

/*---------------------------------------------------------------------------
        F8X-extension transformation to set last true of array or DIM reduced.
                array-logical = LASTLOC(mask, [dim])
*/
int Tdi3LastLoc(struct descriptor *in_ptr,
		struct descriptor *pmask __attribute__ ((unused)),
		struct descriptor *out_ptr,
		int count0, int count1, int count2, int step0, int step1, int step2)
{
  char *pi0, *pi1, *pi2, *pin = in_ptr->pointer, *pout = out_ptr->pointer;
  int j0, j1, j2;
  int leni = in_ptr->length, stepi0 = leni * step0, stepi1 = leni * step1, stepi2 = leni * step2;
  int size = leni * count0 * count1 * count2;

  for (pi2 = pin + size - leni, j2 = count2; --j2 >= 0; pi2 -= stepi2) {
    for (pi1 = pi2, j1 = count1; --j1 >= 0; pi1 -= stepi1) {
      for (pi0 = pi1, j0 = count0; --j0 >= 0; pi0 -= stepi0)
	if (MaskTrue)
	  break;
      if (j0 >= 0)
	*(pout + (pi0 - pin) / leni) = 1;
    }
  }
  return 1;
}

/*---------------------------------------------------------------------------
        F8X-extension transformation to extend a dimension of array or scalar.
                array-rank-n = REPLICATE(array, dim, ncopies)
*/
int Tdi3Replicate(struct descriptor *in_ptr,
		  struct descriptor *pmask,
		  struct descriptor *out_ptr,
		  int count0 __attribute__ ((unused)),
		  int count1 __attribute__ ((unused)),
		  int count2,
		  int step0 __attribute__ ((unused)),
		  int step1 __attribute__ ((unused)),
		  int step2)
{
  char *pin = in_ptr->pointer, *pout = out_ptr->pointer;
  int j0, j1, ncopies = *(int *)pmask;
  int step = in_ptr->length * step2;

  for (j0 = count2; --j0 >= 0; pin += step)
    for (j1 = ncopies; --j1 >= 0; pout += step)
      memcpy(pout, pin, step);
  return 1;
}

/*---------------------------------------------------------------------------
        F90 transformation to add a dimension before one of array or scalar.
                array-rank-(n+1) = SPREAD(array, dim, ncopies)
*/
int Tdi3Spread(struct descriptor *in_ptr,
	       struct descriptor *pmask,
	       struct descriptor *out_ptr,
	       int count0, int count1 __attribute__ ((unused)),
	       int count2, int step0,
	       int step1 __attribute__ ((unused)),
	       int step2 __attribute__ ((unused)))
{
  char *pin = in_ptr->pointer, *pout = out_ptr->pointer;
  int j0, j1, ncopies = *(int *)pmask;
  int step = in_ptr->length * step0;

  for (j0 = count2 * count0; --j0 >= 0; pin += step)
    for (j1 = ncopies; --j1 >= 0; pout += step)
      memcpy(pout, pin, step);
  return 1;
}
