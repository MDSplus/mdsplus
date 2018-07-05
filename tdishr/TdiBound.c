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
/*      Tdi1Bound.C
        Give bound or shape of an array dimension or as whole.
                integer = LBOUND(array, [dim])
                integer = UBOUND(array, [dim])
                integer = SHAPE(array, [dim]) dim is not F90
                scalar number_of_elements = SIZE(array, [dim])
        These routines give the same as above for array, but for valid dimension give that info.
                bound = ELBOUND(signal, [dim])
                bound = EUBOUND(signal, [dim])
                extent = ESHAPE(signal, [dim]) dim is not F90
                scalar volume = ESIZE(signal, [dim])
        Shape of scalar is null vector.
        Result is scalar if dim is given or size is asked, else rank-one vector of length rank(array).
        ESHAPE and ESIZE for dimensions give the extent and thus do not include the both endpoints for integer values.
        We permit, non-F90: Bound of scalar as null vector.

        Ken Klare, LANL P-4     (c)1989,1990,1991,1992
*/
#include <stdlib.h>
#include <mdsdescrip.h>
#include <tdishr_messages.h>
#include <mdsshr.h>
#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefstandard.h"
#include <STATICdef.h>



extern struct descriptor *TdiItoXSpecial;

extern unsigned short
 OpcEshape, OpcEsize, OpcShape, OpcSize, OpcVector;

extern int TdiGetArgs();
extern int TdiGetLong();
extern int TdiIntrinsic();
extern int TdiProduct();
extern int TdiItoX();
extern int Tdi3Subtract();

STATIC_CONSTANT DESCRIPTOR_A(adsc0, sizeof(int), DTYPE_L, 0, 0);
STATIC_CONSTANT unsigned char dtype_l = DTYPE_L;
STATIC_CONSTANT unsigned short size_l = sizeof(int);

int Tdi1Bound(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  array_bounds *pa = 0;
  struct descriptor_xd sig[1], uni[1], dat[1];
  struct TdiCatStruct cats[2];
  int dim, rank = 0;

  status = TdiGetArgs(opcode, 1, list, sig, uni, dat, cats);
  if STATUS_OK {
    pa = (array_bounds *) dat[0].pointer;
    if (pa)
      switch (pa->class) {
      case CLASS_D:
      case CLASS_S:
	if (opcode == OpcShape || opcode == OpcEshape)
	  rank = 0;
	else
	  rank = 1;
	break;
      case CLASS_A:
	rank = pa->aflags.coeff ? pa->dimct : 1;
	break;
      default:
	status = TdiINVCLADSC;
	break;
    } else
      status = TdiNULL_PTR;
  }
  if (narg > 1 && list[1] != 0) {
    if STATUS_OK
      status = TdiGetLong(list[1], &dim);
    if (STATUS_OK && dim >= rank)
      status = TdiBAD_INDEX;
    if STATUS_OK
      status = MdsGet1DxS(&size_l, &dtype_l, out_ptr);
  } else if (opcode == OpcSize) {
    dim = -1;
    if STATUS_OK
      status = MdsGet1DxS(&size_l, &dtype_l, out_ptr);
    if (rank == 0 && STATUS_OK)
      *(int *)out_ptr->pointer->pointer = pa->dtype != DTYPE_MISSING;
  } else {
    array adsc = *(array *) & adsc0;
    dim = -1;
    adsc.arsize = sizeof(int) * rank;
    if STATUS_OK
      status = MdsGet1DxA((struct descriptor_a *)&adsc, &size_l, &dtype_l, out_ptr);
  }
  if (STATUS_OK && rank > 0)
    (*TdiRefFunction[opcode].f3) (pa, dim, out_ptr->pointer->pointer);
  MdsFree1Dx(&dat[0], NULL);
  MdsFree1Dx(&uni[0], NULL);
  MdsFree1Dx(&sig[0], NULL);
  return status;
}

/*--------------------------------------------------------------
        F90 inquiry for declared lower bound(s) of an array.
*/
void Tdi3Lbound(array_bounds * pa, int dim, int *pbound)
{
  int dimct, j;

  if (pa->class != CLASS_A)
    *pbound = 0;
  else if (pa->aflags.coeff) {
    dimct = pa->dimct;
    if (pa->aflags.bounds)
      if (dim < 0)
	for (j = 0; j < dimct; ++j)
	  *pbound++ = pa->m[dimct + 2 * j];
      else
	*pbound = pa->m[dimct + 2 * dim];
    else if (dim < 0)
      for (j = 0; j < dimct; ++j)
	*pbound++ = 0;
    else
      *pbound = 0;
  } else
    *pbound = 0;
  return;
}

/*--------------------------------------------------------------
        F90 inquiry for declared shape of an array.
*/
void Tdi3Shape(array_bounds * pa, int dim, int *pbound)
{
  int dimct, j;

  if (pa->class != CLASS_A)
    *pbound = -1;
  else if (pa->aflags.coeff) {
    dimct = pa->dimct;
    if (pa->aflags.bounds)
      if (dim < 0)
	for (j = 0; j < dimct; ++j)
	  *pbound++ = pa->m[dimct + 2 * j + 1]
	      - pa->m[dimct + 2 * j] + 1;
      else
	*pbound = pa->m[dimct + 2 * dim + 1]
	    - pa->m[dimct + 2 * dim] + 1;
    else if (dim < 0)
      for (j = 0; j < dimct; ++j)
	*pbound++ = pa->m[j];
    else
      *pbound = pa->m[dim];
  } else
    *pbound = (int)pa->arsize / (int)pa->length;
  return;
}

/*--------------------------------------------------------------
        F90 inquiry for the size of one axis or the total of all axes.
*/
int Tdi3Size(array_bounds * pa, int dim, int *pbound)
{
  int dimct, j;

  if (pa->class != CLASS_A)
    *pbound = pa->dtype == DTYPE_MISSING ? 0 : 1;
  else if (pa->aflags.coeff) {
    *pbound = 1;
    dimct = pa->dimct;
    if (pa->aflags.bounds) {
      if (dim < 0)
	for (j = 0; j < dimct; ++j)
	  *pbound *= pa->m[dimct + 2 * j + 1]
	      - pa->m[dimct + 2 * j] + 1;
      else
	*pbound = pa->m[dimct + 2 * dim + 1]
	    - pa->m[dimct + 2 * dim] + 1;
    } else {
      if (dim < 0)
	for (j = 0; j < dimct; ++j)
	  *pbound *= pa->m[j];
      else
	*pbound = pa->m[dim];
    }
  } else
    *pbound = ((int)pa->length > 0) ? (int)pa->arsize / (int)pa->length : 0;
  return 1;
}

/*--------------------------------------------------------------
        F90 inquiry for declared upper bound(s) of an array.
*/
void Tdi3Ubound(array_bounds * pa, int dim, int *pbound)
{
  int dimct, j;

  if (pa->class != CLASS_A)
    *pbound = pa->dtype == DTYPE_MISSING ? -1 : 0;
  else if (pa->aflags.coeff) {
    dimct = pa->dimct;
    if (pa->aflags.bounds)
      if (dim < 0)
	for (j = 0; j < dimct; ++j)
	  *pbound++ = pa->m[dimct + 2 * j + 1];
      else
	*pbound = pa->m[dimct + 2 * dim + 1];
    else if (dim < 0)
      for (j = 0; j < dimct; ++j)
	*pbound++ = pa->m[j] - 1;
    else
      *pbound = pa->m[dim] - 1;
  } else
    *pbound = (int)pa->arsize / (int)pa->length - 1;
  return;
}

/***************************************************************
        Non-F90 inquiry for Effective bounds.
*/
int Tdi1Ebound(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  array_bounds *pa = 0;
  struct descriptor_xd sig[1], uni[1], dat[1];
  struct TdiCatStruct cats[2];
  struct descriptor_xd outs[MAXDIM];
  struct descriptor *new[MAXDIM];
  unsigned int dim, rank = 0;

  status = TdiGetArgs(opcode, 1, list, sig, uni, dat, cats);
  if STATUS_OK {
    pa = (array_bounds *) dat[0].pointer;
    if (pa)
      switch (pa->class) {
      case CLASS_D:
      case CLASS_S:
	rank = 1;
	break;
      case CLASS_A:
	rank = pa->aflags.coeff ? pa->dimct : 1;
	break;
      default:
	status = TdiINVCLADSC;
	break;
    } else
      status = TdiNULL_PTR;
  }
  if (narg > 1 && list[1] != 0) {
    if STATUS_OK
      status = TdiGetLong(list[1], &dim);
    if (STATUS_OK && dim >= rank)
      status = TdiBAD_INDEX;
    if STATUS_OK
      status = (*TdiRefFunction[opcode].f3) (sig[0].pointer, pa, dim, out_ptr);
  } else {
    if (STATUS_OK && rank > MAXDIM)
      status = TdiNDIM_OVER;
    else {
      for (dim = 0; STATUS_OK && dim < rank; ++dim) {
	outs[dim] = EMPTY_XD;
	status = (*TdiRefFunction[opcode].f3) (sig[0].pointer, pa, dim, &outs[dim]);
	new[dim] = (struct descriptor *)&outs[dim];
      }
      if STATUS_OK
	status = TdiIntrinsic(OpcVector, rank, new, out_ptr);
      if (opcode == OpcEsize && STATUS_OK)
	status = TdiProduct(out_ptr, out_ptr MDS_END_ARG);
      for (; (int)--dim >= 0;)
	MdsFree1Dx(&outs[dim], NULL);
    }
  }
  MdsFree1Dx(&dat[0], NULL);
  MdsFree1Dx(&uni[0], NULL);
  MdsFree1Dx(&sig[0], NULL);
  return status;
}

/*--------------------------------------------------------------
        Effective lower bound.
*/
int Tdi3Elbound(struct descriptor_signal *psig,
		array_bounds * pa, int dim, struct descriptor_xd *pout)
{
  INIT_STATUS;
  if (psig && dim < psig->ndesc - 2 && psig->dimensions[dim]) {
    status = TdiItoX(psig->dimensions[dim], TdiItoXSpecial, pout MDS_END_ARG);
    if STATUS_OK
      pout->pointer->class = CLASS_S;
  } else {
    status = MdsGet1DxS(&size_l, &dtype_l, pout);
    if STATUS_OK
      Tdi3Lbound(pa, dim, (int *)pout->pointer->pointer);
  }
  return status;
}

/*--------------------------------------------------------------
        Effective upper bound.
*/
int Tdi3Eubound(struct descriptor_signal *psig,
		array_bounds * pa, int dim, struct descriptor_xd *pout)
{
  INIT_STATUS;
  if (psig && dim < psig->ndesc - 2 && psig->dimensions[dim]) {
    status = TdiItoX(psig->dimensions[dim], TdiItoXSpecial, pout MDS_END_ARG);
    if STATUS_OK {
      pout->pointer->class = CLASS_S;
      pout->pointer->pointer += pout->pointer->length;
    }
  } else {
    status = MdsGet1DxS(&size_l, &dtype_l, pout);
    if STATUS_OK
      Tdi3Ubound(pa, dim, (int *)pout->pointer->pointer);
  }
  return status;
}

/*--------------------------------------------------------------
        Effective shape.
*/
int Tdi3Eshape(struct descriptor_signal *psig,
	       array_bounds * pa, int dim, struct descriptor_xd *pout)
{
  INIT_STATUS;
  struct descriptor hold;
  if (psig && dim < psig->ndesc - 2 && psig->dimensions[dim]) {
    status = TdiItoX(psig->dimensions[dim], TdiItoXSpecial, pout MDS_END_ARG);
    if STATUS_OK {
      pout->pointer->class = CLASS_S;
      hold = *pout->pointer;
      hold.pointer += hold.length;
      status = Tdi3Subtract(&hold, pout->pointer, pout->pointer);
    }
  } else {
    status = MdsGet1DxS(&size_l, &dtype_l, pout);
    if STATUS_OK
      Tdi3Shape(pa, dim, (int *)pout->pointer->pointer);
  }
  return status;
}

/*--------------------------------------------------------------
        Effective size.
*/
int Tdi3Esize(struct descriptor_signal *psig,
	      array_bounds * pa, int dim, struct descriptor_xd *pout)
{
  return Tdi3Eshape(psig, pa, dim, pout);
}
