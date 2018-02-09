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

                Name:   Tdi3Add

                Type:   C function

                Author: TOM FREDIAN

                Date:   18-FEB-1993

                Purpose: Add two operands

------------------------------------------------------------------------------

        Call sequence:

int Tdi3Add(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

------------------------------------------------------------------------------*/

#include <mdstypes.h>
#include <string.h>
#include <mdsdescrip.h>
#include <tdishr_messages.h>
#include "roprand.h"
#include <STATICdef.h>
#include <int128.h>


extern int CvtConvertFloat();

#define SetupArgs \
  struct descriptor_a *ina1 = (struct descriptor_a *)in1;\
  struct descriptor_a *ina2 = (struct descriptor_a *)in2;\
  struct descriptor_a *outa = (struct descriptor_a *)out;\
  int scalars = 0;\
  unsigned int nout;\
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
    case CLASS_A: if (ina1->arsize/ina1->length < nout) return TdiINV_SIZE; break;\
    default:            return TdiINVCLADSC;\
  }\
  switch (in2->class)\
  {\
    case CLASS_S:\
    case CLASS_D: if (scalars && (nout > 1)) return TdiINV_SIZE; scalars |= 2; break;\
    case CLASS_A: if (ina2->arsize/ina2->length < nout) return TdiINV_SIZE; break;\
    default:            return TdiINVCLADSC;\
  }\

#define Operate(type,operator) \
{ type *in1p = (type *)in1->pointer;\
  type *in2p = (type *)in2->pointer;\
  type *outp = (type *)out->pointer;\
  switch (scalars)\
  {\
    case 0: \
    case 3: while (nout--) *outp++ = (type)(*in1p++ operator *in2p++); break;\
    case 1: {type scalar = *in1p; while (nout--) *outp++ = (type)(scalar operator *in2p++);} break;\
    case 2: {type scalar = *in2p; while (nout--) *outp++ = (type)(*in1p++ operator scalar);} break;\
  }\
  break;\
}

STATIC_CONSTANT const int roprand = 0x8000;

#define OperateFloatOne(type,dtype,native,operator,p1,p2) \
{ type a,b,ans;\
  type p1v = *p1;\
  type p2v = *p2;\
  if (IsRoprand(dtype,&p1v) || IsRoprand(dtype,&p2v))\
    CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0); \
  else\
  {\
    if (dtype == native)\
      *outp++ = p1v operator p2v;\
    else\
    {\
      if (CvtConvertFloat(&p1v,dtype,&a,native,0) && CvtConvertFloat(&p2v,dtype,&b,native,0))\
      { ans = a operator b;\
        CvtConvertFloat(&ans,native,outp++,dtype,0);\
      } else CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0); \
    }\
  }\
}

#define OperateFloat(type,dtype,native,operator) \
{ type *in1p = (type *)in1->pointer;\
  type *in2p = (type *)in2->pointer;\
  type *outp = (type *)out->pointer;\
  switch (scalars)\
  {\
    case 0: \
    case 3: while (nout--) OperateFloatOne(type,dtype,native,operator,in1p++,in2p++) break;\
    case 1: while (nout--) OperateFloatOne(type,dtype,native,operator,in1p,in2p++) break;\
    case 2: while (nout--) OperateFloatOne(type,dtype,native,operator,in1p++,in2p) break;\
  }\
  break;\
}

#define OperateComplexOne(type,dtype,native,operator) \
    OperateFloatOne(type,dtype,native,operator,&in1p[0],&in2p[0]) \
    OperateFloatOne(type,dtype,native,operator,&in1p[1],&in2p[1])

#define OperateComplex(type,dtype,native,operator) \
{ type *in1p = (type *)in1->pointer;\
  type *in2p = (type *)in2->pointer;\
  type *outp = (type *)out->pointer;\
  switch (scalars)\
  {\
    case 0: \
    case 3: while (nout--) { OperateComplexOne(type,dtype,native,operator) in1p += 2; in2p += 2;} break;\
    case 1: while (nout--) { OperateComplexOne(type,dtype,native,operator) in2p += 2;           } break;\
    case 2: while (nout--) { OperateComplexOne(type,dtype,native,operator) in1p += 2;           } break;\
  }\
  break;\
}

#define MultiplyComplexOne(type,dtype,native,operator) \
    { type a,ai,b,bi,ans,ansi;\
      if (CvtConvertFloat(&in1p[0],dtype,&a,native,0) && CvtConvertFloat(&in1p[1],dtype,&ai,native,0) && \
          CvtConvertFloat(&in2p[0],dtype,&b,native,0) && CvtConvertFloat(&in2p[1],dtype,&bi,native,0)) \
      { ans = a * b - ai * bi;\
        ansi = a * bi + ai * b;\
        CvtConvertFloat(&ans,native,outp++,dtype,0);\
        CvtConvertFloat(&ansi,native,outp++,dtype,0);\
      } else {\
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0); \
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0); }}

#define MultiplyComplex(type,dtype,native) \
{ type *in1p = (type *)in1->pointer;\
  type *in2p = (type *)in2->pointer;\
  type *outp = (type *)out->pointer;\
  switch (scalars)\
  {\
    case 0: \
    case 3: while (nout--) { MultiplyComplexOne(type,dtype,native,operator) in1p += 2; in2p += 2;} break;\
    case 1: while (nout--) { MultiplyComplexOne(type,dtype,native,operator)            in2p += 2;} break;\
    case 2: while (nout--) { MultiplyComplexOne(type,dtype,native,operator) in1p += 2;           } break;\
  }\
  break;\
}

#define Operate128(type,routine) \
{ type##_t *in1p = (type##_t*)in1->pointer;\
  type##_t *in2p = (type##_t*)in2->pointer;\
  type##_t *outp = (type##_t*)out->pointer;\
  switch (scalars)\
  {\
    case 0:\
    case 3: while (nout--) type##_##routine(in1p++, in2p++, outp++); break; \
    case 1: while (nout--) type##_##routine(in1p,   in2p++, outp++); break; \
    case 2: while (nout--) type##_##routine(in1p++, in2p,   outp++); break; \
  }\
  break;\
}

int Tdi3Add(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
    case DTYPE_B:  Operate(  int8_t, +)
    case DTYPE_BU: Operate( uint8_t, +)
    case DTYPE_W:  Operate(uint16_t, +)
    case DTYPE_WU: Operate(uint16_t, +)
    case DTYPE_L:  Operate(uint32_t, +)
    case DTYPE_LU: Operate(uint32_t, +)
    case DTYPE_Q:  Operate( int64_t, +)
    case DTYPE_QU: Operate(uint64_t, +)
    case DTYPE_O:  Operate128( int128,add)
    case DTYPE_OU: Operate128(uint128,add)
    case DTYPE_F:  OperateFloat(float, DTYPE_F, DTYPE_NATIVE_FLOAT, +)
    case DTYPE_FS: OperateFloat(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, +)
    case DTYPE_D:  OperateFloat(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, +)
    case DTYPE_G:  OperateFloat(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, +)
    case DTYPE_FT: OperateFloat(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, +)
    case DTYPE_FC: OperateComplex(float, DTYPE_F, DTYPE_NATIVE_FLOAT, +)
    case DTYPE_FSC:OperateComplex(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, +)
    case DTYPE_GC: OperateComplex(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, +)
    case DTYPE_DC: OperateComplex(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, +)
    case DTYPE_FTC:OperateComplex(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, +)
    default:return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3Subtract(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
    case DTYPE_B:  Operate(  int8_t, -)
    case DTYPE_BU: Operate( uint8_t, -)
    case DTYPE_W:  Operate(uint16_t, -)
    case DTYPE_WU: Operate(uint16_t, -)
    case DTYPE_L:  Operate(uint32_t, -)
    case DTYPE_LU: Operate(uint32_t, -)
    case DTYPE_Q:  Operate( int64_t, -)
    case DTYPE_QU: Operate(uint64_t, -)
    case DTYPE_O:  Operate128( int128,sub)
    case DTYPE_OU: Operate128(uint128,sub)
    case DTYPE_F:  OperateFloat(float, DTYPE_F, DTYPE_NATIVE_FLOAT, -)
    case DTYPE_FS: OperateFloat(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, -)
    case DTYPE_D:  OperateFloat(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, -)
    case DTYPE_G:  OperateFloat(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, -)
    case DTYPE_FT: OperateFloat(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, -)
    case DTYPE_FC: OperateComplex(float, DTYPE_F, DTYPE_NATIVE_FLOAT, -)
    case DTYPE_FSC:OperateComplex(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, -)
    case DTYPE_GC: OperateComplex(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, -)
    case DTYPE_DC: OperateComplex(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, -)
    case DTYPE_FTC:OperateComplex(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, -)
    default:return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3Multiply(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
    case DTYPE_B:  Operate(  int8_t, *)
    case DTYPE_BU: Operate( uint8_t, *)
    case DTYPE_W:  Operate(uint16_t, *)
    case DTYPE_WU: Operate(uint16_t, *)
    case DTYPE_L:  Operate(uint32_t, *)
    case DTYPE_LU: Operate(uint32_t, *)
    case DTYPE_Q:  Operate( int64_t, *)
    case DTYPE_QU: Operate(uint64_t, *)
    case DTYPE_O:  Operate128( int128,mul)
    case DTYPE_OU: Operate128(uint128,mul)
    case DTYPE_F:  OperateFloat(float, DTYPE_F, DTYPE_NATIVE_FLOAT, *)
    case DTYPE_FS: OperateFloat(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, *)
    case DTYPE_D:  OperateFloat(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, *)
    case DTYPE_G:  OperateFloat(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, *)
    case DTYPE_FT: OperateFloat(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, *)
    case DTYPE_FC: MultiplyComplex(float, DTYPE_F, DTYPE_NATIVE_FLOAT)
    case DTYPE_FSC:MultiplyComplex(float, DTYPE_FS, DTYPE_NATIVE_FLOAT)
    case DTYPE_GC: MultiplyComplex(double, DTYPE_G, DTYPE_NATIVE_DOUBLE)
    case DTYPE_DC: MultiplyComplex(double, DTYPE_D, DTYPE_NATIVE_DOUBLE)
    case DTYPE_FTC:MultiplyComplex(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE)
    default:return TdiINVDTYDSC;
  }
  return 1;
}

int TdiMultiplyOctaword(unsigned int *a, unsigned int *b, unsigned int *ans){
  return int128_mul((int128_t*)a,(int128_t*)b,(int128_t*)ans);
}

int TdiAddOctaword(unsigned int *a, unsigned int *b, unsigned int *ans){
  return int128_add((int128_t*)a,(int128_t*)b,(int128_t*)ans);
}

int TdiSubtractOctaword(unsigned int *a, unsigned int *b, unsigned int *ans){
  return int128_sub((int128_t*)a,(int128_t*)b,(int128_t*)ans);
}
