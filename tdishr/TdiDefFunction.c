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
/*      TDI$$DEF_FUNCTION.C
        Internal/intrinsic function table definition.
        Yacc tokens are put into TdiRefFunction table here.
        Precedence is associated with a token.

        Ken Klare, LANL CTR-7   (c)1989,1990
*/
#include <STATICdef.h>
#include <mds_stdarg.h>
#include <libroutines.h>
#include <tdishr_messages.h>
#include <mdsdescrip.h>
#define COM



extern int TdiIntrinsic();
#define MdsEND_ARG_64 (void *)-1
#ifndef va_count
#define  va_count(narg) if (first != MdsEND_ARG && first != MdsEND_ARG_64) \
{ struct descriptor *arg;\
  va_start(incrmtr, first); \
  arg = va_arg(incrmtr, struct descriptor *); \
  for (narg=1; (narg < 256) && \
               (arg != MdsEND_ARG) && \
               (arg != MdsEND_ARG_64); \
               narg++,arg=va_arg(incrmtr, struct descriptor *)); \
} else narg=0

#endif				/* va_count */

#define OPC(name,builtin,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) \
extern EXPORT int Tdi##name ( struct descriptor *first, ... ) \
{ \
  int narg; \
  va_list incrmtr; \
  va_count(narg); \
  if (narg && narg < 256) \
  { \
    int i; \
    struct descriptor *args[256]; \
    va_start(incrmtr, first); \
    args[0] = first; \
    for (i=1;i<narg;i++) \
      args[i] = va_arg(incrmtr, struct descriptor *); \
    return TdiIntrinsic(__LINE__ - 25,narg-1,args,args[narg-1]); \
  } \
  else \
    return TdiNO_OUTPTR; \
}
#include <opcbuiltins.h>
