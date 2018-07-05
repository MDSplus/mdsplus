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
        Implementation only for unit8. cast is taken care of in TdiSame

------------------------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include <mdsdescrip.h>
#include <tdishr_messages.h>
#include <STATICdef.h>

static inline int Operate(struct descriptor *in1, struct descriptor *in2, struct descriptor *out,char operator(const char, const char)) {
  if (in1->dtype!=2) {
    fprintf(stderr,"FATAL: TdiAnd.c function called with other than DTYPE_BU (%d,%d)\n",in1->dtype,in2->dtype);
    return TdiINVDTYDSC;
  }
  struct descriptor_a *ina1 = (struct descriptor_a *)in1;
  struct descriptor_a *ina2 = (struct descriptor_a *)in2;
  struct descriptor_a *outa = (struct descriptor_a *)out;
  int scalars = 0;
  unsigned int nout;
  switch (out->class) {
    case CLASS_S:
    case CLASS_D: nout = 1; break;
    case CLASS_A: nout = outa->arsize/outa->length; if (nout == 0) return 1; break;
    default:            return TdiINVCLADSC;
  }
  switch (in1->class) {
    case CLASS_S:
    case CLASS_D: scalars |= 1; break;
    case CLASS_A: if (ina1->arsize/ina1->length < nout) return TdiINV_SIZE; break;
    default:            return TdiINVCLADSC;
  }
  switch (in2->class) {
    case CLASS_S:
    case CLASS_D: if (scalars && (nout > 1)) return TdiINV_SIZE; scalars |= 2; break;
    case CLASS_A: if (ina2->arsize/ina2->length < nout) return TdiINV_SIZE; break;
    default:            return TdiINVCLADSC;
  }
  char *in1p = (char *)in1->pointer;
  char *in2p = (char *)in2->pointer;
  char *outp = (char *)out->pointer;
  switch (scalars) {
    case 0: while (nout--) *outp++ = operator(*in1p++,*in2p++); break;
    case 1: while (nout--) *outp++ = operator(*in1p  ,*in2p++); break;
    case 2: while (nout--) *outp++ = operator(*in1p++,*in2p  ); break;
    case 3:                *outp   = operator(*in1p  ,*in2p  ); break;
  }
  return MDSplusSUCCESS;
}

static inline char  and    (const char a, const char b) {return (char)(1 &  (a & b));}
static inline char nand    (const char a, const char b) {return (char)(1 & ~(a & b));}
static inline char  and_not(const char a, const char b) {return (char)(1 &  (a &~b));}
static inline char nand_not(const char a, const char b) {return (char)(1 & ~(a &~b));}
static inline char   or    (const char a, const char b) {return (char)(1 &  (a | b));}
static inline char  nor    (const char a, const char b) {return (char)(1 & ~(a | b));}
static inline char   or_not(const char a, const char b) {return (char)(1 &  (a |~b));}
static inline char  nor_not(const char a, const char b) {return (char)(1 & ~(a |~b));}
static inline char  eqv    (const char a, const char b) {return (char)(1 &  (a ^~b));}
static inline char neqv    (const char a, const char b) {return (char)(1 &  (a ^ b));}

int  Tdi3And   (struct descriptor *in1, struct descriptor *in2, struct descriptor *out) { return Operate(in1,in2,out, and    );}
int Tdi3Nand   (struct descriptor *in1, struct descriptor *in2, struct descriptor *out) { return Operate(in1,in2,out,nand    );}
int  Tdi3AndNot(struct descriptor *in1, struct descriptor *in2, struct descriptor *out) { return Operate(in1,in2,out, and_not);}
int Tdi3NandNot(struct descriptor *in1, struct descriptor *in2, struct descriptor *out) { return Operate(in1,in2,out,nand_not);}
int  Tdi3Or    (struct descriptor *in1, struct descriptor *in2, struct descriptor *out) { return Operate(in1,in2,out,  or    );}
int Tdi3Nor    (struct descriptor *in1, struct descriptor *in2, struct descriptor *out) { return Operate(in1,in2,out, nor    );}
int  Tdi3OrNot (struct descriptor *in1, struct descriptor *in2, struct descriptor *out) { return Operate(in1,in2,out,  or_not);}
int Tdi3NorNot (struct descriptor *in1, struct descriptor *in2, struct descriptor *out) { return Operate(in1,in2,out, nor_not);}
int  Tdi3Eqv   (struct descriptor *in1, struct descriptor *in2, struct descriptor *out) { return Operate(in1,in2,out, eqv    );}
int Tdi3Neqv   (struct descriptor *in1, struct descriptor *in2, struct descriptor *out) { return Operate(in1,in2,out,neqv    );}
