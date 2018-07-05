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
/*	MDS$COMPRESS.C
	Compress waveform or other data using specified or delta method.
		status = MDS$COMPRESS(%descr(cimage), %descr(centry), %descr(input), %ref(output_xd))
	Where:
		cimage	is an input text descriptor of the logical name of file with default SYS$SHARE:.EXE.
		centry	is an input text descriptor of the routine name in the image.
		input	is an input array of acceptable type.
			WARNING: An input XD is assumed compact and self-contained and the XD is freed.
			Also assumed compressible. This is fine for TREE$COPY_TO_RECORD, the primary user.
			Otherwise, you will have to use MDS$COPY_DXXD to gather and compact the input XD.
		output_xd is an extended dynamic descriptor to be filled.

	The input is converted into a CLASS_CA descriptor and the referenced data
	is compressed by the method of "centry" in "cimage" and placed in a simple array.
	Memory is allocated for the maximum sized result. It will be freed after it is copied.
	The output_xd is filled with a CLASS_CA descriptor:
	CA-dtype[0] ==> R-FUNCTION-DECOMPRESS	==> S-T ==> ximage text or NULL
						==> S-T ==> xentry text or NULL
						==> CA-dtype[1] ==> NULL
						==> A-B ==> compressed

	If result is larger than original, then we keep:
	A-dtype ==> uncompressed

The compression routine "centry":
		status = centry(nitems, %descr(input), %descr(packed), bit, [%descr(ximage), %descr(xentry)])
	Where:
		nitems	is the number of element of the input to compress.
		input	is an input array of acceptable type.
		packed	is an output temporary array to hold the compressed data.
		bit	is the number of bits in packed and sets the final length.
		ximage	is an output dynamic text descriptor of the logical name or file with default SYS$SHARE:.EXE.
		xentry	is an output dynamic text descriptor of the routine name in the image.
	If ximage and xentry are unchanged, the default expansion method is used.
	If packed is too small, ximage and xentry should not be set and status=LIB$_STRTRU returned.

The expansion routine "xentry":
		status = xentry(nitems, %descr(packed), %descr(output), [bit])
	Where:
		nitems	is the number of element of the input to compress.
		packed	is an output temporary array to hold the compressed data.
		output	is an output array to receive the expanded data.
		bit	is the output number of bits in packed. (untested)

	Ken Klare, LANL CTR-7	(c)1990
*/
#define OpcDECOMPRESS 120

#include <string.h>
#include <mdstypes.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <libroutines.h>
#include <strroutines.h>
#include <mdsshr_messages.h>
#include <STATICdef.h>
#include <mdsplus/mdsplus.h>
#include "mdsshrp.h"

#define _MOVC3(a,b,c) memcpy(c,b,(size_t)(a))
#define align(bytes,size) ((((bytes) + (size) - 1)/(size)) * (size))
typedef ARRAY_COEFF(char, 1) array_coef;
typedef RECORD(4) record_four;

STATIC_CONSTANT unsigned short opcode = OpcDECOMPRESS;
STATIC_CONSTANT record_four rec0 =
  { sizeof(opcode), DTYPE_FUNCTION, CLASS_R, (unsigned char *)&opcode, 4,
    __fill_value__ {0, 0, 0, 0} };
STATIC_CONSTANT DESCRIPTOR_A(dat0, 1, DTYPE_BU, 0, 0);
STATIC_CONSTANT struct descriptor_d EMPTY_D = { 0, DTYPE_T, CLASS_D, 0 };

STATIC_CONSTANT EMPTYXD(EMPTY_XD);
/*--------------------------------------------------------------------------
	The inner routine scans some classes and tries to compress arrays.
	If successful returns 1, if unsuccessful returns NORMAL.
*/
STATIC_ROUTINE int compress(const struct descriptor *pcimage,
			    const struct descriptor *pcentry, int64_t delta, struct descriptor *pwork)
{
  int j, stat1, status = 1;
  unsigned int bit = 0;
  int nitems, (*symbol) ();
  char *pcmp, *plim;
  array_coef *pca0, *pca1;
  struct descriptor_a *pdat, *porig;
  record_four *prec;
  struct descriptor_d dximage, dxentry;
  struct descriptor *pd0, *pd1, **ppd;
  size_t asize,align_size;
  if (pwork)
    switch (pwork->class) {
    case CLASS_APD:
//      pd1 = (struct descriptor *) pwork->pointer;
      ppd = (struct descriptor **)pwork->pointer;
      j = (int)(((struct descriptor_a *)pwork)->arsize / pwork->length);
      while ((--j >= 0) && STATUS_OK)
//      if ((stat1 = compress(pcimage, pcentry, delta, pd1++)) != 1)
	if ((stat1 = compress(pcimage, pcentry, delta, *ppd++)) != 1)
	  status = stat1;
      break;
    case CLASS_CA:
      if (pwork->pointer)
	status = compress(pcimage, pcentry, delta, (struct descriptor *)pwork->pointer);
      break;
    case CLASS_R:
      ppd = &((struct descriptor_r *)pwork)->dscptrs[0];
      j = ((struct descriptor_r *)pwork)->ndesc;
      while ((--j >= 0) && (status & 1))
	if ((stat1 = compress(pcimage, pcentry, delta, *(ppd++))) != 1)
	  status = stat1;
      break;

    /*************************************************
    Some work to do. Try to build this:
    ca0 (same as CLASS_A except class) -> rec
    rec $RECORD(4) -> opcode, image, routine, ca1, dat
    ca1 (same as ca0) -> NULL
    dat_dsc -> dat
    dat (the compressed stuff)

    image_dsc -> image_text, if not NULL
    image_text
    routine_dsc -> routine_text, if not NULL
    routine_text

    opcode added by final COPY_DXXD.
    *************************************************/
    case CLASS_A:
      porig = (struct descriptor_a *)((char *)pwork + delta);
      asize = sizeof(struct descriptor_a) +
	  (porig->aflags.coeff ? sizeof(void *) + porig->dimct * sizeof(int) : 0) +
	  (porig->aflags.bounds ? (size_t)(porig->dimct * 2) * sizeof(int) : 0);
      align_size = (porig->dtype == DTYPE_T) ? 1 : porig->length;
      asize = align(asize, align_size);
  /**************************************************************
    Check if we have minimum requirements.
    Make two CLASS_CA descriptors with a0=offset.
    First points to function descriptor.
    Second is dummy for expansion function.
    ASSUME compressor fails gracefully and only changes *pdat data.
    **************************************************************/
      prec = (record_four *) align((size_t) ((char *)pwork + asize), sizeof(void *));
      pca1 = (array_coef *) ((char *)prec + sizeof(rec0));
      pdat = (struct descriptor_a *)align((size_t) ((char *)pca1 + asize), sizeof(void *));
      pcmp = (char *)pdat + sizeof(struct descriptor_a);
      plim = porig->pointer + porig->arsize - sizeof(opcode);
      if (pcmp >= plim)
	break;

    /************************
    Use data from saved copy.
    ************************/
      pwork->pointer += delta;

      *prec = rec0;
      *pdat = *(struct descriptor_a *)&dat0;
      pdat->pointer = pcmp;
      pdat->arsize = (unsigned int)(plim - pcmp);

      nitems = (int)porig->arsize / (int)porig->length;
      if (pcentry) {
	dximage = EMPTY_D;
	dxentry = EMPTY_D;
	status = LibFindImageSymbol(pcimage, pcentry, &symbol);
	if (status & 1)
	  status = (*symbol) (&nitems, pwork, pdat, &bit, &dximage, &dxentry);
	pdat->arsize = (bit + 7) / 8;
	pd0 = (struct descriptor *)(pdat->pointer + pdat->arsize);
	if (dximage.pointer) {
	  pd1 = &pd0[1] + dximage.length;
	  if ((char *)pd1 < (char *)plim) {
	    prec->dscptrs[0] = pd0;
	    *pd0 = *(struct descriptor *)&dximage;
	    _MOVC3(dximage.length, dximage.pointer, pd0->pointer = (char *)&pd0[1]);
	  }
	  pd0 = pd1;
	  StrFree1Dx(&dximage);
	}
	if (dxentry.pointer) {
	  pd1 = &pd0[1] + dxentry.length;
	  if ((char *)pd1 < (char *)plim) {
	    prec->dscptrs[1] = pd0;
	    *pd0 = *(struct descriptor *)&dxentry;
	    _MOVC3(dxentry.length, dxentry.pointer, pd0->pointer = (char *)&pd0[1]);
	  }
	  pd0 = pd1;
	  StrFree1Dx(&dxentry);
	}
	if ((status & 1) && (status != LibSTRTRU) && ((char *)pd0 < (char *)plim))
	  goto good;
      /**************************************************
      If it doesn't fit then must restore old data field.
      **************************************************/
	bit = 0;
	prec->dscptrs[0] = 0;
	prec->dscptrs[1] = 0;
	_MOVC3(pdat->arsize = (unsigned int)(plim - pcmp), pcmp + delta, pcmp);
      }

    /********************
    Standard compression.
    ********************/
      status = MdsCmprs(&nitems, (struct descriptor_a *)pwork, pdat, (int*)&bit);
      pdat->arsize = (bit + 7) / 8;
      if ((status & 1) && (status != LibSTRTRU))
	goto good;
    /*************************************
    Did not do a good job, so restore all.
    *************************************/
      status = 1;
      _MOVC3(asize + porig->arsize, porig, pwork);
      break;
 good:
      pca0 = (array_coef *) pwork;
      pca0->class = CLASS_CA;
      if (pca0->aflags.coeff)
	pca0->a0 = (char *)(pca0->a0 - porig->pointer);
      _MOVC3((short)asize, (char *)pca0, (char *)pca1);
      pca0->pointer = (char *)prec;
      pca1->pointer = 0;

      prec->dscptrs[2] = (struct descriptor *)pca1;
      prec->dscptrs[3] = (struct descriptor *)pdat;
      break;
    default:
      break;
    }
  return status;
}

/*--------------------------------------------------------------------------
	The outside routine.
*/
EXPORT int MdsCompress(const struct descriptor *cimage_ptr,
		const struct descriptor *centry_ptr,
		const struct descriptor *in_ptr, struct descriptor_xd *out_ptr)
{
  int status = 1;
  struct descriptor_xd work;
  STATIC_CONSTANT unsigned char dsc_dtype = DTYPE_DSC;
  if (in_ptr == 0)
    return MdsFree1Dx(out_ptr, NULL);
  switch (in_ptr->class) {
  case CLASS_XD:
    work = *(struct descriptor_xd *)in_ptr;
    *(struct descriptor_xd *)in_ptr = EMPTY_XD;
    break;
  case CLASS_R:
    if (((struct descriptor_r *)in_ptr)->ndesc == 0)
      return MdsCopyDxXd(in_ptr, out_ptr);
    MDS_ATTR_FALLTHROUGH
  case CLASS_A:
  case CLASS_APD:
  case CLASS_CA:
    work = EMPTY_XD;
    status = MdsCopyDxXd(in_ptr, &work);
    if (status == MdsCOMPRESSIBLE)
      break;
    MdsFree1Dx(out_ptr, NULL);
    *out_ptr = work;
    return status;
  default:
    return MdsCopyDxXd(in_ptr, out_ptr);
  }
/********************************
Copy original to output and work.
Compact/copy from work.
********************************/
#ifdef _RECURSIVE_COMPRESS
  while (status == MdsCOMPRESSIBLE)
#else
  if (status & 1)
#endif
  {
    status = MdsGet1Dx(&work.l_length, &dsc_dtype, out_ptr, NULL);
    if (status & 1) {
#ifdef _RECURSIVE_COMPRESS
      int orig_len = work.l_length;
#endif
      _MOVC3(work.l_length, work.pointer, out_ptr->pointer);
      status =
	  compress(cimage_ptr, centry_ptr, (char *)out_ptr->pointer - (char *)work.pointer,
		   work.pointer);
      if (status & 1)
	status = MdsCopyDxXd(work.pointer, out_ptr);
      MdsFree1Dx(&work, NULL);
#ifdef _RECURSIVE_COMPRESS
      if ((status == MdsCOMPRESSIBLE) && (orig_len / 2 > out_ptr->l_length)) {
	work = *out_ptr;
	out_ptr->pointer = 0;
	out_ptr->l_length = 0;
      } else
	status = 1;
#endif
    }
  }
  return status;
}

/*--------------------------------------------------------------------------
	Expansion of compressed data.

		status = MdsDecompress(%ref(class_ca or r_function), %ref(output_xd))
*/
EXPORT int MdsDecompress(const struct descriptor_r *rec_ptr, struct descriptor_xd *out_ptr)
{
  const struct descriptor_r *prec = rec_ptr;
  int status, (*symbol) ();
  if (prec == 0) {
    MdsFree1Dx(out_ptr, NULL);
    return 1;
  }
  if (rec_ptr->class == CLASS_XD && rec_ptr->dtype == DTYPE_DSC)
    return MdsDecompress((struct descriptor_r *)rec_ptr->pointer, out_ptr);
  if (prec->class == CLASS_CA && prec->pointer)
    prec = (struct descriptor_r *)prec->pointer;
  if (prec->class != CLASS_R
      || prec->dtype != DTYPE_FUNCTION
      || prec->pointer == 0 || *(unsigned short *)prec->pointer != OpcDECOMPRESS)
    status = MdsCopyDxXd((struct descriptor *)prec, out_ptr);
  else {
    struct descriptor_a *pa = (struct descriptor_a *)prec->dscptrs[2];
    int nitems = (int)pa->arsize / (int)pa->length;
    int bit = 0;
    if (prec->dscptrs[1]) {
      status = LibFindImageSymbol(prec->dscptrs[0], prec->dscptrs[1], &symbol);
      if (!(status & 1))
	return status;
    } else {
      symbol = MdsXpand;
      status = 1;
    }
    if (status & 1)
      status = MdsGet1DxA(pa, &pa->length, &pa->dtype, out_ptr);
    if (status & 1) {
      if (prec->dscptrs[3]->class == CLASS_CA) {
	EMPTYXD(tmp_xd);
	status = MdsDecompress((struct descriptor_r *)prec->dscptrs[3], &tmp_xd);
	if (status & 1)
	  status = (*symbol) (&nitems, tmp_xd.pointer, out_ptr->pointer, &bit);
	MdsFree1Dx(&tmp_xd, 0);
      } else
	status = (*symbol) (&nitems, prec->dscptrs[3], out_ptr->pointer, &bit);
    }
  }
  return status;
}
