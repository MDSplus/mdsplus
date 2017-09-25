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
#pragma once
#include <mdstypes.h>
#include <mdsdescrip.h>

#ifdef COM
#undef COM
#endif
#define COM

#define OPC(name,builtin,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) extern int Tdi##name ( struct descriptor *first, ... );
#include <opcbuiltins.h>
#undef OPC
#define OPC(name,builtin,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) extern unsigned short Opc##name;
#include <opcbuiltins.h>
#undef OPC
#undef COM

extern int TdiGetLong(struct descriptor *indsc, int *out_int);
extern int TdiGetFloat(struct descriptor *index, float *out_float);
extern int CvtConvertFloat(void *invalue, uint32_t indtype, void *outvalue, uint32_t outdtype, uint32_t options);
