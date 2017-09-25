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

                Name:   Tdi3MATH2   

                Type:   C function

                Author: TOM FREDIAN

                Date:   5-OCT-1993 

                Purpose:
 
        Take two-argument arctangent or other math function of scalar or array.
        out = ATAN2(in1,in2)    arctangent in radians, 4 quadrants
        out = ATAN2D(in1,in2)   arctangent in degrees, 4 quadrants
        out = MOD(in1,in2)      remainder, in1 - int(in1/in2)*in2

------------------------------------------------------------------------------

        Call sequence: 

int Tdi3Atan2(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
int Tdi3Atan2d(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
int Tdi3Mod(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)

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
#include <math.h>
#include <mdsdescrip.h>
#include <mdsdescrip.h>
#include <tdishr_messages.h>



extern int CvtConvertFloat();
extern double WideIntToDouble();
extern void DoubleToWideInt();

#define radians_to_degrees 57.295778

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

STATIC_ROUTINE double mod_d(double in1, double in2)
{
  double intpart;
  modf((in2 != 0.0) ? in1 / in2 : 0., &intpart);
  return in1 - intpart * in2;
}

#define OperateFloatOne(dtype,routine,p1,p2) \
{ double a,b,ans;\
  if (CvtConvertFloat(p1,dtype,&a,DTYPE_NATIVE_DOUBLE,0) && CvtConvertFloat(p2,dtype,&b,DTYPE_NATIVE_DOUBLE,0))\
  { ans = routine(a,b);\
    CvtConvertFloat(&ans,DTYPE_NATIVE_DOUBLE,outp++,dtype,0);\
  } else CvtConvertFloat(&roprand,DTYPE_F,outp++,dtype,0); }

#define OperateFloat(type,dtype,routine) \
{ type *in1p = (type *)in1->pointer;\
  type *in2p = (type *)in2->pointer;\
  type *outp = (type *)out->pointer;\
  switch (scalars)\
  {\
    case 0: \
    case 3: while (nout--) OperateFloatOne(dtype,routine,in1p++,in2p++) break;\
    case 1: while (nout--) OperateFloatOne(dtype,routine,in1p,in2p++) break;\
    case 2: while (nout--) OperateFloatOne(dtype,routine,in1p++,in2p) break;\
  }\
  break;\
}

STATIC_ROUTINE void mod_bin(int size, int is_signed, char *in1, char *in2, char *out)
{
  double in1_d = WideIntToDouble(in1, size/sizeof(int), is_signed);
  double in2_d = WideIntToDouble(in2, size/sizeof(int), is_signed);
  double ans = mod_d(in1_d, in2_d);
  DoubleToWideInt(&ans, size/sizeof(int), out);
}

#define OperateBin(size,is_signed,routine) \
{ char *in1p = in1->pointer;\
  char *in2p = in2->pointer;\
  char *outp = out->pointer;\
  switch (scalars)\
  {\
    case 0:\
    case 3: while (nout--) {routine(size,is_signed,in1p, in2p, outp);   in1p += size; in2p += size; outp += size; } break; \
    case 1: while (nout--) {routine(size,is_signed,in1p, in2p, outp);                 in2p += size; outp += size; } break; \
    case 2: while (nout--) {routine(size,is_signed,in1p, in2p, outp);   in1p += size;               outp += size; } break; \
  }\
  break;\
}

int Tdi3Mod(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
    Operate(char, %)
    case DTYPE_BU:Operate(unsigned char, %)
    case DTYPE_W:Operate(short, %)
    case DTYPE_WU:Operate(unsigned short, %)
    case DTYPE_L:Operate(int, %)
    case DTYPE_LU:Operate(unsigned int, %)
    case DTYPE_Q:OperateBin(in1->length, 1, mod_bin)
    case DTYPE_QU:OperateBin(in1->length, 0, mod_bin)
    case DTYPE_O:OperateBin(in1->length, 1, mod_bin)
    case DTYPE_OU:OperateBin(in1->length, 0, mod_bin)
    case DTYPE_F:OperateFloat(float, DTYPE_F, mod_d);
  case DTYPE_FS:
    OperateFloat(float, DTYPE_FS, mod_d);
  case DTYPE_D:
    OperateFloat(double, DTYPE_D, mod_d);
  case DTYPE_G:
    OperateFloat(double, DTYPE_G, mod_d);
  case DTYPE_FT:
    OperateFloat(double, DTYPE_FT, mod_d);
  default:
    return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3Atan2(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_F:
    OperateFloat(float, DTYPE_F, atan2);
  case DTYPE_FS:
    OperateFloat(float, DTYPE_FS, atan2);
  case DTYPE_D:
    OperateFloat(double, DTYPE_D, atan2);
  case DTYPE_G:
    OperateFloat(double, DTYPE_G, atan2);
  case DTYPE_FT:
    OperateFloat(double, DTYPE_FT, atan2);
  default:
    return TdiINVDTYDSC;
  }
  return 1;
}

int Tdi3Atan2d(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_F:
    OperateFloat(float, DTYPE_F, radians_to_degrees * atan2);
  case DTYPE_FS:
    OperateFloat(float, DTYPE_FS, radians_to_degrees * atan2);
  case DTYPE_D:
    OperateFloat(double, DTYPE_D, radians_to_degrees * atan2);
  case DTYPE_G:
    OperateFloat(double, DTYPE_G, radians_to_degrees * atan2);
  case DTYPE_FT:
    OperateFloat(double, DTYPE_FT, radians_to_degrees * atan2);
  default:
    return TdiINVDTYDSC;
  }
  return 1;
}

/*  CMS REPLACEMENT HISTORY, Element Tdi3MATH2.C */
/*  *34   21-AUG-1996 14:26:09 TWF "Add ieee support" */
/*  *33    9-AUG-1996 15:14:02 TWF "include mathdef.h" */
/*  *32    1-AUG-1996 17:35:22 TWF "Use int instead of long" */
/*  *31   26-JUL-1996 12:26:41 TWF "Special handling for alpha and vms" */
/*  *30    3-JUL-1996 10:16:22 TWF "fix atan2" */
/*  *29   24-JUN-1996 11:45:26 TWF "Port to Unix/Windows" */
/*  *28   17-OCT-1995 16:17:07 TWF "use <builtins.h> form" */
/*  *27   19-OCT-1994 12:26:40 TWF "Use TDI$MESSAGES" */
/*  *26   19-OCT-1994 10:38:17 TWF "No longer support VAXC" */
/*  *25   19-OCT-1994 10:36:12 TWF "No longer support VAXC" */
/*  *24   15-NOV-1993 10:09:54 TWF "Add memory block" */
/*  *23   15-NOV-1993 09:42:30 TWF "Add memory block" */
/*  *22   28-OCT-1993 11:34:27 MRL "" */
/*  *21   28-OCT-1993 11:26:36 MRL "" */
/*  *20   28-OCT-1993 10:02:05 MRL "" */
/*  *19   27-OCT-1993 21:45:32 MRL "" */
/*  *18   27-OCT-1993 16:27:56 MRL "" */
/*  *17   16-OCT-1993 13:56:26 MRL "" */
/*  *16   15-OCT-1993 14:51:59 MRL "" */
/*  *15   15-OCT-1993 14:38:37 MRL "" */
/*  *14   15-OCT-1993 14:26:16 MRL "" */
/*  *13   15-OCT-1993 14:24:00 MRL "" */
/*  *12   14-OCT-1993 10:40:19 MRL "" */
/*  *11   13-OCT-1993 16:47:11 MRL "" */
/*  *10   12-OCT-1993 10:36:13 MRL "" */
/*  *9     9-OCT-1993 11:55:19 MRL "" */
/*  *8     8-OCT-1993 13:42:18 MRL "" */
/*  *7     8-OCT-1993 13:39:35 MRL "" */
/*  *6     8-OCT-1993 12:54:42 MRL "" */
/*  *5     8-OCT-1993 12:37:22 MRL "" */
/*  *4     8-OCT-1993 12:15:31 MRL "" */
/*  *3     8-OCT-1993 12:12:40 MRL "" */
/*  *2     8-OCT-1993 11:07:53 MRL "" */
/*  *1     8-OCT-1993 10:57:57 LONDON "" */
/*  CMS REPLACEMENT HISTORY, Element Tdi3MATH2.C */
