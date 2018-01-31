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



extern int CvtConvertFloat();

int TdiMultiplyOctaword();
int TdiAddOctaword();
int TdiSubtractOctaword();

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

#define OperateSpecial(size,routine) \
{ char *in1p = in1->pointer;\
  char *in2p = in2->pointer;\
  char *outp = out->pointer;\
  switch (scalars)\
  {\
    case 0:\
    case 3: while (nout--) {routine(in1p, in2p, outp); in1p += size; in2p += size; outp += size; } break; \
    case 1: while (nout--) {routine(in1p, in2p, outp);                 in2p += size; outp += size; } break; \
    case 2: while (nout--) {routine(in1p, in2p, outp);   in1p += size;               outp += size; } break; \
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
    case DTYPE_O:  OperateSpecial(16, TdiAddOctaword)
    case DTYPE_OU: OperateSpecial(16, TdiAddOctaword)
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
    case DTYPE_O:  OperateSpecial(16, TdiSubtractOctaword)
    case DTYPE_OU: OperateSpecial(16, TdiSubtractOctaword)
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
    case DTYPE_O:  OperateSpecial(16, TdiMultiplyOctaword)
    case DTYPE_OU: OperateSpecial(16, TdiMultiplyOctaword)
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

#ifndef __VAX
 #ifdef WORDS_BIGENDIAN
  #define swapquad(in) {int stmp; int *iptr = (int *)in; stmp=iptr[0]; iptr[0]=iptr[1]; iptr[1]=stmp;}
  #define swapocta(in) {int stmp; int *iptr = (int *)in; stmp=iptr[0]; iptr[0]=iptr[3]; iptr[3]=stmp; \
                                                         stmp=iptr[1]; iptr[1]=iptr[2]; iptr[2]=stmp;}
 #else
  #define swapquad(in)
  #define swapocta(in)
 #endif
#endif

#define HI_WORD 0xFFFFFFFF00000000LL
#define LO_WORD 0x00000000FFFFFFFFLL
typedef struct int28_s{
  int64_t low;
 uint64_t high;
} int128_t;
int TdiMultiplyOctaword(int128_t *xi, int128_t *yi, int128_t *rv2){
  /* as by 128-bit integer arithmetic for C++, by Robert Munafo */
  swapocta(*xi);
  swapocta(*yi);

  uint64_t acc, ac2, carry, o1, o2;
  uint64_t a, b, c, d, e, f, g, h;

/************************
 x      a  b  c  d
 y      e  f  g  h
-------------------------
        -o2-  -o1-
 ************************/

  d =  xi->low  & LO_WORD;
  c = (xi->low  & HI_WORD) >> 32LL;
  b =  xi->high & LO_WORD;
  a = (xi->high & HI_WORD) >> 32LL;

  h =  yi->low  & LO_WORD;
  g = (yi->low  & HI_WORD) >> 32LL;
  f =  yi->high & LO_WORD;
  e = (yi->high & HI_WORD) >> 32LL;

  acc = d * h;
  o1  = acc & LO_WORD;
  acc >>= 32LL;
  carry = 0;
  ac2 = acc + c * h; if (ac2 < acc) { carry++; }
  acc = ac2 + d * g; if (acc < ac2) { carry++; }
  rv2->low = o1 | (acc << 32LL);
  ac2 = (acc >> 32LL) | (carry << 32LL); carry = 0;

  acc = ac2 + b * h; if (acc < ac2) { carry++; }
  ac2 = acc + c * g; if (ac2 < acc) { carry++; }
  acc = ac2 + d * f; if (acc < ac2) { carry++; }
  o2  = acc & LO_WORD;
  ac2 = (acc >> 32LL) | (carry << 32LL);

  acc = ac2 + a * h;
  ac2 = acc + b * g;
  acc = ac2 + c * f;
  ac2 = acc + d * e;
  rv2->high = (ac2 << 32LL) | o2;
  swapocta(rv2);
  return 1;
}

int TdiAddOctaword(unsigned int *a, unsigned int *b, unsigned int *ans)
{
  int i;
  int carry = 0;
  unsigned int la[4];
  unsigned int lb[4];
  memcpy(la, a, 16);
  memcpy(lb, b, 16);
  swapocta(la);
  swapocta(lb);
  for (i = 0; i < 4; i++) {
    unsigned int _a = la[i];
    unsigned int _b = lb[i];
    ans[i] = _a + _b + carry;
    carry = (ans[i] <= _a) && ((_b != 0) || (carry != 0));
  }
  swapocta(ans);
  return !carry;
}

int TdiSubtractOctaword(unsigned int *a, unsigned int *b, unsigned int *ans)
{
  int i;
  unsigned int lb[4];
  unsigned int sub[4];
  memcpy(lb, b, 16);
  swapocta(lb);
  for (i = 0; i < 4; i++) {
    sub[i] = ~lb[i];
    if (i == 0)
      sub[i]++;
    else if (sub[i - 1] < (~lb[i - 1]))
      sub[i]++;
  }
  swapocta(sub);
  return TdiAddOctaword(a, sub, ans);
}
