/*------------------------------------------------------------------------------

		Name:   Tdi3Add   

		Type:   C function

     		Author:	TOM FREDIAN

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


#include <mdsdescrip.h>
#include <mdsdescrip.h>
#include <string.h>
#include <tdimessages.h>

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
    default:		return TdiINVCLADSC;\
  }\
  switch (in1->class)\
  {\
    case CLASS_S:\
    case CLASS_D: scalars |= 1; break;\
    case CLASS_A: if (ina1->arsize/ina1->length < nout) return TdiINV_SIZE; break;\
    default:		return TdiINVCLADSC;\
  }\
  switch (in2->class)\
  {\
    case CLASS_S:\
    case CLASS_D: if (scalars && (nout > 1)) return TdiINV_SIZE; scalars |= 2; break;\
    case CLASS_A: if (ina2->arsize/ina2->length < nout) return TdiINV_SIZE; break;\
    default:		return TdiINVCLADSC;\
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

static const int roprand = 0x8000;

#define OperateFloatOne(type,dtype,native,operator,p1,p2) \
{ type a,b,ans;\
  if (CvtConvertFloat(p1,dtype,&a,native,0) && CvtConvertFloat(p2,dtype,&b,native,0))\
  { ans = a operator b;\
    CvtConvertFloat(&ans,native,outp++,dtype,0);\
  } else CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0); }

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
  SetupArgs
  switch (in1->dtype)
  {
    case DTYPE_B:  Operate(char,+)
    case DTYPE_BU: Operate(unsigned char,+)
    case DTYPE_W:  Operate(short,+)
    case DTYPE_WU: Operate(unsigned short,+)
    case DTYPE_L:  Operate(int,+)
    case DTYPE_LU: Operate(unsigned int,+)
    case DTYPE_Q:  OperateSpecial(8,TdiAddQuadword)
    case DTYPE_QU: OperateSpecial(8,TdiAddQuadword)
    case DTYPE_O:  OperateSpecial(16,TdiAddOctaword)
    case DTYPE_OU: OperateSpecial(16,TdiAddOctaword)
    case DTYPE_F:  OperateFloat(float,DTYPE_F,DTYPE_FLOAT,+)
    case DTYPE_FS: OperateFloat(float,DTYPE_FS,DTYPE_FLOAT,+)
    case DTYPE_D:  OperateFloat(double,DTYPE_D,DTYPE_DOUBLE,+)
    case DTYPE_G:  OperateFloat(double,DTYPE_G,DTYPE_DOUBLE,+)
    case DTYPE_FT: OperateFloat(double,DTYPE_FT,DTYPE_DOUBLE,+)
    case DTYPE_FC:  OperateComplex(float,DTYPE_F,DTYPE_FLOAT,+)
    case DTYPE_FSC: OperateComplex(float,DTYPE_FS,DTYPE_FLOAT,+)
    case DTYPE_GC:  OperateComplex(double,DTYPE_G,DTYPE_DOUBLE,+)
    case DTYPE_DC:  OperateComplex(double,DTYPE_D,DTYPE_DOUBLE,+)
    case DTYPE_FTC: OperateComplex(double,DTYPE_FT,DTYPE_DOUBLE,+)
    default: return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3Subtract(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs
  switch (in1->dtype)
  {
    case DTYPE_B:  Operate(char,-)
    case DTYPE_BU: Operate(unsigned char,-)
    case DTYPE_W:  Operate(short,-)
    case DTYPE_WU: Operate(unsigned short,-)
    case DTYPE_L:  Operate(int,-)
    case DTYPE_LU: Operate(unsigned int,-)
    case DTYPE_Q:  OperateSpecial(8,TdiSubtractQuadword)
    case DTYPE_QU: OperateSpecial(8,TdiSubtractQuadword)
    case DTYPE_O:  OperateSpecial(16,TdiSubtractOctaword)
    case DTYPE_OU: OperateSpecial(16,TdiSubtractOctaword)
    case DTYPE_F:  OperateFloat(float,DTYPE_F,DTYPE_FLOAT,-)
    case DTYPE_FS: OperateFloat(float,DTYPE_FS,DTYPE_FLOAT,-)
    case DTYPE_D:  OperateFloat(double,DTYPE_D,DTYPE_DOUBLE,-)
    case DTYPE_G:  OperateFloat(double,DTYPE_G,DTYPE_DOUBLE,-)
    case DTYPE_FT: OperateFloat(double,DTYPE_FT,DTYPE_DOUBLE,-)
    case DTYPE_FC:  OperateComplex(float,DTYPE_F,DTYPE_FLOAT,-)
    case DTYPE_FSC: OperateComplex(float,DTYPE_FS,DTYPE_FLOAT,-)
    case DTYPE_GC:  OperateComplex(double,DTYPE_G,DTYPE_DOUBLE,-)
    case DTYPE_DC:  OperateComplex(double,DTYPE_D,DTYPE_DOUBLE,-)
    case DTYPE_FTC: OperateComplex(double,DTYPE_FT,DTYPE_DOUBLE,-)
    default: return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3Multiply(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs
  switch (in1->dtype)
  {
    case DTYPE_B:  Operate(char,*)
    case DTYPE_BU: Operate(unsigned char,*)
    case DTYPE_W:  Operate(short,*)
    case DTYPE_WU: Operate(unsigned short,*)
    case DTYPE_L:  Operate(int,*)
    case DTYPE_LU: Operate(unsigned int,*)
    case DTYPE_Q:  OperateSpecial(8,TdiMultiplyQuadword)
    case DTYPE_QU: OperateSpecial(8,TdiMultiplyQuadword)
    case DTYPE_O:  OperateSpecial(16,TdiMultiplyOctaword)
    case DTYPE_OU: OperateSpecial(16,TdiMultiplyOctaword)
    case DTYPE_F:  OperateFloat(float,DTYPE_F,DTYPE_FLOAT,*)
    case DTYPE_FS: OperateFloat(float,DTYPE_FS,DTYPE_FLOAT,*)
    case DTYPE_D:  OperateFloat(double,DTYPE_D,DTYPE_DOUBLE,*)
    case DTYPE_G:  OperateFloat(double,DTYPE_G,DTYPE_DOUBLE,*)
    case DTYPE_FT: OperateFloat(double,DTYPE_FT,DTYPE_DOUBLE,*)
    case DTYPE_FC:  MultiplyComplex(float,DTYPE_F,DTYPE_FLOAT)
    case DTYPE_FSC: MultiplyComplex(float,DTYPE_FS,DTYPE_FLOAT)
    case DTYPE_GC:  MultiplyComplex(double,DTYPE_G,DTYPE_DOUBLE)
    case DTYPE_DC:  MultiplyComplex(double,DTYPE_D,DTYPE_DOUBLE)
    case DTYPE_FTC: MultiplyComplex(double,DTYPE_FT,DTYPE_DOUBLE)
    default: return TdiINVDTYDSC;
  }
  return 1;
}

#if defined(__alpha) && defined(__vms)
typedef __int64 _int64;
#define swapquad(in)
#define swapocta(in)
#elif defined(__osf__) 
typedef long long _int64;
#define swapquad(in)
#define swapocta(in)
#elif defined(__irix__)
typedef long long _int64;
#define swapquad(in)
#define swapocta(in)
#elif defined(__hpux__)
typedef long long _int64;
#define swapquad(in) {int stmp; int *iptr = (int *)in; stmp=iptr[0]; iptr[0]=iptr[1]; iptr[1]=stmp;}
#define swapocta(in) {int stmp; int *iptr = (int *)in; stmp=iptr[0]; iptr[0]=iptr[3]; iptr[3]=stmp; \
                                                                   stmp=iptr[1]; iptr[1]=iptr[2]; iptr[2]=stmp;}
#else
#define swapquad(in)
#define swapocta(in)
#endif

static int zero=0;

#ifdef __VAX
#define emul lib##$emul
extern int emul();
#else
static int emul(int *m1, int *m2, int *add, int *out)
{
  _int64 m1_64 = *m1;
  _int64 m2_64 = *m2;
  _int64 add_64 = *add;
  _int64 prod;
  int *prodp = (int *)&prod;
  prod = m1_64 * m2_64 + add_64;
  out[0]=prodp[0];
  out[1]=prodp[1];
  return 1;
}
#endif


int TdiMultiplyQuadword(int *in1, int *in2, int *out)
{
  int tmp[3];
  int in1l[2];
  int in2l[2];
  static int endiantest=1;
  int littleendian = (int)*(char *)&endiantest;
  in1l[0] = littleendian ? in1[0] : in1[1];
  in1l[1] = littleendian ? in1[1] : in1[0];
  in2l[0] = littleendian ? in2[0] : in2[1];
  in2l[1] = littleendian ? in2[1] : in2[0];
  emul(&in2l[0],&in1l[0],&zero,&tmp[0]);
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
  return 1;
}

int TdiMultiplyOctaword(int *in1, int *in2, int *out)
{
  int tmp[8];
  int in1l[4];
  int in2l[4];
  int tmp2[4];
  int tmp3[4];
  static int endiantest=1;
  int littleendian = (int)*(char *)&endiantest;
  in1l[0] = littleendian ? in1[0] : in1[3];
  in1l[1] = littleendian ? in1[1] : in1[2];
  in1l[2] = littleendian ? in1[2] : in1[1];
  in1l[3] = littleendian ? in1[3] : in1[0];
  in2l[0] = littleendian ? in2[0] : in2[3];
  in2l[1] = littleendian ? in2[1] : in2[2];
  in2l[2] = littleendian ? in2[2] : in2[1];
  in2l[3] = littleendian ? in2[3] : in2[0];
  emul(&in2l[0],in1l,&zero,&tmp[0]);
  swapquad(&tmp[0]);
  emul(&in2l[1],in1l,&tmp[1],&tmp[1]);
  swapquad(&tmp[1]);
  emul(&in2l[2],in1l,&tmp[2],&tmp[2]);
  swapquad(&tmp[2]);
  emul(&in2l[3],in1l,&tmp[3],&tmp[3]);
  swapquad(&tmp[3]);
  emul(&in2l[0],&in1l[1],&zero,&tmp[4]);
  swapquad(&tmp[4]);
  emul(&in2l[1],&in1l[1],&tmp[5],&tmp[5]);
  swapquad(&tmp[5]);
  emul(&in2l[2],&in1l[1],&tmp[6],&tmp[6]);
  swapquad(&tmp[6]);
  memcpy(tmp2,&tmp[4],16);
  memcpy(tmp3,&tmp[1],16);
  swapocta(tmp2);
  swapocta(tmp3);
  TdiAddOctaword(tmp2,tmp3,&tmp[1]);
  swapocta(&tmp[1]);
  emul(&in2l[0],&in1l[2],&zero,&tmp[4]);
  swapquad(&tmp[4]);
  emul(&in2l[1],&in1l[2],&tmp[5],&tmp[5]);
  swapquad(&tmp[5]);
  memcpy(tmp2,&tmp[4],8);
  memcpy(tmp3,&tmp[2],8);
  swapquad(tmp2);
  swapquad(tmp3);
  TdiAddQuadword(tmp2,tmp3,&tmp[2]);
  swapquad(&tmp[2]);
  emul(&in2l[0],&in1l[3],&tmp[3],&tmp[3]);
  swapquad(&tmp[3]);
  if (in1l[0] < 0)
  {
    tmp[7] = tmp[4];
    memcpy(tmp2,&tmp[1],16);
    swapocta(tmp2);
    TdiAddOctaword(in2,tmp2,&tmp[1]);
    swapocta(&tmp[1]);
    tmp[4] = tmp[7];
  }
  if (in1l[1] < 0)
  {
    memcpy(tmp2,&tmp[2],8);
    swapquad(tmp2);
    TdiAddQuadword(littleendian ? in2 : in2+2 ,tmp2,&tmp[2]);
    swapquad(&tmp[2]);
  }
  if (in1l[2] < 0)
    tmp[3] += in2l[0];
  if (in2l[0] < 0)
  {
    tmp[7] = tmp[4];
    memcpy(tmp2,&tmp[1],16);
    swapocta(tmp2);
    TdiAddOctaword(in1,tmp2,&tmp[1]);
    swapocta(&tmp[1]);
    tmp[4] = tmp[7];
  }
  if (in2l[1] < 0)
  {
    memcpy(tmp2,&tmp[2],8);
    swapquad(tmp2);
    TdiAddQuadword(littleendian ? in1 : in1+2 ,tmp2,&tmp[2]);
    swapquad(&tmp[2]);
  }
  if (in2l[2] < 0)
    tmp[3] += in1l[0];
  out[0] = littleendian ? tmp[0] : tmp[3];
  out[1] = littleendian ? tmp[1] : tmp[2];
  out[2] = littleendian ? tmp[2] : tmp[1];
  out[3] = littleendian ? tmp[3] : tmp[0];
  return 1;
}

int TdiAddQuadword(unsigned int *a, unsigned int *b, unsigned int *ans)
{
  int i;
  int carry=0;
  static int endiantest = 1;
  int littleendian = (int)*(char *)&endiantest;
  unsigned int la[2];
  unsigned int lb[2];
  la[0] = a[0];
  la[1] = a[1];
  lb[0] = b[0];
  lb[1] = b[1];
  swapquad(la);
  swapquad(lb);
  for (i=0; i<2; i++) {
    unsigned int _a = la[i];
    unsigned int _b = lb[i];
    ans[i] = _a + _b + carry;
    carry = (ans[i] <= _a) && ((_b != 0) || (carry != 0));
  }
  swapquad(ans);
  return !carry;
}

int TdiAddOctaword(unsigned int *a, unsigned int *b, unsigned int *ans)
{
  int i;
  int carry=0;
  static int endiantest = 1;
  int littleendian = (int)*(char *)&endiantest;
  unsigned int la[4];
  unsigned int lb[4];
  memcpy(la,a,16);
  memcpy(lb,b,16);
  swapocta(la);
  swapocta(lb);
  for (i=0; i<4; i++) {
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
  int status;
  static int endiantest = 1;
  int littleendian = (int)*(char *)&endiantest;
  unsigned int lb[2];
  unsigned int sub[2];
  lb[0] = b[0];
  lb[1] = b[1];
  swapquad(lb);
  for (i=0 ; i<2; i++) {
    sub[i] = ~lb[i];
    if (i == 0) 
      sub[i]++;
    else
      if(sub[i-1] < (~lb[i-1])) 
        sub[i]++;
  }
  swapquad(sub);
  status = TdiAddQuadword(a, sub, ans);
  return status;
}

int TdiSubtractOctaword(unsigned int *a, unsigned int *b, unsigned int *ans)
{
  int i;
  int status;
  static int endiantest = 1;
  int littleendian = (int)*(char *)&endiantest;
  unsigned int lb[4];
  unsigned int sub[4];
  memcpy(lb,b,16);
  swapocta(lb);
  for (i=0 ; i<4; i++) {
    sub[i] = ~lb[i];
    if (i == 0) 
      sub[i]++;
    else
      if(sub[i-1] < (~lb[i-1])) 
        sub[i]++;
  }
  swapocta(sub);
  status = TdiAddOctaword(a, sub, ans);
  return status;
}
