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

       Name:    Tdi3$ABS

       Type:    C function

       Author:  Mark London
               MIT Plasma Fusion Center

       Date:    7-SEP-1993

       Purpose:
        Routine for simple operations on one VMS data types and classes.
        BU-H    = ABS(BU-HC)    absolute value, abs(x)=sqrt(real**2+imag**2)
        BU-H    = ABS1(BU-HC)   absolute value, abs1(x)=abs(real)+abs(imag)
        BU-H    = ABSSQ(BU-HC)  absolute value squared, abssq(x)=abs(x)**2=x*conjg(x)
        BU-H    = AIMAG(BU-HC)  imaginary part of complex
        BU-HC   = CONJG(BU-HC)  complex conjugate
        BU-OU   = INOT(BU-O)    complement of integer
        BU      = LOGICAL(BU-O) low bit of integer, 0 or 1
        BU      = NOT(BU-O)     reverse of low bit of integer
        BU-O    = NINT(BU-HC)   nearest integer, LONG used for DGH
        B-HC    = - (BU-HC)     negative

        Unsupported: LOGICAL, NOT, or INOT for real or complex.
        Input and output memory overlap produces UNDEFIINOTD results.
        Coincidence of input and output, however, will work here. WATCH THIS!!!!
        Lengths must be correct for all classes.

       Call sequence:
               struct descriptor *in_ptr;
               struct descriptor *out_ptr;

               status = Tdi3Abs(in,out)
               status = Tdi3UnaryMinus(in,out)
       returns:
                TdiINVDTYDSC - if unsupported data type
                1 - if supported data type

------------------------------------------------------------------------------
       Copyright (c) 1993
       Property of Massachusetts Institute of Technology, Cambridge MA 02139.
       This program cannot be copied or distributed in any form for non-MIT
       use without specific written approval of MIT Plasma Fusion Center
       Management.
------------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include <mdsdescrip.h>
#include <tdishr_messages.h>
#include <math.h>
#include <STATICdef.h>
#include <int128.h>

extern int TdiConvert();
extern int TdiUnary();
extern int Tdi3Multiply();
extern int CvtConvertFloat();

#define min(a,b) ((a)<(b)) ? (a) : (b)
#define max(a,b) ((a)<(b)) ? (b) : (a)

STATIC_CONSTANT const int roprand = 0x8000;

#define start_operate(type) {\
  type *in=(type *)(in_ptr->pointer); \
  type *out=(type *)(out_ptr->pointer); \
  for (i=0;i<out_count;i++) {

#define start_operate1(itype,otype) {\
  itype *in=(itype *)(in_ptr->pointer); \
  otype *out=(otype *)(out_ptr->pointer); \
  for (i=0;i<out_count;i++) {

#define start_operate2(type) {\
  type *out=(type *)(out_ptr->pointer); \
  for (i=0;i<out_count;i++) {

#define end_operate } break; }

#define AbsFloat(dtype) \
     double tmp;\
     if (CvtConvertFloat(&in[i],dtype,&tmp,DTYPE_NATIVE_DOUBLE,0)) \
     {\
       tmp = tmp > 0 ? tmp : -tmp;\
       CvtConvertFloat(&tmp,DTYPE_NATIVE_DOUBLE,&out[i],dtype,0);\
     }\
     else\
       CvtConvertFloat(&roprand,DTYPE_F,&out[i],dtype,0);

#define UnaryMinusFloat(dtype) \
     double tmp;\
     if (CvtConvertFloat(&in[i],dtype,&tmp,DTYPE_NATIVE_DOUBLE,0)) \
     {\
       tmp = -tmp;\
       CvtConvertFloat(&tmp,DTYPE_NATIVE_DOUBLE,&out[i],dtype,0);\
     }\
     else\
       CvtConvertFloat(&roprand,DTYPE_F,&out[i],dtype,0);

#define UnaryMinusComplex(dtype) \
     double tmp;\
     if (CvtConvertFloat(&in[i*2],dtype,&tmp,DTYPE_NATIVE_DOUBLE,0)) \
     {\
       tmp = -tmp;\
       CvtConvertFloat(&tmp,DTYPE_NATIVE_DOUBLE,&out[i*2],dtype,0);\
     }\
     else\
       CvtConvertFloat(&roprand,DTYPE_F,&out[i*2],dtype,0);\
     if (CvtConvertFloat(&in[i*2+1],dtype,&tmp,DTYPE_NATIVE_DOUBLE,0)) \
     {\
       tmp = -tmp;\
       CvtConvertFloat(&tmp,DTYPE_NATIVE_DOUBLE,&out[i*2+1],dtype,0);\
     }\
     else\
       CvtConvertFloat(&roprand,DTYPE_F,&out[i*2+1],dtype,0);

#define NintFloat(dtype) \
     double tmp;\
     if (CvtConvertFloat(&in[i],dtype,&tmp,DTYPE_NATIVE_DOUBLE,0)) \
     {\
       out[i] = (int)(tmp + (tmp > 0 ? .5 : -.5));\
     }\
     else\
       out[i] = 0;

#define NintComplex(dtype) \
     double tmp;\
     if (CvtConvertFloat(&in[i*2],dtype,&tmp,DTYPE_NATIVE_DOUBLE,0)) \
     {\
       out[i] = (int)(tmp + (tmp > 0 ? .5 : -.5));\
     }\
     else\
       out[i] = 0;

#define AbssqComplex(dtype) \
     double r;\
     double imag;\
     double ans;\
     if (CvtConvertFloat(&in[i*2],dtype,&r,DTYPE_NATIVE_DOUBLE,0) && \
         CvtConvertFloat(&in[i*2+1],dtype,&imag,DTYPE_NATIVE_DOUBLE,0)) \
     {\
       ans = r * r + imag * imag;\
       CvtConvertFloat(&ans,DTYPE_NATIVE_DOUBLE,&out[i],dtype,0);\
     }\
     else\
       CvtConvertFloat(&roprand,DTYPE_F,&out[i],dtype,0);

#define ConjgComplex(dtype) \
     double imag;\
     double ans;\
     memcpy(&out[i*2],&in[i*2],sizeof(in[i*2]));\
     if (CvtConvertFloat(&in[i*2+1],dtype,&imag,DTYPE_NATIVE_DOUBLE,0)) \
     {\
       ans = -imag;\
       CvtConvertFloat(&ans,DTYPE_NATIVE_DOUBLE,&out[i*2+1],dtype,0);\
     }\
     else\
       CvtConvertFloat(&roprand,DTYPE_F,&out[i*2+1],dtype,0);

#define AbsComplex(dtype) \
     double r;\
     double imag;\
     double mx;\
     double mn;\
     double ans;\
     int bad=0;\
     if (CvtConvertFloat(&in[i*2],dtype,&r,DTYPE_NATIVE_DOUBLE,0) &&\
         CvtConvertFloat(&in[i*2+1],dtype,&imag,DTYPE_NATIVE_DOUBLE,0))\
     {\
       r = r > 0 ? r : -r;\
       imag = imag > 0 ? imag : -imag;\
       mx = r >  imag ? r : imag;\
       mn = r <= imag ? r : imag;\
       if (mx > 0)\
         ans = mx * sqrt((mn*mn)/(mx*mx) + 1.);\
       else\
         ans=0.0;\
     }\
     else\
       bad = 1;\
     if (bad)\
       CvtConvertFloat(&roprand,DTYPE_F,&out[i],dtype,0);\
     else\
       CvtConvertFloat(&ans,DTYPE_NATIVE_DOUBLE,&out[i],dtype,0);

#define Abs1Complex(dtype) \
     double r;\
     double imag;\
     double ans;\
     int bad=0;\
     if (CvtConvertFloat(&in[i*2],dtype,&r,DTYPE_NATIVE_DOUBLE,0) &&\
         CvtConvertFloat(&in[i*2+1],dtype,&imag,DTYPE_NATIVE_DOUBLE,0))\
     {\
       r = r > 0 ? r : -r;\
       imag = imag > 0 ? imag : -imag;\
       ans = r + imag;\
     }\
     else\
       bad = 1;\
     if (bad)\
       CvtConvertFloat(&roprand,DTYPE_F,&out[i],dtype,0);\
     else\
       CvtConvertFloat(&ans,DTYPE_NATIVE_DOUBLE,&out[i],dtype,0);

int MthJNINT();
int MthJIGNNT();

int Tdi3Abs(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int out_count = 1;
  register int i;

  status = TdiUnary(in_ptr, out_ptr, &out_count);
  if STATUS_NOT_OK
    return status;

  switch (in_ptr->dtype) {
  case DTYPE_BU:
    start_operate(uint8_t) out[i] = in[i];
    end_operate case DTYPE_WU:start_operate(uint16_t) out[i] = in[i];
    end_operate case DTYPE_LU:start_operate(uint32_t) out[i] = in[i];
    end_operate case DTYPE_QU:start_operate(uint64_t) out[i] = in[i];
    end_operate case DTYPE_OU:start_operate(uint128_t); memcpy(&out[i],&in[i],sizeof(uint128_t));
    end_operate case DTYPE_B:start_operate(int8_t) out[i] = (int8_t)(in[i] > 0 ? in[i] : -in[i]);
    end_operate case DTYPE_W:start_operate(int16_t)
        out[i] = (int16_t)(in[i] > 0 ? in[i] : -in[i]);
    end_operate case DTYPE_L:start_operate(int32_t) out[i] = in[i] > 0 ? in[i] : -in[i];
    end_operate case DTYPE_Q:start_operate(int64_t) out[i] = in[i] > 0 ? in[i] : -in[i];
    end_operate case DTYPE_O:start_operate(int128_t) int128_abs(&in[i],&out[i]);
    end_operate case DTYPE_F:start_operate(float) AbsFloat(DTYPE_F)
    end_operate case DTYPE_FS:start_operate(float) AbsFloat(DTYPE_FS)
    end_operate case DTYPE_G:start_operate(double) AbsFloat(DTYPE_G)
    end_operate case DTYPE_D:start_operate(double) AbsFloat(DTYPE_D)
    end_operate case DTYPE_FT:start_operate(double) AbsFloat(DTYPE_FT)
    end_operate case DTYPE_FC:start_operate(float) AbsComplex(DTYPE_F)
    end_operate case DTYPE_FSC:start_operate(float) AbsComplex(DTYPE_FS)
    end_operate case DTYPE_GC:start_operate(double) AbsComplex(DTYPE_G)
    end_operate case DTYPE_DC:start_operate(double) AbsComplex(DTYPE_D)
    end_operate case DTYPE_FTC:start_operate(double) AbsComplex(DTYPE_FT)
    end_operate default:status = TdiINVDTYDSC;
  }
  return status;
}

int Tdi3Abs1(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int out_count = 1;
  register int i;

  status = TdiUnary(in_ptr, out_ptr, &out_count);
  if STATUS_NOT_OK
    return status;

  switch (in_ptr->dtype) {
  case DTYPE_BU:
    start_operate(uint8_t) out[i] = in[i];
    end_operate case DTYPE_WU:start_operate(uint16_t) out[i] = in[i];
    end_operate case DTYPE_LU:start_operate(uint32_t) out[i] = in[i];
    end_operate case DTYPE_QU:start_operate(uint64_t) out[i] = in[i];
    end_operate case DTYPE_OU:start_operate(uint128_t); memcpy(&out[i],&in[i],sizeof(uint128_t));
    end_operate case DTYPE_B:start_operate(int8_t) out[i] = (int8_t)(in[i] > 0 ? in[i] : -in[i]);
    end_operate case DTYPE_W:start_operate(int16_t) out[i] = (int16_t)(in[i] > 0 ? in[i] : -in[i]);
    end_operate case DTYPE_L:start_operate(int32_t) out[i] = in[i] > 0 ? in[i] : -in[i];
    end_operate case DTYPE_Q:start_operate(int64_t) out[i] = in[i] > 0 ? in[i] : -in[i];
    end_operate case DTYPE_O:start_operate(int128_t) int128_abs(&in[i],&out[i]);
    end_operate case DTYPE_F:start_operate(float) AbsFloat(DTYPE_F)
    end_operate case DTYPE_FS:start_operate(float) AbsFloat(DTYPE_FS)
    end_operate case DTYPE_G:start_operate(double) AbsFloat(DTYPE_G)
    end_operate case DTYPE_D:start_operate(double) AbsFloat(DTYPE_D)
    end_operate case DTYPE_FT:start_operate(double) AbsFloat(DTYPE_FT)
    end_operate case DTYPE_FC:start_operate(float) Abs1Complex(DTYPE_F)
    end_operate case DTYPE_FSC:start_operate(float) Abs1Complex(DTYPE_FS)
    end_operate case DTYPE_GC:start_operate(double) Abs1Complex(DTYPE_G)
    end_operate case DTYPE_DC:start_operate(double) Abs1Complex(DTYPE_D)
    end_operate case DTYPE_FTC:start_operate(double) Abs1Complex(DTYPE_FT)
    end_operate default:status = TdiINVDTYDSC;
  }
  return status;
}


int Tdi3AbsSq(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int out_count = 1;
  register int i;

  status = TdiUnary(in_ptr, out_ptr, &out_count);
  if STATUS_NOT_OK
    return status;

  switch (in_ptr->dtype) {
  case DTYPE_BU:
  case DTYPE_WU:
  case DTYPE_LU:
  case DTYPE_QU:
  case DTYPE_OU:
  case DTYPE_B:
  case DTYPE_W:
  case DTYPE_L:
  case DTYPE_Q:
  case DTYPE_O:
  case DTYPE_F:
  case DTYPE_FS:
  case DTYPE_G:
  case DTYPE_D:
  case DTYPE_FT:
    status = Tdi3Multiply(in_ptr, in_ptr, out_ptr);
    break;
  case DTYPE_FC:
    start_operate(float) AbssqComplex(DTYPE_F);
    end_operate case DTYPE_FSC:start_operate(float) AbssqComplex(DTYPE_FS);
    end_operate case DTYPE_GC:start_operate(double) AbssqComplex(DTYPE_G);
    end_operate case DTYPE_DC:start_operate(double) AbssqComplex(DTYPE_D);
    end_operate case DTYPE_FTC:start_operate(double) AbssqComplex(DTYPE_FT);
    end_operate default:status = TdiINVDTYDSC;
  }

  return status;
}

int Tdi3Aimag(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int out_count = 1;
  register int i;

  status = TdiUnary(in_ptr, out_ptr, &out_count);
  if STATUS_NOT_OK
    return status;

  switch (in_ptr->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    start_operate2(int8_t) out[i] = 0;
    end_operate case DTYPE_W:case DTYPE_WU:start_operate2(int16_t) out[i] = 0;
    end_operate case DTYPE_L:case DTYPE_LU:start_operate2(int32_t) out[i] = 0;
    end_operate case DTYPE_Q:case DTYPE_QU:start_operate2(int64_t) out[i] = 0;
    end_operate case DTYPE_O:case DTYPE_OU:start_operate2(int128_t)   out[i].low=0;out[i].high=0;;
    end_operate case DTYPE_F:start_operate2(float)
    float ans = (float)0.0;
    CvtConvertFloat(&ans, DTYPE_NATIVE_FLOAT, &out[i], DTYPE_F, 0);
    end_operate case DTYPE_FS:start_operate2(float)
    float ans = (float)0.0;
    CvtConvertFloat(&ans, DTYPE_NATIVE_FLOAT, &out[i], DTYPE_FS, 0);
    end_operate case DTYPE_G:start_operate2(double)
    double ans = 0.0;
    CvtConvertFloat(&ans, DTYPE_NATIVE_DOUBLE, &out[i], DTYPE_G, 0);
    end_operate case DTYPE_D:start_operate2(double)
    double ans = 0.0;
    CvtConvertFloat(&ans, DTYPE_NATIVE_DOUBLE, &out[i], DTYPE_D, 0);
    end_operate case DTYPE_FT:start_operate2(double)
    double ans = 0.0;
    CvtConvertFloat(&ans, DTYPE_NATIVE_DOUBLE, &out[i], DTYPE_FT, 0);
    end_operate case DTYPE_FC:case DTYPE_FSC:start_operate(int) out[i] = in[i * 2 + 1];
    end_operate case DTYPE_GC:case DTYPE_DC:case DTYPE_FTC:
	start_operate(double) memcpy(&out[i],&in[i*2+1],sizeof(double));
    end_operate default:status = TdiINVDTYDSC;
  }
  return status;
}

int Tdi3Conjg(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int out_count = 1;
  register int i;
  status = TdiUnary(in_ptr, out_ptr, &out_count);
  if STATUS_NOT_OK
    return status;
  switch (in_ptr->dtype) {
                case DTYPE_BU:case DTYPE_B:start_operate( uint8_t) out[i] = in[i];
    end_operate case DTYPE_WU:case DTYPE_W:start_operate(uint16_t) out[i] = in[i];
    end_operate case DTYPE_LU:case DTYPE_L:start_operate(uint32_t) out[i] = in[i];
    end_operate case DTYPE_QU:case DTYPE_Q:start_operate(uint64_t) out[i] = in[i];
    end_operate case DTYPE_OU:case DTYPE_O:start_operate(uint128_t)  memcpy(&out[i],&in[i],sizeof(int128_t));
    end_operate case DTYPE_F:case DTYPE_FS:start_operate(float)    out[i] = in[i];
    end_operate case DTYPE_G:case DTYPE_D:case DTYPE_FT:start_operate(double) out[i] = in[i];
    end_operate case DTYPE_FC:start_operate(float) ConjgComplex(DTYPE_F)
    end_operate case DTYPE_FSC:start_operate(float) ConjgComplex(DTYPE_FS)
    end_operate case DTYPE_GC:start_operate(double) ConjgComplex(DTYPE_G)
    end_operate case DTYPE_DC:start_operate(double) ConjgComplex(DTYPE_D)
    end_operate case DTYPE_FTC:start_operate(double) ConjgComplex(DTYPE_FT)
    end_operate default:status = TdiINVDTYDSC;
  }
  return status;
}

int Tdi3Inot(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int out_count = 1;
  register int i;
  status = TdiUnary(in_ptr, out_ptr, &out_count);
  if STATUS_NOT_OK
    return status;
  switch (in_ptr->dtype) {
                case DTYPE_B:case DTYPE_BU:start_operate(  uint8_t) out[i] = ~in[i];
    end_operate case DTYPE_W:case DTYPE_WU:start_operate( uint16_t) out[i] = ~in[i];
    end_operate case DTYPE_L:case DTYPE_LU:start_operate( uint32_t) out[i] = ~in[i];
    end_operate case DTYPE_Q:case DTYPE_QU:start_operate( uint64_t) out[i] = ~in[i];
    end_operate case DTYPE_O:case DTYPE_OU:start_operate(uint128_t) out[i].low=~in[i].low; out[i].high=~in[i].high;
    end_operate default:status = TdiINVDTYDSC;
  }
  return status;
}

int Tdi3Logical(struct descriptor *in_ptr, struct descriptor *kind __attribute__ ((unused)), struct descriptor *out_ptr)
{
  INIT_STATUS;
  int out_count = 1;
  register int i;
  status = TdiUnary(in_ptr, out_ptr, &out_count);
  if STATUS_NOT_OK
    return status;
  switch (in_ptr->dtype) {
                case DTYPE_B:case DTYPE_BU:start_operate1(  uint8_t, uint8_t) out[i] = (uint8_t)(1 & in[i]);
    end_operate case DTYPE_W:case DTYPE_WU:start_operate1( uint16_t, uint8_t) out[i] = (uint8_t)(1 & in[i]);
    end_operate case DTYPE_L:case DTYPE_LU:start_operate1( uint32_t, uint8_t) out[i] = (uint8_t)(1 & in[i]);
    end_operate case DTYPE_Q:case DTYPE_QU:start_operate1( uint64_t, uint8_t) out[i] = (uint8_t)(1 & in[i]);
    end_operate case DTYPE_O:case DTYPE_OU:start_operate1(uint128_t, uint8_t) out[i] = (uint8_t)(1 & in[i].low);
    end_operate default:status = TdiINVDTYDSC;
  }
  return status;
}

int Tdi3Not(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int out_count = 1;
  register int i;
  status = TdiUnary(in_ptr, out_ptr, &out_count);
  if STATUS_NOT_OK
    return status;
  switch (in_ptr->dtype) {
                case DTYPE_B:case DTYPE_BU:start_operate1(  uint8_t,uint8_t) out[i] = (uint8_t)!(in[i] & 1);
    end_operate case DTYPE_W:case DTYPE_WU:start_operate1( uint16_t,uint8_t) out[i] = (uint8_t)!(in[i] & 1);
    end_operate case DTYPE_L:case DTYPE_LU:start_operate1( uint32_t,uint8_t) out[i] = (uint8_t)!(in[i] & 1);
    end_operate case DTYPE_Q:case DTYPE_QU:start_operate1( uint64_t,uint8_t) out[i] = (uint8_t)!(in[i] & 1);
    end_operate case DTYPE_O:case DTYPE_OU:start_operate1(uint128_t,uint8_t) out[i] = (uint8_t)!(1 & in[i].low);
    end_operate default:status = TdiINVDTYDSC;
  }

  return status;
}

int Tdi3Nint(struct descriptor *in_ptr, struct descriptor *kind __attribute__ ((unused)), struct descriptor *out_ptr)
{
  INIT_STATUS;
  int out_count = 1;
  register int i;

  status = TdiUnary(in_ptr, out_ptr, &out_count);
  if STATUS_NOT_OK
    return status;

  switch (in_ptr->dtype) {
                case DTYPE_BU: start_operate(  uint8_t) out[i] = in[i];
    end_operate case DTYPE_WU: start_operate( uint16_t) out[i] = in[i];
    end_operate case DTYPE_LU: start_operate( uint32_t) out[i] = in[i];
    end_operate case DTYPE_QU: start_operate( uint64_t) out[i] = in[i];
    end_operate case DTYPE_OU: start_operate(uint128_t) memcpy(&out[i],&in[i],sizeof(uint128_t));
    end_operate case DTYPE_B:  start_operate(   int8_t) out[i] = in[i];
    end_operate case DTYPE_W:  start_operate(  int16_t) out[i] = in[i];
    end_operate case DTYPE_L:  start_operate(  int32_t) out[i] = in[i];
    end_operate case DTYPE_Q:  start_operate(  int64_t) out[i] = in[i];
    end_operate case DTYPE_O:  start_operate( int128_t) memcpy(&out[i],&in[i],sizeof(int128_t));
    end_operate case DTYPE_F:  start_operate1(float,  int) NintFloat(DTYPE_F);
    end_operate case DTYPE_FS: start_operate1(float,  int) NintFloat(DTYPE_FS);
    end_operate case DTYPE_G:  start_operate1(double, int) NintFloat(DTYPE_G);
    end_operate case DTYPE_D:  start_operate1(double, int) NintFloat(DTYPE_D);
    end_operate case DTYPE_FT: start_operate1(double, int) NintFloat(DTYPE_FT);
    end_operate case DTYPE_FC: start_operate1(float,  int) NintComplex(DTYPE_F);
    end_operate case DTYPE_FSC:start_operate1(float,  int) NintComplex(DTYPE_FS);
    end_operate case DTYPE_GC: start_operate1(double, int) NintComplex(DTYPE_G);
    end_operate case DTYPE_DC: start_operate1(double, int) NintComplex(DTYPE_D);
    end_operate case DTYPE_FTC:start_operate1(double, int) NintComplex(DTYPE_FT);
    end_operate default:status = TdiINVDTYDSC;
  }

  return status;
}

int Tdi3UnaryMinus(struct descriptor *in_ptr, struct descriptor *out_ptr)
{
  INIT_STATUS;
  int out_count = 1;
  register int i;

  status = TdiUnary(in_ptr, out_ptr, &out_count);
  if STATUS_NOT_OK
    return status;

  switch (in_ptr->dtype) {
                case DTYPE_BU: start_operate(  int8_t) out[i] = -in[i];
    end_operate case DTYPE_WU: start_operate( int16_t) out[i] = -in[i];
    end_operate case DTYPE_LU: start_operate( int32_t) out[i] = -in[i];
    end_operate case DTYPE_QU: start_operate( int64_t) out[i] = -in[i];
    end_operate case DTYPE_OU: start_operate(int128_t) int128_minus((int128_t*)&in[i], &out[i]);
    end_operate case DTYPE_B:  start_operate(  int8_t) out[i] = -in[i];
    end_operate case DTYPE_W:  start_operate( int16_t) out[i] = -in[i];
    end_operate case DTYPE_L:  start_operate( int32_t) out[i] = -in[i];
    end_operate case DTYPE_Q:  start_operate( int64_t) out[i] = -in[i];
    end_operate case DTYPE_O:  start_operate(int128_t) int128_minus((int128_t*)&in[i], &out[i]);
    end_operate case DTYPE_F:  start_operate(float)  UnaryMinusFloat(DTYPE_F)
    end_operate	case DTYPE_FS: start_operate(float)  UnaryMinusFloat(DTYPE_FS)
    end_operate case DTYPE_G:  start_operate(double) UnaryMinusFloat(DTYPE_G)
    end_operate case DTYPE_D:  start_operate(double) UnaryMinusFloat(DTYPE_D)
    end_operate case DTYPE_FT: start_operate(double) UnaryMinusFloat(DTYPE_FT)
    end_operate case DTYPE_FC: start_operate(float)  UnaryMinusComplex(DTYPE_F)
    end_operate case DTYPE_FSC:start_operate(float)  UnaryMinusComplex(DTYPE_FS)
    end_operate case DTYPE_GC: start_operate(double) UnaryMinusComplex(DTYPE_G)
    end_operate case DTYPE_DC: start_operate(double) UnaryMinusComplex(DTYPE_D)
    end_operate case DTYPE_FTC:start_operate(double) UnaryMinusComplex(DTYPE_FT)
    end_operate default:status = TdiINVDTYDSC;
  }
  return status;
}
