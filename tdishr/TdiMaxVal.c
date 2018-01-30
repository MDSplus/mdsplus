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

                Name:   Tdi3MaxVal

                Type:   C function

                Author: MARK LONDON

                Date:   5-OCT-1993

                Purpose:

 Scalar/vector results from simple operations on one VMS data type and class.
 Works along the dimension.

form:   MAXLOC(array,[mask]) offset vector of size = rank of input
extern int Tdi3MaxLoc
extern int Tdi3MinLoc

form:   MAXVAL(array,[dim],[mask]) value scalar if no dim, vector if dim
extern int Tdi3MaxVal
extern int Tdi3MinVal
extern int Tdi3Mean
extern int Tdi3Product
extern int Tdi3Sum
extern int Tdi3Accumulate
------------------------------------------------------------------------------

        Call sequence:

int Tdi3xxxxx(struct descriptor *in, struct descriptor *mask,
               struct descriptor *out,int cnt_dim, int cnt_bef, int cnt_aft,
               int stp_dim, int stp_bef, int stp_aft)

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:


------------------------------------------------------------------------------*/

extern int Tdi3Lt();
extern int Tdi3Gt();
extern int Tdi3Divide();
#include <STATICdef.h>
#include <stdio.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <string.h>
#include <tdishr_messages.h>



extern int CvtConvertFloat();

const int roprand = 0x8000;

typedef struct {
  int o0;
  int o1;
  int o2;
  int o3;
} octaword;


#define ozero { 0, 0, 0, 0 };
#define oone  { 1, 0, 0, 0 };
#define uqmax 0xffffffffffffffff;
#define uqmin 0x0000000000000000;
#define qmax  0x7fffffffffffffff;
#define qmin  0x8000000000000000;

#define ozero { 0, 0, 0, 0 };
#define oone  { 1, 0, 0, 0 };
#define uomax { -1, -1, -1, -1 };
#define uomin { 0, 0, 0, 0 };
#define omax  { -1, -1, -1, 0x7fffffff };
#define omin  { 0, 0, 0, 0x80000000 };

#if DTYPE_NATIVE_DOUBLE == DTYPE_D
#define HUGE 1.7E38
#elif DTYPE_NATIVE_DOUBLE == DTYPE_G
#define HUGE 8.9E307
#else
#define HUGE 1.7E308
#endif

extern int TdiMultiplyQuadword();
extern int TdiMultiplyOctaword();
extern int TdiAddQuadword();
extern int TdiAddOctaword();

int TdiLtO();
int TdiGtO();
int TdiLtQ();
int TdiGtQ();

#define TdiDivO(in1,in2,out,data_type)\
{DESCRIPTOR_A(i1, sizeof(octaword), data_type , 0, 0);\
 DESCRIPTOR_A(i2, sizeof(octaword), data_type , 0, 0);\
 DESCRIPTOR_A(o, sizeof(octaword), data_type , 0, 0);\
 i1.arsize = i2.arsize = o.arsize = sizeof(octaword);\
 i1.pointer = (char *)(in1); i2.pointer = (char *)(in2);\
 o.pointer = (char *)(out);\
 Tdi3Divide(&i1,&i2,&o);}

#define SetupArgs \
  int stepm0,stepm1,stepm2,lenm;\
  switch (out->class)\
  {\
    case CLASS_S:\
    case CLASS_D:\
    case CLASS_A: break;\
    default:            return TdiINVCLADSC;\
  }\
  switch (in->class)\
  {\
    case CLASS_S:\
    case CLASS_D:\
    case CLASS_A: break;\
    default:            return TdiINVCLADSC;\
  }\
  switch (mask->class)\
  {\
    case CLASS_S:\
    case CLASS_D: stepm0 = stepm1 = stepm2 = 0; break;\
    case CLASS_A: lenm = mask->length, stepm0 = stp_dim*lenm,\
                        stepm1= stp_bef*lenm, stepm2 = stp_aft*lenm; break;\
    default:            return TdiINVCLADSC;\
  }

#define Operate(type,setup,testit,doit,final) \
{ int j0, j1, ja;\
  type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  for (ja = 0; ja++ < cnt_aft; pi2 += stp_aft, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < cnt_bef; pi1 += stp_bef,pm1 += stepm1) {\
      type result = setup;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < cnt_dim; j0++, pi0 += stp_dim,\
           pm0 += stepm0) {\
        if (*pm0 & 1 && testit) {doit}\
      }\
      final;\
    }\
  }\
  break;\
}

#define OperateL(type,setup,testit,doit,final) \
{ int j0, j1, ja;\
  int *outp = (int *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  for (ja = 0; ja++ < cnt_aft; pi2 += stp_aft, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < cnt_bef; pi1 += stp_bef,pm1 += stepm1) {\
      type result = setup;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < cnt_dim; j0++, pi0 += stp_dim,pm0 += stepm0) {\
        if (*pm0 & 1 && testit) {doit}\
      }\
      final;\
    }\
  }\
  break;\
}

#define OperateTloc(testit) \
{ int j0, j1, ja;\
  int *outp = (int *)out->pointer;\
  char *pi0=(char *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  char testval;\
  struct descriptor s_d={in->length,DTYPE_T,CLASS_S,0};\
  struct descriptor o_d={1,DTYPE_B,CLASS_S,0};\
  struct descriptor result={in->length,DTYPE_T,CLASS_S,0};\
  o_d.pointer=(char *)&testval;\
  result.pointer=pi0;\
  stp_dim=in->length;\
  for (ja = 0; ja++ < cnt_aft; pi2 += stp_aft, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < cnt_bef; pi1 += stp_bef,pm1 += stepm1) {\
      for (count=0, j0 = 0, pi0 = pi1, pm0 = pm1; j0 < cnt_dim; j0++, pi0 += stp_dim,pm0 += stepm0) {\
        if (*pm0 & 1) {\
           s_d.pointer=pi0;\
           testit(&s_d,&result,&o_d);\
           if (testval) {\
             result.pointer=pi0;\
             count=j0;\
           }\
        }\
      }\
      *outp++ = count;\
    }\
  }\
  break;\
}

#define OperateFloc(type,dtype,start,operator) \
{ int j0, j1, ja;\
  int *outp = (int *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  for (ja = 0; ja++ < cnt_aft; pi2 += stp_aft, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < cnt_bef; pi1 += stp_bef,pm1 += stepm1) {\
      double result = start;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < cnt_dim; j0++, pi0 += stp_dim,pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          if (CvtConvertFloat(pi0,dtype,&val,DTYPE_NATIVE_DOUBLE,0) && (val operator result))\
            {result = val; count = j0;}\
        }\
      }\
      *outp++ = count;\
    }\
  }\
  break;\
}

int Tdi3MaxLoc(struct descriptor *in, struct descriptor *mask,
	       struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
	       int stp_dim, int stp_bef, int stp_aft)
{
  int count = -1;
  SetupArgs switch (in->dtype) {
  case DTYPE_T:  OperateTloc(Tdi3Gt)
  case DTYPE_B:  OperateL(   int8_t,       -128, *pi0 > result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_BU: OperateL( u_int8_t,          0, *pi0 > result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_W:  OperateL(  int16_t,     -32768, *pi0 > result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_WU: OperateL(u_int16_t,          0, *pi0 > result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_L:  OperateL(  int32_t, 0x80000000, *pi0 > result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_LU: OperateL(u_int32_t,          0, *pi0 > result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_Q:  OperateL(  int64_t,       qmin, *pi0 > result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_QU: OperateL(u_int64_t,          0, *pi0 > result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_O:  OperateL( octaword, omin, TdiGtO(pi0, &result, 1), result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_OU: OperateL( octaword,uomin, TdiGtO(pi0, &result, 0), result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_F:  OperateFloc(float,  DTYPE_F, -HUGE, >)
  case DTYPE_FS: OperateFloc(float,  DTYPE_FS,-HUGE, >)
  case DTYPE_G:  OperateFloc(double, DTYPE_G, -HUGE, >)
  case DTYPE_D:  OperateFloc(double, DTYPE_D, -HUGE, >)
  case DTYPE_FT: OperateFloc(double, DTYPE_FT,-HUGE, >)
  default:return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3MinLoc(struct descriptor *in, struct descriptor *mask,
	       struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
	       int stp_dim, int stp_bef, int stp_aft)
{
  int count=-1;
  SetupArgs switch (in->dtype) {
  case DTYPE_T:  OperateTloc(Tdi3Lt)
  case DTYPE_B:  OperateL(   int8_t,            127, *pi0 < result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_BU: OperateL( u_int8_t,             -1, *pi0 < result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_W:  OperateL(  int16_t,          32767, *pi0 < result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_WU: OperateL(u_int16_t,             -1, *pi0 < result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_L:  OperateL(  int32_t,     0x7fffffff, *pi0 < result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_LU: OperateL(u_int32_t,             -1, *pi0 < result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_Q:  OperateL(  int64_t,           qmax, *pi0 < result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_QU: OperateL(u_int64_t,             -1, *pi0 < result, result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_O:  OperateL( octaword, omax, TdiLtO(pi0, &result, 1), result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_OU: OperateL( octaword,uomax, TdiLtO(pi0, &result, 0), result = *pi0; count = j0;, *outp++ = count)
  case DTYPE_F:  OperateFloc(float,  DTYPE_F, -HUGE, <)
  case DTYPE_FS: OperateFloc(float,  DTYPE_FS,-HUGE, <)
  case DTYPE_G:  OperateFloc(double, DTYPE_G, -HUGE, <)
  case DTYPE_D:  OperateFloc(double, DTYPE_D, -HUGE, <)
  case DTYPE_FT: OperateFloc(double, DTYPE_FT,-HUGE, <)
  default:return TdiINVDTYDSC;
  }
  return 1;
}

#define OperateFval(type,dtype,start,operator) \
{ int j0, j1, ja;\
  type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  for (ja = 0; ja++ < cnt_aft; pi2 += stp_aft, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < cnt_bef; pi1 += stp_bef,pm1 += stepm1) {\
      double result = start;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < cnt_dim; j0++, pi0 += stp_dim,pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          if (CvtConvertFloat(pi0,dtype,&val,DTYPE_NATIVE_DOUBLE,0) && (val operator result))\
            {result = val;}						\
        }\
      }\
      CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,outp++,dtype,0);\
    }\
  }\
  break;\
}

#define OperateTval(testit) \
{ int j0, j1, ja;\
  char *outp = (char *)out->pointer;\
  char *pi0=(char *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  char testval;\
  struct descriptor s_d={in->length,DTYPE_T,CLASS_S,0};\
  struct descriptor o_d={1,DTYPE_B,CLASS_S,0};\
  struct descriptor result={in->length,DTYPE_T,CLASS_S,0};\
  o_d.pointer=(char *)&testval;\
  result.pointer=pi0;\
  stp_dim=in->length;\
  for (ja = 0; ja++ < cnt_aft; pi2 += stp_aft, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < cnt_bef; pi1 += stp_bef,pm1 += stepm1) {\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < cnt_dim; j0++, pi0 += stp_dim,pm0 += stepm0) {\
        if (*pm0 & 1) {\
           s_d.pointer=pi0;\
           testit(&s_d,&result,&o_d);\
           if (testval) {\
             result.pointer=pi0;\
           }\
        }\
      }\
      memmove(outp,result.pointer,stp_dim);\
      outp += stp_dim;\
    }\
  }\
  break;\
}

int Tdi3MaxVal(struct descriptor *in, struct descriptor *mask,
	       struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
	       int stp_dim, int stp_bef, int stp_aft)
{
  SetupArgs switch (in->dtype) {
  case DTYPE_T:  OperateTval(Tdi3Gt)
  case DTYPE_B:  OperateL(   int8_t,       -128, *pi0 > result, result = *pi0;, *outp++ = result)
  case DTYPE_BU: OperateL( u_int8_t,          0, *pi0 > result, result = *pi0;, *outp++ = result)
  case DTYPE_W:  OperateL(  int16_t,     -32768, *pi0 > result, result = *pi0;, *outp++ = result)
  case DTYPE_WU: OperateL(u_int16_t,          0, *pi0 > result, result = *pi0;, *outp++ = result)
  case DTYPE_L:  OperateL(  int32_t, 0x80000000, *pi0 > result, result = *pi0;, *outp++ = result)
  case DTYPE_LU: OperateL(u_int32_t,          0, *pi0 > result, result = *pi0;, *outp++ = result)
  case DTYPE_Q:  OperateL(  int64_t,       qmin, *pi0 > result, result = *pi0;, *outp++ = result)
  case DTYPE_QU: OperateL(u_int64_t,          0, *pi0 > result, result = *pi0;, *outp++ = result)
  case DTYPE_O:  OperateL( octaword, omin, TdiGtO(pi0, &result, 1), memcpy(&result, pi0, sizeof(result));, memcpy(outp++, &result, sizeof(result)))
  case DTYPE_OU: OperateL( octaword,uomin, TdiGtO(pi0, &result, 0), memcpy(&result, pi0, sizeof(result));, memcpy(outp++, &result, sizeof(result)))
  case DTYPE_F:  OperateFval(float,  DTYPE_F, -HUGE, >)
  case DTYPE_FS: OperateFval(float,  DTYPE_FS,-HUGE, >)
  case DTYPE_G:  OperateFval(double, DTYPE_G, -HUGE, >)
  case DTYPE_D:  OperateFval(double, DTYPE_D, -HUGE, >)
  case DTYPE_FT: OperateFval(double, DTYPE_FT,-HUGE, >)
  default:return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3MinVal(struct descriptor *in, struct descriptor *mask,
	       struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
	       int stp_dim, int stp_bef, int stp_aft)
{
  SetupArgs switch (in->dtype) {
  case DTYPE_T:  OperateTval(Tdi3Lt)
  case DTYPE_B:  OperateL(   int8_t,            127, *pi0 < result, result = *pi0;, *outp++ = result)
  case DTYPE_BU: OperateL( u_int8_t,             -1, *pi0 < result, result = *pi0;, *outp++ = result)
  case DTYPE_W:  OperateL(  int16_t,          32767, *pi0 < result, result = *pi0;, *outp++ = result)
  case DTYPE_WU: OperateL(u_int16_t,             -1, *pi0 < result, result = *pi0;, *outp++ = result)
  case DTYPE_L:  OperateL(  int32_t,     0x7fffffff, *pi0 < result, result = *pi0;, *outp++ = result)
  case DTYPE_LU: OperateL(u_int32_t,             -1, *pi0 < result, result = *pi0;, *outp++ = result)
  case DTYPE_Q:  OperateL(  int64_t,           qmax, *pi0 < result, result = *pi0;, *outp++ = result)
  case DTYPE_QU: OperateL(u_int64_t,             -1, *pi0 < result, result = *pi0;, *outp++ = result)
  case DTYPE_O:  OperateL( octaword, omax, TdiLtO(pi0, &result, 1), memcpy(&result, pi0, sizeof(result));, memcpy(outp++, &result, sizeof(result)))
  case DTYPE_OU: OperateL( octaword,uomax, TdiLtO(pi0, &result, 0), memcpy(&result, pi0, sizeof(result));, memcpy(outp++, &result, sizeof(result)))
  case DTYPE_F:  OperateFval(float,  DTYPE_F, -HUGE, <)
  case DTYPE_FS: OperateFval(float,  DTYPE_FS,-HUGE, <)
  case DTYPE_G:  OperateFval(double, DTYPE_G, -HUGE, <)
  case DTYPE_D:  OperateFval(double, DTYPE_D, -HUGE, <)
  case DTYPE_FT: OperateFval(double, DTYPE_FT,-HUGE, <)
  default:return TdiINVDTYDSC;
  }
  return 1;
}

#define OperateFmean(type,dtype) \
{ int j0, j1, ja;\
  type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  for (ja = 0; ja++ < cnt_aft; pi2 += stp_aft, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < cnt_bef; pi1 += stp_bef,pm1 += stepm1) {\
      double result = 0;\
          count = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < cnt_dim; j0++, pi0 += stp_dim,pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          if (CvtConvertFloat(pi0,dtype,&val,DTYPE_NATIVE_DOUBLE,0))\
            {result += val; count++;}\
        }\
      }\
      if (count)\
      {\
        result /= count;\
        CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,outp++,dtype,0);\
      }\
      else\
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
    }\
  }\
  break;\
}

#define OperateCmean(type,dtype) \
{ int j0, j1, ja;\
  type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  for (ja = 0; ja++ < cnt_aft; pi2 += stp_aft, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < cnt_bef; pi1 += stp_bef,pm1 += stepm1) {\
      double result = 0;\
      double resulti = 0;\
          count = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < cnt_dim; j0++, pi0 += (2 * stp_dim),pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          double vali;\
          if (CvtConvertFloat(&pi0[0],dtype,&val,DTYPE_NATIVE_DOUBLE,0) && \
              CvtConvertFloat(&pi0[1],dtype,&vali,DTYPE_NATIVE_DOUBLE,0)) \
            {result += val; resulti += vali; count++;}\
        }\
      }\
      if (count)\
      {\
        result /= count;\
        resulti /= count;\
        CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,outp++,dtype,0);\
        CvtConvertFloat(&resulti,DTYPE_NATIVE_DOUBLE,outp++,dtype,0);\
      }\
      else\
      {\
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
      }\
    }\
  }\
  break;\
}

#undef Operate
#define Operate(type,setup,testit,doit,final) \
{ int j0, j1, ja;\
  type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  for (ja = 0; ja++ < cnt_aft; pi2 += stp_aft, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < cnt_bef; pi1 += stp_bef,pm1 += stepm1) {\
      type result = setup;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < cnt_dim; j0++, pi0 += stp_dim,\
           pm0 += stepm0) {\
        if (*pm0 & 1) {doit}\
      }\
      final;\
    }\
  }\
  break;\
}

int Tdi3Mean(struct descriptor *in, struct descriptor *mask,
	     struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
	     int stp_dim, int stp_bef, int stp_aft)
{
  int count;
  SetupArgs switch (in->dtype) {
  case DTYPE_B:  Operate(   int8_t,  0; count = 0, 1, result+=*pi0; count++;, *outp++ = (result / count))
  case DTYPE_BU: Operate( u_int8_t,  0; count = 0, 1, result+=*pi0; count++;, *outp++ = (result / count))
  case DTYPE_W:  Operate(  int16_t,  0; count = 0, 1, result+=*pi0; count++;, *outp++ = (result / count))
  case DTYPE_WU: Operate(u_int16_t,  0; count = 0, 1, result+=*pi0; count++;, *outp++ = (result / count))
  case DTYPE_L:  Operate(  int32_t,  0; count = 0, 1, result+=*pi0; count++;, *outp++ = (result / count))
  case DTYPE_LU: Operate(u_int32_t,  0; count = 0, 1, result+=*pi0; count++;, *outp++ = (result / count))
  case DTYPE_Q:  Operate(  int64_t,  0; count = 0, 1, result+=*pi0; count++;, *outp++ = (result / count))
  case DTYPE_QU: Operate(u_int64_t,  0; count = 0, 1, result+=*pi0; count++;, *outp++ = (result / count))
  case DTYPE_OU: Operate(octaword, ozero; count = 0, 1, TdiAddOctaword(pi0, &result, &result); count++;, {
	    octaword temp = ozero; memcpy(&temp, &count, sizeof(int)); TdiDivO(&result, &temp, outp++, DTYPE_OU);})
  case DTYPE_O:  Operate(octaword, ozero; count = 0, 1, TdiAddOctaword(pi0, &result, &result); count++;, {
	    octaword temp = ozero; memcpy(&temp, &count, sizeof(int)); TdiDivO(&result, &temp, outp++, DTYPE_O);})
  case DTYPE_F:  OperateFmean(float,  DTYPE_F )
  case DTYPE_FS: OperateFmean(float,  DTYPE_FS)
  case DTYPE_G:  OperateFmean(double, DTYPE_G )
  case DTYPE_D:  OperateFmean(double, DTYPE_D )
  case DTYPE_FT: OperateFmean(double, DTYPE_FT)
  case DTYPE_FC: OperateCmean(float,  DTYPE_F )
  case DTYPE_FSC:OperateCmean(float,  DTYPE_FS)
  case DTYPE_GC: OperateCmean(double, DTYPE_G )
  case DTYPE_DC: OperateCmean(double, DTYPE_D )
  case DTYPE_FTC:OperateCmean(double, DTYPE_FT)
  default:return TdiINVDTYDSC;
  }
  return 1;
}

#define OperateFprod(type,dtype) \
{ int j0, j1, ja;\
  type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  for (ja = 0; ja++ < cnt_aft; pi2 += stp_aft, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < cnt_bef; pi1 += stp_bef,pm1 += stepm1) {\
      double result = 1;\
      int bad = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < cnt_dim; j0++, pi0 += stp_dim,pm0 += stepm0) {\
        if (!bad && *pm0 & 1)\
        {\
          double val;\
          if (CvtConvertFloat(pi0,dtype,&val,DTYPE_NATIVE_DOUBLE,0)) result *= val; else bad = 1;\
        }\
      }\
      if (!bad)\
        CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,outp++,dtype,0);\
      else\
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
    }\
  }\
  break;\
}

#define OperateCprod(type,dtype) \
{ int j0, j1, ja;\
  type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  for (ja = 0; ja++ < cnt_aft; pi2 += stp_aft, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < cnt_bef; pi1 += stp_bef,pm1 += stepm1) {\
      int first = 1;\
      double result = 1;\
      double resulti = 1;\
      int bad = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < cnt_dim; j0++, pi0 += (2 * stp_dim),pm0 += stepm0) {\
        if (!bad && *pm0 & 1)\
        {\
          double val;\
          double vali;\
          if (CvtConvertFloat(&pi0[0],dtype,&val,DTYPE_NATIVE_DOUBLE,0) && \
              CvtConvertFloat(&pi0[1],dtype,&vali,DTYPE_NATIVE_DOUBLE,0)) \
          {\
            if (first) { result = val; resulti = vali; first = 0;} else \
            {double oldresult = result; result = result * val - resulti * vali; resulti = oldresult * vali + resulti * val;}\
          } else bad=1;\
        }\
      }\
      if (!bad)\
      {\
        CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,outp++,dtype,0);\
        CvtConvertFloat(&resulti,DTYPE_NATIVE_DOUBLE,outp++,dtype,0);\
      }\
      else\
      {\
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
      }\
    }\
  }\
  break;\
}

int Tdi3Product(struct descriptor *in, struct descriptor *mask,
		struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
		int stp_dim, int stp_bef, int stp_aft)
{
  SetupArgs switch (in->dtype) {
  case DTYPE_B:  Operate(   int8_t, 1, 1, result = (char)(result * *pi0);, *outp++ = result)
  case DTYPE_BU: Operate( u_int8_t, 1, 1, result = (unsigned char)(result * *pi0);, *outp++ = result)
  case DTYPE_W:  Operate(  int16_t, 1, 1, result = (short)(result * *pi0);, *outp++ = result)
  case DTYPE_WU: Operate(u_int16_t, 1, 1, result = (unsigned short)(result * *pi0);, *outp++ = result)
  case DTYPE_L:  Operate(  int32_t, 1, 1, result *= *pi0;, *outp++ = result)
  case DTYPE_LU: Operate(u_int32_t, 1, 1, result *= *pi0;, *outp++ = result)
  case DTYPE_Q:  Operate(  int64_t, 1, 1, result *= *pi0;, *outp++ = result)
  case DTYPE_QU: Operate(u_int64_t, 1, 1, result *= *pi0;, *outp++ = result)
  case DTYPE_O:
  case DTYPE_OU: Operate(octaword, oone, 1, TdiMultiplyOctaword(pi0, &result, &result);, *outp++ = result)
  case DTYPE_F:  OperateFprod(float, DTYPE_F )
  case DTYPE_FS: OperateFprod(float, DTYPE_FS)
  case DTYPE_G:  OperateFprod(double,DTYPE_G )
  case DTYPE_D:  OperateFprod(double,DTYPE_D )
  case DTYPE_FT: OperateFprod(double,DTYPE_FT)
  case DTYPE_FC: OperateCprod(float, DTYPE_F )
  case DTYPE_FSC:OperateCprod(float, DTYPE_FS)
  case DTYPE_GC: OperateCprod(double,DTYPE_G )
  case DTYPE_DC: OperateCprod(double,DTYPE_D )
  case DTYPE_FTC:OperateCprod(double,DTYPE_FT)
  default:return TdiINVDTYDSC;
  }
  return 1;
}

#define OperateFsum(type,dtype) \
{ int j0, j1, ja;\
  type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  int count=0;\
  for (ja = 0; ja++ < cnt_aft; pi2 += stp_aft, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < cnt_bef; pi1 += stp_bef,pm1 += stepm1) {\
      double result = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < cnt_dim; j0++, pi0 += stp_dim,pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          if (CvtConvertFloat(pi0,dtype,&val,DTYPE_NATIVE_DOUBLE,0))\
            {result += val;count++;}\
        }\
      }\
      if (count)\
        CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,outp++,dtype,0);\
      else\
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
    }\
  }\
  break;\
}

#define OperateCsum(type,dtype) \
{ int j0, j1, ja;\
  type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  int count = 0;\
  for (ja = 0; ja++ < cnt_aft; pi2 += stp_aft, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < cnt_bef; pi1 += stp_bef,pm1 += stepm1) {\
      double result = 0;\
      double resulti = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < cnt_dim; j0++, pi0 += (2 * stp_dim),pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          double vali;\
          if (CvtConvertFloat(&pi0[0],dtype,&val,DTYPE_NATIVE_DOUBLE,0) && \
              CvtConvertFloat(&pi0[1],dtype,&vali,DTYPE_NATIVE_DOUBLE,0)) \
            {result += val; resulti += vali; count++;}\
        }\
      }\
      if (count)\
      {\
        CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,outp++,dtype,0);\
        CvtConvertFloat(&resulti,DTYPE_NATIVE_DOUBLE,outp++,dtype,0);\
      }\
      else\
      {\
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
      }\
    }\
  }\
  break;\
}

int Tdi3Sum(struct descriptor *in, struct descriptor *mask,
	    struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
	    int stp_dim, int stp_bef, int stp_aft)
{
  SetupArgs switch (in->dtype) {
  case DTYPE_B:  Operate(   int8_t, 0, 1, result+=*pi0;, *outp++ = result)
  case DTYPE_BU: Operate( u_int8_t, 0, 1, result+=*pi0;, *outp++ = result)
  case DTYPE_W:  Operate(  int16_t, 0, 1, result+=*pi0;, *outp++ = result)
  case DTYPE_WU: Operate(u_int16_t, 0, 1, result+=*pi0;, *outp++ = result)
  case DTYPE_L:  Operate(  int32_t, 0, 1, result+=*pi0;, *outp++ = result)
  case DTYPE_LU: Operate(u_int32_t, 0, 1, result+=*pi0;, *outp++ = result)
  case DTYPE_Q:  Operate(  int64_t, 0, 1, result+=*pi0;, *outp++ = result)
  case DTYPE_QU: Operate(u_int64_t, 0, 1, result+=*pi0;, *outp++ = result)
  case DTYPE_OU:
  case DTYPE_O:  Operate(octaword, ozero, 1, TdiAddOctaword(pi0, &result, &result);, *outp++ = result)
  case DTYPE_F:  OperateFsum(float, DTYPE_F )
  case DTYPE_FS: OperateFsum(float, DTYPE_FS)
  case DTYPE_G:  OperateFsum(double,DTYPE_G )
  case DTYPE_D:  OperateFsum(double,DTYPE_D )
  case DTYPE_FT: OperateFsum(double,DTYPE_FT)
  case DTYPE_FC: OperateCsum(float, DTYPE_F )
  case DTYPE_FSC:OperateCsum(float, DTYPE_FS)
  case DTYPE_GC: OperateCsum(double,DTYPE_G )
  case DTYPE_DC: OperateCsum(double,DTYPE_D )
  case DTYPE_FTC:OperateCsum(double,DTYPE_FT)
  default:return TdiINVDTYDSC;
  }
  return 1;
}

typedef struct _args_t{
char*  inp;
char*  outp;
char*  maskp;
int    stp_dim;
int    stp_bef;
int    stp_aft;
int    stpm_dim;
int    stpm_bef;
int    stpm_aft;
int    cnt_dim;
int    cnt_bef;
int    cnt_aft;
} args_t;

static inline int setupArgs(struct descriptor *in, struct descriptor *mask,
	struct descriptor *out, int stp_dim, int stp_bef, int stp_aft, args_t* a){
  switch (out->class)
  {
    case CLASS_S:
    case CLASS_D:
    case CLASS_A: break;
    default: return C_ERROR;
  }
  switch (in->class)
  {
    case CLASS_S:
    case CLASS_D:
    case CLASS_A: break;
    default: return C_ERROR;
  }
  switch (mask->class)
  {
    case CLASS_S:
    case CLASS_D: break;
    case CLASS_A:
	a->stpm_dim=stp_dim*mask->length,
	a->stpm_bef=stp_bef*mask->length,
	a->stpm_aft=stp_aft*mask->length;
	break;
    default: return C_ERROR;
  }
  return C_OK;
}
#define SETUP_ARGS(args)   args_t args = {\
  in->pointer,out->pointer,mask->pointer,\
  stp_dim*in->length,stp_bef*in->length,stp_aft*in->length,\
  0,0,0,\
  cnt_dim,cnt_bef,cnt_aft};\
  if (setupArgs(in,out,mask,stp_dim,stp_bef,stp_aft,&args))\
    return TdiINVCLADSC;

#define OperateIaccum(type,zero,addpid,dtype) {\
  args_t* a = &args; \
  int ja, jb, jd;\
  char *poa, *pob, *pod;\
  char *pia, *pib, *pid;\
  char *pma, *pmb, *pmd;\
  for (ja=0,poa=a->outp,pia=a->inp,pma=a->maskp; ja++ < a->cnt_aft; poa += a->stp_aft, pia += a->stp_aft, pma += a->stpm_aft) {\
    for (jb=0,pob=poa,  pib=pia,   pmb=pma;      jb++ < a->cnt_bef; pob += a->stp_bef, pib += a->stp_bef, pmb += a->stpm_bef) {\
      type result = zero;\
      for (jd=0,pod=pob,pid=pib,   pmd=pmb;      jd++ < a->cnt_dim; pod += a->stp_dim, pid += a->stp_dim, pmd += a->stpm_dim) {\
        if (*pmd & 1) {\
          addpid;\
          memcpy(pod,&result,sizeof(result));\
        }\
      }\
    }\
  }\
}

static inline void OperateFaccum(char dtype,args_t* a){
  int ja, jb, jd;
  char *poa, *pob, *pod;
  char *pia, *pib, *pid;
  char *pma, *pmb, *pmd;
  for (ja=0,poa=a->outp,pia=a->inp,pma=a->maskp; ja++ < a->cnt_aft; poa += a->stp_aft, pia += a->stp_aft, pma += a->stpm_aft) {
    for (jb=0,pob=poa,  pib=pia,   pmb=pma;      jb++ < a->cnt_bef; pob += a->stp_bef, pib += a->stp_bef, pmb += a->stpm_bef) {
      double result = 0;
      for (jd=0,pod=pob,pid=pib,   pmd=pmb;      jd++ < a->cnt_dim; pod += a->stp_dim, pid += a->stp_dim, pmd += a->stpm_dim) {
        if (*pmd & 1){
          double val;
          if (CvtConvertFloat(pid,dtype,&val,DTYPE_NATIVE_DOUBLE,0)) {
            result += val;
            CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,pod,dtype,0);
          } else CvtConvertFloat(&roprand,DTYPE_F,pod,dtype,0);
        }
      }
    }
  }
}

static inline void OperateCaccum(char dtype,args_t* a,size_t size){
  int ja, jb, jd;
  char *poa, *pob, *pod;
  char *pia, *pib, *pid;
  char *pma, *pmb, *pmd;
  for (ja=0,poa=a->outp,pia=a->inp,pma=a->maskp; ja++ < a->cnt_aft; poa += a->stp_aft, pia += a->stp_aft, pma += a->stpm_aft) {
    for (jb=0,pob=poa,  pib=pia,   pmb=pma;      jb++ < a->cnt_bef; pob += a->stp_bef, pib += a->stp_bef, pmb += a->stpm_bef) {
      double result = 0, resulti = 0;
      for (jd=0,pod=pob,pid=pib,   pmd=pmb;      jd++ < a->cnt_dim; pod += a->stp_dim, pid += a->stp_dim, pmd += a->stpm_dim) {
        if (*pmd & 1){
          double val, vali;
          if (CvtConvertFloat(pid,dtype,&val,DTYPE_NATIVE_DOUBLE,0) &&
              CvtConvertFloat(pid+size,dtype,&vali,DTYPE_NATIVE_DOUBLE,0)) {
            result += val; resulti += vali;
            CvtConvertFloat(&result,DTYPE_NATIVE_DOUBLE,pod,dtype,0);
            CvtConvertFloat(&resulti,DTYPE_NATIVE_DOUBLE,pod+size,dtype,0);
          } else {
            CvtConvertFloat(&roprand,DTYPE_F,pod,dtype,0);
            CvtConvertFloat(&roprand,DTYPE_F,pod+size,dtype,0);
          }
        }
      }
    }
  }
}

int Tdi3Accumulate(struct descriptor *in, struct descriptor *mask,
		   struct descriptor *out, int cnt_dim, int cnt_bef, int cnt_aft,
		   int stp_dim, int stp_bef, int stp_aft)
{
  SETUP_ARGS(args);
  switch (in->dtype) {
    case DTYPE_B:  OperateIaccum(   int8_t, 0, result += *(   int8_t*)pid, DTYPE_B );break;
    case DTYPE_BU: OperateIaccum( u_int8_t, 0, result += *( u_int8_t*)pid, DTYPE_BU);break;
    case DTYPE_W:  OperateIaccum(  int16_t, 0, result += *(  int16_t*)pid, DTYPE_W );break;
    case DTYPE_WU: OperateIaccum(u_int16_t, 0, result += *(u_int16_t*)pid, DTYPE_WU);break;
    case DTYPE_L:  OperateIaccum(  int32_t, 0, result += *(  int32_t*)pid, DTYPE_L );break;
    case DTYPE_LU: OperateIaccum(u_int32_t, 0, result += *(u_int32_t*)pid, DTYPE_LU);break;
    case DTYPE_Q:  OperateIaccum(  int64_t, 0, result += *(  int64_t*)pid, DTYPE_Q );break;
    case DTYPE_QU: OperateIaccum(u_int64_t, 0, result += *(u_int64_t*)pid, DTYPE_QU);break;
    case DTYPE_O:  OperateIaccum(octaword, ozero, TdiAddOctaword((octaword*)pid, (octaword*)&result, (octaword*)&result), DTYPE_O ); break;
    case DTYPE_OU: OperateIaccum(octaword, ozero, TdiAddOctaword((octaword*)pid, (octaword*)&result, (octaword*)&result), DTYPE_OU); break;
    case DTYPE_F:  OperateFaccum(DTYPE_F, &args);break;
    case DTYPE_FS: OperateFaccum(DTYPE_FS,&args);break;
    case DTYPE_G:  OperateFaccum(DTYPE_G, &args);break;
    case DTYPE_D:  OperateFaccum(DTYPE_D, &args);break;
    case DTYPE_FT: OperateFaccum(DTYPE_FT,&args);break;
    case DTYPE_FC: OperateCaccum(DTYPE_F, &args,sizeof(float)); break;
    case DTYPE_FSC:OperateCaccum(DTYPE_FS,&args,sizeof(float)); break;
    case DTYPE_GC: OperateCaccum(DTYPE_G, &args,sizeof(double));break;
    case DTYPE_DC: OperateCaccum(DTYPE_D, &args,sizeof(double));break;
    case DTYPE_FTC:OperateCaccum(DTYPE_FT,&args,sizeof(double));break;
    default:return TdiINVDTYDSC;
  }
  return 1;
}

int TdiLtO(unsigned int *in1, unsigned int *in2, int is_signed)
{
  int j, longwords = 4, k = 0, *i1, *i2;
  for (j = longwords - 1; j >= 0; j--)
    if (!j || in1[j] != in2[j]) {
      if (!is_signed || j != longwords - 1) {
	k = (in1[j] < in2[j]);
	break;
      } else {
	i1 = (int *)(in1 + j);
	i2 = (int *)(in2 + j);
	k = (*i1 < *i2);
	break;
      }
    }
  return k;
}

int TdiGtO(unsigned int *in1, unsigned int *in2, int is_signed)
{
  int j, longwords = 4, k = 0, *i1, *i2;
  for (j = longwords - 1; j >= 0; j--) {
    if (!j || in1[j] != in2[j]) {
      if (!is_signed || j != longwords - 1) {
	k = (in1[j] > in2[j]);
	break;
      } else {
	i1 = (int *)(in1 + j);
	i2 = (int *)(in2 + j);
	k = (*i1 > *i2);
	break;
      }
    }
  }
  return k;
}

int TdiLtQ(unsigned int *in1, unsigned int *in2, int is_signed)
{
  int j, longwords = 2, k = 0, *i1, *i2;
  for (j = longwords - 1; j >= 0; j--) {
    if (!j || in1[j] != in2[j]) {
      if (!is_signed || j != longwords - 1) {
	k = (in1[j] < in2[j]);
	break;
      } else {
	i1 = (int *)(in1 + j);
	i2 = (int *)(in2 + j);
	k = (*i1 < *i2);
	break;
      }
    }
  }
  return k;
}

int TdiGtQ(unsigned int *in1, unsigned int *in2, int is_signed)
{
  int j, longwords = 2, k = 0, *i1, *i2;
  for (j = longwords - 1; j >= 0; j--) {
    if (!j || in1[j] != in2[j]) {
      if (!is_signed || j != longwords - 1) {
	k = (in1[j] > in2[j]);
	break;
      } else {
	i1 = (int *)(in1 + j);
	i2 = (int *)(in2 + j);
	k = (*i1 > *i2);
	break;
      }
    }
  }
  return k;
}

