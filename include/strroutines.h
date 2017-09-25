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

#include <mdsdescrip.h>

extern int StrAppend(struct descriptor_d *out, struct descriptor *tail);
extern int StrCaseBlindCompare(struct descriptor *one, struct descriptor *two);
extern int StrCompare(struct descriptor *str1, struct descriptor *str2);
extern int StrConcat(struct descriptor *out, struct descriptor *first, ...);
extern int StrCopyDx(struct descriptor *out, struct descriptor *in);
extern int StrCopyR(struct descriptor *dest, unsigned short *len, char *source);
extern int StrElement(struct descriptor *dest, int *num, struct descriptor *delim, struct descriptor *src);
extern int StrFindFirstInSet(struct descriptor *source, struct descriptor *set);
extern int StrFindFirstNotInSet(struct descriptor *source, struct descriptor *set);
extern int StrFree1Dx(struct descriptor_d *inout);
extern int StrGet1Dx(unsigned short *len, struct descriptor_d *out);
extern int StrLenExtr(struct descriptor *dest, struct descriptor *source, int *start_in, int *len_in);
extern unsigned int StrMatchWild(struct descriptor *candidate, struct descriptor *pattern);
extern int StrPosition(struct descriptor *source, struct descriptor *substring, int *start);
extern int StrReplace(struct descriptor *dest, struct descriptor *src, int *start_idx, int *end_idx,
		      struct descriptor *rep);
extern int StrRight(struct descriptor *out, struct descriptor *in, unsigned short *start);
extern int StrTranslate(struct descriptor *dest, struct descriptor *src, struct descriptor *tran,
			struct descriptor *match);
extern int StrTrim(struct descriptor *out, struct descriptor *in, unsigned short *lenout);
extern int StrUpcase(struct descriptor *out, struct descriptor *in);

