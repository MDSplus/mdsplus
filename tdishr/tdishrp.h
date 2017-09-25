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
#ifndef TDISHRP_H
#define TDISHRP_H

#include "tdirefcat.h"

int CvtConvertFloat(void const *input_value, const int input_type, void *output_value,
		    const int output_type);
int TdiAddOctaword(unsigned const int *a, unsigned const int *b, unsigned int *ans);
int TdiAddQuadword(unsigned const int *a, unsigned const int *b, unsigned int *ans);
int TdiBinary(struct descriptor *in1_ptr, struct descriptor *in2_ptr, struct descriptor *out_ptr,
	      unsigned int *out_count);
int TdiConvert(struct descriptor *pdin, struct descriptor_a *pdout);
int TdiCvtArgs(int narg, struct descriptor_xd dat[1], struct TdiCatStruct cats[1]);
int TdiGetArgs(void *dbid, int opcode, int narg, struct descriptor *list[],
	       struct descriptor_xd sig[], struct descriptor_xd uni[], struct descriptor_xd dat[],
	       struct TdiCatStruct cats[]);
int TdiGetFloat(struct descriptor *in_ptr, float *val_ptr);
int TdiGetRecord(int nid, struct descriptor_xd *out);
int TdiGetShape(int narg, struct descriptor_xd dat[1], unsigned short length, unsigned char dtype,
		int *cmode_ptr, struct descriptor_xd *out_ptr);
int TdiHash(int len, char *pstring);
int _TdiIntrinsic(void *dbid, int opcode, int narg, struct descriptor *list[],
		  struct descriptor_xd *out_ptr);
int TdiMasterData(int nsig, struct descriptor_xd sig[1], struct descriptor_xd uni[1],
		  int *cmode_ptr, struct descriptor_xd *out_ptr);
int Tdi3Multiply(struct descriptor *in1, struct descriptor *in2, struct descriptor *out);
int TdiSubtractOctaword(unsigned int *a, unsigned int *b, unsigned int *ans);
int TdiSubtractQuadword(unsigned int *a, unsigned int *b, unsigned int *ans);
int TdiUnary(struct descriptor *in_ptr, struct descriptor *out_ptr, unsigned int *out_count);
void UseNativeFloat(struct TdiCatStruct *cat);
void TdiResetGetRecord();
int TdiExtPython(struct descriptor *modname_d,
		 int nargs, struct descriptor **args, struct descriptor_xd *out_ptr);
#endif
