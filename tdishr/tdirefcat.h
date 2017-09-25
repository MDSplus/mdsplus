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
/*	tdirefcat.h
	Data type information table reference.

	Ken Klare, LANL CTR-7	(c)1989,1990
*/
#define TdiCAT_COMPLEX	0x1000
#define TdiCAT_WIDE_EXP	0x0800
#define TdiCAT_FLOAT		0x0700
#define TdiCAT_B		0x0300
#define TdiCAT_BU		0x0100
#define TdiCAT_LENGTH	0x00ff
#define TdiCAT_SIGNED	(0x8000 | TdiCAT_B | TdiCAT_LENGTH)
#define TdiCAT_UNSIGNED	(0x8000 | TdiCAT_BU | TdiCAT_LENGTH)
#define TdiCAT_F		(0x8000 | TdiCAT_FLOAT | 3)
#define TdiCAT_D		(0x8000 | TdiCAT_FLOAT | 7)
#define TdiCAT_FC		(TdiCAT_COMPLEX | TdiCAT_F)
struct TdiCatStruct {
  unsigned short in_cat;
  unsigned char in_dtype;
  unsigned short out_cat;
  unsigned char out_dtype;
  unsigned short digits;
};
struct TdiCatStruct_table {
  char *name;			/*text for decompile */
  unsigned short cat;		/*category code */
  unsigned char length;		/*size in bytes */
  unsigned char digits;		/*size of text conversion */
  char *fname;			/*exponent name for floating decompile */
};

extern const unsigned char TdiCAT_MAX;
extern const struct TdiCatStruct_table TdiREF_CAT[];
