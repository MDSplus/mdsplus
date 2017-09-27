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
/*------------------------------------------------------------------------------

                Name:   Tdi3Iand   

                Type:   C function

                Author: TOM FREDIAN

                Date:   18-FEB-1993

                Purpose: Bitwise logical operations on two operands 

        For AB  = 00,01,10,11
        FALSE   = 0,0,0,0       use 0b
        AND     = 0,0,0,1       .
        AND_NOT = 0,0,1,0       .
        A       = 0,0,1,1       use a
        NOR_NOT = 0,1,0,0       use AND_NOT(b,a)
        B       = 0,1,0,1       use b
        EOR     = 0,1,1,0       (is NEQV)
        OR      = 0,1,1,1       .
        NOR     = 1,0,0,0       .
NEOR =  EOR_NOT = 1,0,0,1       (is EQV)
        NOT B   = 1,0,1,0       use NOT(b)
        OR_NOT  = 1,0,1,1       .
        NOT A   = 1,1,0,0       use NOT(a)
        NAND_NOT= 1,1,0,1       use OR_NOT(b,a)
        NAND    = 1,1,1,0       .
        TRUE    = 1,1,1,1       use -1b

------------------------------------------------------------------------------

        Call sequence: 

int Tdi3Iand(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

------------------------------------------------------------------------------*/

#include <STATICdef.h>
#include <string.h>
#include <mdsdescrip.h>
#include <tdishr_messages.h>



extern int CvtConvertFloat();
extern void DoubleToWideInt();

#define Operate(type,operator) \
{ type *in1p = (type *)in1->pointer;\
  type *in2p = (type *)in2->pointer;\
  type *outp = (type *)out->pointer;\
  switch (scalars)\
  {\
    case 0: \
    case 3: while (nout--) {*outp++ = (type)(not(*in1p operator *in2p)); in1p++; in2p++; } break; \
    case 1: while (nout--) {*outp++ = (type)(not(*in1p operator *in2p));         in2p++; } break; \
    case 2: while (nout--) {*outp++ = (type)(not(*in1p operator *in2p)); in1p++;         } break; \
  }\
  break;\
}

#define OperateSpecial(size,operator) \
{ unsigned int *in1p = (unsigned int *)in1->pointer;\
  unsigned int *in2p = (unsigned int *)in2->pointer;\
  unsigned int *outp = (unsigned int *)out->pointer;\
  int i;\
  switch (scalars)\
  {\
    case 0: \
    case 3: while (nout--) {for (i=0;i<size;i++) *outp++ = (char)(not(in1p[i] operator in2p[i])); in1p += size; in2p += size;} break; \
    case 1: while (nout--) {for (i=0;i<size;i++) *outp++ = (char)(not(in1p[i] operator in2p[i]));               in2p += size;} break; \
    case 2: while (nout--) {for (i=0;i<size;i++) *outp++ = (char)(not(in1p[i] operator in2p[i])); in1p += size;              } break; \
  }\
  break;\
}

#define OperateFloatOne(dtype,operator) \
    *outp++ = (CvtConvertFloat(in1p,dtype,&a,DTYPE_NATIVE_FLOAT,0) && CvtConvertFloat(in2p,dtype,&b,DTYPE_NATIVE_FLOAT,0)) ? \
      not((unsigned int)a operator (unsigned int)b) : (unsigned int)0;

#define OperateFloat(dtype,operator) \
{ float *in1p = (float *)in1->pointer;\
  float *in2p = (float *)in2->pointer;\
  unsigned int *outp = (unsigned int *)out->pointer;\
  float a,b;\
  switch (scalars)\
  {\
    case 0: \
    case 3: while (nout--) {OperateFloatOne(dtype,operator) in1p++; in2p++; } break; \
    case 1: while (nout--) {OperateFloatOne(dtype,operator)         in2p++; } break; \
    case 2: while (nout--) {OperateFloatOne(dtype,operator) in1p++;         } break; \
  }\
  break;\
}

#define OperateFloatC(dtype,operator) \
{ float *in1p = (float *)in1->pointer;\
  float *in2p = (float *)in2->pointer;\
  unsigned int *outp = (unsigned int *)out->pointer;\
  float a,b;\
  int j;\
  switch (scalars)\
  {\
    case 0: \
    case 3: while (nout--) {for (j=0;j<2;j++) { OperateFloatOne(dtype,operator) in1p++; in2p++; }} break; \
    case 1: while (nout--) {in1p = (float *)in1->pointer;\
                            for (j=0;j<2;j++) { OperateFloatOne(dtype,operator) in1p++; in2p++; }} break; \
    case 2: while (nout--) {in2p = (float *)in2->pointer;\
                            for (j=0;j<2;j++) { OperateFloatOne(dtype,operator) in1p++; in2p++; }} break; \
  }\
  break;\
}

#define OperateDoubleOne(dtype,operator) \
      if (CvtConvertFloat(in1p,dtype,&a,DTYPE_NATIVE_DOUBLE,0) && CvtConvertFloat(in2p,dtype,&b,DTYPE_NATIVE_DOUBLE,0)) \
      { unsigned int a_int[2],b_int[2];\
        DoubleToWideInt(&a,2,a_int); DoubleToWideInt(&b,2,b_int); \
        for (i=0;i<2;i++) {*outp++ = not(a_int[i] operator b_int[i]);}\
      }\
      else { *outp++ = (unsigned int)0; *outp++ = (unsigned int)0; }

#define OperateDouble(dtype,operator) \
{ double *in1p = (double *)in1->pointer;\
  double *in2p = (double *)in2->pointer;\
  unsigned int *outp = (unsigned int *)out->pointer;\
  double a,b;\
  int i;\
  switch (scalars)\
  {\
    case 0: \
    case 3: while (nout--) {OperateDoubleOne(dtype,operator) in1p++; in2p++; } break; \
    case 1: while (nout--) {OperateDoubleOne(dtype,operator)         in2p++; } break; \
    case 2: while (nout--) {OperateDoubleOne(dtype,operator) in1p++;         } break; \
  }\
  break;\
}

#define OperateDoubleC(dtype,operator) \
{ double *in1p = (double *)in1->pointer;\
  double *in2p = (double *)in2->pointer;\
  unsigned int *outp = (unsigned int *)out->pointer;\
  double a,b;\
  int i;\
  int j;\
  switch (scalars)\
  {\
    case 0: \
    case 3: while (nout--) {for (j=0;j<2;j++) {OperateDoubleOne(dtype,operator) in1p++; in2p++; }} break; \
    case 1: while (nout--) {in1p = (double *)in1->pointer;\
                            for (j=0;j<2;j++) {OperateDoubleOne(dtype,operator) in1p++; in2p++; }} break; \
    case 2: while (nout--) {in2p = (double *)in2->pointer;\
                            for (j=0;j<2;j++) {OperateDoubleOne(dtype,operator) in1p++; in2p++; }} break; \
  }\
  break;\
}

#define common(name,operator)\
int Tdi3##name(struct descriptor *in1, struct descriptor *in2, struct descriptor *out) {\
  struct descriptor_a *ina1 = (struct descriptor_a *)in1;\
  struct descriptor_a *ina2 = (struct descriptor_a *)in2;\
  struct descriptor_a *outa = (struct descriptor_a *)out;\
  int scalars = 0;\
  int nout;\
  switch (out->class)\
  {\
    case CLASS_S:\
    case CLASS_D: nout = 1; break;\
    case CLASS_A: nout = outa->arsize/outa->length; if (nout == 0) return 1; break;\
    default:            return TdiINVCLADSC;\
  }\
  switch (in1->class)\
  {\
    case CLASS_S:\
    case CLASS_D: scalars |= 1; break;\
    case CLASS_A: if (ina1->arsize/ina1->length < (unsigned int)nout) return TdiINV_SIZE; break;\
    default:            return TdiINVCLADSC;\
  }\
  switch (in2->class)\
  {\
    case CLASS_S:\
    case CLASS_D: if (scalars && (nout > 1)) return TdiINV_SIZE; scalars |= 2; break;\
    case CLASS_A: if (ina2->arsize/ina2->length < (unsigned int)nout) return TdiINV_SIZE; break;\
    default:            return TdiINVCLADSC;\
  }\
  switch (in1->dtype)\
  {\
    case DTYPE_B:\
    case DTYPE_BU:  Operate(unsigned char,operator)\
    case DTYPE_W:\
    case DTYPE_WU:  Operate(unsigned short,operator)\
    case DTYPE_L:\
    case DTYPE_LU:  Operate(unsigned int,operator)\
    case DTYPE_Q:   OperateSpecial(2,operator)\
    case DTYPE_QU:  OperateSpecial(2,operator)\
    case DTYPE_O:   OperateSpecial(4,operator)\
    case DTYPE_OU:  OperateSpecial(4,operator)\
    case DTYPE_F:   OperateFloat(DTYPE_F, operator)\
    case DTYPE_FS:  OperateFloat(DTYPE_FS, operator)\
    case DTYPE_G:   OperateDouble(DTYPE_G, operator)\
    case DTYPE_D:   OperateDouble(DTYPE_D, operator)\
    case DTYPE_FT:  OperateDouble(DTYPE_FT, operator)\
    case DTYPE_FC:  OperateFloatC(DTYPE_F, operator)\
    case DTYPE_FSC: OperateFloatC(DTYPE_FS, operator)\
    case DTYPE_GC:  OperateDoubleC(DTYPE_G, operator)\
    case DTYPE_DC:  OperateDoubleC(DTYPE_D, operator)\
    case DTYPE_FTC: OperateDoubleC(DTYPE_FT, operator)\
    default: return TdiINVDTYDSC;\
  }\
  return 1;\
}

#define not ~
common(Inand, &)
    common(InandNot, &~)
    common(Inor, |)
    common(InorNot, |~)
#undef not
#define not
    common(Iand, &)
    common(IandNot, &~)
    common(Ior, |)
    common(IorNot, |~)
    common(Ieor, ^)
    common(IeorNot, ^~)
#undef not
