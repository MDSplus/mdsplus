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
/*      Tdi3Exponent.C
        EXPONENT FIX_ROPRAND FRACTION SCALE SET_EXPONENT
        Floating point elemental operations.

        Ken Klare, LANL P-4     (c)1989,1990,1991
*/
#include <mdsdescrip.h>
#include <mdsdescrip.h>
#include "tdinelements.h"
#include <tdishr_messages.h>
#include <STATICdef.h>



extern int IsRoprand();

#define F_FLOAT_EXPONENT(val) (((*(int *)val) >> 7) & 0xFF)
#define F_FLOAT_SET_EXPONENT(val,exp) (*(int *)val = (*(int *)val & ~(0xff << 7)) | (((exp) & 0xff) << 7))
#define F_FLOAT_BIAS 0x80
#define F_FLOAT_PREC 23
#define FS_FLOAT_EXPONENT(val) (((*(int *)val) >> 23) & 0xFF)
#define FS_FLOAT_SET_EXPONENT(val,exp) (*(int *)val = (*(int *)val & ~(0xff << 23)) | (((exp) & 0xff) << 23))
#define FS_FLOAT_BIAS 0x80
#define FS_FLOAT_PREC 23
#define G_FLOAT_EXPONENT(val) (((*(int *)val) >> 4) & 0x7FF)
#define G_FLOAT_SET_EXPONENT(val,exp) (*(int *)val = (*(int *)val & ~(0x7ff << 4)) | (((exp) & 0x7ff) << 4))
#define G_FLOAT_BIAS 0x400
#define G_FLOAT_PREC 52
#define D_FLOAT_EXPONENT(val) (((*(int *)val) >> 7) & 0xFF)
#define D_FLOAT_SET_EXPONENT(val,exp) (*(int *)val = (*(int *)val & ~(0xff << 7)) | (((exp) & 0xff) << 7))
#define D_FLOAT_BIAS 0x80
#define D_FLOAT_PREC 55
#define FT_FLOAT_EXPONENT(val) ((((int *)val)[1] >> 20) & 0x7FF)
#define FT_FLOAT_SET_EXPONENT(val,exp) (((int *)val)[1] = (((int *)val)[1] & ~(0x7ff << 20)) | (((exp) & 0x7ff) << 20))
#define FT_FLOAT_BIAS 0x400
#define FT_FLOAT_PREC 52

#define compute_exponent(dtype) for (; --n >= 0; pi += step, ++po) \
*po = dtype##_FLOAT_EXPONENT(pi) ? dtype##_FLOAT_EXPONENT(pi) - dtype##_FLOAT_BIAS : 0; break;

/*-------------------------------------------------------------------
        F8X exponent part of model: sign * base**exponent * fraction.
        This is computer-dependent. VAX base=2.
        EXPONENT(1.0) is 1, EXPONENT(4.1) is 3, EXPONENT(0.0) is 0, EXPONENT(ROPRAND) is 0.
                integer = EXPONENT(real)
*/
int Tdi3Exponent(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int n, step = in_ptr->length;
  char *pi = in_ptr->pointer;
  int *po = (int *)out_ptr->pointer;

  N_ELEMENTS(out_ptr, n);
  if STATUS_OK
    switch (in_ptr->dtype) {
    case DTYPE_FC:
    case DTYPE_FSC:
    case DTYPE_DC:
    case DTYPE_GC:
    case DTYPE_FTC:
      status = TdiNO_CMPLX;
      break;
    case DTYPE_F:
      compute_exponent(F)
    case DTYPE_FS:
      compute_exponent(FS)
    case DTYPE_D:
      compute_exponent(D)
    case DTYPE_G:
      compute_exponent(G)
    case DTYPE_FT:
      compute_exponent(FT)
    default:
      status = TdiINVDTYDSC;
      break;
    }
  return status;
}

/*-------------------------------------------------------------------
        Find the valid values.
                FINITE(expression)
        VAX dependent.
*/
int Tdi3Finite(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  char *pin = in_ptr->pointer;
  char *pout = out_ptr->pointer;
  int step = in_ptr->length;
  int n;

  N_ELEMENTS(out_ptr, n);
  switch (in_ptr->dtype) {
  case DTYPE_F:
    for (; --n >= 0; pin += step)
      *pout++ = (char)!IsRoprand(DTYPE_F, pin);
    break;
  case DTYPE_FS:
    for (; --n >= 0; pin += step)
      *pout++ = (char)!IsRoprand(DTYPE_FS, pin);
    break;
  case DTYPE_D:
    for (; --n >= 0; pin += step)
      *pout++ = (char)!IsRoprand(DTYPE_D, pin);
    break;
  case DTYPE_G:
    for (; --n >= 0; pin += step)
      *pout++ = (char)!IsRoprand(DTYPE_G, pin);
    break;
  case DTYPE_FT:
    for (; --n >= 0; pin += step)
      *pout++ = (char)!IsRoprand(DTYPE_FT, pin);
    break;
  case DTYPE_FC:
    for (; --n >= 0; pin += step)
      *pout++ = (char)!(IsRoprand(DTYPE_F, pin) || IsRoprand(DTYPE_F, pin + step / 2));
    break;
  case DTYPE_FSC:
    for (; --n >= 0; pin += step)
      *pout++ = (char)!(IsRoprand(DTYPE_FS, pin) || IsRoprand(DTYPE_FS, pin + step / 2));
    break;
  case DTYPE_DC:
    for (; --n >= 0; pin += step)
      *pout++ = (char)!(IsRoprand(DTYPE_D, pin) || IsRoprand(DTYPE_D, pin + step / 2));
    break;
  case DTYPE_GC:
    for (; --n >= 0; pin += step)
      *pout++ = (char)!(IsRoprand(DTYPE_G, pin) || IsRoprand(DTYPE_G, pin + step / 2));
    break;
  case DTYPE_FTC:
    for (; --n >= 0; pin += step)
      *pout++ = (char)!(IsRoprand(DTYPE_FT, pin) || IsRoprand(DTYPE_FT, pin + step / 2));
    break;
  default:
    for (; --n >= 0;)
      *pout++ = (char)1;
    break;
  }
  return status;
}

/*-------------------------------------------------------------------
        Fix any reserved operands (overflows, errors) in the data.
        If the expression and replacement is complex both elements are replaced.
        If the expression is complex but the replacement is real, replace either with that real.
        This is computer dependent, reserved operand is VAX feature.
                FIX_ROPRAND(expression, replacement)
        NEED special case for no fixup? Remember we have fetched the data.
        if-else is more efficient than ?: for loop.
        NEED to understand why VAXC fetches 16/8 bytes for some struct copies in ?:.
*/

#define do_complex(type,dtype) \
  if (cmplx) {\
    for (; --n >= 0; inp += inp_inc, rep += rep_inc, outp += out_inc) {\
      *(type *)outp = IsRoprand(dtype,inp) ? *(type *)rep : *(type *)inp;\
      *(type *)(outp+4) = IsRoprand(dtype,inp+sizeof(type)) ? *(float *)(rep+sizeof(type)) : *(float *)(inp+sizeof(type));\
    }\
    break;\
  }\
  n += n; inp_inc >>= 1;

#define do_simple(type,dtype) \
  for (; --n >= 0; inp += inp_inc, rep += rep_inc, outp += out_inc)\
    *(type *)outp = IsRoprand(dtype,inp) ? *(type *)rep : *(type *)inp;\
  break;

#define do_fix(type,dtype) case DTYPE##dtype##C : do_complex(type,DTYPE##dtype)  case DTYPE##dtype : do_simple(type,DTYPE##dtype)

int Tdi3FixRoprand(struct descriptor *old_ptr,
		   struct descriptor *rep_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  char *inp = old_ptr->pointer;
  char *rep = rep_ptr->pointer;
  char *outp = out_ptr->pointer;
  int inp_inc = (old_ptr->class == CLASS_A) ? old_ptr->length : 0;
  int rep_inc = (rep_ptr->class == CLASS_A) ? rep_ptr->length : 0;
  int out_inc = (out_ptr->class == CLASS_A) ? out_ptr->length : 0;
  int n, cmplx;

  N_ELEMENTS(out_ptr, n);
  switch (rep_ptr->dtype) {
  case DTYPE_F:
  case DTYPE_FS:
  case DTYPE_D:
  case DTYPE_G:
  case DTYPE_FT:
    cmplx = 0;
    break;
  default:
    cmplx = 1;
    break;
  }
  if STATUS_OK
    switch (out_ptr->dtype) {
      do_fix(float, _F) do_fix(float, _FS) do_fix(double, _D) do_fix(double, _G) do_fix(double, _FT)
      default:break;
    }
  return status;
}

#define do_fraction(dtype,copydouble) \
  case DTYPE_##dtype##C : n += n; inc >>= 1;\
  case DTYPE_##dtype :\
    for (; --n >= 0; inp += inc, outp += inc) {\
      *(int *)outp = *(int *)inp;\
      copydouble\
      if (dtype##_FLOAT_EXPONENT(outp) != 0) dtype##_FLOAT_SET_EXPONENT(outp,dtype##_FLOAT_BIAS);\
    }\
    break;

/*-------------------------------------------------------------------
        F8X elemental for fraction part of model: sign * base**exponent * fraction.
        This is computer-dependent. VAX base=2.
        FRACTION(3.0) is .75, FRACTION(0.0) is 0.0, FRACTION(ROPRAND) is ROPRAND.
                real = FRACTION(real)
*/
int Tdi3Fraction(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  char *inp = in_ptr->pointer;
  char *outp = out_ptr->pointer;
  int inc = out_ptr->length, n;

  N_ELEMENTS(out_ptr, n);
  if STATUS_OK
    switch (out_ptr->dtype) {
      do_fraction(F,;)
	  do_fraction(FS,;)
	  do_fraction(G, ((int *)outp)[1] = ((int *)inp)[1];)
	  do_fraction(D, ((int *)outp)[1] = ((int *)inp)[1];)
	  do_fraction(FT, ((int *)outp)[1] = ((int *)inp)[1];)
    default:
      status = TdiINVDTYDSC;
      break;
    }
  return status;
}

#define do_rrspacing(dtype,copydouble) \
  case DTYPE_##dtype##C : n += n; inc >>= 1;\
  case DTYPE_##dtype :\
    for (; --n >= 0; inp += inc, outp += inc) {\
      *(int *)outp = *(int *)inp;\
      copydouble\
      if (dtype##_FLOAT_EXPONENT(outp) != 0) dtype##_FLOAT_SET_EXPONENT(outp,dtype##_FLOAT_PREC + 1 + dtype##_FLOAT_BIAS);\
    }\
    break;

/*-------------------------------------------------------------------
        F8X elemental for reciprocal of the relative spacing of model: sign * base**exponent * fraction.
        This is computer-dependent. VAX base=2. abs(X*b^(prec-offset)).
        RRSPACING(3.0) is .75*2^24.
                real = RRSPACING(real)
*/
int Tdi3RrSpacing(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int n;
  int inc = out_ptr->length;
  char *inp = in_ptr->pointer;
  char *outp = out_ptr->pointer;

  N_ELEMENTS(out_ptr, n);
  if STATUS_OK
    switch (in_ptr->dtype) {
      do_rrspacing(F,;)
	  do_rrspacing(FS,;)
	  do_rrspacing(G, ((int *)outp)[1] = ((int *)inp)[1];)
	  do_rrspacing(D, ((int *)outp)[1] = ((int *)inp)[1];)
	  do_rrspacing(FT, ((int *)outp)[1] = ((int *)inp)[1];)
    default:
      status = TdiINVDTYDSC;
      break;
    }
  return status;
}

#define do_scale(dtype,copydouble) \
  case DTYPE_##dtype##C : n += n; inp_inc >>= 1; outp_inc >>= 1; inp2_inc >>= 1;\
  case DTYPE_##dtype :\
    for (; --n >= 0; inp += inp_inc, outp += outp_inc, inp2 += inp2_inc) {\
      *(int *)outp = *(int *)inp;\
      copydouble\
      if (dtype##_FLOAT_EXPONENT(outp) != 0) dtype##_FLOAT_SET_EXPONENT(outp,dtype##_FLOAT_EXPONENT(outp) + *(int *)inp2);\
    }\
    break;
/*-------------------------------------------------------------------
        F8X elemental to scale number of model: sign * base**exponent * fraction.
        This is computer-dependent. VAX base=2.
        SCALE(3.0, 2) is 12.0, SCALE(0.0, any) is 0.0, SCALE(ROPRAND, any) is ROPRAND.
                real = SCALE(real, integer)
        NEED overflow detection, complex.
*/
int Tdi3Scale(struct descriptor *in1_ptr, struct descriptor *in2_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int n;
  int inp_inc = (in1_ptr->class == CLASS_A) ? in1_ptr->length : 0;
  int inp2_inc = (in2_ptr->class == CLASS_A) ? in2_ptr->length : 0;
  int outp_inc = out_ptr->length;
  char *inp = in1_ptr->pointer;
  char *inp2 = in2_ptr->pointer;
  char *outp = out_ptr->pointer;

  N_ELEMENTS(out_ptr, n);
  if (in1_ptr->class != CLASS_A)
    inp_inc = 0;
  if (in2_ptr->class != CLASS_A)
    inp2_inc = 0;
  if STATUS_OK
    switch (in1_ptr->dtype) {
      do_scale(F,;)
	  do_scale(FS,;)
	  do_scale(G, ((int *)outp)[1] = ((int *)inp)[1];)
	  do_scale(D, ((int *)outp)[1] = ((int *)inp)[1];)
	  do_scale(FT, ((int *)outp)[1] = ((int *)inp)[1];)
    default:
      status = TdiINVDTYDSC;
      break;
    }
  return status;
}

#define do_setexponent(dtype,copydouble) \
  case DTYPE_##dtype##C : n += n; inp_inc >>= 1; outp_inc >>= 1; inp2_inc >>=1;\
  case DTYPE_##dtype :\
    for (; --n >= 0; inp += inp_inc, outp += outp_inc, inp2 += inp2_inc) {\
      *(int *)outp = *(int *)inp;\
      copydouble\
      if (dtype##_FLOAT_EXPONENT(outp) != 0) dtype##_FLOAT_SET_EXPONENT(outp,*(int *)inp2 + dtype##_FLOAT_BIAS);\
    }\
    break;
/*-------------------------------------------------------------------
        F8X elemental to set exponent part of non-zeroes with model: sign * base**exponent * fraction.
        This is computer-dependent. VAX base=2.
        SET_EXPONENT(3.0, 1) is 1.5, SET_EXPONENT(0.0, any) is 0.0, SET_EXPONENT(ROPRAND, any) is ROPRAND.
                real = SET_EXPONENT(real, integer)
        NEED overflow detection.
*/
int Tdi3SetExponent(struct descriptor *in1_ptr,
		    struct descriptor *in2_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int n;
  int inp_inc = (in1_ptr->class == CLASS_A) ? in1_ptr->length : 0;
  int inp2_inc = (in2_ptr->class == CLASS_A) ? in2_ptr->length : 0;
  int outp_inc = out_ptr->length;
  char *inp = in1_ptr->pointer;
  char *inp2 = in2_ptr->pointer;
  char *outp = out_ptr->pointer;

  N_ELEMENTS(out_ptr, n);
  if STATUS_OK
    switch (in1_ptr->dtype) {
      do_setexponent(F,;)
	  do_setexponent(FS,;)
	  do_setexponent(G, ((int *)outp)[1] = ((int *)inp)[1];)
	  do_setexponent(D, ((int *)outp)[1] = ((int *)inp)[1];)
	  do_setexponent(FT, ((int *)outp)[1] = ((int *)inp)[1];)
    default:
      status = TdiINVDTYDSC;
      break;
    }
  return status;
}

#define do_spacing(dtype,zerodouble) \
  case DTYPE_##dtype##C : n += n; inc >>= 1;\
  case DTYPE_##dtype :\
    for (; --n >= 0; inp += inc, outp += inc) {\
      test = dtype##_FLOAT_EXPONENT(inp) - dtype##_FLOAT_PREC - 1;\
      *(int *)outp = 0;\
      zerodouble\
      if (test >= 1) dtype##_FLOAT_SET_EXPONENT(outp,test);\
      else dtype##_FLOAT_SET_EXPONENT(outp,1);\
    }\
    break;

/*-------------------------------------------------------------------
        F8X elemental for the absolute spacing of model numbers near the argument: sign * base**exponent * fraction.
        This is computer-dependent. VAX base=2. base^(exp-prec)
        SPACING(3.0) is 2^22.
                real = SPACING(real)
        NEED overflow detection.
*/
int Tdi3Spacing(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int n, test;
  int inc = out_ptr->length;
  char *inp = in_ptr->pointer;
  char *outp = out_ptr->pointer;

  N_ELEMENTS(out_ptr, n);
  if STATUS_OK
    switch (in_ptr->dtype) {
      do_spacing(F,;)
	  do_spacing(FS,;)
	  do_spacing(G, ((int *)outp)[1] = 0;)
	  do_spacing(D, ((int *)outp)[1] = 0;)
	  do_spacing(FT, ((int *)outp)[1] = 0;)
    default:
      status = TdiINVDTYDSC;
      break;
    }
  return status;
}
