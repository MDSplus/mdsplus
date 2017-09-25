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

                Name:   Tdi3And

                Type:   C function

                Author: TOM FREDIAN

                Date:   18-FEB-1993

                Purpose: Logical LOW-BIT tests on two operands

        For AB  = 00,01,10,11
        FALSE   = 0,0,0,0       use 0b
        AND     = 0,0,0,1       .
        AND_NOT = 0,0,1,0       .
        A       = 0,0,1,1       use a
        NOR_NOT = 0,1,0,0       use AND_NOT(b,a)
        B       = 0,1,0,1       use b
        EOR     = 0,1,1,0       (is NEQV)
        OR      = 0,1,1,1       .
        NOR     = 1,0,0,0       .
NEOR =  EOR_NOT = 1,0,0,1       (is EQV)
        NOT B   = 1,0,1,0       use NOT(b)
        OR_NOT  = 1,0,1,1       .
        NOT A   = 1,1,0,0       use NOT(a)
        NAND_NOT= 1,1,0,1       use OR_NOT(b,a)
        NAND    = 1,1,1,0       .
        TRUE    = 1,1,1,1       use -1b

------------------------------------------------------------------------------

        Call sequence:

int Tdi3And(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)

------------------------------------------------------------------------------
   Copyright (c) 1993
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

        Description:

------------------------------------------------------------------------------*/

#include <string.h>

#include <mdsdescrip.h>
#include <tdishr_messages.h>
#include <STATICdef.h>



typedef struct {
  int longword[2];
} quadword;
typedef struct {
  int longword[4];
} octaword;

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
  }

#define Operate(type,operator) \
{ type *in1p = (type *)in1->pointer;\
  type *in2p = (type *)in2->pointer;\
  char *outp = (char *)out->pointer;\
  switch (scalars)\
  {\
    case 0: while (nout--) {*outp++ = (char)(1 & (*in1p++ operator *in2p++));} break; \
    case 1: while (nout--) {*outp++ = (char)(1 & (*in1p operator *in2p++));} break; \
    case 2: while (nout--) {*outp++ = (char)(1 & (*in1p++ operator *in2p));} break; \
    case 3: *outp = (char)(1 & (*in1p operator *in2p)); break; \
  }\
  break;\
}

#define OperateN(type,operator) \
{ type *in1p = (type *)in1->pointer;\
  type *in2p = (type *)in2->pointer;\
  char *outp = (char *)out->pointer;\
  switch (scalars)\
   {\
    case 0: while (nout--) {*outp++ = (char)(1 & ~(*in1p++ operator *in2p++));} break; \
    case 1: while (nout--) {*outp++ = (char)(1 & ~(*in1p operator *in2p++));} break; \
    case 2: while (nout--) {*outp++ = (char)(1 & ~(*in1p++ operator *in2p));} break; \
    case 3: *outp = (char)(1 & ~(*in1p operator *in2p)); break; \
  }\
  break;\
}

#ifdef WORDS_BIGENDIAN
#define setPointers in1p += (size-1); in2p += (size-1);
#else
#define setPointers
#endif

#define OperateSpecial(type,operator) \
{\
  char *in1p = (char *)in1->pointer;\
  char *in2p = (char *)in2->pointer;\
  char *outp = (char *)out->pointer;\
  int size = sizeof(type);\
  setPointers\
  switch (scalars)\
  {\
    case 0: while (nout--) {*outp++ = (char)(1 & (*in1p operator *in2p)); in1p += size; in2p += size;} break; \
    case 1: while (nout--) {*outp++ = (char)(1 & (*in1p operator *in2p)); in2p += size;} break; \
    case 2: while (nout--) {*outp++ = (char)(1 & (*in1p operator *in2p)); in1p += size;} break; \
    case 3:                 *outp =   (char)(1 & (*in1p operator *in2p)); break; \
  }\
  break;\
}

#define OperateSpecialN(type,operator) \
{\
  char *in1p = (char *)in1->pointer;\
  char *in2p = (char *)in2->pointer;\
  char *outp = (char *)out->pointer;\
  int size = sizeof(type);\
  setPointers\
  switch (scalars)\
  {\
    case 0: while (nout--) {*outp++ = (char)(1 & ~(*in1p operator *in2p)); in1p += size; in2p += size;} break; \
    case 1: while (nout--) {*outp++ = (char)(1 & ~(*in1p operator *in2p)); in2p += size;} break; \
    case 2: while (nout--) {*outp++ = (char)(1 & ~(*in1p operator *in2p)); in1p += size;} break; \
    case 3:                 *outp =   (char)(1 & ~(*in1p operator *in2p)); break; \
  }\
  break;\
}

int Tdi3And(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    Operate(unsigned char, &)
    case DTYPE_W:case DTYPE_WU:Operate(unsigned short, &)
    case DTYPE_L:case DTYPE_LU:Operate(unsigned int, &)
    case DTYPE_Q:OperateSpecial(quadword, &)
    case DTYPE_QU:OperateSpecial(quadword, &)
    case DTYPE_O:OperateSpecial(octaword, &)
    case DTYPE_OU:OperateSpecial(octaword, &)
    case DTYPE_F:case DTYPE_G:case DTYPE_FC:case DTYPE_GC:case DTYPE_D:case
	DTYPE_DC:return TdiINVDTYDSC;
  case DTYPE_H:
  case DTYPE_HC:
    return TdiNO_OPC;
  default:
    return TdiINVDTYDSC;
  }
  return MDSplusSUCCESS;
}

int Tdi3AndNot(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    Operate(unsigned char, &~)
    case DTYPE_W:case DTYPE_WU:Operate(unsigned short, &~)
    case DTYPE_L:case DTYPE_LU:Operate(unsigned int, &~)
    case DTYPE_Q:OperateSpecial(quadword, &~)
    case DTYPE_QU:OperateSpecial(quadword, &~)
    case DTYPE_O:OperateSpecial(octaword, &~)
    case DTYPE_OU:OperateSpecial(octaword, &~)
    case DTYPE_F:case DTYPE_G:case DTYPE_FC:case DTYPE_GC:case DTYPE_D:case
	DTYPE_DC:return TdiINVDTYDSC;
  case DTYPE_H:
  case DTYPE_HC:
    return TdiNO_OPC;
  default:
    return TdiINVDTYDSC;
  }
  return MDSplusSUCCESS;
}

int Tdi3Nand(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    OperateN(unsigned char, &)
    case DTYPE_W:case DTYPE_WU:OperateN(unsigned short, &)
    case DTYPE_L:case DTYPE_LU:OperateN(unsigned int, &)
    case DTYPE_Q:OperateSpecialN(quadword, &)
    case DTYPE_QU:OperateSpecialN(quadword, &)
    case DTYPE_O:OperateSpecialN(octaword, &)
    case DTYPE_OU:OperateSpecialN(octaword, &)
    case DTYPE_F:case DTYPE_G:case DTYPE_FC:case DTYPE_GC:case DTYPE_D:case
	DTYPE_DC:return TdiINVDTYDSC;
  case DTYPE_H:
  case DTYPE_HC:
    return TdiNO_OPC;
  default:
    return TdiINVDTYDSC;
  }
  return MDSplusSUCCESS;
}

int Tdi3NandNot(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    OperateN(unsigned char, &~)
    case DTYPE_W:case DTYPE_WU:OperateN(unsigned short, &~)
    case DTYPE_L:case DTYPE_LU:OperateN(unsigned int, &~)
    case DTYPE_Q:OperateSpecialN(quadword, &~)
    case DTYPE_QU:OperateSpecialN(quadword, &~)
    case DTYPE_O:OperateSpecialN(octaword, &~)
    case DTYPE_OU:OperateSpecialN(octaword, &~)
    case DTYPE_F:case DTYPE_G:case DTYPE_FC:case DTYPE_GC:case DTYPE_D:case
	DTYPE_DC:return TdiINVDTYDSC;
  case DTYPE_H:
  case DTYPE_HC:
    return TdiNO_OPC;
  default:
    return TdiINVDTYDSC;
  }
  return MDSplusSUCCESS;
}

int Tdi3Or(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    Operate(unsigned char, |)
    case DTYPE_W:case DTYPE_WU:Operate(unsigned short, |)
    case DTYPE_L:case DTYPE_LU:Operate(unsigned int, |)
    case DTYPE_Q:OperateSpecial(quadword, |)
    case DTYPE_QU:OperateSpecial(quadword, |)
    case DTYPE_O:OperateSpecial(octaword, |)
    case DTYPE_OU:OperateSpecial(octaword, |)
    case DTYPE_F:case DTYPE_G:case DTYPE_FC:case DTYPE_GC:case DTYPE_D:case
	DTYPE_DC:return TdiINVDTYDSC;
  case DTYPE_H:
  case DTYPE_HC:
    return TdiNO_OPC;
  default:
    return TdiINVDTYDSC;
  }
  return MDSplusSUCCESS;
}

int Tdi3Nor(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    OperateN(unsigned char, |)
    case DTYPE_W:case DTYPE_WU:OperateN(unsigned short, |)
    case DTYPE_L:case DTYPE_LU:OperateN(unsigned int, |)
    case DTYPE_Q:OperateSpecialN(quadword, |)
    case DTYPE_QU:OperateSpecialN(quadword, |)
    case DTYPE_O:OperateSpecialN(octaword, |)
    case DTYPE_OU:OperateSpecialN(octaword, |)
    case DTYPE_F:case DTYPE_G:case DTYPE_FC:case DTYPE_GC:case DTYPE_D:case
	DTYPE_DC:return TdiINVDTYDSC;
  case DTYPE_H:
  case DTYPE_HC:
    return TdiNO_OPC;
  default:
    return TdiINVDTYDSC;
  }
  return MDSplusSUCCESS;
}

int Tdi3NorNot(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    OperateN(unsigned char, |~)
    case DTYPE_W:case DTYPE_WU:OperateN(unsigned short, |~)
    case DTYPE_L:case DTYPE_LU:OperateN(unsigned int, |~)
    case DTYPE_Q:OperateSpecialN(quadword, |~)
    case DTYPE_QU:OperateSpecialN(quadword, |~)
    case DTYPE_O:OperateSpecialN(octaword, |~)
    case DTYPE_OU:OperateSpecialN(octaword, |~)
    case DTYPE_F:case DTYPE_G:case DTYPE_FC:case DTYPE_GC:case DTYPE_D:case
	DTYPE_DC:return TdiINVDTYDSC;
  case DTYPE_H:
  case DTYPE_HC:
    return TdiNO_OPC;
  default:
    return TdiINVDTYDSC;
  }
  return MDSplusSUCCESS;
}

int Tdi3OrNot(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    Operate(unsigned char, |~)
    case DTYPE_W:case DTYPE_WU:Operate(unsigned short, |~)
    case DTYPE_L:case DTYPE_LU:Operate(unsigned int, |~)
    case DTYPE_Q:OperateSpecial(quadword, |~)
    case DTYPE_QU:OperateSpecial(quadword, |~)
    case DTYPE_O:OperateSpecial(octaword, |~)
    case DTYPE_OU:OperateSpecial(octaword, |~)
    case DTYPE_F:case DTYPE_G:case DTYPE_FC:case DTYPE_GC:case DTYPE_D:case
	DTYPE_DC:return TdiINVDTYDSC;
  case DTYPE_H:
  case DTYPE_HC:
    return TdiNO_OPC;
  default:
    return TdiINVDTYDSC;
  }
  return MDSplusSUCCESS;
}

int Tdi3Neqv(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    Operate(unsigned char, ^)
    case DTYPE_W:case DTYPE_WU:Operate(unsigned short, ^)
    case DTYPE_L:case DTYPE_LU:Operate(unsigned int, ^)
    case DTYPE_Q:OperateSpecial(quadword, ^)
    case DTYPE_QU:OperateSpecial(quadword, ^)
    case DTYPE_O:OperateSpecial(octaword, ^)
    case DTYPE_OU:OperateSpecial(octaword, ^)
    case DTYPE_F:case DTYPE_G:case DTYPE_FC:case DTYPE_GC:case DTYPE_D:case
	DTYPE_DC:return TdiINVDTYDSC;
  case DTYPE_H:
  case DTYPE_HC:
    return TdiNO_OPC;
  default:
    return TdiINVDTYDSC;
  }
  return MDSplusSUCCESS;
}

int Tdi3Eqv(struct descriptor *in1, struct descriptor *in2, struct descriptor *out)
{
  SetupArgs switch (in1->dtype) {
  case DTYPE_B:
  case DTYPE_BU:
    Operate(unsigned char, ^~)
    case DTYPE_W:case DTYPE_WU:Operate(unsigned short, ^~)
    case DTYPE_L:case DTYPE_LU:Operate(unsigned int, ^~)
    case DTYPE_Q:OperateSpecial(quadword, ^~)
    case DTYPE_QU:OperateSpecial(quadword, ^~)
    case DTYPE_O:OperateSpecial(octaword, ^~)
    case DTYPE_OU:OperateSpecial(octaword, ^~)
    case DTYPE_F:case DTYPE_G:case DTYPE_FC:case DTYPE_GC:case DTYPE_D:case
	DTYPE_DC:return TdiINVDTYDSC;
  case DTYPE_H:
  case DTYPE_HC:
    return TdiNO_OPC;
  default:
    return TdiINVDTYDSC;
  }
  return MDSplusSUCCESS;
}

/*  CMS REPLACEMENT HISTORY, Element Tdi3And.C */
/*  *13   26-AUG-1996 15:06:46 TWF "remove ints.h" */
/*  *12   26-JUL-1996 12:23:14 TWF "Special handling for alpha and vms" */
/*  *11    8-JUL-1996 15:10:03 TWF "Fix operatespecial" */
/*  *10    8-JUL-1996 14:25:30 TWF "Fix octaword operations" */
/*  *9     8-JUL-1996 13:00:49 TWF "Fix octaword bugs" */
/*  *8    19-JUN-1996 09:46:39 TWF "Port to Unix/Windows" */
/*  *7    17-OCT-1995 16:16:54 TWF "use <builtins.h> form" */
/*  *6    19-OCT-1994 12:26:08 TWF "Use TDI$MESSAGES" */
/*  *5    19-OCT-1994 10:32:31 TWF "No longer support VAXC" */
/*  *4    15-NOV-1993 10:09:35 TWF "Add memory block" */
/*  *3    15-NOV-1993 09:42:13 TWF "Add memory block" */
/*  *2     9-NOV-1993 17:10:37 MRL "" */
/*  *1     9-NOV-1993 17:02:33 MRL "Tdi3And.C" */
/*  CMS REPLACEMENT HISTORY, Element Tdi3And.C */
