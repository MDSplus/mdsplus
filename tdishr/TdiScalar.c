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
/*      Tdi1Scalar.C
        Common routine for operations that collapse the input shape and lose signality.
        1 argument: (2$ONE 2$ANY) (ALLOCATED) BIT_SIZE DIGITS EPSILON HUGE LEN MAXEXPONENT MINEXPONENT
                RADIX PRECISION RANGE RANK SELECTED_INT_KIND SELECTED_REAL_KIND TINY
        2 arguments: (2$ADD) DOT_PRODUCT.
                out = name(in1, ...)

        Ken Klare, LANL P-4     (c)1989,1990,1991
*/

#define _MOVC3(a,b,c) memcpy(c,b,a)
extern unsigned short OpcDotProduct;
#include <mdsplus/mdsplus.h>
#include <STATICdef.h>
#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefstandard.h"
#include "tdinelements.h"
#include <tdishr_messages.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <string.h>



extern int Tdi2Keep();
extern int TdiCvtArgs();
extern int TdiGetArgs();
extern int TdiGetShape();
extern int TdiGetLong();
extern int Tdi3Conjg();
extern int Tdi3Multiply();
extern int TdiSum();
extern int TdiMasterData();
extern int TdiDigits();
extern int TdiPower();

#define _factor ((float).30103)

int Tdi1Scalar(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd sig[2], uni[2], dat[2];
  struct TdiCatStruct cats[3];
  struct TdiFunctionStruct *fun_ptr = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
  int cmode = 0, j, (*routine) () = fun_ptr->f3;

  dat[1].pointer = 0;

	/******************************************
        Fetch signals and data and data's category.
        ******************************************/
  status = TdiGetArgs(opcode, narg, list, sig, uni, dat, cats);

	/******************************************
        Adjust category needed to match data types.
        ******************************************/
  if STATUS_OK
    status = (*fun_ptr->f2) (narg, uni, dat, cats, &routine);

	/******************************
        Do the needed type conversions.
        ******************************/
  if STATUS_OK
    status = TdiCvtArgs(narg, dat, cats);

	/******************
        Find correct shape.
        ******************/
  if STATUS_OK
    status = TdiGetShape(0, dat, 0, cats[narg].out_dtype, &cmode, out_ptr);

	/*****************************
        Act on data, linear arguments.
        *****************************/
  if STATUS_OK
    switch (fun_ptr->m2) {
    case 1:
      status = (*routine) (dat[0].pointer, out_ptr->pointer);
      break;
    case 2:
      status = (*routine) (dat[0].pointer, dat[1].pointer, out_ptr->pointer);
      break;
    }

  if (STATUS_OK && (opcode == OpcDotProduct || fun_ptr->f2 == Tdi2Keep))
    status = TdiMasterData(0, sig, uni, &cmode, out_ptr);

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

/*---------------------------------------------------
        F90 inquiry for the number of bits in an element.
        F90 restricted to integers and returns same type.
                integer = BIT_SIZE(expression)

        NEED handling of bit and packed types.
*/
int Tdi3BitSize(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;

  switch (in_ptr->class) {
  case CLASS_A:
  case CLASS_S:
  case CLASS_D:
    *(int *)out_ptr->pointer = in_ptr->length * 8;
    break;
  default:
    status = TdiINVCLADSC;
    break;
  }
  return status;
}

/*---------------------------------------------------
        F90 inquiry for number of (binary) digits in
        integer model: sign * number or
        real model: sign * base**exponent * fraction.
        This is computer-dependent. VAX base=2.
        DIGITS(3.0) is 24, DIGITS(4) 31.
                integer = DIGITS(number)
*/
int Tdi3Digits(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  int status = MDSplusSUCCESS, n;

  switch (in_ptr->dtype) {
  default:
    n = 0;
    status = TdiINVDTYDSC;
    break;
  case DTYPE_B:
  case DTYPE_W:
  case DTYPE_L:
  case DTYPE_Q:
  case DTYPE_O:
    n = 8 * in_ptr->length - 1;
    break;
  case DTYPE_BU:
  case DTYPE_WU:
  case DTYPE_LU:
  case DTYPE_QU:
  case DTYPE_OU:
    n = 8 * in_ptr->length;
    break;
  case DTYPE_FC:
  case DTYPE_F:
    n = 24;
    break;
  case DTYPE_FSC:
  case DTYPE_FS:
    n = 24;
    break;
  case DTYPE_DC:
  case DTYPE_D:
    n = 56;
    break;
  case DTYPE_GC:
  case DTYPE_G:
    n = 53;
    break;
  case DTYPE_FTC:
  case DTYPE_FT:
    n = 53;
    break;
  }
  *(int *)out_ptr->pointer = n;
  return status;
}

/*---------------------------------------------------
        F90 inquiry for number almost negligible w.r.t. 1.0.
        real model: sign * base**exponent * fraction.
        This is computer-dependent. VAX base=2.
        EPSILON(3.0) is 2**(-23), EPSILON(4g0) 2**(-52).
                real = EPSILON(real-model)
*/

int Tdi3Epsilon(struct descriptor *x_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  double digits_d;
  struct descriptor digits = { sizeof(digits_d), DTYPE_NATIVE_DOUBLE, CLASS_S, 0 };
  digits.pointer = (char *)&digits_d;
  status = TdiDigits(x_ptr, &digits MDS_END_ARG);
  if STATUS_OK {
    STATIC_CONSTANT double two_d = 2.;
    STATIC_CONSTANT struct descriptor two =
	{ sizeof(two_d), DTYPE_NATIVE_DOUBLE, CLASS_S, (char *)&two_d };
    digits_d = 1. - digits_d;
    status = TdiPower(&two, &digits, out_ptr MDS_END_ARG);
  }
  return status;
}

/*---------------------------------------------------
        F90 inquiry for largest positive number in model: sign * base**exponent * fraction.
        This is computer-dependent. VAX base=2.
        HUGE(3.0) is 2**(+127)*(1-2**(-24)).
                real = HUGE(real)
*/
STATIC_CONSTANT const unsigned int BU_HUGE[] = { 0xff };
STATIC_CONSTANT const unsigned int B_HUGE[] = { 0x7f };
STATIC_CONSTANT const unsigned int WU_HUGE[] = { 0xffff };
STATIC_CONSTANT const unsigned int W_HUGE[] = { 0x7fff };
STATIC_CONSTANT const unsigned int LU_HUGE[] = { 0xffffffff };
STATIC_CONSTANT const unsigned int L_HUGE[] = { 0x7fffffff };
STATIC_CONSTANT const unsigned int QU_HUGE[] = { 0xffffffff, 0xffffffff };
STATIC_CONSTANT const unsigned int Q_HUGE[] = { 0xffffffff, 0x7fffffff };
STATIC_CONSTANT const unsigned int OU_HUGE[] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
STATIC_CONSTANT const unsigned int O_HUGE[] = { 0xffffffff, 0xffffffff, 0xffffffff, 0x7fffffff };
STATIC_CONSTANT const unsigned int F_HUGE[] = { 0xFFFF7FFF };
STATIC_CONSTANT const unsigned int FC_HUGE[] = { 0xFFFF7FFF };
STATIC_CONSTANT const unsigned int D_HUGE[] = { 0xFFFF7FFF, 0xFFFFFFFF };
STATIC_CONSTANT const unsigned int DC_HUGE[] = { 0xFFFF7FFF, 0xFFFFFFFF };
STATIC_CONSTANT const unsigned int G_HUGE[] = { 0xFFFF7FFF, 0xFFFFFFFF };
STATIC_CONSTANT const unsigned int GC_HUGE[] = { 0xFFFF7FFF, 0xFFFFFFFF };
STATIC_CONSTANT const unsigned int FS_HUGE[] = { 0x7F7FFFFF };
STATIC_CONSTANT const unsigned int FSC_HUGE[] = { 0x7F7FFFFF };
STATIC_CONSTANT const unsigned int FT_HUGE[] = { 0xFFFFFFFF, 0x7FF7FFFF };
STATIC_CONSTANT const unsigned int FTC_HUGE[] = { 0xFFFFFFFF, 0x7FF7FFFF };

#define CASE(dtype) case DTYPE_##dtype : memcpy(out_ptr->pointer,dtype##_HUGE,sizeof(dtype##_HUGE)); break;

int Tdi3Huge(struct descriptor *x_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;

  switch (x_ptr->dtype) {
  default:
    status = TdiINVDTYDSC;
    break;
    CASE(BU)
	CASE(B)
	CASE(WU)
	CASE(W)
	CASE(LU)
	CASE(L)
	CASE(Q)
	CASE(QU)
	CASE(O)
	CASE(OU)
	CASE(F)
	CASE(FC)
	CASE(D)
	CASE(DC)
	CASE(G)
	CASE(GC)
	CASE(FS)
	CASE(FSC)
	CASE(FT)
	CASE(FTC)
  }
  return status;
}

/*---------------------------------------------------
        F90 inquiry for length of one element in bytes.
        F90 restricted to strings.
                integer = LEN(expression)

        NEED handling of bit and packed types.
*/
int Tdi3Len(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;

  switch (in_ptr->class) {
  case CLASS_A:
  case CLASS_S:
  case CLASS_D:
    *(int *)out_ptr->pointer = in_ptr->length;
    break;
  default:
    status = TdiINVCLADSC;
    break;
  }
  return status;
}

/*---------------------------------------------------
        F90 inquiry for maximum exponent in real model: sign * base**exponent * fraction.
        This is computer-dependent. VAX base=2.
        MAXEXPONENT(3.0) is +127, MAXEXPONENT(4g0) +1023.
                integer = MAXEXPONENT(real)
*/
int Tdi3MaxExponent(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  int status = MDSplusSUCCESS, n;

  switch (in_ptr->dtype) {
  default:
    n = 0;
    status = TdiINVDTYDSC;
    break;
  case DTYPE_FS:
  case DTYPE_FSC:
  case DTYPE_FC:
  case DTYPE_F:
    n = 127;
    break;
  case DTYPE_DC:
  case DTYPE_D:
    n = 127;
    break;
  case DTYPE_FT:
  case DTYPE_FTC:
  case DTYPE_GC:
  case DTYPE_G:
    n = 1023;
    break;
  }
  *(int *)out_ptr->pointer = n;
  return status;
}

/*---------------------------------------------------
        F90 inquiry for minimum exponent in real model: sign * base**exponent * fraction.
        This is computer-dependent. VAX base=2.
        MINEXPONENT(3.0) is -127, MINEXPONENT(4g0) -1023.
                integer = MINEXPONENT(real)
*/
int Tdi3MinExponent(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  int status = MDSplusSUCCESS, n;

  switch (in_ptr->dtype) {
  default:
    n = 0;
    status = TdiINVDTYDSC;
    break;
  case DTYPE_FS:
  case DTYPE_FSC:
  case DTYPE_FC:
  case DTYPE_F:
    n = -127;
    break;
  case DTYPE_DC:
  case DTYPE_D:
    n = -127;
    break;
  case DTYPE_FT:
  case DTYPE_FTC:
  case DTYPE_GC:
  case DTYPE_G:
    n = -1023;
    break;
  }
  *(int *)out_ptr->pointer = n;
  return status;
}

/*---------------------------------------------------
        F90 inquiry for number of decimal digits in real model: sign * base**exponent * fraction.
                INT((digits-1)*LOG10(base) + k (k=1 for base = power of 10)
        This is computer-dependent. VAX base=2.
        PRECISION(3.0) is 6, PRECISION(4g0) 15.
                integer = PRECISION(real)
*/
int Tdi3Precision(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  int status = MDSplusSUCCESS, n;

  switch (in_ptr->dtype) {
  default:
    n = 0;
    status = TdiINVDTYDSC;
    break;
  case DTYPE_FS:
  case DTYPE_FSC:
  case DTYPE_FC:
  case DTYPE_F:
    n = (int)(_factor * 23);
    break;
  case DTYPE_DC:
  case DTYPE_D:
    n = (int)(_factor * 55);
    break;
  case DTYPE_FT:
  case DTYPE_FTC:
  case DTYPE_GC:
  case DTYPE_G:
    n = (int)(_factor * 52);
    break;
  }
  *(int *)out_ptr->pointer = n;
  return status;
}

/*---------------------------------------------------
        F90 inquiry for radix or base of
        integer model: sign * number or
        real model: sign * base**exponent * fraction.
        This is computer-dependent. VAX base=2.
        RADIX(3.0) is 2, RADIX(4) is 2.
                integer = RADIX(number)
*/
int Tdi3Radix(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int n;

  switch (in_ptr->dtype) {
  default:
    n = 0;
    status = TdiINVDTYDSC;
    break;
  case DTYPE_B:
  case DTYPE_W:
  case DTYPE_L:
  case DTYPE_Q:
  case DTYPE_O:
  case DTYPE_BU:
  case DTYPE_WU:
  case DTYPE_LU:
  case DTYPE_QU:
  case DTYPE_OU:
  case DTYPE_FC:
  case DTYPE_F:
  case DTYPE_DC:
  case DTYPE_D:
  case DTYPE_GC:
  case DTYPE_G:
  case DTYPE_FS:
  case DTYPE_FT:
  case DTYPE_FSC:
  case DTYPE_FTC:
    n = 2;
  }
  *(int *)out_ptr->pointer = n;
  return status;
}

/*---------------------------------------------------
        F90 inquiry for exponent range in real model: sign * base**exponent * fraction.
                INT(MIN(LOG10(huge),-LOG10(tiny))
        This is computer-dependent. VAX base=2. VAX huge < 1/tiny.
        RANGE(3.0) is 38, RANGE(4g0) 307.
                integer = RANGE(real)
*/
int Tdi3Range(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  int status = MDSplusSUCCESS, n;

  switch (in_ptr->dtype) {
  default:
    n = 0;
    status = TdiINVDTYDSC;
    break;
  case DTYPE_FS:
  case DTYPE_FSC:
  case DTYPE_FC:
  case DTYPE_F:
    n = (int)(_factor * 127);
    break;
  case DTYPE_DC:
  case DTYPE_D:
    n = (int)(_factor * 127);
    break;
  case DTYPE_FT:
  case DTYPE_FTC:
  case DTYPE_GC:
  case DTYPE_G:
    n = (int)(_factor * 1023);
    break;
  }
  *(int *)out_ptr->pointer = n;
  return status;
}

/*---------------------------------------------------
        F90 extension inquiry for declared number of dimensions of an array.
        Returns zero for a scalar.
*/
int Tdi3Rank(struct descriptor_a *arr_ptr, struct descriptor *out_ptr)
{
  int n;

  if (arr_ptr->class == CLASS_A)
    n = arr_ptr->aflags.coeff ? arr_ptr->dimct : 1;
  else if (arr_ptr->dtype == DTYPE_MISSING)
    n = -1;
  else
    n = 0;
  *(int *)out_ptr->pointer = n;
  return 1;
}

/*---------------------------------------------------
        Return kind (dtype) of smallest integer with sufficient range.
*/
int Tdi3SelectedIntKind(struct descriptor *pdr, struct descriptor *pout)
{
  int range, status;
  float frange;

  status = TdiGetLong(pdr, &range);
  if STATUS_OK {
    frange = range / _factor;
    if (frange <= 07)
      range = DTYPE_B;
    else if (frange <= 15)
      range = DTYPE_W;
    else if (frange <= 31)
      range = DTYPE_L;
    else if (frange <= 63)
      range = DTYPE_Q;
    else if (frange <= 127)
      range = DTYPE_O;
    else
      range = -1;
    *(int *)pout->pointer = range;
  }
  return status;
}

/*---------------------------------------------------
        Return kind (dtype) of smallest real with sufficient precision and range.
*/
int Tdi3SelectedRealKind(struct descriptor *pdp, struct descriptor *pdr, struct descriptor *pout)
{
  int prec, range, status;
  float fprec, frange;

  status = TdiGetLong(pdp, &prec);
  if STATUS_OK
    status = TdiGetLong(pdr, &range);
  if STATUS_OK {
    fprec = prec / _factor;
    frange = range / _factor;
    if (fprec <= 23 && frange <= 127)
      range = DTYPE_NATIVE_FLOAT;
    else if (fprec <= 52 && frange <= 1023)
      range = DTYPE_NATIVE_DOUBLE;
    else if (fprec <= 55 && frange <= 127)
      range = DTYPE_D;
    else
      range = 0;
    *(int *)pout->pointer = range;
  }
  return status;
}

/*---------------------------------------------------
        Return length of all data elements in bytes, excluding its descriptor.
                int = SIZEOF(expression)
        NEED handling of bit and packed types.
*/
int Tdi3SizeOf(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int size = 0;

  switch (in_ptr->class) {
  case CLASS_A:
    size = ((struct descriptor_a *)in_ptr)->arsize;
    break;
  case CLASS_S:
  case CLASS_D:
    size = in_ptr->length;
    break;
  default:
    status = TdiINVCLADSC;
    break;
  }
  *(int *)out_ptr->pointer = size;
  return status;
}

/*---------------------------------------------------
        F90 inquiry for smallest positive number in model: sign * base**exponent * fraction.
        This is computer-dependent. VAX base=2.
        TINY(3.0) is 2**(-127), TINY(4G0) is 2**(-1023).
                real = TINY(real)
*/
STATIC_CONSTANT unsigned int F_TINY[] = { 0x80 };
STATIC_CONSTANT unsigned int D_TINY[] = { 0x80, 0 };
STATIC_CONSTANT unsigned int G_TINY[] = { 0x10, 0 };
STATIC_CONSTANT unsigned int FS_TINY[] = { 0x800000 };
STATIC_CONSTANT unsigned int FT_TINY[] = { 0, 0x100000 };

int Tdi3Tiny(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;

  switch (in_ptr->dtype) {
  default:
    status = TdiINVDTYDSC;
    break;
  case DTYPE_F:
  case DTYPE_FC:
    memcpy(out_ptr->pointer, F_TINY, sizeof(F_TINY));
    break;
  case DTYPE_FS:
  case DTYPE_FSC:
    memcpy(out_ptr->pointer, FS_TINY, sizeof(FS_TINY));
    break;
  case DTYPE_D:
  case DTYPE_DC:
    memcpy(out_ptr->pointer, D_TINY, sizeof(D_TINY));
    break;
  case DTYPE_G:
  case DTYPE_GC:
    memcpy(out_ptr->pointer, G_TINY, sizeof(G_TINY));
    break;
  case DTYPE_FT:
  case DTYPE_FTC:
    memcpy(out_ptr->pointer, FT_TINY, sizeof(FT_TINY));
    break;
  }
  return status;
}

/*---------------------------------------------------
        Dot product of two vectors.
*/
int Tdi3DotProduct(struct descriptor_a *in1_ptr,
		   struct descriptor_a *in2_ptr, struct descriptor *out_ptr)
{
  int lsum;			/*need wsum and bsum when supported */
  char *p1 = in1_ptr->pointer;
  int inc1 = in1_ptr->class == CLASS_A ? in1_ptr->length : 0;
  char *p2 = in2_ptr->pointer;
  int inc2 = in2_ptr->class == CLASS_A ? in2_ptr->length : 0;
  int status = MDSplusSUCCESS, n, n2;
  EMPTYXD(tmp);

  N_ELEMENTS(in1_ptr, n);
  N_ELEMENTS(in2_ptr, n2);
  if (n > n2)
    n = n2;
  switch (out_ptr->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    for (lsum = 0; --n >= 0; p1 += inc1, p2 += inc2)
      lsum += *(char *)p1 **(char *)p2;
    *(char *)out_ptr->pointer = (char)lsum;
    break;
  case DTYPE_W:
  case DTYPE_WU:
    for (lsum = 0; --n >= 0; p1 += inc1, p2 += inc2)
      lsum += *(short *)p1 **(short *)p2;
    *(short *)out_ptr->pointer = (short)lsum;
    break;
  case DTYPE_L:
  case DTYPE_LU:
    for (lsum = 0; --n >= 0; p1 += inc1, p2 += inc2)
      lsum += *(int *)p1 **(int *)p2;
    *(int *)out_ptr->pointer = lsum;
    break;
  case DTYPE_FC:
  case DTYPE_DC:
  case DTYPE_GC:
  case DTYPE_FSC:
  case DTYPE_FTC:
    status = Tdi3Conjg(in1_ptr, in1_ptr);
    if (STATUS_NOT_OK)
      break;
    MDS_ATTR_FALLTHROUGH
  default:
    if (inc1) {
      status = Tdi3Multiply(in1_ptr, in2_ptr, in1_ptr);
      if STATUS_OK
	status = TdiSum(in1_ptr, &tmp MDS_END_ARG);
    } else {
      status = Tdi3Multiply(in1_ptr, in2_ptr, in2_ptr);
      if STATUS_OK
	status = TdiSum(in2_ptr, &tmp MDS_END_ARG);
    }
    if STATUS_OK
      _MOVC3(out_ptr->length, tmp.pointer->pointer, out_ptr->pointer);
    break;
  }
  MdsFree1Dx(&tmp, 0);
  return status;
}
