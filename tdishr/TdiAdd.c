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

int TdiMultiplyQuadword();
int TdiMultiplyOctaword();
int TdiAddQuadword();
int TdiAddOctaword();
int TdiSubtractQuadword();
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
  case DTYPE_B:
    Operate(char, +)
    case DTYPE_BU:Operate(unsigned char, +)
    case DTYPE_W:Operate(short, +)
    case DTYPE_WU:Operate(unsigned short, +)
    case DTYPE_L:Operate(int, +)
    case DTYPE_LU:Operate(unsigned int, +)
    case DTYPE_Q:OperateSpecial(8, TdiAddQuadword)
    case DTYPE_QU:OperateSpecial(8, TdiAddQuadword)
    case DTYPE_O:OperateSpecial(16, TdiAddOctaword)
    case DTYPE_OU:OperateSpecial(16, TdiAddOctaword)
    case DTYPE_F:OperateFloat(float, DTYPE_F, DTYPE_NATIVE_FLOAT, +)
    case DTYPE_FS:OperateFloat(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, +)
    case DTYPE_D:OperateFloat(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, +)
    case DTYPE_G:OperateFloat(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, +)
    case DTYPE_FT:OperateFloat(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, +)
    case DTYPE_FC:OperateComplex(float, DTYPE_F, DTYPE_NATIVE_FLOAT, +)
    case DTYPE_FSC:OperateComplex(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, +)
    case DTYPE_GC:OperateComplex(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, +)
    case DTYPE_DC:OperateComplex(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, +)
    case DTYPE_FTC:OperateComplex(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, +)
    default:return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3Subtract(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
    Operate(char, -)
    case DTYPE_BU:Operate(unsigned char, -)
    case DTYPE_W:Operate(short, -)
    case DTYPE_WU:Operate(unsigned short, -)
    case DTYPE_L:Operate(int, -)
    case DTYPE_LU:Operate(unsigned int, -)
    case DTYPE_Q:OperateSpecial(8, TdiSubtractQuadword)
    case DTYPE_QU:OperateSpecial(8, TdiSubtractQuadword)
    case DTYPE_O:OperateSpecial(16, TdiSubtractOctaword)
    case DTYPE_OU:OperateSpecial(16, TdiSubtractOctaword)
    case DTYPE_F:OperateFloat(float, DTYPE_F, DTYPE_NATIVE_FLOAT, -)
    case DTYPE_FS:OperateFloat(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, -)
    case DTYPE_D:OperateFloat(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, -)
    case DTYPE_G:OperateFloat(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, -)
    case DTYPE_FT:OperateFloat(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, -)
    case DTYPE_FC:OperateComplex(float, DTYPE_F, DTYPE_NATIVE_FLOAT, -)
    case DTYPE_FSC:OperateComplex(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, -)
    case DTYPE_GC:OperateComplex(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, -)
    case DTYPE_DC:OperateComplex(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, -)
    case DTYPE_FTC:OperateComplex(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, -)
    default:return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3Multiply(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
    Operate(char, *)
    case DTYPE_BU:Operate(unsigned char, *)
    case DTYPE_W:Operate(short, *)
    case DTYPE_WU:Operate(unsigned short, *)
    case DTYPE_L:Operate(int, *)
    case DTYPE_LU:Operate(unsigned int, *)
    case DTYPE_Q:OperateSpecial(8, TdiMultiplyQuadword)
    case DTYPE_QU:OperateSpecial(8, TdiMultiplyQuadword)
    case DTYPE_O:OperateSpecial(16, TdiMultiplyOctaword)
    case DTYPE_OU:OperateSpecial(16, TdiMultiplyOctaword)
    case DTYPE_F:OperateFloat(float, DTYPE_F, DTYPE_NATIVE_FLOAT, *)
    case DTYPE_FS:OperateFloat(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, *)
    case DTYPE_D:OperateFloat(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, *)
    case DTYPE_G:OperateFloat(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, *)
    case DTYPE_FT:OperateFloat(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, *)
    case DTYPE_FC:MultiplyComplex(float, DTYPE_F, DTYPE_NATIVE_FLOAT)
    case DTYPE_FSC:MultiplyComplex(float, DTYPE_FS, DTYPE_NATIVE_FLOAT)
    case DTYPE_GC:MultiplyComplex(double, DTYPE_G, DTYPE_NATIVE_DOUBLE)
    case DTYPE_DC:MultiplyComplex(double, DTYPE_D, DTYPE_NATIVE_DOUBLE)
    case DTYPE_FTC:MultiplyComplex(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE)
    default:return TdiINVDTYDSC;
  }
  return 1;
}

STATIC_CONSTANT int zero[] = { 0, 0 };

#ifdef __VAX
#define emul lib##$emul
extern int emul();
#else

#ifdef WORDS_BIGENDIAN
#define swapquad(in) {int stmp; int *iptr = (int *)in; stmp=iptr[0]; iptr[0]=iptr[1]; iptr[1]=stmp;}
#define swapocta(in) {int stmp; int *iptr = (int *)in; stmp=iptr[0]; iptr[0]=iptr[3]; iptr[3]=stmp; \
                                                                   stmp=iptr[1]; iptr[1]=iptr[2]; iptr[2]=stmp;}
#else
#define swapquad(in)
#define swapocta(in)
#endif

STATIC_ROUTINE int emul(int *m1, int *m2, int *add, int *out)
{
  *(int64_t *) out = (int64_t) * m1 * (int64_t) * m2 + (int64_t) * add;
  return 1;
}
#endif

int TdiMultiplyQuadword(int *in1, int *in2, int *out)
{
  int64_t *arg1 = (int64_t *) in1;
  int64_t *arg2 = (int64_t *) in2;
  *((int64_t *) out) = *arg1 * *arg2;
  return 1;
  /*

     int tmp[3] = {0,0,0};
     int in1l[2];
     int in2l[2];
     #ifdef WORDS_BIGENDIAN
     in1l[0] = in1[1];
     in1l[1] = in1[0];
     in2l[0] = in2[1];
     in2l[1] = in2[0];
     #else
     in1l[0] = in1[0];
     in1l[1] = in1[1];
     in2l[0] = in2[0];
     in2l[1] = in2[1];
     #endif
     emul(&in2l[0],&in1l[0],zero,&tmp[0]);
     swapquad(&tmp[0])
     emul(&in2l[1],&in1l[0],&tmp[1],&tmp[1]);
     swapquad(&tmp[1])
     emul(&in2l[0],&in1l[1],&tmp[1],&tmp[1]);
     swapquad(&tmp[1])
     if (in1l[0] < 0)
     tmp[1] += in2l[0];
     if (in2l[0] < 0)
     tmp[1] += in1l[0];
     swapquad(&tmp[0])
     memcpy(out,tmp,8);
   */
  return 1;
}

int TdiMultiplyOctaword(int *in1, int *in2, int *out)
{
  int tmp[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  int in1l[4];
  int in2l[4];
  int tmp2[4];
  int tmp3[4];

#ifdef WORDS_BIGENDIAN
  in1l[0] = in1[3];
  in1l[1] = in1[2];
  in1l[2] = in1[1];
  in1l[3] = in1[0];
  in2l[0] = in2[3];
  in2l[1] = in2[2];
  in2l[2] = in2[1];
  in2l[3] = in2[0];
#else
  in1l[0] = in1[0];
  in1l[1] = in1[1];
  in1l[2] = in1[2];
  in1l[3] = in1[3];
  in2l[0] = in2[0];
  in2l[1] = in2[1];
  in2l[2] = in2[2];
  in2l[3] = in2[3];
#endif

  emul(&in2l[0], in1l, zero, &tmp[0]);
  swapquad(&tmp[0]);
  emul(&in2l[1], in1l, &tmp[1], &tmp[1]);
  swapquad(&tmp[1]);
  emul(&in2l[2], in1l, &tmp[2], &tmp[2]);
  swapquad(&tmp[2]);
  emul(&in2l[3], in1l, &tmp[3], &tmp[3]);
  swapquad(&tmp[3]);
  emul(&in2l[0], &in1l[1], zero, &tmp[4]);
  swapquad(&tmp[4]);
  emul(&in2l[1], &in1l[1], &tmp[5], &tmp[5]);
  swapquad(&tmp[5]);
  emul(&in2l[2], &in1l[1], &tmp[6], &tmp[6]);
  swapquad(&tmp[6]);
  memcpy(tmp2, &tmp[4], 16);
  memcpy(tmp3, &tmp[1], 16);
  swapocta(tmp2);
  swapocta(tmp3);
  TdiAddOctaword(tmp2, tmp3, &tmp[1]);
  swapocta(&tmp[1]);
  emul(&in2l[0], &in1l[2], zero, &tmp[4]);
  swapquad(&tmp[4]);
  emul(&in2l[1], &in1l[2], &tmp[5], &tmp[5]);
  swapquad(&tmp[5]);
  memcpy(tmp2, &tmp[4], 8);
  memcpy(tmp3, &tmp[2], 8);
  swapquad(tmp2);
  swapquad(tmp3);
  TdiAddQuadword(tmp2, tmp3, &tmp[2]);
  swapquad(&tmp[2]);
  emul(&in2l[0], &in1l[3], &tmp[3], &tmp[3]);
  swapquad(&tmp[3]);
  if (in1l[0] < 0) {
    tmp[7] = tmp[4];
    memcpy(tmp2, &tmp[1], 16);
    swapocta(tmp2);
    TdiAddOctaword(in2, tmp2, &tmp[1]);
    swapocta(&tmp[1]);
    tmp[4] = tmp[7];
  }
  if (in1l[1] < 0) {
    memcpy(tmp2, &tmp[2], 8);
    swapquad(tmp2);
#ifdef WORDS_BIGENDIAN
    TdiAddQuadword(in2 + 2, tmp2, &tmp[2]);
#else
    TdiAddQuadword(in2, tmp2, &tmp[2]);
#endif
    swapquad(&tmp[2]);
  }
  if (in1l[2] < 0)
    tmp[3] += in2l[0];
  if (in2l[0] < 0) {
    tmp[7] = tmp[4];
    memcpy(tmp2, &tmp[1], 16);
    swapocta(tmp2);
    TdiAddOctaword(in1, tmp2, &tmp[1]);
    swapocta(&tmp[1]);
    tmp[4] = tmp[7];
  }
  if (in2l[1] < 0) {
    memcpy(tmp2, &tmp[2], 8);
    swapquad(tmp2);
#ifdef WORDS_BIGENDIAN
    TdiAddQuadword(in1 + 2, tmp2, &tmp[2]);
#else
    TdiAddQuadword(in1, tmp2, &tmp[2]);
#endif
    swapquad(&tmp[2]);
  }
  if (in2l[2] < 0)
    tmp[3] += in1l[0];
#ifdef WORDS_BIGENDIAN
  out[0] = tmp[3];
  out[1] = tmp[2];
  out[2] = tmp[1];
  out[3] = tmp[0];
#else
  out[0] = tmp[0];
  out[1] = tmp[1];
  out[2] = tmp[2];
  out[3] = tmp[3];
#endif
  return 1;
}

int TdiAddQuadword(unsigned int *a, unsigned int *b, unsigned int *ans)
{
  uint64_t *arg1 = (uint64_t *) a;
  uint64_t *arg2 = (uint64_t *) b;
  uint64_t *out = (uint64_t *) ans;
  *out = *arg1 + *arg2;
#if defined(_MSC_VER) && _MSC_VER <= 1300
  return (*out & 0x800000000000000U i64) != 0;
#else
  return (*out & 0x800000000000000ULL) != 0;
#endif
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

int TdiSubtractQuadword(unsigned int *a, unsigned int *b, unsigned int *ans)
{
  int i;
  unsigned int lb[2];
  unsigned int sub[2];
  lb[0] = b[0];
  lb[1] = b[1];
  swapquad(lb);
  for (i = 0; i < 2; i++) {
    sub[i] = ~lb[i];
    if (i == 0)
      sub[i]++;
    else if (sub[i - 1] < (~lb[i - 1]))
      sub[i]++;
  }
  swapquad(sub);
  return TdiAddQuadword(a, sub, ans);
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
