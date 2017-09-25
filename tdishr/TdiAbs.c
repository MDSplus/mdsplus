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
#include <string.h>
#include <mdsdescrip.h>
#include <tdishr_messages.h>
#include <math.h>
#include <STATICdef.h>

extern int TdiConvert();
extern int TdiSubtractQuadword();
extern int TdiSubtractOctaword();
extern int TdiUnary();
extern int Tdi3Multiply();
extern int CvtConvertFloat();

#define min(a,b) ((a)<(b)) ? (a) : (b)
#define max(a,b) ((a)<(b)) ? (b) : (a)

typedef struct {
  int int32[2];
} Int64;
typedef struct {
  unsigned int int32[2];
} uInt64;
typedef struct {
  Int64 longword[2];
} octaword;

#define negate128 TdiSubtractOctaword(&octazero,&in[i],&out[i])

STATIC_CONSTANT octaword octazero = {{{{0,0}},{{0,0}}}};

#define copy64 out[i].int32[0]=in[i].int32[0]; out[i].int32[1]=in[i].int32[1]
#define zero64 out[i].int32[0]=0; out[i].int32[1]=0
#define negate64 TdiSubtractQuadword(&octazero,&in[i],&out[i])
#define abs64  if (in[i].int32[1] < 0) { negate64; } else { copy64;}
#define zero128 out[i].longword[0].int32[0]=0; out[i].longword[0].int32[1]=0;\
         out[i].longword[1].int32[0]=0; out[i].longword[1].int32[1]=0;
#define copy128 out[i].longword[0].int32[0]=in[i].longword[0].int32[0]; \
                out[i].longword[0].int32[1]=in[i].longword[0].int32[1]; \
                out[i].longword[1].int32[0]=in[i].longword[1].int32[0]; \
                out[i].longword[1].int32[1]=in[i].longword[1].int32[1];
#define abs128 if (in[i].longword[1].int32[1] < 0) TdiSubtractOctaword(&octazero,&in[i],&out[i]); else { copy128; }
#define not64 out[i].int32[0] = ~in[i].int32[0]; out[i].int32[1] = ~in[i].int32[1]

#ifdef WORDS_BIGENDIAN
#define bool64 out[i]=(unsigned char)(1 & in[i].int32[1])
#define bool128 out[i]=(unsigned char)(1 & in[i].longword[0].int32[3])
#else
#define bool64 out[i]=(unsigned char)(1 & in[i].int32[0])
#define bool128 out[i]=(unsigned char)(1 & in[i].longword[0].int32[0])
#endif

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
    start_operate(unsigned char) out[i] = in[i];
    end_operate case DTYPE_WU:start_operate(unsigned short int) out[i] = in[i];
    end_operate case DTYPE_LU:start_operate(unsigned int) out[i] = in[i];
    end_operate case DTYPE_QU:start_operate(uInt64) copy64;
    end_operate case DTYPE_OU:out_count = out_count * 2;
    start_operate(uInt64)
	copy64;
    end_operate case DTYPE_B:start_operate(char) out[i] = (char)(in[i] > 0 ? in[i] : -in[i]);
    end_operate case DTYPE_W:start_operate(short int)
	out[i] = (short int)(in[i] > 0 ? in[i] : -in[i]);
    end_operate case DTYPE_L:start_operate(int) out[i] = in[i] > 0 ? in[i] : -in[i];
    end_operate case DTYPE_Q:start_operate(Int64)
    abs64 end_operate case DTYPE_O:start_operate(octaword) abs128;
    end_operate case DTYPE_F:start_operate(float)
	AbsFloat(DTYPE_F)
	end_operate
	case DTYPE_FS:start_operate(float)
	AbsFloat(DTYPE_FS)
	end_operate
	case DTYPE_G:start_operate(double)
	AbsFloat(DTYPE_G)
	end_operate
	case DTYPE_D:start_operate(double)
	AbsFloat(DTYPE_D)
	end_operate
	case DTYPE_FT:start_operate(double)
	AbsFloat(DTYPE_FT)
	end_operate
	case DTYPE_FC:start_operate(float)
	AbsComplex(DTYPE_F)
	end_operate
	case DTYPE_FSC:start_operate(float)
	AbsComplex(DTYPE_FS)
	end_operate
	case DTYPE_GC:start_operate(double)
	AbsComplex(DTYPE_G)
	end_operate
	case DTYPE_DC:start_operate(double)
	AbsComplex(DTYPE_D)
	end_operate
	case DTYPE_FTC:start_operate(double)
	AbsComplex(DTYPE_FT) end_operate default:status = TdiINVDTYDSC;
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
    start_operate(unsigned char) out[i] = in[i];
    end_operate case DTYPE_WU:start_operate(unsigned short int) out[i] = in[i];
    end_operate case DTYPE_LU:start_operate(unsigned int) out[i] = in[i];
    end_operate case DTYPE_QU:start_operate(uInt64) copy64;
    end_operate case DTYPE_OU:out_count = out_count * 2;
    start_operate(uInt64)
	copy64;
    end_operate case DTYPE_B:start_operate(char) out[i] = (char)(in[i] > 0 ? in[i] : -in[i]);
    end_operate case DTYPE_W:start_operate(short int)
	out[i] = (short int)(in[i] > 0 ? in[i] : -in[i]);
    end_operate case DTYPE_L:start_operate(int) out[i] = in[i] > 0 ? in[i] : -in[i];
    end_operate case DTYPE_Q:start_operate(Int64)
    abs64 end_operate case DTYPE_O:start_operate(octaword) abs128;
    end_operate case DTYPE_F:start_operate(float)
	AbsFloat(DTYPE_F)
	end_operate
	case DTYPE_FS:start_operate(float)
	AbsFloat(DTYPE_FS)
	end_operate
	case DTYPE_G:start_operate(double)
	AbsFloat(DTYPE_G)
	end_operate
	case DTYPE_D:start_operate(double)
	AbsFloat(DTYPE_D)
	end_operate
	case DTYPE_FT:start_operate(double)
	AbsFloat(DTYPE_FT)
	end_operate
	case DTYPE_FC:start_operate(float)
	Abs1Complex(DTYPE_F)
	end_operate
	case DTYPE_FSC:start_operate(float)
	Abs1Complex(DTYPE_FS)
	end_operate
	case DTYPE_GC:start_operate(double)
	Abs1Complex(DTYPE_G)
	end_operate
	case DTYPE_DC:start_operate(double)
	Abs1Complex(DTYPE_D)
	end_operate
	case DTYPE_FTC:start_operate(double)
	Abs1Complex(DTYPE_FT) end_operate default:status = TdiINVDTYDSC;
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
    start_operate2(char) out[i] = 0;
    end_operate case DTYPE_W:case DTYPE_WU:start_operate2(short int) out[i] = 0;
    end_operate case DTYPE_L:case DTYPE_LU:start_operate2(int) out[i] = 0;
    end_operate case DTYPE_Q:case DTYPE_QU:start_operate2(Int64) zero64;
    end_operate case DTYPE_O:case DTYPE_OU:out_count = out_count * 2;
    start_operate2(Int64)
	zero64;
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
    end_operate case DTYPE_GC:case DTYPE_DC:case DTYPE_FTC:start_operate(double) memcpy(&out[i],
											&in[i * 2 +
											    1],
											sizeof
											(double));
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
  case DTYPE_BU:
  case DTYPE_B:
    start_operate(unsigned char) out[i] = in[i];
    end_operate case DTYPE_WU:case DTYPE_W:start_operate(unsigned short int) out[i] = in[i];
    end_operate case DTYPE_LU:case DTYPE_L:start_operate(unsigned int) out[i] = in[i];
    end_operate case DTYPE_QU:case DTYPE_Q:start_operate(uInt64) copy64;
    end_operate case DTYPE_OU:case DTYPE_O:out_count = out_count * 2;
    start_operate(uInt64)
	copy64;
    end_operate case DTYPE_F:case DTYPE_FS:start_operate(int) out[i] = in[i];
    end_operate case DTYPE_G:case DTYPE_D:case DTYPE_FT:start_operate(double) memcpy(&out[i],
										     &in[i],
										     sizeof
										     (double));
    end_operate case DTYPE_FC:start_operate(float) ConjgComplex(DTYPE_F) end_operate case
	DTYPE_FSC:start_operate(float) ConjgComplex(DTYPE_FS) end_operate case
	DTYPE_GC:start_operate(double) ConjgComplex(DTYPE_G) end_operate case
	DTYPE_DC:start_operate(double) ConjgComplex(DTYPE_D) end_operate case
	DTYPE_FTC:start_operate(double) ConjgComplex(DTYPE_FT) end_operate default:status =
	TdiINVDTYDSC;
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
  case DTYPE_BU:
    start_operate(unsigned char) out[i] = (unsigned char)~in[i];
    end_operate case DTYPE_WU:start_operate(unsigned short int) out[i] = (unsigned short)~in[i];
    end_operate case DTYPE_LU:start_operate(unsigned int) out[i] = ~in[i];
    end_operate case DTYPE_QU:start_operate(uInt64) not64;
    end_operate case DTYPE_OU:out_count = out_count * 2;
    start_operate(uInt64)
	not64;
    end_operate case DTYPE_B:start_operate(char) out[i] = (char)~in[i];
    end_operate case DTYPE_W:start_operate(short int) out[i] = (short int)~in[i];
    end_operate case DTYPE_L:start_operate(int) out[i] = ~in[i];
    end_operate case DTYPE_Q:start_operate(Int64) not64;
    end_operate case DTYPE_O:out_count = out_count * 2;
    start_operate(uInt64)
	not64;
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
  case DTYPE_BU:
    start_operate1(unsigned char, unsigned char) out[i] = (unsigned char)(1 & in[i]);
    end_operate case DTYPE_WU:start_operate1(unsigned short int, unsigned char)
	out[i] = (unsigned char)(1 & in[i]);
    end_operate case DTYPE_LU:start_operate1(unsigned int, unsigned char)
	out[i] = (unsigned char)(1 & in[i]);
    end_operate case DTYPE_QU:start_operate1(uInt64, unsigned char) bool64;
    end_operate case DTYPE_OU:start_operate1(octaword, unsigned char) bool128;
    end_operate case DTYPE_B:start_operate1(char, unsigned char) out[i] =
	(unsigned char)(1 & in[i]);
    end_operate case DTYPE_W:start_operate1(short int, unsigned char) out[i] =
	(unsigned char)(1 & in[i]);
    end_operate case DTYPE_L:start_operate1(int, unsigned char) out[i] = (unsigned char)(1 & in[i]);
    end_operate case DTYPE_Q:start_operate1(Int64, unsigned char) bool64;
    end_operate case DTYPE_O:start_operate1(octaword, unsigned char) bool128;
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
  case DTYPE_BU:
    start_operate1(unsigned char, unsigned char) out[i] = (unsigned char)!(in[i] & 1);
    end_operate case DTYPE_WU:start_operate1(unsigned short int, unsigned char)
	out[i] = (unsigned char)!(in[i] & 1);
    end_operate case DTYPE_LU:start_operate1(unsigned int, unsigned char)
	out[i] = (unsigned char)!(in[i] & 1);
    end_operate case DTYPE_QU:start_operate1(uInt64, unsigned char) bool64;
    out[i] = (unsigned char)!out[i];
    end_operate case DTYPE_OU:start_operate1(octaword, unsigned char) bool128;
    out[i] = (unsigned char)!out[i];
    end_operate case DTYPE_B:start_operate1(char, unsigned char)
	out[i] = (unsigned char)!(in[i] & 1);
    end_operate case DTYPE_W:start_operate1(short int, unsigned char)
	out[i] = (unsigned char)!(in[i] & 1);
    end_operate case DTYPE_L:start_operate1(int, unsigned char) out[i] =
	(unsigned char)!(in[i] & 1);
    end_operate case DTYPE_Q:start_operate1(Int64, unsigned char) bool64;
    out[i] = (unsigned char)!out[i];
    end_operate case DTYPE_O:start_operate1(octaword, unsigned char) bool128;
    out[i] = (unsigned char)!out[i];
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
  case DTYPE_BU:
    start_operate(unsigned char) out[i] = in[i];
    end_operate case DTYPE_WU:start_operate(unsigned short int) out[i] = in[i];
    end_operate case DTYPE_LU:start_operate(unsigned int) out[i] = in[i];
    end_operate case DTYPE_QU:start_operate(Int64) copy64;
    end_operate case DTYPE_OU:out_count = out_count * 2;
    start_operate(uInt64)
	copy64;
    end_operate case DTYPE_B:start_operate(char) out[i] = in[i];
    end_operate case DTYPE_W:start_operate(short int) out[i] = in[i];
    end_operate case DTYPE_L:start_operate(int) out[i] = in[i];
    end_operate case DTYPE_Q:start_operate(Int64) copy64;
    end_operate case DTYPE_O:out_count = out_count * 2;
    start_operate(uInt64)
	copy64;
    end_operate case DTYPE_F:start_operate1(float, int) NintFloat(DTYPE_F);
    end_operate case DTYPE_FS:start_operate1(float, int) NintFloat(DTYPE_FS);
    end_operate case DTYPE_G:start_operate1(double, int) NintFloat(DTYPE_G);
    end_operate case DTYPE_D:start_operate1(double, int) NintFloat(DTYPE_D);
    end_operate case DTYPE_FT:start_operate1(double, int) NintFloat(DTYPE_FT);
    end_operate case DTYPE_FC:start_operate1(float, int) NintComplex(DTYPE_F);
    end_operate case DTYPE_FSC:start_operate1(float, int) NintComplex(DTYPE_FS);
    end_operate case DTYPE_GC:start_operate1(double, int) NintComplex(DTYPE_G);
    end_operate case DTYPE_DC:start_operate1(double, int) NintComplex(DTYPE_D);
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
  case DTYPE_BU:
    start_operate(char) out[i] = (char)-in[i];
    end_operate case DTYPE_WU:start_operate(short int) out[i] = (short int)-in[i];
    end_operate case DTYPE_LU:start_operate(int) out[i] = -in[i];
    end_operate case DTYPE_QU:start_operate(Int64) negate64;
    end_operate case DTYPE_OU:start_operate(octaword)
	TdiSubtractOctaword(&octazero, &in[i], &out[i]);
    end_operate case DTYPE_B:start_operate(char) out[i] = (char)-in[i];
    end_operate case DTYPE_W:start_operate(short int) out[i] = (short int)-in[i];
    end_operate case DTYPE_L:start_operate(int) out[i] = -in[i];
    end_operate case DTYPE_Q:start_operate(Int64) negate64;
    end_operate case DTYPE_O:start_operate(octaword)
	TdiSubtractOctaword(&octazero, &in[i], &out[i]);
    end_operate case DTYPE_F:start_operate(float)
	UnaryMinusFloat(DTYPE_F)
	end_operate
	case DTYPE_FS:start_operate(float)
	UnaryMinusFloat(DTYPE_FS)
	end_operate
	case DTYPE_G:start_operate(double)
	UnaryMinusFloat(DTYPE_G)
	end_operate
	case DTYPE_D:start_operate(double)
	UnaryMinusFloat(DTYPE_D)
	end_operate
	case DTYPE_FT:start_operate(double)
	UnaryMinusFloat(DTYPE_FT)
	end_operate
	case DTYPE_FC:start_operate(float)
	UnaryMinusComplex(DTYPE_F)
	end_operate
	case DTYPE_FSC:start_operate(float)
	UnaryMinusComplex(DTYPE_FS)
	end_operate
	case DTYPE_GC:start_operate(double)
	UnaryMinusComplex(DTYPE_G)
	end_operate
	case DTYPE_DC:start_operate(double)
	UnaryMinusComplex(DTYPE_D)
	end_operate case DTYPE_FTC:start_operate(double) UnaryMinusComplex(DTYPE_FT)
    end_operate default:status = TdiINVDTYDSC;
  }
  return status;
}

/*  CMS REPLACEMENT HISTORY, Element Tdi3Abs.C */
/*  *59   30-AUG-1996 14:56:28 TWF "Add another case to nint" */
/*  *58   26-AUG-1996 17:04:37 TWF "Fix compile warnings" */
/*  *57   22-AUG-1996 09:02:19 TWF "remove mathdef" */
/*  *56   13-AUG-1996 17:07:21 TWF "Return 0 for abs(complex(0,0))" */
/*  *55   13-AUG-1996 16:04:47 TWF "Fix conjg" */
/*  *54   13-AUG-1996 15:33:08 TWF "Fix complex" */
/*  *53   13-AUG-1996 15:09:39 TWF "Fix sign" */
/*  *52   13-AUG-1996 14:02:35 TWF "remove LibAddx" */
/*  *51   12-AUG-1996 17:03:15 TWF "Add ieee support" */
/*  *50    9-AUG-1996 15:14:17 TWF "Include mdsshr.h" */
/*  *49    1-AUG-1996 17:21:03 TWF "Use int instead of long" */
/*  *48   29-JUL-1996 13:50:08 TWF "Fix conjg" */
/*  *47   29-JUL-1996 11:27:30 TWF "Fix unary minus" */
/*  *46   29-JUL-1996 11:24:19 TWF "Fix unary_minus" */
/*  *45   29-JUL-1996 10:13:13 TWF "Fix operatespecial" */
/*  *44   26-JUL-1996 12:21:45 TWF "Special handling for alpha and vms" */
/*  *43   10-JUL-1996 07:57:41 TWF "Fix DC" */
/*  *42   27-JUN-1996 16:26:04 TWF "Port to Unix/Windows" */
/*  *41   25-JUN-1996 08:46:43 TWF "Port to Unix/Windows" */
/*  *40   19-JUN-1996 09:27:26 TWF "Port to Unix/Windows" */
/*  *39   19-JUN-1996 08:48:23 TWF "Port to Unix/Windows" */
/*  *38   18-JUN-1996 11:17:59 TWF "Port to Unix/Windows" */
/*  *37   18-JUN-1996 11:16:01 TWF "Port to Unix/Windows" */
/*  *36   18-JUN-1996 10:54:10 TWF "Port to Unix/Windows" */
/*  *35   18-JUN-1996 10:50:31 TWF "Port to Unix/Windows" */
/*  *34   18-JUN-1996 09:39:59 TWF "Port to Unix/Windows" */
/*  *33   17-OCT-1995 16:16:48 TWF "use <builtins.h> form" */
/*  *32   19-OCT-1994 12:26:00 TWF "Use TDI$MESSAGES" */
/*  *31   19-OCT-1994 09:57:29 TWF "No longer support VAXC" */
/*  *30   15-NOV-1993 10:09:27 TWF "Add memory block" */
/*  *29   15-NOV-1993 09:42:06 TWF "Add memory block" */
/*  *28   20-OCT-1993 14:20:54 MRL "FIX NINT." */
/*  *27   20-OCT-1993 14:19:19 MRL "FIX NINT." */
/*  *26   20-OCT-1993 14:14:22 MRL "FIX NINT." */
/*  *25   20-OCT-1993 14:03:32 MRL "FIX NINT." */
/*  *24   20-OCT-1993 13:23:38 MRL "FIX NINT." */
/*  *23   20-OCT-1993 12:59:36 MRL "FIX NINT." */
/*  *22   20-OCT-1993 12:44:31 MRL "FIX NINT." */
/*  *21    1-OCT-1993 10:27:56 MRL "fix Octoword" */
/*  *20    1-OCT-1993 10:01:10 MRL "FIX SubtractOctoword(octozero" */
/*  *19   27-SEP-1993 12:24:29 MRL "Fix TdiFault" */
/*  *18   27-SEP-1993 12:18:18 MRL "fix TdiFault" */
/*  *17   27-SEP-1993 11:45:42 MRL "fix TdiFault" */
/*  *16   27-SEP-1993 11:37:23 MRL "typo" */
/*  *15   27-SEP-1993 11:35:43 MRL "add end_operateo" */
/*  *14   27-SEP-1993 11:19:31 MRL "typo" */
/*  *13   27-SEP-1993 11:17:16 MRL "typo" */
/*  *12   27-SEP-1993 11:12:38 MRL "Add fault test.   " */
/*  *11   25-SEP-1993 13:18:35 MRL "fix abssq" */
/*  *10   24-SEP-1993 22:35:12 MRL "fix abs1" */
/*  *9    24-SEP-1993 22:26:25 MRL "aimag fix" */
/*  *8    24-SEP-1993 22:15:56 MRL "TYPO" */
/*  *7    24-SEP-1993 22:00:59 MRL "TYPO" */
/*  *6    24-SEP-1993 21:51:42 MRL "TYPO" */
/*  *5    24-SEP-1993 21:45:04 MRL "TYPO" */
/*  *4    24-SEP-1993 21:42:27 MRL "Fix NOT." */
/*  *3    24-SEP-1993 21:37:35 MRL "Fix logical" */
/*  *2    24-SEP-1993 14:52:41 MRL "Fix bugs." */
/*  *1    24-SEP-1993 11:14:37 MRL "Tdi3Abs for ALPHA" */
/*  CMS REPLACEMENT HISTORY, Element Tdi3Abs.C */
