/*------------------------------------------------------------------------------

                Name:   Tdi3Dim

                Type:   C function

                Author: MARK LONDON

                Date:   5-OCT-1993 

                Purpose:
 
        Routines for bit change of data. in2 is ASSUMED long.
        any     = IBCLR(any,L)
        any     = IBSET(any,L)
        BU      = BTEST(any,L)
        Routine to join 2 real numbers into a complex number.
        Real part of complex numbers used.
        complex = F_COMPLEX([r or c], [r or c])
        complex = D_COMPLEX([r or c], [r or c])
        complex = G_COMPLEX([r or c], [r or c])
        complex = H_COMPLEX([r or c], [r or c])
        complex = CMPLX([r or c], [r or c], [mold])
        Routines to diminish and find pair-wise integer/real maximum/minimum.
        Not defined for complex numbers.
        i or r  = DIM(i or r,same)
        i or r  = MAX(i or r,same)
        i or r  = MIN(i or r,same)
        Routine to shift logical integers. in2 assumes integer, uses byte.
        SHIFT_LEFT/RIGHT is logical unsigned or arithmetic signed.
        integer = ISHFT(integer,byte)
        integer = SHIFT_LEFT(integer,byte)
        integer = SHIFT_RIGHT(integer,byte)

------------------------------------------------------------------------------

        Call sequence: 

int Tdi3xxxxx(struct descriptor *in1, struct descriptor *in2,
               struct descriptor *out)

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

 
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#include <mdstypes.h>
#include <mdsdescrip.h>
#include <tdishr_messages.h>
#include <STATICdef.h>



extern int TdiConvert();
extern int Tdi3Subtract();
extern int CvtConvertFloat();

STATIC_CONSTANT int roprand = 0x8000;
typedef struct {
  int longword[2];
} quadword;
typedef struct {
  int longword[4];
} octaword;

extern int TdiGtO();
extern int TdiLtO();
extern int TdiLtQ();
extern int TdiGtQ();

#define SetupArgs \
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
  }

#define conv(data_type)\
{DESCRIPTOR_A(i1, sizeof(double), data_type , 0, 0);\
 DESCRIPTOR_A(i2, sizeof(double), data_type , 0, 0);\
 DESCRIPTOR_A(o, sizeof(double), data_type , 0, 0);\
 double *i1p = malloc(((scalars & 1) ? 1 : nout)*sizeof(double));\
 double *i2p = malloc(((scalars & 1) ? 1 : nout)*sizeof(double));\
 double *op = malloc(nout*sizeof(double));\
 i1.arsize = (scalars & 1) ? 1 : nout;\
 i2.arsize = (scalars & 2) ? 1 : nout; o.arsize = nout;\
 i1.pointer = (char *)i1p; i2.pointer = (char *)i2p; o.pointer = (char *)op;\
 TdiConvert(in1,&i1); TdiConvert(in2,&i2);\
 {struct descriptor_a *in1 = (struct descriptor_a *)&i1;\
  struct descriptor_a *in2 = (struct descriptor_a *)&i2;\
  struct descriptor_a *out = (struct descriptor_a *)&o;

#define unconv } TdiConvert(&o,out); free(i1p); free(i2p); free(op);}

#ifdef WORDS_BIGENDIAN
#define _offset size - *in2p/8 - 1
#else
#define _offset *in2p/8
#endif

int Tdi3Ibset(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  int size;
  SetupArgs TdiConvert(in1, out);
  switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    size = sizeof(char);
    break;
  case DTYPE_W:
  case DTYPE_WU:
    size = sizeof(short);
    break;
  case DTYPE_L:
  case DTYPE_LU:
    size = sizeof(int);
    break;
  case DTYPE_Q:
  case DTYPE_QU:
    size = sizeof(quadword);
    break;
  case DTYPE_O:
  case DTYPE_OU:
    size = sizeof(octaword);
    break;
  case DTYPE_F:
  case DTYPE_FS:
    size = sizeof(float);
    break;
  case DTYPE_D:
  case DTYPE_G:
  case DTYPE_FT:
    size = sizeof(double);
    break;
  case DTYPE_FC:
  case DTYPE_FSC:
    size = sizeof(float) * 2;
    break;
  case DTYPE_DC:
  case DTYPE_GC:
  case DTYPE_FTC:
    size = sizeof(double) * 2;
    break;
  default:
    return TdiINVDTYDSC;
  }

  {
    int *in2p = (int *)in2->pointer;
    char *outp = (char *)out->pointer;
    switch (scalars) {
    case 0:
    case 1:
      while (nout--) {
	if (*in2p < size * 8)
	  *(outp + _offset) |= 1 << (*in2p % 8);
	in2p++;
	outp += size;
      }
      break;
    case 2:
      while (nout--) {
	if (*in2p < size * 8)
	  *(outp + _offset) |= 1 << (*in2p % 8);
	outp += size;
      }
      break;
    case 3:
      if (*in2p < size * 8)
	*(outp + _offset) |= 1 << (*in2p % 8);
      break;
    }
  }
  return 1;
}

int Tdi3Ibclr(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  int size;
  SetupArgs TdiConvert(in1, out);
  switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    size = sizeof(char);
    break;
  case DTYPE_W:
  case DTYPE_WU:
    size = sizeof(short);
    break;
  case DTYPE_L:
  case DTYPE_LU:
    size = sizeof(int);
    break;
  case DTYPE_Q:
  case DTYPE_QU:
    size = sizeof(int) * 2;
    break;
  case DTYPE_O:
  case DTYPE_OU:
    size = sizeof(int) * 4;
    break;
  case DTYPE_F:
  case DTYPE_FS:
    size = sizeof(float);
    break;
  case DTYPE_D:
  case DTYPE_G:
  case DTYPE_FT:
    size = sizeof(double);
    break;
  case DTYPE_FC:
  case DTYPE_FSC:
    size = sizeof(float) * 2;
    break;
  case DTYPE_DC:
  case DTYPE_GC:
  case DTYPE_FTC:
    size = sizeof(double) * 2;
    break;
  default:
    return TdiINVDTYDSC;
  }

  {
    int *in2p = (int *)in2->pointer;
    char *outp = (char *)out->pointer;
    switch (scalars) {
    case 0:
    case 1:
      while (nout--) {
	if (*in2p < size * 8)
	  *(outp + _offset) &= ~(1 << (*in2p % 8));
	in2p++;
	outp += size;
      }
      break;
    case 2:
      while (nout--) {
	if (*in2p < size * 8)
	  *(outp + _offset) &= ~(1 << (*in2p % 8));
	outp += size;
      }
      break;
    case 3:
      if (*in2p < size * 8)
	*(outp + _offset) &= ~(1 << (*in2p % 8));
      break;
    }
  }
  return 1;
}

int Tdi3Btest(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  int size;
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    size = sizeof(char);
    break;
  case DTYPE_W:
  case DTYPE_WU:
    size = sizeof(short);
    break;
  case DTYPE_L:
  case DTYPE_LU:
    size = sizeof(int);
    break;
  case DTYPE_Q:
  case DTYPE_QU:
    size = sizeof(int) * 2;
    break;
  case DTYPE_O:
  case DTYPE_OU:
    size = sizeof(int) * 4;
    break;
  case DTYPE_F:
  case DTYPE_FS:
    size = sizeof(float);
    break;
  case DTYPE_D:
  case DTYPE_G:
  case DTYPE_FT:
    size = sizeof(double);
    break;
  case DTYPE_FC:
  case DTYPE_FSC:
    size = sizeof(float) * 2;
    break;
  case DTYPE_DC:
  case DTYPE_GC:
  case DTYPE_FTC:
    size = sizeof(double) * 2;
    break;
  default:
    return TdiINVDTYDSC;
  }

  {
    char *in1p = (char *)in1->pointer;
    int *in2p = (int *)in2->pointer;
    char *outp = (char *)out->pointer;
    switch (scalars) {
    case 0:
      while (nout--) {
	if (*in2p < size * 8)
	  *outp = (char)((*(in1p + *in2p / 8) & (1 << (*in2p % 8))) > 0);
	in1p += size;
	outp++;
	in2p++;
      } break;
    case 1:
      while (nout--) {
	if (*in2p < size * 8)
	  *outp = (char)((*(in1p + *in2p / 8) & (1 << (*in2p % 8))) > 0);
	outp++;
	in2p++;
      } break;
    case 2:
      while (nout--) {
	if (*in2p < size * 8)
	  *outp = (char)((*(in1p + *in2p / 8) & (1 << (*in2p % 8))) > 0);
	in1p++;
	outp++;
      } break;
    case 3:
      if (*in2p < size * 8)
	*outp = (char)((*(in1p + *in2p / 8) & (1 << (*in2p % 8))) > 0);
      break;
    }
  }
  return 1;
}

int Tdi3Complex(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  int size, isize;
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
  case DTYPE_W:
  case DTYPE_WU:
  case DTYPE_L:
  case DTYPE_LU:
  case DTYPE_Q:
  case DTYPE_QU:
  case DTYPE_O:
  case DTYPE_OU:
    return TdiINVDTYDSC;
  case DTYPE_F:
  case DTYPE_FS:
    size = sizeof(float);
    isize = size;
    break;
  case DTYPE_D:
  case DTYPE_G:
  case DTYPE_FT:
    size = sizeof(double);
    isize = size;
    break;
  case DTYPE_FC:
  case DTYPE_FSC:
    size = sizeof(float);
    isize = 2 * size;
    break;
  case DTYPE_DC:
  case DTYPE_GC:
  case DTYPE_FTC:
    size = sizeof(double);
    isize = 2 * size;
    break;
  default:
    return TdiINVDTYDSC;
  }

  {
    char *in1p = (char *)in1->pointer;
    char *in2p = (char *)in2->pointer;
    char *outp = (char *)out->pointer;
    switch (scalars) {
    case 0:
      while (nout--) {
	memmove(outp, in1p, size);
	outp += size;
	memmove(outp, in2p, size);
	outp += size;
	in1p += isize;
	in2p += isize;
      }
      break;
    case 1:
      while (nout--) {
	memmove(outp, in1p, size);
	outp += size;
	memmove(outp, in2p, size);
	outp += size;
	in2p += isize;
      }
      break;
    case 2:
      while (nout--) {
	memmove(outp, in1p, size);
	outp += size;
	memmove(outp, in2p, size);
	outp += size;
	in1p += isize;
      }
      break;
    case 3:
      memmove(outp, in1p, size);
      outp += size;
      memmove(outp, in2p, size);
      break;
    }
  }
  return 1;
}

#define Operate(type,testit) \
{ type *in1p = (type *)in1->pointer;\
  type *in2p = (type *)in2->pointer;\
  type *outp = (type *)out->pointer;\
  switch (scalars)\
  {\
    case 0: \
    case 3: while (nout--) { *outp++ = *((testit) ? in1p : in2p); in1p++; in2p++; } break;\
    case 1: while (nout--) { *outp++ = *((testit) ? in1p : in2p);         in2p++; } break;\
    case 2: while (nout--) { *outp++ = *((testit) ? in1p : in2p); in1p++;         } break;\
  }\
  break;\
}

#define OperateF(type,dtype,native,testit) \
{ type *in1p = (type *)in1->pointer;\
  type *in2p = (type *)in2->pointer;\
  type *outp = (type *)out->pointer;\
  type a,b;\
  switch (scalars)\
  {\
    case 0: \
    case 3: while (nout--) { if (CvtConvertFloat(in1p,dtype,&a,native,0) && CvtConvertFloat(in2p,dtype,&b,native,0)) \
                             { type ans = (testit) ? a : b; CvtConvertFloat(&ans,native,outp++,dtype,0);}\
                             else CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0); in1p++; in2p++; } break;\
    case 1: while (nout--) { if (CvtConvertFloat(in1p,dtype,&a,native,0) && CvtConvertFloat(in2p,dtype,&b,native,0)) \
                             { type ans = (testit) ? a : b; CvtConvertFloat(&ans,native,outp++,dtype,0);}\
                             else CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);         in2p++; } break;\
    case 2: while (nout--) { if (CvtConvertFloat(in1p,dtype,&a,native,0) && CvtConvertFloat(in2p,dtype,&b,native,0)) \
                             { type ans = (testit) ? a : b; CvtConvertFloat(&ans,native,outp++,dtype,0);}\
                             else CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0); in1p++;         } break;\
  }\
  break;\
}

int Tdi3Max(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_T:
    return TdiINVDTYDSC;
  case DTYPE_B:
    Operate(char, *in1p > *in2p)
    case DTYPE_BU:Operate(unsigned char, *in1p > *in2p)
    case DTYPE_W:Operate(short, *in1p > *in2p)
    case DTYPE_WU:Operate(unsigned short, *in1p > *in2p)
    case DTYPE_L:Operate(int, *in1p > *in2p)
    case DTYPE_LU:Operate(unsigned int, *in1p > *in2p)
    case DTYPE_QU:Operate(quadword, TdiGtQ(in1p, in2p, 0))
    case DTYPE_Q:Operate(quadword, TdiGtQ(in1p, in2p, 1))
    case DTYPE_OU:Operate(octaword, TdiGtO(in1p, in2p, 0))
    case DTYPE_O:Operate(octaword, TdiGtO(in1p, in2p, 1))
    case DTYPE_F:OperateF(float, DTYPE_F, DTYPE_NATIVE_FLOAT, a > b)
    case DTYPE_FS:OperateF(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, a > b)
    case DTYPE_G:OperateF(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, a > b)
    case DTYPE_D:OperateF(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, a > b)
    case DTYPE_FT:OperateF(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, a > b)
    default:return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3Min(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_T:
    return TdiINVDTYDSC;
  case DTYPE_B:
    Operate(char, *in1p < *in2p)
    case DTYPE_BU:Operate(unsigned char, *in1p < *in2p)
    case DTYPE_W:Operate(short, *in1p < *in2p)
    case DTYPE_WU:Operate(unsigned short, *in1p < *in2p)
    case DTYPE_L:Operate(int, *in1p < *in2p)
    case DTYPE_LU:Operate(unsigned int, *in1p < *in2p)
    case DTYPE_QU:Operate(quadword, TdiLtQ(in1p, in2p, 0))
    case DTYPE_Q:Operate(quadword, TdiLtQ(in1p, in2p, 1))
    case DTYPE_OU:Operate(octaword, TdiLtO(in1p, in2p, 0))
    case DTYPE_O:Operate(octaword, TdiLtO(in1p, in2p, 1))
    case DTYPE_F:OperateF(float, DTYPE_F, DTYPE_NATIVE_FLOAT, a < b)
    case DTYPE_FS:OperateF(float, DTYPE_FS, DTYPE_NATIVE_FLOAT, a < b)
    case DTYPE_G:OperateF(double, DTYPE_G, DTYPE_NATIVE_DOUBLE, a < b)
    case DTYPE_D:OperateF(double, DTYPE_D, DTYPE_NATIVE_DOUBLE, a < b)
    case DTYPE_FT:OperateF(double, DTYPE_FT, DTYPE_NATIVE_DOUBLE, a < b)
    default:return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3Dim(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  INIT_STATUS;
  typedef struct {
    double l[2];
  } octaword_aligned;
  STATIC_CONSTANT octaword_aligned zero = { {0} };
  STATIC_CONSTANT struct descriptor dzero = { 0, 0, CLASS_S, (void *)&zero };

  switch (in1->dtype) {
  case DTYPE_FC:
  case DTYPE_FSC:
  case DTYPE_DC:
  case DTYPE_GC:
  case DTYPE_FTC:
  case DTYPE_H:
  case DTYPE_HC:
    return TdiINVDTYDSC;
  }
  status = Tdi3Subtract(in1, in2, out);
  if STATUS_NOT_OK
    return status;

  dzero.length = in1->length;
  dzero.dtype = in1->dtype;

  switch (in1->dtype) {
  case DTYPE_BU:
    out->dtype = DTYPE_B;
    break;
  case DTYPE_WU:
    out->dtype = DTYPE_W;
    break;
  case DTYPE_LU:
    out->dtype = DTYPE_L;
    break;
  case DTYPE_QU:
    out->dtype = DTYPE_Q;
    break;
  case DTYPE_OU:
    out->dtype = DTYPE_O;
    break;
  }
  status = Tdi3Max(out, &dzero, out);
  out->dtype = in1->dtype;
  return status;
}

#define char_min -128
#define short_min -32768
#define int_min 0x80000000
#define int64_t_min LONG_LONG_CONSTANT(0x8000000000000000)
#define min_struct(type) type##_min

#undef Operate
#define Operate(type) \
{ type *in1p = (type *)in1->pointer;\
  type *in2p = (type *)in2->pointer;\
  type *outp = (type *)out->pointer;\
  switch (scalars)\
  {\
    case 0: while (nout--) {if (*in2p > 0) *outp++ = (type)(*in1p++ << *in2p++); \
            else  {*outp = (type)(*in1p++ >> (-1 * (*in2p))); \
            if (*outp < 0) *outp = (type)(*outp & ~(min_struct(type) >> \
            (-1 * (*in2p - 1)))); in2p++; outp++;} \
            } break;\
    case 1: while (nout--) {if (*in2p > 0) *outp++ = (type)(*in1p << *in2p++); \
            else  {*outp = (type)(*in1p >> (-1 * (*in2p))); \
            if (*outp < 0) *outp = (type)(*outp & ~(min_struct(type) >> \
            (-1 * (*in2p - 1)))); in2p++; outp++;} \
            } break;\
    case 2: while (nout--) {if (*in2p > 0) *outp++ = (type)(*in1p++ << *in2p); \
            else  {*outp = (type)(*in1p++ >> (-1 * (*in2p))); \
            if (*outp < 0) *outp = (type)(*outp & ~(min_struct(type) >> \
            (-1 * (*in2p - 1)))); outp++;} \
            } break;\
    case 3: if (*in2p > 0) *outp++ = (type)(*in1p << *in2p); \
            else  {*outp = (type)(*in1p >> (-1 * (*in2p))); \
            if (*outp < 0) *outp = (type)(*outp & ~(min_struct(type) >> \
            (-1 * (*in2p - 1))));} \
            break;\
  }\
  break;\
}

#define OperateU(type1,type2) \
{ type1 *in1p = (type1 *)in1->pointer;\
  type2 *in2p = (type2 *)in2->pointer;\
  type1 *outp = (type1 *)out->pointer;\
  switch (scalars)\
  {\
    case 0: while (nout--) {if (*in2p > 0) *outp++ = (type1)(*in1p++ << *in2p++); \
            else *outp++ = (type1)(*in1p++ >> (-1 * (*in2p++))); \
            } break;\
    case 1: while (nout--) {if (*in2p > 0) *outp++ = (type1)(*in1p << *in2p++); \
            else *outp++ = (type1)(*in1p >> (-1 * (*in2p++))); \
            } break;\
    case 2: while (nout--) {if (*in2p > 0) *outp++ = (type1)(*in1p++ << *in2p); \
            else *outp++ = (type1)(*in1p++ >> (-1 * (*in2p))); \
            } break;\
    case 3: if (*in2p > 0) *outp = (type1)(*in1p << *in2p); \
            else *outp = (type1)(*in1p >> (-1 * (*in2p))); \
            break;\
  }\
  break;\
}

int Tdi3Ishft(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
    Operate(char)
    case DTYPE_BU:OperateU(unsigned char, char)
    case DTYPE_W:Operate(short)
    case DTYPE_WU:OperateU(unsigned short, short)
    case DTYPE_L:Operate(int)
    case DTYPE_LU:OperateU(unsigned int, int)
    case DTYPE_Q:Operate(int64_t)
    case DTYPE_QU:OperateU(uint64_t, int64_t)
    default:return TdiINVDTYDSC;
  }
  return 1;
}

#undef Operate
#define Operate(type,operator) \
{ type *in1p = (type *)in1->pointer;\
  type *in2p = (type *)in2->pointer;\
  type *outp = (type *)out->pointer;\
  switch (scalars)\
  {\
    case 0: while (nout--) {*outp++ = (type)(*in1p++ operator *in2p++); \
            } break;\
    case 1: while (nout--) {*outp++ = (type)(*in1p operator *in2p++); \
            } break;\
    case 2: while (nout--) {*outp++ = (type)(*in1p++ operator *in2p); \
            } break;\
    case 3: *outp = (type)(*in1p operator *in2p); \
            break;\
  }\
  break;\
}

int Tdi3ShiftRight(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
    Operate(char, >>)
    case DTYPE_BU:Operate(unsigned char, >>)
    case DTYPE_W:Operate(short, >>)
    case DTYPE_WU:Operate(unsigned short, >>)
    case DTYPE_L:Operate(int, >>)
    case DTYPE_LU:Operate(unsigned int, >>)
    case DTYPE_Q:Operate(int64_t, >>)
    case DTYPE_QU:Operate(uint64_t, >>)
    default:return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3ShiftLeft(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
    Operate(char, <<)
    case DTYPE_BU:Operate(unsigned char, <<)
    case DTYPE_W:Operate(short, <<)
    case DTYPE_WU:Operate(unsigned short, <<)
    case DTYPE_L:Operate(int, <<)
    case DTYPE_LU:Operate(unsigned int, <<)
    case DTYPE_Q:Operate(int64_t, <<)
    case DTYPE_QU:Operate(uint64_t, <<)
    default:return TdiINVDTYDSC;
  }
  return 1;
}
