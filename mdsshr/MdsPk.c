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
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element MdsPk.C */
/*  *9     2-DEC-1994 16:53:57 JAS "my head waw wedged" */
/*  *8    10-NOV-1994 11:34:21 JAS "make it portable to unix" */
/*  *7    24-OCT-1994 11:45:58 TWF "Take out ifdef vax" */
/*  *6     3-NOV-1993 13:39:41 KKLARE "return after ots$move to avoid clobber of *ppack" */
/*  *5     2-NOV-1993 09:38:00 TWF "Fix bug (if (off))" */
/*  *4    10-MAY-1993 14:37:05 TWF "Make it shareable" */
/*  *3     1-MAR-1993 08:42:11 TWF "Use standard indentation" */
/*  *2     3-FEB-1993 11:11:59 TWF "Make it portable" */
/*  *1     4-JAN-1993 14:52:41 TWF "CC MdsPk and MdsUnpk for compression" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element MdsPk.C */
/*	MdsPk.C
	Pack or unpack bits.
		MdsPk(&nbits,&nitems,&pack,&items,&bit)

	Ken Klare, LANL CTR-7	(c)1990
	Limitation: clears high bits of final word.
	Limitation: 32-bit two's-complement longs
	Optimization for Vax: negative left shift is right arithmetic shift.

	VAXstation 3100, 1000 interates, 1000 items	2	10	31	32 bits
	Pack, signed or unsigned bits and us/item:	3.63	4.20	5.75	4.32
	Unpack, unsigned bits and us/item:		3.30	4.04	5.94	4.14
	Unpack, signed bits and us/item:		4.23	4.94	6.73	4.14
	Pack or unpack 32-bit on byte boundaries 0.88, on long boundary 0.73
	For reference,
	Pack Macro timings				3.25	3.50		3.40
	Unpack Macro timings				3.01	3.07		3.02
*/
#include <mdsplus/mdsconfig.h>
#include <STATICdef.h>
STATIC_CONSTANT unsigned int masks[33] = { 0,
  0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff,
  0x1ff, 0x3ff, 0x7ff, 0xfff, 0x1fff, 0x3fff, 0x7fff, 0xffff,
  0x1ffff, 0x3ffff, 0x7ffff, 0xfffff, 0x1fffff, 0x3fffff, 0x7fffff, 0xffffff,
  0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff,
};

#include  <string.h>
#include <mdsdescrip.h>

STATIC_ROUTINE int SwapBytes(char *in_c)
{
  int out;
  char *out_c = (char *)&out;
  int i;
#ifdef WORDS_BIGENDIAN
  for (i = 0; i < 4; i++)
    out_c[i] = in_c[3 - i];
#else
  for (i = 0; i < 4; i++)
    out_c[i] = in_c[i];
#endif
  return out;
}

#define getppack SwapBytes((char *)ppack)

void MdsPk(signed char *nbits_ptr, int *nitems_ptr, int pack[], int items[], int *bit_ptr)
{
  int nbits = *nbits_ptr;
  int nitems = *nitems_ptr;
  int *ppack = &pack[*bit_ptr >> 5];
  int *pitems = &items[0];
  int size = nbits >= 0 ? nbits : -nbits;
  int off = *bit_ptr & 31;
  unsigned int mask;
  int test;
#ifdef WORDS_BIGENDIAN
  int i, j;
  signed char *pin;
  signed char *pout;
  unsigned int hold = 0;
  if (off) {
    for (i = 0; i < 4; i++)
      ((char *)&hold)[i] = ((char *)ppack)[3 - i];
    hold = hold & masks[off];
  }
#else
  unsigned int hold = off ? *(unsigned int *)ppack & masks[off] : 0;
#endif
  if (size == 0 || nitems <= 0)
    return;
  *bit_ptr += size * nitems;
  if (size == 32) {
    if ((off & 7) == 0) {
#ifdef WORDS_BIGENDIAN
      for (i = 0, pout = ((signed char *)ppack) + (off >> 3), pin = (signed char *)pitems;
	   i < nitems; i++, pout += 4, pin += 4)
	for (j = 0; j < 4; j++)
	  pout[j] = pin[3 - j];
#else
      memcpy(((char *)ppack) + (off >> 3), pitems, sizeof(int) * (size_t)nitems);
#endif
      return;
    } else
      for (; --nitems >= 0;) {
	hold |= *(unsigned int *)pitems << off;
#ifdef __APPLE__
	*ppack++ = SwapBytes((char *)&hold);
#else
#ifdef WORDS_BIGENDIAN
	for (i = 0; i < 4; i++)
	  ((char *)ppack)[i] = ((char *)&hold)[3 - i];
	ppack++;
#else
	*ppack++ = (int)hold;
#endif
#endif
	hold = *(unsigned int *)pitems++ >> (32 - off);
      }
  } else {
    mask = masks[size];
    test = 32 - size;
    for (; --nitems >= 0; ++pitems) {
      hold |= (mask & *(unsigned int *)pitems) << off;
      if (off >= test) {
#ifdef __APPLE__
	*ppack++ = SwapBytes((char *)&hold);
#else
#ifdef WORDS_BIGENDIAN
	for (i = 0; i < 4; i++)
	  ((char *)ppack)[i] = ((char *)&hold)[3 - i];
	ppack++;
#else
	*ppack++ = (int)hold;
#endif
#endif
	hold = (mask & *(unsigned int *)pitems) >> (32 - off);
	off -= test;
      } else
	off += size;
    }
  }
  if (off)
#ifdef WORDS_BIGENDIAN
    for (i = 0; i < 4; i++)
      ((char *)ppack)[i] = ((char *)&hold)[3 - i];
#else
    *ppack = (int)hold;
#endif
  return;
}

/*-------------------------------------------------------------*/
void MdsUnpk(signed char *nbits_ptr, int *nitems_ptr, int pack[], int items[], int *bit_ptr)
{
  int nbits = *nbits_ptr;
  int nitems = *nitems_ptr;
  int *ppack = &pack[*bit_ptr >> 5];
  int *pitems = &items[0];
  int size = nbits >= 0 ? nbits : -nbits;
  int off = *bit_ptr & 31;
  unsigned int hold, full, max, mask = masks[size];
  int test = 32 - size;
  *bit_ptr += size * nitems;
/*32-bit data*/

  if (test == 0) {
    if ((off & 7) == 0) {
      int i;
      ppack = (int *)(((char *)ppack) + (off >> 3));
      for (i = 0; i < nitems; i++, ppack++)
	pitems[i] = getppack;
    } else
      for (; --nitems >= 0;) {
	hold = ((unsigned int)getppack) >> off;
	ppack++;
	hold |= ((unsigned int)getppack) << (32 - off);
	*pitems++ = (int)hold;
      }
  }
/*sign extended*/
  else if (nbits < 0) {
    full = mask + 1;
    max = mask >> 1;
    for (; --nitems >= 0;) {
      if (off >= test) {
	hold = ((unsigned int)getppack) >> off;
	ppack++;
	hold |= (((unsigned int)getppack) << (32 - off)) & mask;
	if (hold > max)
	  *pitems++ = (int)(hold - full);
	else
	  *pitems++ = (int)hold;
	off -= test;
      } else {
	hold = (((unsigned int)getppack) >> off) & mask;
	if (hold > max)
	  *pitems++ = (int)(hold - full);
	else
	  *pitems++ = (int)hold;
	off += size;
      }
    }
  }
/*zero extended*/
  else if (nbits > 0)
    for (; --nitems >= 0;) {
      if (off >= test) {
	hold = ((unsigned int)getppack) >> off;
	ppack++;
	hold |= (((unsigned int)getppack) << (32 - off)) & mask;
	*pitems++ = (int)hold;
	off -= test;
      } else {
	hold = (((unsigned int)getppack) >> off) & mask;
	*pitems++ = (int)hold;
	off += size;
      }
    }
/*zero fill*/
  else
    for (; --nitems >= 0;)
      *pitems++ = 0;
  return;
}
