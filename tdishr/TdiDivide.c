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

                Name:   Tdi3Divide

                Type:   C function

                Author: TOM FREDIAN

                Date:   5-OCT-1993

                Purpose: Divide two operands

------------------------------------------------------------------------------

        Call sequence:

int Tdi3Divide(struct descriptor *in1, struct descriptor *in2, struct descriptor
*out)

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

------------------------------------------------------------------------------*/

#include <int128.h>
#include <mdsdescrip.h>
#include <mdstypes.h>
#include <tdishr_messages.h>

extern int CvtConvertFloat();
extern double WideIntToDouble();
extern void DoubleToWideInt();

static const int roprand = 0x8000;

#define SetupArgs                                         \
  struct descriptor_a *ina1 = (struct descriptor_a *)in1; \
  struct descriptor_a *ina2 = (struct descriptor_a *)in2; \
  struct descriptor_a *outa = (struct descriptor_a *)out; \
  int scalars = 0;                                        \
  int nout;                                               \
  switch (out->class)                                     \
  {                                                       \
  case CLASS_S:                                           \
  case CLASS_D:                                           \
    nout = 1;                                             \
    break;                                                \
  case CLASS_A:                                           \
    nout = outa->arsize / outa->length;                   \
    if (nout == 0)                                        \
      return 1;                                           \
    break;                                                \
  default:                                                \
    return TdiINVCLADSC;                                  \
  }                                                       \
  switch (in1->class)                                     \
  {                                                       \
  case CLASS_S:                                           \
  case CLASS_D:                                           \
    scalars |= 1;                                         \
    break;                                                \
  case CLASS_A:                                           \
    if (ina1->arsize / ina1->length < (unsigned int)nout) \
      return TdiINV_SIZE;                                 \
    break;                                                \
  default:                                                \
    return TdiINVCLADSC;                                  \
  }                                                       \
  switch (in2->class)                                     \
  {                                                       \
  case CLASS_S:                                           \
  case CLASS_D:                                           \
    if (scalars && (nout > 1))                            \
      return TdiINV_SIZE;                                 \
    scalars |= 2;                                         \
    break;                                                \
  case CLASS_A:                                           \
    if (ina2->arsize / ina2->length < (unsigned int)nout) \
      return TdiINV_SIZE;                                 \
    break;                                                \
  default:                                                \
    return TdiINVCLADSC;                                  \
  }

#define Operate(type, operator)                             \
  {                                                         \
    type *in1p = (type *)in1->pointer;                      \
    type *in2p = (type *)in2->pointer;                      \
    type *outp = (type *)out->pointer;                      \
    switch (scalars)                                        \
    {                                                       \
    case 0:                                                 \
    case 3:                                                 \
      while (nout--)                                        \
      {                                                     \
        if (*in2p) {                                        \
          *outp++ = (type)(*in1p operator *in2p);           \
        } else {                                            \
          return TdiDIV_BY_ZERO;                            \
        }                                                   \
        in1p++;                                             \
        in2p++;                                             \
      }                                                     \
      break;                                                \
    case 1:                                                 \
      while (nout--)                                        \
      {                                                     \
        if (*in2p) {                                        \
          *outp++ = (type)(*in1p operator *in2p);           \
        } else {                                            \
          return TdiDIV_BY_ZERO;                            \
        }                                                   \
        in2p++;                                             \
      }                                                     \
      break;                                                \
    case 2:                                                 \
      while (nout--)                                        \
      {                                                     \
        if (*in2p) {                                        \
          *outp++ = (type)(*in1p operator *in2p);           \
        } else {                                            \
          return TdiDIV_BY_ZERO;                            \
        }                                                   \
        in1p++;                                             \
      }                                                     \
      break;                                                \
    }                                                       \
    break;                                                  \
  }

#define OperateFloatOne(type, dtype, native, routine)          \
  if (CvtConvertFloat(in1p, dtype, &a, native, 0) &&           \
      CvtConvertFloat(in2p, dtype, &b, native, 0) && b != 0.0) \
  {                                                            \
    ans = routine(a, b);                                       \
    CvtConvertFloat(&ans, native, outp++, dtype, 0);           \
  }                                                            \
  else                                                         \
    CvtConvertFloat(&roprand, DTYPE_F, outp++, dtype, 0);

#define OperateFloat(type, dtype, native, routine)            \
  {                                                           \
    type *in1p = (type *)in1->pointer;                        \
    type *in2p = (type *)in2->pointer;                        \
    type *outp = (type *)out->pointer;                        \
    type a, b, ans;                                           \
    switch (scalars)                                          \
    {                                                         \
    case 0:                                                   \
    case 3:                                                   \
      while (nout--)                                          \
      {                                                       \
        OperateFloatOne(type, dtype, native, routine) in1p++; \
        in2p++;                                               \
      }                                                       \
      break;                                                  \
    case 1:                                                   \
      while (nout--)                                          \
      {                                                       \
        OperateFloatOne(type, dtype, native, routine) in2p++; \
      }                                                       \
      break;                                                  \
    case 2:                                                   \
      while (nout--)                                          \
      {                                                       \
        OperateFloatOne(type, dtype, native, routine) in1p++; \
      }                                                       \
      break;                                                  \
    }                                                         \
    break;                                                    \
  }

#define Divide128(type)                        \
  {                                            \
    type##_t *in1p = (type##_t *)in1->pointer; \
    type##_t *in2p = (type##_t *)in2->pointer; \
    type##_t *outp = (type##_t *)out->pointer; \
    switch (scalars)                           \
    {                                          \
    case 0:                                    \
      while (nout--)                           \
        type##_div(in1p++, in2p++, outp++);    \
      break;                                   \
    case 1:                                    \
      while (nout--)                           \
        type##_div(in1p, in2p++, outp++);      \
      break;                                   \
    case 2:                                    \
      while (nout--)                           \
        type##_div(in1p, in2p++, outp++);      \
      break;                                   \
    case 3:                                    \
      type##_div(in1p, in2p, outp);            \
      break;                                   \
    }                                          \
    break;                                     \
  }

#define DivideComplexOne(type, dtype)                                 \
  if (CvtConvertFloat(&in1p[0], dtype, &a, DTYPE_NATIVE_DOUBLE, 0) && \
      CvtConvertFloat(&in1p[1], dtype, &b, DTYPE_NATIVE_DOUBLE, 0) && \
      CvtConvertFloat(&in2p[0], dtype, &c, DTYPE_NATIVE_DOUBLE, 0) && \
      CvtConvertFloat(&in2p[1], dtype, &d, DTYPE_NATIVE_DOUBLE, 0))   \
  {                                                                   \
    double abs_c = (c > 0.0) ? c : -c;                                \
    double abs_d = (d > 0.0) ? d : -d;                                \
    double r, den, e, f;                                              \
    if (abs_c >= abs_d)                                               \
    {                                                                 \
      r = d / c;                                                      \
      den = c + r * d;                                                \
      e = (a + b * r) / den;                                          \
      f = (b - a * r) / den;                                          \
    }                                                                 \
    else                                                              \
    {                                                                 \
      r = c / d;                                                      \
      den = d + r * c;                                                \
      e = (a * r + b) / den;                                          \
      f = (b * r - a) / den;                                          \
    }                                                                 \
    CvtConvertFloat(&e, DTYPE_NATIVE_DOUBLE, outp++, dtype, 0);       \
    CvtConvertFloat(&f, DTYPE_NATIVE_DOUBLE, outp++, dtype, 0);       \
  }                                                                   \
  else                                                                \
  {                                                                   \
    CvtConvertFloat(&roprand, DTYPE_F, outp++, dtype, 0);             \
    CvtConvertFloat(&roprand, DTYPE_F, outp++, dtype, 0);             \
  }

#define DivideComplex(type, dtype)                \
  {                                               \
    type *in1p = (type *)in1->pointer;            \
    type *in2p = (type *)in2->pointer;            \
    type *outp = (type *)out->pointer;            \
    double a, b, c, d;                            \
    switch (scalars)                              \
    {                                             \
    case 0:                                       \
    case 3:                                       \
      while (nout--)                              \
      {                                           \
        DivideComplexOne(type, dtype) in1p += 2; \
        in2p += 2;                                \
      }                                           \
      break;                                      \
    case 1:                                       \
      while (nout--)                              \
      {                                           \
        DivideComplexOne(type, dtype) in2p += 2; \
      }                                           \
      break;                                      \
    case 2:                                       \
      while (nout--)                              \
      {                                           \
        DivideComplexOne(type, dtype) in1p += 2; \
      }                                           \
      break;                                      \
    }                                             \
    break;                                        \
  }

#define divide(a, b) ((a) / (b))

int Tdi3Divide(struct descriptor *in1, struct descriptor *in2,
               struct descriptor *out)
{
  SetupArgs;
  switch (in1->dtype)
  {
  case DTYPE_B:
    Operate(char, /);
  case DTYPE_BU:
    Operate(unsigned char, /);
  case DTYPE_W:
    Operate(short, /);
  case DTYPE_WU:
    Operate(unsigned short, /);
  case DTYPE_L:
    Operate(int, /);
  case DTYPE_LU:
    Operate(unsigned int, /);
  case DTYPE_Q:
    Operate(int64_t, /);
  case DTYPE_QU:
    Operate(uint64_t, /);
  case DTYPE_O:
    Divide128(int128);
  case DTYPE_OU:
    Divide128(uint128);
  case DTYPE_F:
    OperateFloat(float, DTYPE_F, DTYPE_NATIVE_FLOAT, divide)
  case DTYPE_FS:
    OperateFloat(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, divide);
  case DTYPE_G:
    OperateFloat(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, divide);
  case DTYPE_D:
    OperateFloat(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, divide);
  case DTYPE_FT:
    OperateFloat(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, divide);
  case DTYPE_FC:
    DivideComplex(float, DTYPE_F);
  case DTYPE_FSC:
    DivideComplex(float, DTYPE_FS);
  case DTYPE_GC:
    DivideComplex(double, DTYPE_G);
  case DTYPE_DC:
    DivideComplex(double, DTYPE_D);
  case DTYPE_FTC:
    DivideComplex(double, DTYPE_FT);
  default:
    return TdiINVDTYDSC;
  }
  return 1;
}

static inline double mod_float(double x, double m)
{
  if (m == 0.0)
    return x;
  double intpart;
  modf(x / m, &intpart);
  return x - intpart * m;
}

static void mod_bin(int size, int is_signed, char *in1, char *in2, char *out)
{
  double in2_d = WideIntToDouble(in2, size / sizeof(int), is_signed);
  double in1_d = WideIntToDouble(in1, size / sizeof(int), is_signed);
  double ans = mod_float(in1_d, in2_d);
  DoubleToWideInt(&ans, size / sizeof(int), out);
}

#define OperateBin(size, is_signed, routine)        \
  {                                                 \
    char *in1p = in1->pointer;                      \
    char *in2p = in2->pointer;                      \
    char *outp = out->pointer;                      \
    switch (scalars)                                \
    {                                               \
    case 0:                                         \
    case 3:                                         \
      while (nout--)                                \
      {                                             \
        routine(size, is_signed, in1p, in2p, outp); \
        in1p += size;                               \
        in2p += size;                               \
        outp += size;                               \
      }                                             \
      break;                                        \
    case 1:                                         \
      while (nout--)                                \
      {                                             \
        routine(size, is_signed, in1p, in2p, outp); \
        in2p += size;                               \
        outp += size;                               \
      }                                             \
      break;                                        \
    case 2:                                         \
      while (nout--)                                \
      {                                             \
        routine(size, is_signed, in1p, in2p, outp); \
        in1p += size;                               \
        outp += size;                               \
      }                                             \
      break;                                        \
    }                                               \
    break;                                          \
  }

int Tdi3Mod(struct descriptor *in1, struct descriptor *in2,
            struct descriptor *out)
{
  SetupArgs;
  switch (in1->dtype)
  {
  case DTYPE_B:
    Operate(int8_t, %);
  case DTYPE_BU:
    Operate(uint8_t, %);
  case DTYPE_W:
    Operate(int16_t, %)
  case DTYPE_WU:
    Operate(uint16_t, %);
  case DTYPE_L:
    Operate(int32_t, %)
  case DTYPE_LU:
    Operate(uint32_t, %);
  case DTYPE_Q:
    Operate(int64_t, %)
  case DTYPE_QU:
    Operate(uint64_t, %);
  case DTYPE_O:
    OperateBin(in1->length, 1, mod_bin);
  case DTYPE_OU:
    OperateBin(in1->length, 0, mod_bin);
  case DTYPE_F:
    OperateFloat(float, DTYPE_F, DTYPE_NATIVE_FLOAT, mod_float);
  case DTYPE_FS:
    OperateFloat(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, mod_float);
  case DTYPE_D:
    OperateFloat(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, mod_float);
  case DTYPE_G:
    OperateFloat(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, mod_float);
  case DTYPE_FT:
    OperateFloat(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, mod_float);
  default:
    return TdiINVDTYDSC;
  }
  return 1;
}
