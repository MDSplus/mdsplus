/*------------------------------------------------------------------------------

		Name:   Tdi3MaxVal   

		Type:   C function

     		Author:	MARK LONDON

		Date:   5-OCT-1993 

    		Purpose:
 
 Scalar/vector results from simple operations on one VMS data type and class.
 Works along the dimension.

form:	MAXLOC(array,[mask]) offset vector of size = rank of input
extern int Tdi3MaxLoc
extern int Tdi3MinLoc

form:	MAXVAL(array,[dim],[mask]) value scalar if no dim, vector if dim
extern int Tdi3MaxVal
extern int Tdi3MinVal
extern int Tdi3Mean
extern int Tdi3Product
extern int Tdi3Sum
extern int Tdi3Accumulate
------------------------------------------------------------------------------

	Call sequence: 

int Tdi3xxxxx(struct descriptor *in, struct descriptor *mask, 
	       struct descriptor *out,int count0, int count1, int count2,
	       int step0, int step1, int step2)

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:

 
------------------------------------------------------------------------------*/


extern int Tdi3Lt(  );
extern int Tdi3Gt(  );
extern int Tdi3Divide(  );

#include <stdlib.h>
#include <mdsdescrip.h>
#include <mdsdescrip.h>
#include <string.h>
#include <tdimessages.h>

extern int CvtConvertFloat();

static int roprand = 0x8000;

typedef struct {int q0; int q1;} quadword;
typedef struct {int o0; int o1; int o2; int o3;} octaword;
static quadword qzero = {0,0};
static octaword ozero = {0,0,0,0};
static quadword maxquad = {0xffffffff,0x7fffffff};
static octaword maxocta = {0xffffffff,0xffffffff,0xffffffff,0x7fffffff};
static quadword umaxquad = {0xffffffff,0xffffffff};
static int umax64[2] = {0xffffffff,0xffffffff};
static octaword umaxocta = {0xffffffff,0xffffffff,0xffffffff,0xffffffff};
static quadword  minquad = {0x00000000,0x80000000};
static octaword minocta = {0x00000000,0x00000000,0x00000000,0x80000000};
static quadword uminquad = {0x00000000,0x00000000};
static octaword uminocta = {0x00000000,0x00000000,0x00000000,0x00000000};

#if DTYPE_DOUBLE == DTYPE_D
#define HUGE 1.7E38
#elif DTYPE_DOUBLE == DTYPE_G
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

#define TdiDivQ(in1,in2,out,data_type)\
{DESCRIPTOR_A(i1, sizeof(quadword), data_type , 0, 0);\
 DESCRIPTOR_A(i2, sizeof(quadword), data_type , 0, 0);\
 DESCRIPTOR_A(o, sizeof(quadword), data_type , 0, 0);\
 i1.arsize = i2.arsize = o.arsize = sizeof(octaword);\
 i1.pointer = (char *)(in1); i2.pointer = (char *)(in2);\
 o.pointer = (char *)(out);\
 Tdi3Divide(&i1,&i2,&o);}

#define SetupArgs \
  struct descriptor_a *ina = (struct descriptor_a *)in;\
  struct descriptor_a *maska = (struct descriptor_a *)mask;\
  struct descriptor_a *outa = (struct descriptor_a *)out;\
  int stepm0,stepm1,stepm2,lenm;\
  int j0, j1, j2;\
  switch (out->class)\
  {\
    case CLASS_S:\
    case CLASS_D:\
    case CLASS_A: break;\
    default:		return TdiINVCLADSC;\
  }\
  switch (in->class)\
  {\
    case CLASS_S:\
    case CLASS_D:\
    case CLASS_A: break;\
    default:		return TdiINVCLADSC;\
  }\
  switch (mask->class)\
  {\
    case CLASS_S:\
    case CLASS_D: stepm0 = stepm1 = stepm2 = 0; break;\
    case CLASS_A: lenm = mask->length, stepm0 = step0*lenm,\
		        stepm1= step1*lenm, stepm2 = step2*lenm; break;\
    default:		return TdiINVCLADSC;\
  }

#define Operate(type,setup,testit,doit,final) \
{ type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  type result=setup;\
  for (j2 = 0; j2++ < count2; pi2 += step2, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < count1; pi1 += step1,pm1 += stepm1) {\
      result = setup;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < count0; j0++, pi0 += step0,\
           pm0 += stepm0) {\
        if (*pm0 & 1 && testit) {doit}\
      }\
      final;\
    }\
  }\
  break;\
}

#define OperateL(type,setup,testit,doit,final) \
{ int *outp = (int *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  type result=setup;\
  for (j2 = 0; j2++ < count2; pi2 += step2, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < count1; pi1 += step1,pm1 += stepm1) {\
      result = setup;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < count0; j0++, pi0 += step0,pm0 += stepm0) {\
        if (*pm0 & 1 && testit) {doit}\
      }\
      final;\
    }\
  }\
  break;\
}

#define OperateFloc(type,dtype,start,operator) \
{ int *outp = (int *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  count = -1;\
  for (j2 = 0; j2++ < count2; pi2 += step2, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < count1; pi1 += step1,pm1 += stepm1) {\
      double result = start;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < count0; j0++, pi0 += step0,pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          if (CvtConvertFloat(pi0,dtype,&val,DTYPE_DOUBLE,0) && (val operator result))\
            {result = val; count = j0;}\
        }\
      }\
      *outp++ = count;\
    }\
  }\
  break;\
}

int Tdi3MaxLoc(struct descriptor *in, struct descriptor *mask, 
	       struct descriptor *out,int count0, int count1, int count2,
	       int step0, int step1, int step2)
{
  int count;
  SetupArgs
  switch (in->dtype)
  {
    case DTYPE_T:  OperateL(int,(int)pi1;count = -1,Tdi3Gt(pi0,*(int *)result),
		         result = (int)pi0;count = j0;,*outp++ = count)
    case DTYPE_B:  OperateL(char,(char)-127;count = -1,*pi0 > result,result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_BU: OperateL(unsigned char,(unsigned char)0;count = -1,*pi0 > result,result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_W:  OperateL(short,(short)-32768;count = -1,*pi0 > result,result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_WU: OperateL(unsigned short,(unsigned short)0;count = -1,*pi0 > result,result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_L:  OperateL(int,(int)0x80000000;count = -1,*pi0 > result,result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_LU: OperateL(unsigned int,(unsigned int)0;count = -1,*pi0 > result,result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_QU: OperateL(quadword,minquad;count = -1,TdiGtQ(pi0,&result,0),
			result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_Q: OperateL(quadword,uminquad;count = -1,TdiGtQ(pi0,&result,1),
			result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_OU: OperateL(octaword,minocta;count = -1,TdiGtO(pi0,&result,0),
			result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_O: OperateL(octaword,uminocta;count = -1,TdiGtO(pi0,&result,1),
			result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_F:  OperateFloc(float,DTYPE_F  ,-HUGE,>)
    case DTYPE_FS: OperateFloc(float,DTYPE_FS ,-HUGE,>)
    case DTYPE_G:  OperateFloc(double,DTYPE_G ,-HUGE,>)
    case DTYPE_D:  OperateFloc(double,DTYPE_D ,-HUGE,>)
    case DTYPE_FT: OperateFloc(double,DTYPE_FT,-HUGE,>)
    default: return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3MinLoc(struct descriptor *in, struct descriptor *mask, 
	       struct descriptor *out,int count0, int count1, int count2,
	       int step0, int step1, int step2)
{
  int count;
  SetupArgs
  switch (in->dtype)
  {
    case DTYPE_T:  OperateL(int,(int)pi1;count = -1,Tdi3Lt(pi0,*(int *)result),
		         result = (int)pi0;count = j0;,*outp++ = count)
    case DTYPE_B:  OperateL(char,(char)127;count = -1,*pi0 < result,result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_BU: OperateL(unsigned char,(unsigned char)255;count = -1,*pi0 < result,result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_W:  OperateL(short,(short)32767;count = -1,*pi0 < result,result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_WU: OperateL(unsigned short,(unsigned short)0xffff;count = -1,*pi0 < result,result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_L:  OperateL(int,(int)2147483647;count = -1,*pi0 < result,result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_LU: OperateL(unsigned int,(unsigned int)0xffffffff;count = -1,*pi0 < result,result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_QU: OperateL(quadword,maxquad;count = -1,TdiLtQ(pi0,&result,0),
			result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_Q: OperateL(quadword,umaxquad;count = -1,TdiLtQ(pi0,&result,1),
			result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_OU: OperateL(octaword,maxocta;count = -1,TdiLtO(pi0,&result,0),
			result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_O: OperateL(octaword,umaxocta;count = -1,TdiLtO(pi0,&result,1),
			result = *pi0;count = j0;,*outp++ = count)
    case DTYPE_F:  OperateFloc(float,DTYPE_F  ,HUGE,<)
    case DTYPE_FS: OperateFloc(float,DTYPE_FS ,HUGE,<)
    case DTYPE_G:  OperateFloc(double,DTYPE_G ,HUGE,<)
    case DTYPE_D:  OperateFloc(double,DTYPE_D ,HUGE,<)
    case DTYPE_FT: OperateFloc(double,DTYPE_FT,HUGE,<)
    default: return TdiINVDTYDSC;
  }
  return 1;
}

#define OperateFval(type,dtype,start,operator) \
{ type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  count = -1;\
  for (j2 = 0; j2++ < count2; pi2 += step2, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < count1; pi1 += step1,pm1 += stepm1) {\
      double result = start;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < count0; j0++, pi0 += step0,pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          if (CvtConvertFloat(pi0,dtype,&val,DTYPE_DOUBLE,0) && (val operator result))\
            {result = val; count = j0;}\
        }\
      }\
      CvtConvertFloat(&result,DTYPE_DOUBLE,outp++,dtype,0);\
    }\
  }\
  break;\
}

int Tdi3MaxVal(struct descriptor *in, struct descriptor *mask, 
	       struct descriptor *out,int count0, int count1, int count2,
	       int step0, int step1, int step2)
{
  int count;
  SetupArgs
  switch (in->dtype)
  {
    case DTYPE_T:  Operate(int,(int)pi1,Tdi3Gt(pi0,*(int *)result),
		         result = (int)pi0;count = j0;,memmove(out++,(int *)result,step0))
    case DTYPE_B:  Operate(char,(char)-128,*pi0 > result,result = *pi0;,*outp++ = result)
    case DTYPE_BU: Operate(unsigned char,(unsigned char)0,*pi0 > result,result = *pi0;,*outp++ = result)
    case DTYPE_W:  Operate(short,(short)-32768,*pi0 > result,result = *pi0;,*outp++ = result)
    case DTYPE_WU: Operate(unsigned short,(unsigned short)0,*pi0 > result,result = *pi0;,*outp++ = result)
    case DTYPE_L:  Operate(int,(int)0x80000000,*pi0 > result,result = *pi0;,*outp++ = result)
    case DTYPE_LU: Operate(unsigned int,(unsigned int)0,*pi0 > result,result = *pi0;,*outp++ = result)
    case DTYPE_QU: Operate(quadword,uminquad,TdiGtQ(pi0,&result,0),
			memcpy(&result,pi0,sizeof(result));,memcpy(outp++,&result,sizeof(result)))
    case DTYPE_Q: Operate(quadword,minquad,TdiGtQ(pi0,&result,1),
			memcpy(&result,pi0,sizeof(result));,memcpy(outp++,&result,sizeof(result)))
    case DTYPE_OU: Operate(octaword,uminocta,TdiGtO(pi0,&result,0),
			memcpy(&result,pi0,sizeof(result));,memcpy(outp++,&result,sizeof(result)))
    case DTYPE_O: Operate(octaword,minocta,TdiGtO(pi0,&result,1),
			memcpy(&result,pi0,sizeof(result));,memcpy(outp++,&result,sizeof(result)))
    case DTYPE_F:  OperateFval(float,DTYPE_F  ,-HUGE,>)
    case DTYPE_FS: OperateFval(float,DTYPE_FS ,-HUGE,>)
    case DTYPE_G:  OperateFval(double,DTYPE_G ,-HUGE,>)
    case DTYPE_D:  OperateFval(double,DTYPE_D ,-HUGE,>)
    case DTYPE_FT: OperateFval(double,DTYPE_FT,-HUGE,>)
    default: return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3MinVal(struct descriptor *in, struct descriptor *mask, 
	       struct descriptor *out,int count0, int count1, int count2,
	       int step0, int step1, int step2)
{
  int count;
  SetupArgs
  switch (in->dtype)
  {
    case DTYPE_T:  Operate(int,(int)pi1,Tdi3Lt(pi0,*(int *)result),
		         result = (int)pi0;count = j0;,memmove(out++,(int *)result,step0))
    case DTYPE_B:  Operate(char,(char)127,*pi0 < result,result = *pi0;,*outp++ = result)
    case DTYPE_BU: Operate(unsigned char,(unsigned char)255,*pi0 < result,result = *pi0;,*outp++ = result)
    case DTYPE_W:  Operate(short,(short)32767,*pi0 < result,result = *pi0;,*outp++ = result)
    case DTYPE_WU: Operate(unsigned short,(unsigned short)0xffff,*pi0 < result,result = *pi0;,*outp++ = result)
    case DTYPE_L:  Operate(int,(int)2147483647,*pi0 < result,result = *pi0;,*outp++ = result)
    case DTYPE_LU: Operate(unsigned int,(unsigned int)0xffffffff,*pi0 < result,result = *pi0;,*outp++ = result)
    case DTYPE_QU: Operate(quadword,umaxquad,TdiLtQ(pi0,&result,0),
			result = *pi0;,*outp++ = result)
    case DTYPE_Q: Operate(quadword,maxquad,TdiLtQ(pi0,&result,1),
			result = *pi0;,*outp++ = result)
    case DTYPE_OU: Operate(octaword,umaxocta,TdiLtO(pi0,&result,0),
			result = *pi0;,*outp++ = result)
    case DTYPE_O: Operate(octaword,maxocta,TdiLtO(pi0,&result,1),
			result = *pi0;,*outp++ = result)
    case DTYPE_F:  OperateFval(float,DTYPE_F  ,HUGE,<)
    case DTYPE_FS: OperateFval(float,DTYPE_FS ,HUGE,<)
    case DTYPE_G:  OperateFval(double,DTYPE_G ,HUGE,<)
    case DTYPE_D:  OperateFval(double,DTYPE_D ,HUGE,<)
    case DTYPE_FT: OperateFval(double,DTYPE_FT,HUGE,<)
    default: return TdiINVDTYDSC;
  }
  return 1;
}

#define OperateFmean(type,dtype) \
{ type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  count = 0;\
  for (j2 = 0; j2++ < count2; pi2 += step2, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < count1; pi1 += step1,pm1 += stepm1) {\
      double result = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < count0; j0++, pi0 += step0,pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          if (CvtConvertFloat(pi0,dtype,&val,DTYPE_DOUBLE,0))\
            {result += val; count++;}\
        }\
      }\
      if (count)\
      {\
        result /= count;\
        CvtConvertFloat(&result,DTYPE_DOUBLE,outp++,dtype,0);\
      }\
      else\
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
    }\
  }\
  break;\
}

#define OperateFmeanc(type,dtype) \
{ type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  count = 0;\
  for (j2 = 0; j2++ < count2; pi2 += step2, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < count1; pi1 += step1,pm1 += stepm1) {\
      double result = 0;\
      double resulti = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < count0; j0++, pi0 += step0,pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          double vali;\
          if (CvtConvertFloat(&pi0[0],dtype,&val,DTYPE_DOUBLE,0) && \
              CvtConvertFloat(&pi0[1],dtype,&vali,DTYPE_DOUBLE,0)) \
            {result += val; resulti += vali; count++;}\
        }\
      }\
      if (count)\
      {\
        result /= count;\
        resulti /= count;\
        CvtConvertFloat(&result,DTYPE_DOUBLE,outp++,dtype,0);\
        CvtConvertFloat(&resulti,DTYPE_DOUBLE,outp++,dtype,0);\
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
{ type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  type result=setup;\
  for (j2 = 0; j2++ < count2; pi2 += step2, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < count1; pi1 += step1,pm1 += stepm1) {\
      result = setup;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < count0; j0++, pi0 += step0,\
           pm0 += stepm0) {\
        if (*pm0 & 1) {doit}\
      }\
      final;\
    }\
  }\
  break;\
}

int Tdi3Mean(struct descriptor *in, struct descriptor *mask, 
	       struct descriptor *out,int count0, int count1, int count2,
	       int step0, int step1, int step2)
{
  int count;
  SetupArgs
  switch (in->dtype)
  {
    case DTYPE_B:  Operate(char,(char)0;count = 0,1,result = (char)(result + *pi0); count++;,*outp++ = (char)(result/count))
    case DTYPE_BU: Operate(unsigned char,(unsigned char)0;count = 0,1,result = (unsigned char)(result + *pi0); count++;,
                         *outp++ = (unsigned char)(result/count))
    case DTYPE_W:  Operate(short,(short)0;count = 0,1,result = (short)(result + *pi0); count++;,*outp++ = (short)(result/count))
    case DTYPE_WU: Operate(unsigned short,(unsigned short)0;count = 0,1,result = (unsigned short)(result + *pi0); count++;,
                                       *outp++ = (unsigned short)(result/count))
    case DTYPE_L:  Operate(int,(int)0;count = 0,1,result += *pi0; count++;,*outp++ = result/count)
    case DTYPE_LU: Operate(unsigned int,(unsigned int)0;count = 0,1,result += *pi0; count++;,*outp++ = result/count)
    case DTYPE_QU: 
    case DTYPE_Q: Operate(quadword,qzero;count = 0,1,
			TdiAddQuadword(pi0, &result, &result); count++;,
			{quadword temp = qzero;memcpy(&temp,&count,sizeof(int));
			TdiDivQ(&result,&temp,outp++,DTYPE_Q);})
    case DTYPE_OU: Operate(octaword,ozero;count = 0,1,
			TdiAddOctaword(pi0, &result, &result); count++;,
			{octaword temp = ozero;count;memcpy(&temp,&count,sizeof(int));
			TdiDivO(&result,&temp,outp++,DTYPE_OU);})
    case DTYPE_O: Operate(octaword,ozero;count = 0,1,
			TdiAddOctaword(pi0, &result, &result); count++;,
			{octaword temp = ozero;count;memcpy(&temp,&count,sizeof(int));
			TdiDivO(&result,&temp,outp++,DTYPE_O);})
    case DTYPE_F:  OperateFmean(float,DTYPE_F)
    case DTYPE_FS: OperateFmean(float,DTYPE_FS)
    case DTYPE_G:  OperateFmean(double,DTYPE_G)
    case DTYPE_D:  OperateFmean(double,DTYPE_D)
    case DTYPE_FT: OperateFmean(double,DTYPE_FT)
    case DTYPE_FC:  OperateFmeanc(float,DTYPE_F)
    case DTYPE_FSC: OperateFmeanc(float,DTYPE_FS)
    case DTYPE_GC:  OperateFmeanc(double,DTYPE_G)
    case DTYPE_DC:  OperateFmeanc(double,DTYPE_D)
    case DTYPE_FTC: OperateFmeanc(double,DTYPE_FT)
    default: return TdiINVDTYDSC;
  }
  return 1;
}

#define OperateFprod(type,dtype) \
{ type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  for (j2 = 0; j2++ < count2; pi2 += step2, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < count1; pi1 += step1,pm1 += stepm1) {\
      double result = 0;\
      int bad = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < count0; j0++, pi0 += step0,pm0 += stepm0) {\
        if (!bad && *pm0 & 1)\
        {\
          double val;\
          if (CvtConvertFloat(pi0,dtype,&val,DTYPE_DOUBLE,0)) result *= val; else bad = 1;\
        }\
      }\
      if (!bad)\
        CvtConvertFloat(&result,DTYPE_DOUBLE,outp++,dtype,0);\
      else\
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
    }\
  }\
  break;\
}

#define OperateFprodc(type,dtype) \
{ type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  for (j2 = 0; j2++ < count2; pi2 += step2, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < count1; pi1 += step1,pm1 += stepm1) {\
      double result = 0;\
      double resulti = 0;\
      int bad = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < count0; j0++, pi0 += step0,pm0 += stepm0) {\
        if (!bad && *pm0 & 1)\
        {\
          double val;\
          double vali;\
          if (CvtConvertFloat(&pi0[0],dtype,&val,DTYPE_DOUBLE,0) && \
              CvtConvertFloat(&pi0[1],dtype,&vali,DTYPE_DOUBLE,0)) \
            {result = result * val - resulti * vali; resulti = result * vali + resulti * val;} else bad=1;\
        }\
      }\
      if (!bad)\
      {\
        CvtConvertFloat(&result,DTYPE_DOUBLE,outp++,dtype,0);\
        CvtConvertFloat(&resulti,DTYPE_DOUBLE,outp++,dtype,0);\
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
	       struct descriptor *out,int count0, int count1, int count2,
	       int step0, int step1, int step2)
{
  SetupArgs
  switch (in->dtype)
  {
    case DTYPE_B:  Operate(char,1,1,result = (char)(result * *pi0);,*outp++ = result)
    case DTYPE_BU: Operate(unsigned char,1,1,result = (unsigned char)(result * *pi0);,*outp++ = result)
    case DTYPE_W:  Operate(short,1,1,result = (short)(result * *pi0);,*outp++ = result)
    case DTYPE_WU: Operate(unsigned short,1,1,result = (unsigned short)(result * *pi0);,*outp++ = result)
    case DTYPE_L:  Operate(int,1,1,result *= *pi0;,*outp++ = result)
    case DTYPE_LU: Operate(unsigned int,1,1,result *= *pi0;,*outp++ = result)
    case DTYPE_QU: 
    case DTYPE_Q: Operate(quadword,qzero;result.q0=1;,1,
			TdiMultiplyQuadword(pi0, &result, &result);,
			*outp++ = result)
    case DTYPE_OU:
    case DTYPE_O: Operate(octaword,ozero;result.o0=1;,1,
			TdiMultiplyOctaword(pi0, &result, &result);,
			*outp++ = result)
    case DTYPE_F:  OperateFprod(float,DTYPE_F)
    case DTYPE_FS: OperateFprod(float,DTYPE_FS)
    case DTYPE_G:  OperateFprod(double,DTYPE_G)
    case DTYPE_D:  OperateFprod(double,DTYPE_D)
    case DTYPE_FT: OperateFprod(double,DTYPE_FT)
    case DTYPE_FC:  OperateFprodc(float,DTYPE_F)
    case DTYPE_FSC: OperateFprodc(float,DTYPE_FS)
    case DTYPE_GC:  OperateFprodc(double,DTYPE_G)
    case DTYPE_DC:  OperateFprodc(double,DTYPE_D)
    case DTYPE_FTC: OperateFprodc(double,DTYPE_FT)
    default: return TdiINVDTYDSC;
  }
  return 1;
}

#define OperateFsum(type,dtype) \
{ type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  int count=0;\
  for (j2 = 0; j2++ < count2; pi2 += step2, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < count1; pi1 += step1,pm1 += stepm1) {\
      double result = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < count0; j0++, pi0 += step0,pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          if (CvtConvertFloat(pi0,dtype,&val,DTYPE_DOUBLE,0))\
            {result += val;count++;}\
        }\
      }\
      if (count)\
        CvtConvertFloat(&result,DTYPE_DOUBLE,outp++,dtype,0);\
      else\
        CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
    }\
  }\
  break;\
}

#define OperateFsumc(type,dtype) \
{ type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  int count = 0;\
  for (j2 = 0; j2++ < count2; pi2 += step2, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < count1; pi1 += step1,pm1 += stepm1) {\
      double result = 0;\
      double resulti = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < count0; j0++, pi0 += step0,pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          double vali;\
          if (CvtConvertFloat(&pi0[0],dtype,&val,DTYPE_DOUBLE,0) && \
              CvtConvertFloat(&pi0[1],dtype,&vali,DTYPE_DOUBLE,0)) \
            {result += val; resulti += vali; count++;}\
        }\
      }\
      if (count)\
      {\
        CvtConvertFloat(&result,DTYPE_DOUBLE,outp++,dtype,0);\
        CvtConvertFloat(&resulti,DTYPE_DOUBLE,outp++,dtype,0);\
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
	     struct descriptor *out,int count0, int count1, int count2,
	     int step0, int step1, int step2)
{
  SetupArgs
  switch (in->dtype)
  {
    case DTYPE_B:  Operate(char,0,1,result = (char)(result + *pi0);,*outp++ = result)
    case DTYPE_BU: Operate(unsigned char,0,1,result = (unsigned char)(result + *pi0);,*outp++ = result)
    case DTYPE_W:  Operate(short,0,1,result = (short)(result + *pi0);,*outp++ = result)
    case DTYPE_WU: Operate(unsigned short,0,1,result = (unsigned short)(result + *pi0);,*outp++ = result)
    case DTYPE_L:  Operate(int,0,1,result += *pi0;,*outp++ = result)
    case DTYPE_LU: Operate(unsigned int,0,1,result += *pi0;,*outp++ = result)
    case DTYPE_QU: 
    case DTYPE_Q: Operate(quadword,qzero,1,
			TdiAddQuadword(pi0, &result, &result);,
			*outp++ = result)
    case DTYPE_OU:
    case DTYPE_O: Operate(octaword,ozero,1,
			TdiAddOctaword(pi0, &result, &result);,
			*outp++ = result)
    case DTYPE_F:  OperateFsum(float,DTYPE_F)
    case DTYPE_FS: OperateFsum(float,DTYPE_FS)
    case DTYPE_G:  OperateFsum(double,DTYPE_G)
    case DTYPE_D:  OperateFsum(double,DTYPE_D)
    case DTYPE_FT: OperateFsum(double,DTYPE_FT)
    case DTYPE_FC:  OperateFsumc(float,DTYPE_F)
    case DTYPE_FSC: OperateFsumc(float,DTYPE_FS)
    case DTYPE_GC:  OperateFsumc(double,DTYPE_G)
    case DTYPE_DC:  OperateFsumc(double,DTYPE_D)
    case DTYPE_FTC: OperateFsumc(double,DTYPE_FT)
    default: return TdiINVDTYDSC;
  }
  return 1;
}

#define OperateFaccum(type,dtype) \
{ type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  count = 0;\
  for (j2 = 0; j2++ < count2; pi2 += step2, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < count1; pi1 += step1,pm1 += stepm1) {\
      double result = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < count0; j0++, pi0 += step0,pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          if (CvtConvertFloat(pi0,dtype,&val,DTYPE_DOUBLE,0))\
          { result += val; count++; CvtConvertFloat(&result,DTYPE_DOUBLE,outp++,dtype,0);}\
          else CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
        }\
      }\
    }\
  }\
  break;\
}

#define OperateFaccumc(type,dtype) \
{ type *outp = (type *)out->pointer;\
  type *pi0=(type *)in->pointer,*pi1=pi0,*pi2=pi0;\
  char *pm0, *pm1, *pm2 = (char *)mask->pointer;\
  count = 0;\
  for (j2 = 0; j2++ < count2; pi2 += step2, pm2 += stepm2) {\
    for (j1 = 0, pi1 = pi2, pm1 = pm2; j1++ < count1; pi1 += step1,pm1 += stepm1) {\
      double result = 0;\
      double resulti = 0;\
      for (j0 = 0, pi0 = pi1, pm0 = pm1; j0 < count0; j0++, pi0 += step0,pm0 += stepm0) {\
        if (*pm0 & 1)\
        {\
          double val;\
          double vali;\
          if (CvtConvertFloat(&pi0[0],dtype,&val,DTYPE_DOUBLE,0) && \
              CvtConvertFloat(&pi0[1],dtype,&vali,DTYPE_DOUBLE,0)) \
          { result += val; resulti += vali; count++; \
            CvtConvertFloat(&result,DTYPE_DOUBLE,outp++,dtype,0);\
            CvtConvertFloat(&resulti,DTYPE_DOUBLE,outp++,dtype,0);}\
          else {\
            CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
            CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);}\
          }\
        }\
      }\
    }\
  }\
  break;\
}

int Tdi3Accumulate(struct descriptor *in, struct descriptor *mask, 
	       struct descriptor *out,int count0, int count1, int count2,
	       int step0, int step1, int step2)
{
  SetupArgs
  switch (in->dtype)
  {
    case DTYPE_B:  Operate(char,0,1,result = (char)(result + *pi0); *outp++ = result;,continue)
    case DTYPE_BU: Operate(unsigned char,0,1,result = (unsigned char)(result + *pi0); *outp++ = result;,continue)
    case DTYPE_W:  Operate(short,0,1,result = (short)(result + *pi0); *outp++ = result;,continue)
    case DTYPE_WU: Operate(unsigned short,0,1,result = (unsigned short)(result + *pi0); *outp++ = result;,continue)
    case DTYPE_L:  Operate(int,0,1,result += *pi0; *outp++ = result;,continue)
    case DTYPE_LU: Operate(unsigned int,0,1,result += *pi0; *outp++ = result;,continue)
    case DTYPE_QU: 
    case DTYPE_Q: Operate(quadword,qzero,1,
			TdiAddQuadword(pi0, &result, &result); *outp++ = result;,
			continue)
    case DTYPE_OU:
    case DTYPE_O: Operate(octaword,ozero,1,
			TdiAddOctaword(pi0, &result, &result); *outp++ = result;,
			continue)
    case DTYPE_F:  OperateFsum(float,DTYPE_F)
    case DTYPE_FS: OperateFsum(float,DTYPE_FS)
    case DTYPE_G:  OperateFsum(double,DTYPE_G)
    case DTYPE_D:  OperateFsum(double,DTYPE_D)
    case DTYPE_FT: OperateFsum(double,DTYPE_FT)
    case DTYPE_FC:  OperateFsumc(float,DTYPE_F)
    case DTYPE_FSC: OperateFsumc(float,DTYPE_FS)
    case DTYPE_GC:  OperateFsumc(double,DTYPE_G)
    case DTYPE_DC:  OperateFsumc(double,DTYPE_D)
    case DTYPE_FTC: OperateFsumc(double,DTYPE_FT)
    default: return TdiINVDTYDSC;
  }
  return 1;
}

int TdiLtO(unsigned int *in1,unsigned int *in2,int is_signed)
{
    int j,longwords=4,k=0,*i1,*i2;
    for (j=longwords-1;j>=0;j--)
      if (!j || in1[j] != in2[j])
        if (!is_signed || j != longwords-1)
          {k = (in1[j] < in2[j]); break;}
        else {i1 = (int *)(in1+j);
	      i2 = (int *)(in2+j);
          k = (*i1 < *i2); break;}
    return k;
}

int TdiGtO(unsigned int *in1,unsigned int *in2,int is_signed)
{
    int j,longwords=4,k=0,*i1,*i2;
    for (j=longwords-1;j>=0;j--)
      if (!j || in1[j] != in2[j])
        if (!is_signed || j != longwords-1)
          {k = (in1[j] > in2[j]); break;}
        else {i1 = (int *)(in1+j);
	      i2 = (int *)(in2+j);
          k = (*i1 > *i2); break;}
    return k;
}

int TdiLtQ(unsigned int *in1,unsigned int *in2,int is_signed)
{
    int j,longwords=2,k=0,*i1,*i2;
    for (j=longwords-1;j>=0;j--)
      if (!j || in1[j] != in2[j])
        if (!is_signed || j != longwords-1)
          {k = (in1[j] < in2[j]); break;}
        else {i1 = (int *)(in1+j);
	      i2 = (int *)(in2+j);
          k = (*i1 < *i2); break;}
    return k;
}

int TdiGtQ(unsigned int *in1,unsigned int *in2,int is_signed)
{
    int j,longwords=2,k=0,*i1,*i2;
    for (j=longwords-1;j>=0;j--)
      if (!j || in1[j] != in2[j])
        if (!is_signed || j != longwords-1)
          {k = (in1[j] > in2[j]); break;}
        else {i1 = (int *)(in1+j);
	      i2 = (int *)(in2+j);
          k = (*i1 > *i2); break;}
    return k;
}
/*  CMS REPLACEMENT HISTORY, Element Tdi3MaxVal.C */
/*  *68   26-AUG-1996 16:07:01 TWF "Fix constant in testit" */
/*  *67   26-AUG-1996 15:08:28 TWF "remove ints.h" */
/*  *66   21-AUG-1996 12:23:48 TWF "Fix min int value" */
/*  *65   13-AUG-1996 14:02:29 TWF "add ieee support" */
/*  *64   29-JUL-1996 15:40:21 TWF "Fix addx and subx calls" */
/*  *63   26-JUL-1996 13:36:05 TWF "Fix extern definition" */
/*  *62   26-JUL-1996 13:29:32 TWF "Add DIVQ" */
/*  *61   26-JUL-1996 12:27:42 TWF "Special handling for alpha and vms" */
/*  *60    3-JUL-1996 09:16:34 TWF "Port to Windows/Unix" */
/*  *59    3-JUL-1996 08:59:05 TWF "Port to Windows/Unix" */
/*  *58    1-JUL-1996 11:46:16 TWF "Port to Unix/Windows" */
/*  *57   28-JUN-1996 11:48:06 TWF "Port to Unix/Windows" */
/*  *56   27-JUN-1996 17:02:20 TWF "Port to Unix/Windows" */
/*  *55   27-JUN-1996 17:00:16 TWF "Port to Unix/Windows" */
/*  *54   27-JUN-1996 16:56:10 TWF "Port to Unix/Windows" */
/*  *53   27-JUN-1996 16:37:59 TWF "Port to Unix/Windows" */
/*  *52   27-JUN-1996 16:35:57 TWF "Port to Unix/Windows" */
/*  *51   27-JUN-1996 16:15:07 TWF "Port to Unix/Windows" */
/*  *50   20-JUN-1996 15:49:24 TWF "Port to Unix/Windows" */
/*  *49   18-OCT-1995 13:51:18 TWF "Fix array reference" */
/*  *48   18-OCT-1995 13:50:50 TWF "Fix array reference" */
/*  *47   18-OCT-1995 13:50:03 TWF "Fix array reference" */
/*  *46   18-OCT-1995 13:49:27 TWF "Fix array reference" */
/*  *45   17-OCT-1995 16:16:18 TWF "use <builtins.h> form" */
/*  *44   19-OCT-1994 12:26:43 TWF "Use TDI$MESSAGES" */
/*  *43   19-OCT-1994 10:36:39 TWF "No longer support VAXC" */
/*  *42   15-NOV-1993 10:09:07 TWF "Add memory block" */
/*  *41   15-NOV-1993 09:41:37 TWF "Add memory block" */
/*  *40    6-NOV-1993 08:56:33 MRL "Fix octal stuff." */
/*  *39    6-NOV-1993 08:53:55 MRL "Fix octal stuff." */
/*  *38    6-NOV-1993 08:34:09 MRL "Fix octal stuff." */
/*  *37    6-NOV-1993 08:20:14 MRL "Fix octal stuff." */
/*  *36    6-NOV-1993 08:17:39 MRL "Fix octal stuff." */
/*  *35    6-NOV-1993 08:10:02 MRL "Fix octal stuff." */
/*  *34    6-NOV-1993 07:23:11 MRL "Fix octal stuff." */
/*  *33    6-NOV-1993 07:18:18 MRL "Fix octal stuff." */
/*  *32    5-NOV-1993 16:37:58 MRL "" */
/*  *31    5-NOV-1993 15:42:47 MRL "" */
/*  *30    5-NOV-1993 13:26:13 MRL "TYPO" */
/*  *29    5-NOV-1993 13:22:54 MRL "TYPO" */
/*  *28    5-NOV-1993 13:02:22 MRL "" */
/*  *27    4-NOV-1993 16:20:17 MRL "Ignore complex." */
/*  *26   15-OCT-1993 16:46:39 MRL "" */
/*  *25   15-OCT-1993 13:43:00 MRL "" */
/*  *24   15-OCT-1993 11:49:28 MRL "" */
/*  *23   15-OCT-1993 11:46:21 MRL "" */
/*  *22   15-OCT-1993 11:42:49 MRL "" */
/*  *21   15-OCT-1993 11:11:44 MRL "" */
/*  *20   15-OCT-1993 10:14:31 MRL "" */
/*  *19   15-OCT-1993 09:47:52 MRL "" */
/*  *18   14-OCT-1993 19:00:01 MRL "" */
/*  *17   14-OCT-1993 18:49:42 MRL "" */
/*  *16   14-OCT-1993 18:46:31 MRL "" */
/*  *15   14-OCT-1993 16:20:21 MRL "" */
/*  *14   14-OCT-1993 16:06:11 MRL "" */
/*  *13   14-OCT-1993 14:10:10 MRL "" */
/*  *12   14-OCT-1993 13:32:38 MRL "" */
/*  *11   14-OCT-1993 13:22:53 MRL "" */
/*  *10   14-OCT-1993 13:17:21 MRL "" */
/*  *9    14-OCT-1993 13:12:26 MRL "" */
/*  *8    14-OCT-1993 12:34:01 MRL "" */
/*  *7    14-OCT-1993 11:28:07 MRL "" */
/*  *6    14-OCT-1993 11:25:58 MRL "" */
/*  *5    14-OCT-1993 10:36:35 MRL "Add tdi$divo" */
/*  *4    14-OCT-1993 10:31:20 MRL "Add tdi$divo" */
/*  *3    14-OCT-1993 10:26:04 MRL "Add tdi$divo" */
/*  *2    13-OCT-1993 16:42:57 MRL "" */
/*  *1    13-OCT-1993 15:42:41 MRL "" */
/*  CMS REPLACEMENT HISTORY, Element Tdi3MaxVal.C */
