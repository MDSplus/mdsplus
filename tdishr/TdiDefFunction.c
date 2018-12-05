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

#define OPC(name,builtin,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) \
extern EXPORT int Tdi##name ( struct descriptor *first, ... ){\
  if (first==MdsEND_ARG) return TdiNO_OUTPTR;\
  int nargs;\
  void* arglist[256];\
  arglist[0] = (void*)first;\
  VA_LIST_MDS_END_ARG(arglist,nargs,1,0,first);\
  if (nargs<256)\
    return TdiIntrinsic(__LINE__ - 25,nargs-1,arglist,arglist[nargs-1]);\
  return TdiNO_OUTPTR;\
}
#include <opcbuiltins.h>
