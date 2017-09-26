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

                Name:   Tdi3MATH1   

                Type:   C function

                Author: TOM FREDIAN

                Date:   5-OCT-1993 

                Purpose:
 
        Take square root or other math function of scalar or array.
        real or complex to same: COS EXP LOG SIN SQRT
        real to real only: ACOS ACOSD AINT ASIN ASIND ATAN ATAND ATANH
                COSD COSH FLOOR LOG2 LOG10 SIND SINH TAN TAND TANH
        complex to real: ARG ARGD

------------------------------------------------------------------------------

        Call sequence: 

int Tdi3xxx(struct descriptor *in, struct descriptor *out)

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

------------------------------------------------------------------------------*/

#include <STATICdef.h>
#include <stdlib.h>
#include <mdsdescrip.h>
#include <mdsdescrip.h>
#include <tdishr_messages.h>
#include <errno.h>
#include <math.h>



extern int CvtConvertFloat();

STATIC_CONSTANT int roprand = 0x8000;
#define radians_to_degrees 57.295778
#define degrees_to_radians 0.017453293

#define SetupArgs \
  struct descriptor_a *ina = (struct descriptor_a *)in;\
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
  switch (in->class)\
  {\
    case CLASS_S:\
    case CLASS_D: scalars |= 1; break;\
    case CLASS_A: if (ina->arsize/ina->length < (unsigned int)nout) return TdiINV_SIZE; break;\
    default:            return TdiINVCLADSC;\
   }

#define Operate(type,dtype,function) \
{ type *inp = (type *)in->pointer;\
  type *outp = (type *)out->pointer;\
  double in;\
  double ans;\
  while (nout--) {errno=0; \
                  if (CvtConvertFloat(inp++,dtype,&in,DTYPE_NATIVE_DOUBLE,0)) ans = function(in); else errno=-1;\
                  if (errno) CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0); else\
                             CvtConvertFloat(&ans,DTYPE_NATIVE_DOUBLE,outp++,dtype,0);} break; }

#define OperateC2C(type,dtype,function) \
{ type *inp = (type *)in->pointer;\
  type *outp = (type *)out->pointer;\
  double in[2];\
  double ans[2];\
  while (nout--) {errno=0; \
                  if (CvtConvertFloat(&inp[0],dtype,&in[0],DTYPE_NATIVE_DOUBLE,0) &&\
                      CvtConvertFloat(&inp[1],dtype,&in[1],DTYPE_NATIVE_DOUBLE,0))\
                       function(in,ans); else errno=-1;\
                  if (errno) {CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);\
                              CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0);} else {\
                              CvtConvertFloat(&ans[0],DTYPE_NATIVE_DOUBLE,outp++,dtype,0);\
                              CvtConvertFloat(&ans[1],DTYPE_NATIVE_DOUBLE,outp++,dtype,0);}\
                  inp+=2;\
                 } break; }

#define OperateC2S(type,dtype,function) \
{ type *inp = (type *)in->pointer;\
  type *outp = (type *)out->pointer;\
  double in[2];\
  double ans;\
  while (nout--) {errno=0; \
                  if (CvtConvertFloat(&inp[0],dtype,&in[0],DTYPE_NATIVE_DOUBLE,0) &&\
                      CvtConvertFloat(&inp[1],dtype,&in[1],DTYPE_NATIVE_DOUBLE,0))\
                       ans = function(in[1],in[0]); else errno=-1;\
                  if (errno) CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0); else \
                              CvtConvertFloat(&ans,DTYPE_NATIVE_DOUBLE,outp++,dtype,0);\
                  inp+=2;\
                 } break; }

#define mathsingle(name,function)\
int Tdi3##name(struct descriptor *in, struct descriptor *out)\
{\
  SetupArgs\
  switch (in->dtype)\
  {\
    case DTYPE_F:  Operate(float, DTYPE_F, function)\
    case DTYPE_FS: Operate(float, DTYPE_FS, function)\
    case DTYPE_G:  Operate(double, DTYPE_G, function)\
    case DTYPE_D:  Operate(double, DTYPE_D, function)\
    case DTYPE_FT: Operate(double, DTYPE_FT, function)\
    default: return TdiINVDTYDSC;\
  }\
  return 1;\
}

#define mathsinglewithkind(name,function)\
int Tdi3##name(struct descriptor *in, struct descriptor *kind, struct descriptor *out)\
{\
  SetupArgs\
  switch (in->dtype)\
  {\
    case DTYPE_F:  Operate(float, DTYPE_F, function)\
    case DTYPE_FS: Operate(float, DTYPE_FS, function)\
    case DTYPE_G:  Operate(double, DTYPE_G, function)\
    case DTYPE_D:  Operate(double, DTYPE_D, function)\
    case DTYPE_FT: Operate(double, DTYPE_FT, function)\
    default: return TdiINVDTYDSC;\
  }\
  return kind ? 1 : 1;\
}

#define mathboth(name,function)\
int Tdi3##name(struct descriptor *in, struct descriptor *out)\
{\
  SetupArgs\
  switch (in->dtype)\
  {\
    case DTYPE_F:   Operate(float, DTYPE_F, function)\
    case DTYPE_FS:  Operate(float, DTYPE_FS, function)\
    case DTYPE_G:   Operate(double, DTYPE_G, function)\
    case DTYPE_D:   Operate(double, DTYPE_D, function)\
    case DTYPE_FT:  Operate(double, DTYPE_FT, function)\
    case DTYPE_FC:  Operate(float, DTYPE_F, function)\
    case DTYPE_FSC: OperateC2C(float, DTYPE_FS, function##_complex)\
    case DTYPE_GC:  OperateC2C(double, DTYPE_G, function##_complex)\
    case DTYPE_DC:  OperateC2C(double, DTYPE_D, function##_complex)\
    case DTYPE_FTC: OperateC2C(double, DTYPE_FT, function##_complex)\
    default: return TdiINVDTYDSC;\
  }\
  return 1;\
}

#define mathcomplex(name,function)\
int Tdi3##name(struct descriptor *in, struct descriptor *out)\
{\
  SetupArgs\
  switch (in->dtype)\
  {\
    case DTYPE_FC:  OperateC2S(float, DTYPE_F, function)\
    case DTYPE_FSC: OperateC2S(float, DTYPE_FS, function)\
    case DTYPE_GC:  OperateC2S(double, DTYPE_G, function)\
    case DTYPE_DC:  OperateC2S(double, DTYPE_D, function)\
    case DTYPE_FTC: OperateC2S(double, DTYPE_FT, function)\
    default: return TdiINVDTYDSC;\
  }\
  return 1;\
}

STATIC_ROUTINE double anint(double val)
{
  int tmp;
  val += (val > 0.0) ? .5 : -.5;
  tmp = (int)val;
  return (double)tmp;
}

STATIC_ROUTINE double trunc_Static(double val)
{
  return (double)((int)val);
}

STATIC_ROUTINE void cos_complex(double *in, double *out)
{
  out[0] = cos(in[0]) * cosh(in[1]);
  out[1] = -sin(in[0]) * sinh(in[1]);
  return;
}

STATIC_ROUTINE void sin_complex(double *in, double *out)
{
  out[0] = sin(in[0]) * cosh(in[1]);
  out[1] = cos(in[0]) * sinh(in[1]);
  return;
}

STATIC_ROUTINE void exp_complex(double *in, double *out)
{
  out[0] = exp(in[0]);
  out[1] = out[0] * sin(in[1]);
  out[0] = out[0] * cos(in[1]);
  return;
}

STATIC_ROUTINE double cabs_d(double in1, double in2)
{
  double x = (in1 > 0) ? in1 : -in1;
  double y = (in2 > 0) ? in2 : -in2;
  if (x == 0.0)
    return y;
  else if (y == 0.0)
    return x;
  else if (x > y)
    return x * sqrt(1 + pow((y / x), 2.0));
  else
    return y * sqrt(1 + pow((x / y), 2.0));
}

STATIC_ROUTINE void sqrt_complex(double *in, double *out)
{
  if (in[0] == (double)0.0 && in[1] == (double)0.0) {
    out[0] = (double)0.0;
    out[1] = (double)0.0;
  } else {
    out[0] = sqrt((fabs(in[0]) + cabs_d(in[0], in[1])) * .5);
    if (in[0] > 0.0)
      out[1] = in[1] / (out[0] + out[0]);
    else {
      out[1] = (in[1] < 0.0) ? -out[0] : out[0];
      out[0] = in[1] / (out[1] + out[1]);
    }
  }
  return;
}

STATIC_ROUTINE void log_complex(double *in, double *out)
{
  double theta = 0;
  if (in[0] > 0.0)
    theta = 0.0;
  else if (in[0] < 0.0 && in[1] >= 0.0)
    theta = M_PI;
  else if (in[0] < 0.0)
    theta = -M_PI;
  else if (in[0] == 0 && in[1] == 0) {
    out[0] = 0.0;
    out[1] = 0.0;
    return;
  } else if (in[0] == 0.0 && in[1] > 0.0) {
    out[0] = log(in[1]);
    out[1] = M_PI_2;
    return;
  } else if (in[0] == 0.0) {
    out[0] = log(fabs(in[1]));
    out[1] = -M_PI_2;
    return;
  }
  out[1] = atan(in[1] / in[0]) + theta;
  out[0] = log(sqrt(in[0] * in[0] + in[1] * in[1]));
  return;
}

STATIC_ROUTINE double acosd_Static(double in)
{
  return radians_to_degrees * acos(in);
}

STATIC_ROUTINE double asind_Static(double in)
{
  return radians_to_degrees * asin(in);
}

STATIC_ROUTINE double atand_Static(double in)
{
  return radians_to_degrees * atan(in);
}

STATIC_ROUTINE double cosd_Static(double in)
{
  return cos(degrees_to_radians * in);
}

STATIC_ROUTINE double log2_Static(double in)
{
  return log10(in) / log10(2.);
}

STATIC_ROUTINE double sind_Static(double in)
{
  return sin(degrees_to_radians * in);
}

STATIC_ROUTINE double tand_Static(double in)
{
  return tan(degrees_to_radians * in);
}

STATIC_ROUTINE double atand2_Static(double in1, double in2)
{
  return radians_to_degrees * atan2(in1, in2);
}

STATIC_ROUTINE double atanh_Static(double in)
{
  double ans;
  if (in <= -1.0 || in >= 1.)
    CvtConvertFloat(&roprand, DTYPE_F, &ans, DTYPE_NATIVE_DOUBLE, 0);
  else
    ans = log((in + 1) / (in - 1)) / 2.0;
  return ans;
}

mathsingle(Atanh, atanh_Static)
    mathsingle(Cosh, cosh)
    mathsingle(Log2, log2_Static)
    mathsingle(Sinh, sinh)
    mathsingle(Tanh, tanh)
    mathsingle(Acos, acos)
    mathsingle(Acosd, acosd_Static)
    mathsingle(Asin, asin)
    mathsingle(Asind, asind_Static)
    mathsingle(Atan, atan)
    mathsingle(Atand, atand_Static)
    mathsingle(Cosd, cosd_Static)
    mathsingle(Floor, floor)
    mathsingle(Log10, log10)
    mathsingle(Sind, sind_Static)
    mathsingle(Tan, tan)
    mathsingle(Tand, tand_Static)
    mathsinglewithkind(Aint, trunc_Static)
    mathsinglewithkind(Anint, anint)
    mathboth(Cos, cos)
    mathboth(Exp, exp)
    mathboth(Log, log)
    mathboth(Sin, sin)
    mathboth(Sqrt, sqrt)
    mathcomplex(Arg, atan2)
    mathcomplex(Argd, atand2_Static)
/*  CMS REPLACEMENT HISTORY, Element Tdi3MATH1.C */
/*  *51   27-AUG-1996 07:42:11 TWF "Fix compile warnings" */
/*  *50   21-AUG-1996 11:56:00 TWF "fix arg" */
/*  *49   21-AUG-1996 11:46:42 TWF "Fix anint" */
/*  *48   21-AUG-1996 11:15:15 TWF "Fix aint and anint" */
/*  *47   21-AUG-1996 10:10:09 TWF "Fix anint and aint" */
/*  *46   21-AUG-1996 09:26:56 TWF "Add degree trig functions" */
/*  *45   16-AUG-1996 16:24:14 TWF "Add ieee support" */
/*  *44   16-AUG-1996 16:18:55 TWF "Add ieee support" */
/*  *43    9-AUG-1996 15:14:22 TWF "include mathdef.h" */
/*  *42   26-JUL-1996 12:26:23 TWF "Special handling for alpha and vms" */
/*  *41    3-JUL-1996 15:49:24 TWF "" */
/*  *40    3-JUL-1996 14:26:00 TWF "fix mod" */
/*  *39    3-JUL-1996 14:23:30 TWF "Fix anint" */
/*  *38    3-JUL-1996 08:26:11 TWF "type the arguments" */
/*  *37    3-JUL-1996 08:18:31 TWF "type the arguments" */
/*  *36   25-JUN-1996 09:06:04 TWF "Port to Unix/Windows" */
/*  *35   25-JUN-1996 08:53:16 TWF "Port to Unix/Windows" */
/*  *34   25-JUN-1996 08:52:18 TWF "Port to Unix/Windows" */
/*  *33   21-JUN-1996 13:58:57 TWF "Port to Unix/Windows" */
/*  *32   18-OCT-1995 13:47:51 TWF "linkage pragma does not support macros on unix" */
/*  *31   17-OCT-1995 16:17:05 TWF "use <builtins.h> form" */
/*  *30   16-DEC-1994 16:30:46 TWF "Fix $ROPRAND" */
/*  *29   16-DEC-1994 11:47:18 TWF "Fix $ROPRAND" */
/*  *28   16-DEC-1994 10:11:43 TWF "check for $ROPRAND before doing operation" */
/*  *27   16-DEC-1994 09:58:51 TWF "check for $ROPRAND before doing operation" */
/*  *26   16-DEC-1994 09:06:25 TWF "check for $ROPRAND before doing operation" */
/*  *25   16-DEC-1994 08:50:31 TWF "check for $ROPRAND before doing operation" */
/*  *24   16-DEC-1994 08:44:36 TWF "check for $ROPRAND before doing operation" */
/*  *23   19-OCT-1994 12:26:37 TWF "Use TDI$MESSAGES" */
/*  *22   19-OCT-1994 10:35:45 TWF "No longer support VAXC" */
/*  *21   15-NOV-1993 10:09:50 TWF "Add memory block" */
/*  *20   15-NOV-1993 09:42:27 TWF "Add memory block" */
/*  *19   28-OCT-1993 11:48:06 MRL "" */
/*  *18   28-OCT-1993 11:32:15 MRL "" */
/*  *17   27-OCT-1993 21:44:13 MRL "" */
/*  *16   27-OCT-1993 21:31:43 MRL "" */
/*  *15   27-OCT-1993 16:34:10 MRL "" */
/*  *14   27-OCT-1993 16:32:27 MRL "" */
/*  *13   27-OCT-1993 16:31:22 MRL "" */
/*  *12   27-OCT-1993 16:27:08 MRL "" */
/*  *11   27-OCT-1993 13:55:13 MRL "" */
/*  *10   26-OCT-1993 13:10:59 MRL "" */
/*  *9    26-OCT-1993 12:23:05 MRL "" */
/*  *8    26-OCT-1993 12:12:59 MRL "" */
/*  *7    26-OCT-1993 11:34:43 MRL "" */
/*  *6    26-OCT-1993 11:31:45 MRL "" */
/*  *5    26-OCT-1993 11:17:38 MRL "" */
/*  *4    26-OCT-1993 10:38:07 MRL "" */
/*  *3    26-OCT-1993 10:36:08 MRL "" */
/*  *2    26-OCT-1993 10:27:37 MRL "" */
/*  *1    26-OCT-1993 10:13:45 MRL "" */
/*  CMS REPLACEMENT HISTORY, Element Tdi3MATH1.C */
