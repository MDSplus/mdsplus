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
/* inftrees.h -- header to use inftrees.c
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* Huffman code lookup table entry--this entry is four bytes for machines
   that have 16-bit pointers (e.g. PC's in the small or medium model). */

typedef struct inflate_huft_s FAR inflate_huft;

struct inflate_huft_s {
  union {
    struct {
      Byte Exop;		/* number of extra bits or operation */
      Byte Bits;		/* number of bits in this code or subcode */
    } what;
    uInt pad;			/* pad structure to a power of 2 (4 bytes for */
  } word;			/*  16-bit, 8 bytes for 32-bit int's) */
  uInt base;			/* literal, length base, distance base,
				   or table offset */
};

/* Maximum size of dynamic tree.  The maximum found in a long but non-
   exhaustive search was 1004 huft structures (850 for length/literals
   and 154 for distances, the latter actually the result of an
   exhaustive search).  The actual maximum is not known, but the
   value below is more than safe. */
#define MANY 1440

extern int inflate_trees_bits OF((uIntf *,	/* 19 code lengths */
				  uIntf *,	/* bits tree desired/actual depth */
				  inflate_huft * FAR *,	/* bits tree result */
				  inflate_huft *,	/* space for trees */
				  z_streamp));	/* for messages */

extern int inflate_trees_dynamic OF((uInt,	/* number of literal/length codes */
				     uInt,	/* number of distance codes */
				     uIntf *,	/* that many (total) code lengths */
				     uIntf *,	/* literal desired/actual bit depth */
				     uIntf *,	/* distance desired/actual bit depth */
				     inflate_huft * FAR *,	/* literal/length tree result */
				     inflate_huft * FAR *,	/* distance tree result */
				     inflate_huft *,	/* space for trees */
				     z_streamp));	/* for messages */

extern int inflate_trees_fixed OF((uIntf *,	/* literal desired/actual bit depth */
				   uIntf *,	/* distance desired/actual bit depth */
				   inflate_huft * FAR *,	/* literal/length tree result */
				   inflate_huft * FAR *,	/* distance tree result */
				   z_streamp));	/* for memory allocation */
