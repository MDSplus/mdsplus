/*	Tdi1Subscript.C
	Select portions of a data array.
		array[subscript...]

	Subscripts may be any DATA-able form:
	*	A null pointer is the full range.
	2	A scalar is a single value. If the last m subscripts are
		scalars the rank of the result is m less.
	4:5	A range is a block of elements.
	*:5	An un-opened range is from the lower bound.
	4:*	An unclosed range is to the upper bound.
	4:4	A single element range is rank 1 but has the same size as a scalar.
	5:4	An improper range is an error (undetected). ///An empty array.///
	6:12:2	A stepped range implies the points at the stepped values.
	[3,2]	A vector is a list of elements. Duplicates are permitted.
	[]	The null vector keep the rank but has zero size.
	[3]	A single element vector keeps rank but has the same size as a scalar.
	other	Expressions are treated by what they produce.

	Insufficient subscripts are treated as *.
	The subscripts for a signal references the independent axes, the dimensions.
	An unstepped range for signals implies all points in the bounded region.
		2:5.5 will be 2.0 to 5.5 seconds or whatever.
	More complex ranges must use $VALUE, e.g.,
		signal[[*:2:$VALUE,5.5:*:$VALUE]] for all but 2 to 5.5.
	Subscripting converts signal dimensions into lists. You get one chance for times.
	The result of subscripting can be subscripted by array position only.

	See also Tdi1Cull.C for culling and extending.
	See also Tdi1ItoX.C for index to axis conversion and vice versa.
	See also Tdi1Map below for index subscripting.

	Ken Klare, LANL P-4	(c)1990,1991
*/
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include "tdinelements.h"
#include <tdimessages.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <string.h>

#define _MOVC3(a,b,c) memcpy(c,b,a)
extern struct descriptor *TdiRANGE_PTRS[3];
extern struct descriptor *TdiSELF_PTR;
extern unsigned short OpcValue;

extern int TdiGetArgs();
extern int TdiMasterData();
extern int TdiIcull();
extern int TdiRecull();
extern int TdiGetShape();
extern int TdiIextend();
extern int TdiLong();
extern int TdiCull();
extern int TdiXtoI();
extern int TdiItoX();
extern int TdiData();
extern int TdiLbound();
extern int TdiUbound();
extern int TdiDtypeRange();
extern int TdiNint();
extern int TdiDataWithUnits();

typedef struct {int x[2];} quadw;

TdiRefStandard(Tdi1Subscript)
static DESCRIPTOR_A_COEFF(coeff0, 1, DTYPE_B, 0, MAXDIM, 1);
register char			*pin, *pout;
int				bounded = 0, cmode = -1, dimct, highest = 0, highdim = 0, row;
int                      	dim;
register int			j, len;
int				stride[MAXDIM+1], *px[MAXDIM], count[MAXDIM];
struct descriptor_signal	*psig;
struct descriptor_dimension *pdim;
struct descriptor		*keeps = TdiSELF_PTR;
array_coeff			*pdat, *pdi=0;
array_coeff			arr = *(array_coeff *)&coeff0;
struct descriptor	        ddim = {sizeof(dim),DTYPE_L,CLASS_S,0};
struct descriptor_xd	ii[MAXDIM], xx[MAXDIM];
struct descriptor_xd	sig[1], uni[1], dat[1];
struct TdiCatStruct		cats[2];
        ddim.pointer = (char *)&dim;
	status = TdiGetArgs(opcode, 1, list, sig, uni, dat, cats);
	if (!(status & 1)) goto baddat;
	psig = (struct descriptor_signal *)sig[0].pointer;
	pdat = (array_coeff *)dat[0].pointer;
	stride[0] = len = pdat->length;

	/****************************
	Set up first element address.
	****************************/
	pin = pdat->pointer;
	switch (pdat->class) {
	case CLASS_S :
	case CLASS_D :
		dimct = 1;
		break;
	case CLASS_A :
		if ((bounded = pdat->aflags.bounds) == 1) pin = pdat->a0;
		if ((dimct = pdat->dimct) > MAXDIM) {status = TdiNDIM_OVER; goto baddat;}
		else if (pdat->aflags.coeff)
			for (j = 0; j < dimct; ++j) stride[j+1] = stride[j] * pdat->m[j];
		else	stride[1] = pdat->arsize;
		break;
	default : status = TdiINVCLADSC; goto baddat;
	}
	if (narg-1 > dimct || (psig && psig->ndesc-2 > dimct)) {status = TdiINV_SIZE; goto baddat;}

	/***************************************
	Signals are indexed by their dimensions.
	Arrays are indexed by integers.
	Open ranges in CULL. Remove units.
	(1) * range.
	(2) dimension is candidate.
	(3) just the data.
	***************************************/
	for (dim = 0; dim < dimct; ++dim) {
		ii[dim] = EMPTY_XD;
		xx[dim] = EMPTY_XD;
		if (status & 1) {
			pdim = (struct descriptor_dimension *)psig;
			if (psig && dim < psig->ndesc-2 && 
                           (pdim = (struct descriptor_dimension *)psig->dimensions[dim]) != 0) {
				TdiSELF_PTR = (struct descriptor *)psig;
				status = TdiCull(psig, &ddim, dim+1 < narg ? list[dim+1] : 0, &xx[dim] MDS_END_ARG);
				if (status & 1) status = TdiXtoI(pdim, xx[dim].pointer, &ii[dim] MDS_END_ARG);
				if (status & 1) status = TdiItoX(pdim, ii[dim].pointer, &xx[dim] MDS_END_ARG);
				TdiSELF_PTR = keeps;
				if (status & 1 && bounded) pin += pdat->m[dim*2+dimct] * stride[dim];
				highdim = dim + 1;
			}
			else if (pdim && pdim->dtype == DTYPE_DIMENSION && dim+1 < narg && list[dim+1]) {
				status = TdiCull(pdat, &ddim, list[dim+1], &xx[dim] MDS_END_ARG);
				if (status & 1) status = TdiData(xx[dim].pointer, &ii[dim] MDS_END_ARG);
			}
			else {
			int left, right;
			struct descriptor dleft = {sizeof(int),DTYPE_L, CLASS_S,0};
			struct descriptor dright = {sizeof(int),DTYPE_L, CLASS_S,0};
                                dleft.pointer = (char *)&left;
                                dright.pointer = (char *)&right;
				status = TdiLbound(pdat, &ddim, &dleft MDS_END_ARG);
				if (status & 1) status = TdiUbound(pdat, &ddim, &dright MDS_END_ARG);
				if (dim+1 < narg && list[dim+1]) {
				struct descriptor *keep[3];
                                        keep[0] = TdiRANGE_PTRS[0];
                                        keep[1] = TdiRANGE_PTRS[1];
                                        keep[2] = TdiRANGE_PTRS[2];
					TdiRANGE_PTRS[0] = (struct descriptor *)&dleft;
					TdiRANGE_PTRS[1] = (struct descriptor *)&dright;
					TdiRANGE_PTRS[2] = 0;
					if (status & 1) status = TdiData(list[dim+1], &ii[dim] MDS_END_ARG);
					TdiRANGE_PTRS[0] = keep[0];
					TdiRANGE_PTRS[1] = keep[1];
					TdiRANGE_PTRS[2] = keep[2];
					if (status & 1) status = TdiLong(&ii[dim], &ii[dim] MDS_END_ARG);
					if (status & 1) status = TdiIcull(left, right, ii[dim].pointer);
					if (status == -1) status = TdiRecull(&ii[dim]);
				}
				else	if (status & 1) status = TdiDtypeRange(&dleft, &dright, &ii[dim] MDS_END_ARG);
			}
		}
		if (status & 1 && ii[dim].pointer->class == CLASS_R) 
                  status = TdiData(&ii[dim], &ii[dim] MDS_END_ARG);
		if (status & 1 && ii[dim].pointer->dtype != DTYPE_L) 
                  status = TdiNint(&ii[dim], &ii[dim] MDS_END_ARG);
		if (status & 1 && (pdi = (array_coeff *)ii[dim].pointer) != 0) {
			if (pdi->class == CLASS_A) highest = dim + 1;
			N_ELEMENTS(pdi, arr.m[dim]);
		}
		if (status & 1 && (arr.arsize *= arr.m[dim]))
                   pin += stride[dim] * *(px[dim] = (int *)pdi->pointer);
	}
	if (!(status & 1)) goto badsub;
	/********************************************
	We know the shape and type, so get the space.
	Result is scalar if all indices are scalar.
	Remove top dimensions with scalar indices.
	Copy the selected values.
	********************************************/
	if (psig && psig->ndesc > highdim + 2) psig->ndesc = (unsigned char)(highdim + 2);
	if (psig) for (j = psig->ndesc; --j >= 0;) psig->dimensions[j] = xx[j].pointer;
	if (highest <= 0) {
                unsigned short llen = (unsigned short)0;
                unsigned char dtype = (unsigned char)DTYPE_MISSING;
		if (arr.arsize) status = MdsGet1DxS(&pdat->length, &pdat->dtype, out_ptr);
		else status = MdsGet1DxS(&llen, &dtype, out_ptr);
		if (status & 1) _MOVC3(len, pin, out_ptr->pointer->pointer);
	}
	else {
		arr.dimct = (unsigned char)highest;
		arr.aflags.coeff = (unsigned char)(highest > 1);
		status = MdsGet1DxA((struct descriptor_a *)&arr, &pdat->length, &pdat->dtype, out_ptr);
		if (!(status & 1)) goto badsub;
		if (arr.arsize == 0) goto empty;
		pout = out_ptr->pointer->pointer;
		/*****************************************
		This is the way you can do multiple loops.
		For efficiency, we write out inner loop.
		Subtract the offset of innermost loop.
		*****************************************/
		for (j = highest; --j > 0;) count[j] = 0;
		pin -= len * *px[0];
		row = arr.m[0] * sizeof(int);
inner:		for (j = 0; j < row; j += sizeof(int))
			_MOVC3(len, pin + len * *(int *)((char *)px[0]+j), pout), pout += len;
		/******************************************************
		Find the index to increment, reset those that overflow.
		Must avoid reading beyond end of vector.
		******************************************************/
		for (j = 0; ++j < highest;) {
			if (++count[j] < arr.m[j]) {
				pin += stride[j] * (*(px[j]+count[j]) - *(px[j]+count[j]-1));
				goto inner;
			}
			pin += stride[j] * (*px[j] - *(px[j] + count[j] - 1));
			count[j] = 0;
		}
	}
	/************************
	Dimensions of new signal.
	************************/
empty:
	if (status & 1) status = TdiMasterData(1, sig, uni, &cmode, out_ptr);

badsub:
	for (j = dimct; --j >= 0;) {
		MdsFree1Dx(&ii[j], NULL);
		MdsFree1Dx(&xx[j], NULL);
	}
baddat:
	MdsFree1Dx(&sig[0], NULL);
	MdsFree1Dx(&uni[0], NULL);
	MdsFree1Dx(&dat[0], NULL);
	return status;
}

/*----------------------------------------------------------------------------
	Integer subscripts of an array may be taken with MAP(A,B).
	MAP gives shape and signality of B and data and units from A.
	B may include open ranges, vectors, or almost anything.
	Each B value out of range uses extreme values of A.
	A is treated as a linear array and subscripting is to nearest integer.
*/
TdiRefStandard(Tdi1Map)
struct descriptor_xd	dwu = EMPTY_XD, sig[1], uni[1], dat[1];
struct TdiCatStruct		cats[2];
struct descriptor_with_units *pwu;
int				cmode = -1, n=0, len;
int			 left, right;
struct descriptor		*keep[3];
struct descriptor		range[2] = {
	{sizeof(int),DTYPE_L,CLASS_S,0},
	{sizeof(int),DTYPE_L,CLASS_S,0}};
array_coeff			*pa;
char				*abase, *po=0;
int				*pindex=0;
        keep[0] = TdiRANGE_PTRS[0];
        keep[1] = TdiRANGE_PTRS[1];
        keep[2] = TdiRANGE_PTRS[2];
        range[0].pointer = (char *)&left;
        range[1].pointer = (char *)&right;
	/*************************
	Get array A and its units.
	*************************/
	status = TdiDataWithUnits(list[0], &dwu MDS_END_ARG);
	if (!(status & 1)) goto bad;
	pwu = (struct descriptor_with_units *)dwu.pointer;
	pa = (array_coeff *)pwu->data;
	len = pa->length;
	abase = pa->pointer;
	left = 0;
	switch (pa->class) {
	case CLASS_A :
		if (pa->aflags.bounds) {
			abase = pa->a0;
			left = (abase - pa->pointer) / len;
		}
		break;
	}
	N_ELEMENTS(pa, right);
	right += left - 1;

	/********************************************
	Get subscript expression B and its signality.
	Must have default ranges defined before get.
	********************************************/
	TdiRANGE_PTRS[0] = &range[0];
	TdiRANGE_PTRS[1] = &range[1];
	TdiRANGE_PTRS[2] = 0;
	if (status & 1) status = TdiGetArgs(opcode, 1, &list[1], sig, uni, dat, cats);
	TdiRANGE_PTRS[0] = keep[0];
	TdiRANGE_PTRS[1] = keep[1];
	TdiRANGE_PTRS[2] = keep[2];
	if (uni[0].pointer) MdsFree1Dx(&uni[0], NULL);
	uni[0].pointer = (struct descriptor *)pwu->units;

	if (status & 1) switch (dat[0].pointer->dtype) {
	case DTYPE_MISSING :
		MdsFree1Dx(out_ptr, NULL);
		goto none;
	case DTYPE_L : case DTYPE_LU :
		break;
	case DTYPE_B : case DTYPE_BU :
	case DTYPE_W : case DTYPE_WU :
	case DTYPE_Q : case DTYPE_QU :
	case DTYPE_O : case DTYPE_OU :
		status = TdiLong(&dat[0], &dat[0] MDS_END_ARG);
		break;
	default :
		status = TdiNint(&dat[0], &dat[0] MDS_END_ARG);
		if (status & 1 && dat[0].pointer->dtype != DTYPE_L) 
		   status = TdiLong(&dat[0], &dat[0] MDS_END_ARG);
		break;
	}

	/********************************
	Shape of B with data type from A.
	********************************/
	if (status & 1) status = TdiGetShape(1, &dat[0], len, pa->dtype, &cmode, out_ptr);
	if (status & 1) {
		po = out_ptr->pointer->pointer;
		status = TdiIextend(left, right, dat[0].pointer);
		pindex = (int *)dat[0].pointer->pointer;
		if (status & 1) N_ELEMENTS(dat[0].pointer, n);
	}
	if (status & 1)
        {
  	  char *cptr = (char *)po;
          short *sptr = (short *)po;
          int *iptr = (int *)po;
          quadw *qptr = (quadw *)po;
          switch (len) {
	    case 0 :	break;
	    case 1 :	while (--n >= 0) {*cptr++ = *((char *)abase + *pindex++);} break;
	    case 2 :	while (--n >= 0) {*sptr++ = *((short *)abase + *pindex++);} break;
	    case 4 :	while (--n >= 0) {*iptr++ = *((int *)abase + *pindex++);} break;
	    case 8 :	while (--n >= 0) {*qptr++ = *((quadw *)abase + *pindex++);} break;
	    default :	while (--n >= 0) {_MOVC3(len, (char *)abase + (len * *pindex++), po); po += len;} break;
	  }
        }
none:
	if (status & 1) status = TdiMasterData(1, sig, uni, &cmode, out_ptr);
	if (sig[0].pointer) MdsFree1Dx(&sig[0], NULL);
	MdsFree1Dx(&dat[0], NULL);
bad:
	MdsFree1Dx(&dwu, NULL);
	return status;
}
