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
/*      Tdi2xxx.C
        Fix categories for various opcodes.

        Ken Klare, LANL P-4     (c)1989,1990,1991
*/
#include <STATICdef.h>
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include <strroutines.h>
#include <tdishr_messages.h>
#include <stdlib.h>
#include <mdsshr.h>



extern int TdiConcat();
extern int TdiTranslate();
extern int TdiGetLong();

STATIC_CONSTANT DESCRIPTOR(asterisk, "*");
STATIC_CONSTANT DESCRIPTOR(slash, "/");
STATIC_CONSTANT DESCRIPTOR(slash_star, "/*");
STATIC_CONSTANT DESCRIPTOR(star_slash, "*/");
STATIC_CONSTANT DESCRIPTOR(bad, "?");
STATIC_CONSTANT struct descriptor_xd BAD = { 0, DTYPE_DSC, CLASS_XS, (struct descriptor *)&bad, 0 };

/********************************
Units must match or one be empty.
********************************/
STATIC_ROUTINE int either(struct descriptor_xd uni[2])
{

  if (uni[0].pointer == 0) {
    uni[0] = uni[1];
    uni[1] = EMPTY_XD;
  } else if (uni[1].pointer) {
    if (StrCompare(uni[0].pointer, uni[1].pointer)) {
      MdsFree1Dx(&uni[0], NULL);
      uni[0] = BAD;
    }
  }
  return 1;
}

/*******************************
Discard units unless mismatched.
*******************************/
STATIC_ROUTINE int only_mismatch(struct descriptor_xd uni[2])
{

  either(uni);
  if (uni[0].pointer && uni[0].pointer != (struct descriptor *)&bad)
    MdsFree1Dx(&uni[0], NULL);
  return 1;
}

/*****************
Concatenate units.
*****************/
STATIC_ROUTINE void multiply(struct descriptor_xd *left_ptr, struct descriptor_xd *right_ptr)
{
  INIT_STATUS;

  if (left_ptr->pointer == 0) {
    *left_ptr = *right_ptr;
    *right_ptr = EMPTY_XD;
  } else if (right_ptr->pointer) {
    /*NEED cleaver code here */
    status = TdiConcat(left_ptr, &asterisk, right_ptr, left_ptr MDS_END_ARG);
    if (STATUS_NOT_OK)
      *left_ptr = BAD;
  }
}

/*****************
Reciprocate units.
*****************/
STATIC_ROUTINE void divide(struct descriptor_xd *left_ptr, struct descriptor_xd *right_ptr)
{
  INIT_STATUS;

  if (right_ptr->pointer) {
    /*NEED cleaver code here */
    /*NEED to fix up leading / or * */
    status = TdiTranslate(right_ptr, &star_slash, &slash_star, right_ptr MDS_END_ARG);
    if STATUS_OK {
      if (left_ptr->pointer)
	status = TdiConcat(left_ptr, &slash, right_ptr, left_ptr MDS_END_ARG);
      else
	status = TdiConcat(&slash, right_ptr, left_ptr MDS_END_ARG);
    }
    if (STATUS_NOT_OK)
      *left_ptr = BAD;
  }
}

/*---------------------------------------------------
        Fix categories for opcodes: ABS and ABS1.
        Unsigned integers are absolute.
*/
int Tdi2Abs(int narg __attribute__ ((unused)), struct descriptor_xd uni[1] __attribute__ ((unused)), struct descriptor_xd dat[1] __attribute__ ((unused)),
	    struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  if ((cats[0].in_cat & TdiCAT_B) == TdiCAT_BU)
    *routine_ptr = NULL;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes: ADD DIM MAX MIN SUBTRACT
        Category is "or" of inputs setting length and type.
        Example: f-complex with h-real makes h-complex.
*/
int Tdi2Add(int narg __attribute__ ((unused)), struct descriptor_xd uni[1], struct descriptor_xd dat[1] __attribute__ ((unused)),
	    struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  either(uni);
  cats[0].out_cat = cats[1].out_cat = cats[2].out_cat;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes: ATAN2 ATAN2D
        Category is "or" of inputs setting length and type.
        Example: f-complex with h-real makes h-complex.
*/
int Tdi2Atan2(int narg __attribute__ ((unused)), struct descriptor_xd uni[1],
	      struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	      int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  only_mismatch(uni);
  cats[0].out_cat = cats[1].out_cat = cats[2].out_cat;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes that accept any units and output none:
        DIGITS DSIZE RANGE PRECISION ESIZE
        LEN LEN_TRIM MAX_EXPONENT MIN_EXPONENT RADIX RANK SIZE_OF SORT?
*/
int Tdi2Any(int narg __attribute__ ((unused)), struct descriptor_xd uni[1], struct descriptor_xd dat[1] __attribute__ ((unused)),
	    struct TdiCatStruct cats[1] __attribute__ ((unused)), int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  if (uni[0].pointer)
    MdsFree1Dx(&uni[0], NULL);
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes: ADJUSTL ADJUSTR TRANSLATE UPCASE.
*/
int Tdi2Adjust(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)),
	       struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	       int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  cats[narg].digits = cats[0].digits;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes: AINT ANINT CEILING FLOOR NINT.
        Second argument is KIND.
*/
int Tdi2Aint(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)), struct descriptor_xd dat[1],
	     struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  INIT_STATUS;

	/**************************
        Check for valid second arg.
        Prevent its conversion.
        **************************/
  if (narg > 1) {
    int kind = -1;
    status = TdiGetLong(dat[1].pointer, &kind);
    if (kind < TdiCAT_MAX)
      cats[narg].out_cat =
	  (unsigned
	   short)((TdiREF_CAT[cats[narg].out_dtype = (unsigned char)kind].cat | TdiREF_CAT[o1].
		   cat) & TdiREF_CAT[o2].cat);
    cats[1].out_dtype = cats[1].in_dtype;
    cats[1].out_cat = cats[1].in_cat;
    cats[1].digits = dat[1].pointer->length;
  }
  if ((cats[0].in_cat & (TdiCAT_FLOAT ^ TdiCAT_B ^ TdiCAT_BU)) == TdiCAT_BU)
    *routine_ptr = NULL;
  return status;
}

/*---------------------------------------------------
        Fix categories for opcodes: BSEARCH, IS_IN.
*/
int Tdi2Bsearch(int narg __attribute__ ((unused)), struct descriptor_xd uni[1],
		struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
		int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  either(uni);
  cats[1].out_cat = cats[0].out_cat |= cats[1].out_cat;
  if (cats[0].digits < cats[1].digits)
    cats[0].digits = cats[1].digits;
  else
    cats[1].digits = cats[0].digits;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes, out=logical in1=any, in2..=long: BTEST
*/
int Tdi2Btest(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)),
	      struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	      int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  int j;

  for (j = narg; --j > 0;) {
    cats[j].out_dtype = DTYPE_L;
    cats[j].out_cat = TdiREF_CAT[DTYPE_L].cat;
  }
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes ACHAR and CHAR, length 1 text.
*/
int Tdi2Char(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)), struct descriptor_xd dat[1],
	     struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  INIT_STATUS;

	/**************************
        Check for valid second arg.
        Prevent its conversion.
        **************************/
  if (narg > 1) {
    int kind = -1;
    status = TdiGetLong(dat[1].pointer, &kind);
    if (kind < TdiCAT_MAX)
      cats[narg].out_cat =
	  (unsigned
	   short)((TdiREF_CAT[cats[narg].out_dtype = (unsigned char)kind].cat | TdiREF_CAT[o1].
		   cat) & TdiREF_CAT[o2].cat);
    cats[1].out_dtype = cats[1].in_dtype;
    cats[1].out_cat = cats[1].in_cat;
  }
  cats[narg].digits = 1;
  return status;
}

/*---------------------------------------------------
        Fix categories for opcode: CMPLX D_COMPLEX ... .
        These expect arguments ([x],[y],[mold])
        Shape is set by array of one of them.
        Array shapes must match if more than one array.
        Data type is set by mold, else by larger.
*/
int Tdi2Cmplx(int narg, struct descriptor_xd uni[1],
	      struct descriptor_xd dat[1], struct TdiCatStruct cats[1],
	      int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  INIT_STATUS;

	/********************************
        Omitted second arg just converts.
        2 arg takes real or complex.
        3 arg converts to type of third.
        Prevent conversion for third arg.
        ********************************/
  if (narg > 1)
    either(uni);
  if (narg > 2) {
    int kind = -1;
    status = TdiGetLong(dat[2].pointer, &kind);
    if (kind < TdiCAT_MAX) {
      cats[narg].out_cat =
	  (unsigned
	   short)((TdiREF_CAT[cats[narg].out_dtype = (unsigned char)kind].cat | TdiREF_CAT[o1].
		   cat) & TdiREF_CAT[o2].cat);
    }
    cats[2].out_dtype = cats[2].in_dtype;
    cats[2].out_cat = cats[2].in_cat;
    cats[0].out_cat = cats[1].out_cat = cats[narg].out_cat;
  } else if (narg == 1) {
    switch (cats[0].out_dtype) {
    case DTYPE_F:
      cats[0].out_dtype = DTYPE_FC;
      cats[0].out_cat = TdiREF_CAT[cats[0].out_dtype].cat;
      break;
    case DTYPE_FS:
      cats[0].out_dtype = DTYPE_FSC;
      cats[0].out_cat = TdiREF_CAT[cats[0].out_dtype].cat;
      break;
    case DTYPE_D:
      cats[0].out_dtype = DTYPE_DC;
      cats[0].out_cat = TdiREF_CAT[cats[0].out_dtype].cat;
      break;
    case DTYPE_G:
      cats[0].out_dtype = DTYPE_GC;
      cats[0].out_cat = TdiREF_CAT[cats[0].out_dtype].cat;
      break;
    case DTYPE_FT:
      cats[0].out_dtype = DTYPE_FTC;
      cats[0].out_cat = TdiREF_CAT[cats[0].out_dtype].cat;
      break;
    }
  }
  if (narg < 2 || cats[1].in_dtype == DTYPE_MISSING)
    *routine_ptr = NULL;
  else {
    if (cats[0].in_cat != cats[0].out_cat)
      cats[0].out_cat &= ~TdiCAT_COMPLEX;
    if (cats[1].in_cat != cats[1].out_cat)
      cats[1].out_cat &= ~TdiCAT_COMPLEX;
  }
  return status;
}

/*---------------------------------------------------
        Fix categories for opcode: CONCAT text concatenation.
*/
int Tdi2Concat(int narg __attribute__ ((unused)), struct descriptor_xd uni[1],
	       struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	       int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  either(uni);
  cats[2].digits = (unsigned short)(cats[0].digits + cats[1].digits);
  return 1;
}

/*---------------------------------------------------
        Fix categories for conversion by example.
                out = CVT(input, example) converts input to dtype of example.
*/
int Tdi2Cvt(int narg __attribute__ ((unused)), struct descriptor_xd uni[1] __attribute__ ((unused)), struct descriptor_xd dat[1] __attribute__ ((unused)),
	    struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  cats[2].out_dtype = cats[0].out_dtype = cats[1].out_dtype;
  cats[2].out_cat = cats[0].out_cat = cats[1].out_cat;
  cats[2].digits = cats[0].digits = cats[1].digits;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcode: DBLE.
*/
int Tdi2Dble(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)), struct descriptor_xd dat[1] __attribute__ ((unused)),
	     struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  cats[0].out_cat = cats[1].out_cat = cats[narg].out_cat =
      (unsigned short)(cats[narg].out_cat + (cats[narg].out_cat & TdiCAT_LENGTH) + 1);
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcode: DIVIDE
*/
int Tdi2Divide(int narg __attribute__ ((unused)), struct descriptor_xd uni[1],
	       struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	       int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  divide(&uni[0], &uni[1]);
  cats[0].out_cat = cats[1].out_cat = cats[2].out_cat;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcode: DPROD.
*/
int Tdi2Dprod(int narg, struct descriptor_xd uni[1],
	      struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	      int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  multiply(&uni[0], &uni[1]);
  cats[0].out_cat = cats[1].out_cat = cats[narg].out_cat =
      (unsigned short)(cats[narg].out_cat + (cats[narg].out_cat & TdiCAT_LENGTH) + 1);
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes: EQ, GE, GT, LE, LT, NE, also LEQ etc.
        Input category is "or" of inputs setting length and type.
        Output category is logical.
        Example: f-complex with h-real makes h-complex.
*/
int Tdi2Eq(int narg __attribute__ ((unused)), struct descriptor_xd uni[1], struct descriptor_xd dat[1] __attribute__ ((unused)),
	   struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  only_mismatch(uni);
  cats[0].out_cat = cats[1].out_cat = (unsigned short)(cats[1].out_cat | cats[0].out_cat);
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcode: ELEMENT(number, delimiter, source)
*/
int Tdi2Element(int narg, struct descriptor_xd uni[1],
		struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
		int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  MdsFree1Dx(&uni[0], NULL);
  uni[0] = uni[2];
  uni[2] = EMPTY_XD;
  cats[narg].digits = cats[2].digits;
  cats[0].out_cat = TdiREF_CAT[DTYPE_L].cat;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcode: EXTRACT(start, length, source)
*/
int Tdi2Extract(int narg, struct descriptor_xd uni[1],
		struct descriptor_xd dat[1], struct TdiCatStruct cats[1],
		int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  int length, status;

  MdsFree1Dx(&uni[0], NULL);
  uni[0] = uni[2];
  uni[2] = EMPTY_XD;
  status = TdiGetLong(dat[1].pointer, &length);
  if (length < 0)
    length = 0;
  cats[narg].digits = (unsigned short)length;
  cats[0].out_cat = cats[1].out_cat = TdiREF_CAT[DTYPE_L].cat;
  return status;
}

/*---------------------------------------------------
        Fix categories for opcode: FIX_ROPRAND.
        These expect arguments (old,replacement)
        Shape is set by array of one of them.
        Array shapes must match if more than one array.
        Data type is set by highest, but complexity is kept.
        Complexity of output is set by first argument.
*/
int Tdi2Fix(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)), struct descriptor_xd dat[1] __attribute__ ((unused)),
	    struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  int cmplx, j;

  cmplx = cats[0].out_cat & TdiCAT_COMPLEX;
  cats[narg].out_cat &= ~TdiCAT_COMPLEX;
  for (j = narg; --j >= 0;)
    cats[j].out_cat = (unsigned short)(cats[narg].out_cat | (cats[j].out_cat & TdiCAT_COMPLEX));
  if (cmplx)
    cats[narg].out_cat |= TdiCAT_COMPLEX;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcode: IAND, IAND_NOT, IEOR, etc.
*/
int Tdi2Iand(int narg, struct descriptor_xd uni[1], struct descriptor_xd dat[1] __attribute__ ((unused)),
	     struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  either(uni);
  cats[0].out_cat = cats[1].out_cat = cats[narg].out_cat;
  if (cats[0].out_cat == (cats[0].in_cat | TdiCAT_B))
    cats[0].out_cat = (unsigned short)(cats[0].in_cat | TdiCAT_BU);
  if (cats[1].out_cat == (cats[1].in_cat | TdiCAT_B))
    cats[1].out_cat = (unsigned short)(cats[1].in_cat | TdiCAT_BU);
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcode: INVERSE
*/
int Tdi2Inverse(int narg __attribute__ ((unused)), struct descriptor_xd uni[1],
		struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1] __attribute__ ((unused)),
		int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  struct descriptor_xd tmp = uni[0];

  uni[0] = EMPTY_XD;
  divide(&uni[0], &tmp);
  MdsFree1Dx(&tmp, NULL);
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes for most 1-argument conversions:
        AIMAG CONJG INOT UNARY_MINUS UNARY_PLUS
        EPSILON FRACTION HUGE RRSPACING SPACING TINY
        BYTE WORD LONG QUADWORD OCTAWORD SIGNED IACHAR ICHAR
        BYTE_UNSIGNED WORD_UNSIGNED LONG_UNSIGNED QUADWORD_UNSIGNED OCTAWORD_UNSIGNED INT_UNSIGNED UNSIGNED
        D_FLOAT F_FLOAT G_FLOAT H_FLOAT
*/
int Tdi2Keep(int narg __attribute__ ((unused)), struct descriptor_xd uni[1] __attribute__ ((unused)), struct descriptor_xd dat[1] __attribute__ ((unused)),
	     struct TdiCatStruct cats[1] __attribute__ ((unused)), int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  return 1;
}

/*---------------------------------------------------
        Fix categories for logical opcodes: AND, AND_NOT, EQV etc.
*/
int Tdi2Land(int narg __attribute__ ((unused)), struct descriptor_xd uni[1], struct descriptor_xd dat[1] __attribute__ ((unused)),
	     struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  only_mismatch(uni);
  if (cats[0].out_cat != DTYPE_BU) {
    cats[0].out_dtype = DTYPE_BU;
    cats[0].out_cat = TdiREF_CAT[DTYPE_BU].cat;
  }
  if (cats[1].out_cat != DTYPE_BU) {
    cats[1].out_dtype = DTYPE_BU;
    cats[1].out_cat = TdiREF_CAT[DTYPE_BU].cat;
  }
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes, out=in1, in2..=long:
                IBCLR IBITS IBSET MEDIAN SCALE SET_EXPONENT SMOOTH.
*/
int Tdi2Long2(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)),
	      struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	      int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  int j;

  cats[narg].out_dtype = cats[0].out_dtype;
  cats[narg].out_cat = cats[0].out_cat;
  for (j = narg; --j > 0;) {
    cats[j].out_dtype = DTYPE_L;
    cats[j].out_cat = TdiREF_CAT[DTYPE_L].cat;
  }
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes, in1=any logical, in2=long-scalar out=logical:
                ALL ANY COUNT FIRSTLOC LASTLOC.
*/
int Tdi2Mask1(int narg __attribute__ ((unused)), struct descriptor_xd uni[1],
	      struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	      int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  if (uni[0].pointer)
    MdsFree1Dx(&uni[0], NULL);
  if (cats[0].out_cat != cats[0].in_cat) {
    cats[0].out_dtype = DTYPE_BU;
    cats[0].out_cat = TdiREF_CAT[DTYPE_BU].cat;
  }
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes, out=in1, in2..=any logical:
                MAXLOC MINLOC.
*/
int Tdi2Mask2(int narg, struct descriptor_xd uni[1],
	      struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	      int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  int j;

  if (uni[0].pointer)
    MdsFree1Dx(&uni[0], NULL);
  for (j = narg; --j > 0;) {
    cats[j].out_cat =
	(unsigned short)((cats[j].out_cat | TdiREF_CAT[DTYPE_BU].cat) & TdiREF_CAT[DTYPE_O].cat);
    if (cats[j].out_cat != cats[j].in_cat)
      cats[j].out_cat = TdiREF_CAT[DTYPE_BU].cat;
  }
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes, out=in1=any numeric, in2=long-scalar in3=any logical:
        MAXVAL MEAN MINVAL RMS STD_DEV SUM.
        NEED code for PRODUCT.
*/
int Tdi2Mask3(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)),
	      struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	      int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  cats[narg] = cats[0];
  if (narg > 2) {
    cats[2].out_cat =
	(unsigned short)((cats[2].in_cat | TdiREF_CAT[DTYPE_BU].cat) & TdiREF_CAT[DTYPE_O].cat);
    if (cats[2].out_cat != cats[2].in_cat) {
      cats[2].out_dtype = DTYPE_BU;
      cats[2].out_cat = TdiREF_CAT[DTYPE_BU].cat;
    }
  }
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes, out=long-scalar, in1=any numeric, in2=long-scalar in3=any logical:
        MAXLOC MINLOC. 
*/
int Tdi2Mask3L(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)),
	       struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	       int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  cats[narg].out_dtype = DTYPE_L;
  cats[narg].out_cat = TdiREF_CAT[DTYPE_L].cat;
  if (narg > 2) {
    cats[2].out_cat =
	(unsigned short)((cats[2].in_cat | TdiREF_CAT[DTYPE_BU].cat) & TdiREF_CAT[DTYPE_O].cat);
    if (cats[2].out_cat != cats[2].in_cat) {
      cats[2].out_dtype = DTYPE_BU;
      cats[2].out_cat = TdiREF_CAT[DTYPE_BU].cat;
    }
  }
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes with input, same, logical:
                any = MERGE(x,y,b).
*/
int Tdi2Merge(int narg, struct descriptor_xd uni[1],
	      struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	      int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  either(uni);
  cats[narg].out_cat = cats[1].out_cat = cats[0].out_cat =
      (unsigned short)((cats[0].out_cat | cats[1].out_cat) & cats[narg].out_cat);
  if (cats[0].digits < cats[1].digits)
    cats[0].digits = cats[1].digits;
  else
    cats[1].digits = cats[0].digits;
  cats[narg].digits = cats[0].digits;
  if (narg > 2)
    cats[2].out_cat = (unsigned short)((cats[2].out_cat | TdiCAT_BU) & TdiCAT_SIGNED);
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcode: MULTIPLY
*/
int Tdi2Multiply(int narg __attribute__ ((unused)), struct descriptor_xd uni[1],
		 struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
		 int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  multiply(&uni[0], &uni[1]);
  cats[0].out_cat = cats[1].out_cat = cats[2].out_cat;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes for most 1-argument function that should not have units.
*/
int Tdi2None(int narg __attribute__ ((unused)), struct descriptor_xd uni[1], struct descriptor_xd dat[1] __attribute__ ((unused)),
	     struct TdiCatStruct cats[1] __attribute__ ((unused)), int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  if (uni[0].pointer) {
    MdsFree1Dx(&uni[0], NULL);
    uni[0] = BAD;
  }
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes with logical result: NOT LOGICAL
*/
int Tdi2Not(int narg __attribute__ ((unused)), struct descriptor_xd uni[1], struct descriptor_xd dat[1] __attribute__ ((unused)),
	    struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  if (uni[0].pointer) {
    MdsFree1Dx(&uni[0], NULL);
    uni[0] = BAD;
  }
  if (cats[0].in_cat != cats[0].out_cat) {
    cats[0].out_dtype = DTYPE_BU;
    cats[0].out_cat = TdiREF_CAT[DTYPE_BU].cat;
  }
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcode without H-complex: COS, EXP, LOG, SIN, SQRT.
        We choose to convert to G-complex.
*/
int Tdi2NoHc(int narg, struct descriptor_xd uni[1], struct descriptor_xd dat[1] __attribute__ ((unused)),
	     struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  if (uni[0].pointer) {
    MdsFree1Dx(&uni[0], NULL);
    uni[0] = BAD;
  }
  if (cats[narg].out_cat == TdiREF_CAT[DTYPE_HC].cat) {
    cats[narg].out_dtype = DTYPE_GC;
    cats[narg].out_cat = TdiREF_CAT[DTYPE_GC].cat;
  }
  cats[0].out_cat = cats[1].out_cat = cats[narg].out_cat;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcode: PACK in1 matches in3 if any, in2 = logical.
*/
int Tdi2Pack(int narg, struct descriptor_xd uni[1], struct descriptor_xd dat[1] __attribute__ ((unused)),
	     struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  if (narg > 2 && uni[2].pointer) {
    MdsFree1Dx(&uni[1], NULL);
    uni[1] = uni[2];
    uni[2] = EMPTY_XD;
    either(&uni[0]);
    cats[0].out_cat |= cats[2].out_cat;
  }
  cats[narg].out_cat = cats[0].out_cat;
  cats[1].out_cat =
      (unsigned short)((cats[1].in_cat | TdiREF_CAT[DTYPE_BU].cat) & TdiREF_CAT[DTYPE_O].cat);
  if (cats[1].out_cat != cats[1].in_cat) {
    cats[1].out_dtype = DTYPE_BU;
    cats[1].out_cat = TdiREF_CAT[DTYPE_BU].cat;
  }
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcode: POWER.
*/
int Tdi2Power(int narg __attribute__ ((unused)), struct descriptor_xd uni[1],
	      struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	      int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

	/******************************************
        Permit any^long, real^same, complex^same.
        Integer exponents become long. Result=base.
        ******************************************/
  if (uni[0].pointer || uni[1].pointer) {
    MdsFree1Dx(&uni[0], NULL);
    uni[0] = BAD;
  }
  if (cats[1].out_cat < TdiCAT_F) {
    cats[1].out_dtype = DTYPE_L;
    cats[1].out_cat = TdiREF_CAT[DTYPE_L].cat;
  } else {
    cats[0].out_cat |= cats[1].out_cat;
    if (cats[0].out_cat == TdiREF_CAT[DTYPE_HC].cat) {
      cats[0].out_dtype = DTYPE_GC;
      cats[0].out_cat = TdiREF_CAT[DTYPE_GC].cat;
    }
    cats[1].out_cat = cats[0].out_cat;
  }
  cats[2].out_cat = cats[0].out_cat;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcode: DTYPE_RANGE, also used in I_TO_X and CULL/EXTEND.
*/
int Tdi2Range(int narg, struct descriptor_xd uni[1],
	      struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1],
	      int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  if (narg > 2)
    either(&uni[1]);
  either(uni);
  cats[2].out_cat = cats[1].out_cat = cats[0].out_cat = cats[narg].out_cat;
  if (cats[2].out_cat == TdiREF_CAT[DTYPE_T].cat) {
    if (cats[0].digits < cats[1].digits)
      cats[0].digits = cats[1].digits;
    if (cats[0].digits < cats[2].digits)
      cats[0].digits = cats[2].digits;
    cats[narg].digits = cats[2].digits = cats[1].digits = cats[0].digits;
  }
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes: FLOAT INT REAL.
        This expects arguments (x,[kind])
*/
int Tdi2Real(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)), struct descriptor_xd dat[1],
	     struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  INIT_STATUS;

	/**************************
        Check for valid second arg.
        Prevent its conversion.
        **************************/
  if (narg > 1) {
    int kind = -1;
    status = TdiGetLong(dat[1].pointer, &kind);
    if (kind < TdiCAT_MAX)
      cats[0].out_cat =
	  (unsigned
	   short)((TdiREF_CAT[cats[0].out_dtype = (unsigned char)kind].cat | TdiREF_CAT[o1].
		   cat) & TdiREF_CAT[o2].cat);
    cats[1].out_dtype = cats[1].in_dtype;
    cats[1].out_cat = cats[1].in_cat;
    cats[1].digits = dat[1].pointer->length;
  }
  return status;
}

/*---------------------------------------------------
        Fix categories for opcode: REPEAT text duplication.
*/
int Tdi2Repeat(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)),
	       struct descriptor_xd dat[1], struct TdiCatStruct cats[1],
	       int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  unsigned int ncopies, status;

  status = TdiGetLong(dat[1].pointer, &ncopies);
  if STATUS_OK {
    if ((ncopies *= cats[0].digits) < 65535)
      cats[narg].digits = (unsigned short)ncopies;
    else
      status = TdiTOO_BIG;
  }
  cats[1].out_cat = cats[1].in_cat;
  return status;
}

/*---------------------------------------------------
        Fix categories for opcodes, output = first:
                SHFT SHIFT_LEFT SHIFT_RIGHT.
*/
int Tdi2Shft(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)), struct descriptor_xd dat[1] __attribute__ ((unused)),
	     struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  cats[1].out_dtype = cats[0].out_dtype;
  cats[1].out_cat = cats[0].out_cat;
  cats[narg].out_dtype = cats[0].out_dtype;
  cats[narg].out_cat = cats[0].out_cat;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcode: SIGN NEAREST.
*/
int Tdi2Sign(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)), struct descriptor_xd dat[1] __attribute__ ((unused)),
	     struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  cats[narg].out_cat = cats[0].out_cat;
  return 1;
}

/*---------------------------------------------------
        Fix categories for opcodes: ABSSQ SQUARE.
*/
int Tdi2Square(int narg __attribute__ ((unused)), struct descriptor_xd uni[1],
	       struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1] __attribute__ ((unused)),
	       int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  struct descriptor_xd tmp = EMPTY_XD;
  INIT_STATUS;

  status = MdsCopyDxXd(uni[0].pointer, &tmp);
  multiply(&uni[0], &tmp);
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*---------------------------------------------------
        Fix categories for opcode: STRING text length specifier.
*/
int Tdi2String(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)),
	       struct descriptor_xd dat[1], struct TdiCatStruct cats[1],
	       int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  unsigned int length, status;

  cats[1].out_dtype = cats[1].in_dtype;
  cats[1].out_cat = cats[1].in_cat;
  cats[1].digits = dat[1].length;
  status = TdiGetLong(dat[1].pointer, &length);
  if STATUS_OK {
    if (length > 65535)
      status = TdiTOO_BIG;
    else if (length > 0)
      cats[0].digits = cats[narg].digits = (unsigned short)length;
  }
  return status;
}

/*---------------------------------------------------
        Fix categories for opcode: TEXT text length specifier.
*/
int Tdi2Text(int narg, struct descriptor_xd uni[1] __attribute__ ((unused)), struct descriptor_xd dat[1],
	     struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  unsigned int length, status;

  cats[1].out_dtype = cats[1].in_dtype;
  cats[1].out_cat = cats[1].in_cat;
  cats[1].digits = dat[1].length;
  status = TdiGetLong(dat[1].pointer, &length);
  if STATUS_OK {
    if (length > 65535)
      status = TdiTOO_BIG;
    else if (length > 0)
      cats[0].digits = cats[narg].digits = (unsigned short)length;
  }
  return status;
}

/*---------------------------------------------------
        Fix categories for opcodes with text, text, logical:
                integer = INDEX(t,t,[b]), SCAN(t,t,[b]), VERIFY(t,t,[b]).
*/
int Tdi2Ttb(int narg, struct descriptor_xd uni[1], struct descriptor_xd dat[1] __attribute__ ((unused)),
	    struct TdiCatStruct cats[1], int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{

  either(uni);
  if (narg > 2)
    cats[2].out_cat = (unsigned short)((cats[2].out_cat | TdiCAT_BU) & TdiCAT_SIGNED);
  return 1;
}

/*---------------------------------------------------
        Fix units for VECTOR, they must match or be missing.
*/
int Tdi2Vector(int narg, struct descriptor_xd uni[1],
	       struct descriptor_xd dat[1] __attribute__ ((unused)), struct TdiCatStruct cats[1] __attribute__ ((unused)),
	       int (**routine_ptr) () __attribute__ ((unused)), int o1 __attribute__ ((unused)), int o2 __attribute__ ((unused)))
{
  int j;

  for (j = narg; --j > 0;) {
    if (uni[0].pointer == 0) {
      uni[0] = uni[j];
      uni[j] = EMPTY_XD;
    } else if (uni[j].pointer) {
      if (StrCompare(uni[0].pointer, uni[j].pointer)) {
	MdsFree1Dx(&uni[0], NULL);
	uni[0] = BAD;
	break;
      }
    }
  }
  return 1;
}
