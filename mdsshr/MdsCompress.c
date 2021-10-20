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
                status = MDS$COMPRESS(%descr(cimage), %descr(centry),
%descr(input), %ref(output_xd)) Where:
                cimage	is an input text descriptor of the logical name of file
with default SYS$SHARE:.EXE.
                centry	is an input text descriptor of the routine name in the
image. input	is an input array of acceptable type. WARNING: An input XD is
assumed compact and self-contained and the XD is freed. Also assumed
compressible. This is fine for TREE$COPY_TO_RECORD, the primary user. Otherwise,
you will have to use MDS$COPY_DXXD to gather and compact the input XD. output_xd
is an extended dynamic descriptor to be filled.

        The input is converted into a CLASS_CA descriptor and the referenced
data is compressed by the method of "centry" in "cimage" and placed in a simple
array. Memory is allocated for the maximum sized result. It will be freed after
it is copied. The output_xd is filled with a CLASS_CA descriptor: CA-dtype[0]
==> R-FUNCTION-DECOMPRESS	==> S-T ==> ximage text or NULL
                                                ==> S-T ==> xentry text or NULL
                                                ==> CA-dtype[1] ==> NULL
                                                ==> A-B ==> compressed

        If result is larger than original, then we keep:
        A-dtype ==> uncompressed

The compression routine "centry":
                status = centry(nitems, %descr(input), %descr(packed), bit,
[%descr(ximage), %descr(xentry)]) Where: nitems	is the number of element of the
input to compress. input	is an input array of acceptable type.
                packed	is an output temporary array to hold the compressed
data. bit	is the number of bits in packed and sets the final length.
                ximage	is an output dynamic text descriptor of the logical name
or file with default SYS$SHARE:.EXE. xentry	is an output dynamic text
descriptor of the routine name in the image. If ximage and xentry are unchanged,
the default expansion method is used. If packed is too small, ximage and xentry
should not be set and status=LIB$_STRTRU returned.

The expansion routine "xentry":
                status = xentry(nitems, %descr(packed), %descr(output), [bit])
        Where:
                nitems	is the number of element of the input to compress.
                packed	is an output temporary array to hold the compressed
data. output	is an output array to receive the expanded data. bit	is the
output number of bits in packed. (untested)

        Ken Klare, LANL CTR-7	(c)1990
*/

#include "mdsshrp.h"
#include <libroutines.h>
#include <mdsdescrip.h>
#include <mdsplus/mdsplus.h>
#include <mdsshr.h>
#include <mdsshr_messages.h>
#include <mdstypes.h>
#include <string.h>
#include <strroutines.h>
#include <tdishr.h>

#define align(bytes, size) ((((bytes) + (size)-1) / (size)) * (size))
typedef ARRAY_COEFF(char, 1) array_coef;
typedef RECORD(4) mds_decompress_t;
static opcode_t OpcDECOMPRESS = OPC_DECOMPRESS;
static mds_decompress_t rec0 = {sizeof(opcode_t),
                                DTYPE_FUNCTION,
                                CLASS_R,
                                (uint8_t *)&OpcDECOMPRESS,
                                4,
                                __fill_value__{0, 0, 0, 0}};
static DESCRIPTOR_A(dat0, 1, DTYPE_BU, 0, 0);
static mdsdsc_d_t EMPTY_D = {0, DTYPE_T, CLASS_D, 0};

static EMPTYXD(EMPTY_XD);
/*--------------------------------------------------------------------------
        The inner routine scans some classes and tries to compress arrays.
        If successful returns 1, if unsuccessful returns NORMAL.
*/
static int compress(const mdsdsc_t *const pcimage,
                    const mdsdsc_t *const pcentry, const int64_t delta,
                    mdsdsc_t *const pwork)
{
  int j, stat1, status = 1;
  unsigned int bit = 0;
  int nitems, (*symbol)();
  char *pcmp, *plim;
  array_coef *pca0, *pca1;
  mdsdsc_a_t *pdat, *porig;
  mds_decompress_t *prec;
  mdsdsc_d_t dximage, dxentry;
  mdsdsc_t *pd0, *pd1, **ppd;
  size_t asize, align_size;
  if (pwork)
    switch (pwork->class)
    {
    case CLASS_APD:
      //      pd1 = (mdsdsc_t *) pwork->pointer;
      ppd = (mdsdsc_t **)pwork->pointer;
      j = (int)(((mdsdsc_a_t *)pwork)->arsize / pwork->length);
      while ((--j >= 0) && STATUS_OK)
        //      if ((stat1 = compress(pcimage, pcentry, delta, pd1++)) != 1)
        if ((stat1 = compress(pcimage, pcentry, delta, *ppd++)) != 1)
          status = stat1;
      break;
    case CLASS_CA:
      if (pwork->pointer)
        status = compress(pcimage, pcentry, delta, (mdsdsc_t *)pwork->pointer);
      break;
    case CLASS_R:
      ppd = &((mdsdsc_r_t *)pwork)->dscptrs[0];
      j = ((mdsdsc_r_t *)pwork)->ndesc;
      while ((--j >= 0) && (STATUS_OK))
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
      porig = (mdsdsc_a_t *)((char *)pwork + delta);
      asize =
          sizeof(mdsdsc_a_t) +
          (porig->aflags.coeff ? sizeof(void *) + porig->dimct * sizeof(int)
                               : 0) +
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
      prec = (mds_decompress_t *)align((size_t)((char *)pwork + asize),
                                       sizeof(void *));
      pca1 = (array_coef *)((char *)prec + sizeof(rec0));
      pdat =
          (mdsdsc_a_t *)align((size_t)((char *)pca1 + asize), sizeof(void *));
      pcmp = (char *)pdat + sizeof(mdsdsc_a_t);
      plim = porig->pointer + porig->arsize - sizeof(opcode_t);
      if (pcmp >= plim)
        break;

      /************************
      Use data from saved copy.
      ************************/
      pwork->pointer += delta;

      *prec = rec0;
      *pdat = *(mdsdsc_a_t *)&dat0;
      pdat->pointer = pcmp;
      pdat->arsize = (unsigned int)(plim - pcmp);

      nitems = (int)porig->arsize / (int)porig->length;
      if (pcentry && pcentry->length && pcentry->pointer)
      {
        dximage = EMPTY_D;
        dxentry = EMPTY_D;
        status = LibFindImageSymbol(pcimage, pcentry, &symbol);
        if (STATUS_OK)
        {
          status = (*symbol)(&nitems, pwork, pdat, &bit, &dximage, &dxentry);
          pdat->arsize = (bit + 7) / 8;
          pd0 = (mdsdsc_t *)(pdat->pointer + pdat->arsize);
          if (dximage.pointer)
          {
            pd1 = &pd0[1] + dximage.length;
            if ((char *)pd1 < (char *)plim)
            {
              prec->dscptrs[0] = pd0;
              *pd0 = *(mdsdsc_t *)&dximage;
              pd0->pointer = (char *)&pd0[1];
              memcpy(pd0->pointer, dximage.pointer,
                     dximage.length);
            }
            pd0 = pd1;
            StrFree1Dx(&dximage);
          }
          if (dxentry.pointer)
          {
            pd1 = &pd0[1] + dxentry.length;
            if ((char *)pd1 < (char *)plim)
            {
              prec->dscptrs[1] = pd0;
              *pd0 = *(mdsdsc_t *)&dxentry;
              pd0->pointer = (char *)&pd0[1];
              memcpy(pd0->pointer, dxentry.pointer,
                     dxentry.length);
            }
            pd0 = pd1;
            StrFree1Dx(&dxentry);
          }
        }
        if ((STATUS_OK) && (status != LibSTRTRU) &&
            ((char *)pd0 < (char *)plim))
          goto good;
        /**************************************************
        If it doesn't fit then must restore old data field.
        **************************************************/
        bit = 0;
        prec->dscptrs[0] = 0;
        prec->dscptrs[1] = 0;
        pdat->arsize = (unsigned int)(plim - pcmp);
        memcpy(pcmp, pcmp + delta, pdat->arsize);
      }

      /********************
      Standard compression.
      ********************/
      status = MdsCmprs(&nitems, (mdsdsc_a_t *)pwork, pdat, (int *)&bit);
      pdat->arsize = (bit + 7) / 8;
      if ((STATUS_OK) && (status != LibSTRTRU))
        goto good;
      /*************************************
      Did not do a good job, so restore all.
      *************************************/
      status = 1;
      memcpy(pwork, porig, asize + porig->arsize);
      break;
    good:
      pca0 = (array_coef *)pwork;
      pca0->class = CLASS_CA;
      if (pca0->aflags.coeff)
        pca0->a0 = (char *)(pca0->a0 - porig->pointer);
      memcpy((char *)pca1, (char *)pca0, (short)asize);
      pca0->pointer = (char *)prec;
      pca1->pointer = 0;

      prec->dscptrs[2] = (mdsdsc_t *)pca1;
      prec->dscptrs[3] = (mdsdsc_t *)pdat;
      break;
    default:
      break;
    }
  return status;
}

/*--------------------------------------------------------------------------
        The outside routine.
*/
EXPORT int MdsCompress(const mdsdsc_t *const cimage_ptr,
                       const mdsdsc_t *const centry_ptr,
                       const mdsdsc_t *const in_ptr,
                       mdsdsc_xd_t *const out_ptr)
{
  int status = 1;
  mdsdsc_xd_t work;
  static dtype_t dsc_dtype = DTYPE_DSC;
  if (in_ptr == 0)
    return MdsFree1Dx(out_ptr, NULL);
  switch (in_ptr->class)
  {
  case CLASS_XD:
    work = *(mdsdsc_xd_t *)in_ptr;
    *(mdsdsc_xd_t *)in_ptr = EMPTY_XD;
    break;
  case CLASS_R:
    if (((mdsdsc_r_t *)in_ptr)->ndesc == 0)
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
  if (STATUS_OK)
#endif
  {
    status = MdsGet1Dx(&work.l_length, &dsc_dtype, out_ptr, NULL);
    if (STATUS_OK)
    {
#ifdef _RECURSIVE_COMPRESS
      int orig_len = work.l_length;
#endif
      memcpy(out_ptr->pointer, work.pointer, work.l_length);
      status = compress(cimage_ptr, centry_ptr,
                        (char *)out_ptr->pointer - (char *)work.pointer,
                        work.pointer);
      if (STATUS_OK)
        status = MdsCopyDxXd(work.pointer, out_ptr);
      MdsFree1Dx(&work, NULL);
#ifdef _RECURSIVE_COMPRESS
      if ((status == MdsCOMPRESSIBLE) && (orig_len / 2 > out_ptr->l_length))
      {
        work = *out_ptr;
        out_ptr->pointer = 0;
        out_ptr->l_length = 0;
      }
      else
        status = 1;
#endif
    }
  }
  return status;
}

/*--------------------------------------------------------------------------
        Expansion of compressed data.

                status = MdsDecompress(%ref(class_ca or r_function),
   %ref(output_xd))
*/
EXPORT int MdsDecompress(const mdsdsc_r_t *rec_ptr, mdsdsc_xd_t *out_ptr)
{
  const mdsdsc_r_t *prec = rec_ptr;
  int status, (*symbol)();
  if (prec == 0)
  {
    MdsFree1Dx(out_ptr, NULL);
    return 1;
  }
  if (rec_ptr->class == CLASS_XD && rec_ptr->dtype == DTYPE_DSC)
    return MdsDecompress((mdsdsc_r_t *)rec_ptr->pointer, out_ptr);
  if (prec->class == CLASS_CA && prec->pointer)
    prec = (mdsdsc_r_t *)prec->pointer;
  if (prec->class != CLASS_R || prec->dtype != DTYPE_FUNCTION ||
      prec->pointer == 0 || *(unsigned short *)prec->pointer != OpcDECOMPRESS)
    status = MdsCopyDxXd((mdsdsc_t *)prec, out_ptr);
  else
  {
    mdsdsc_a_t *pa = (mdsdsc_a_t *)prec->dscptrs[2];
    int nitems = (int)pa->arsize / (int)pa->length;
    int bit = 0;
    if (prec->dscptrs[1])
    {
      status = LibFindImageSymbol(prec->dscptrs[0], prec->dscptrs[1], &symbol);
      if (STATUS_NOT_OK)
        return status;
    }
    else
    {
      symbol = MdsXpand;
      status = 1;
    }
    if (STATUS_OK)
      status = MdsGet1DxA(pa, &pa->length, &pa->dtype, out_ptr);
    if (STATUS_OK)
    {
      if (prec->dscptrs[3]->class == CLASS_CA)
      {
        EMPTYXD(tmp_xd);
        status = MdsDecompress((mdsdsc_r_t *)prec->dscptrs[3], &tmp_xd);
        if (STATUS_OK)
          status = (*symbol)(&nitems, tmp_xd.pointer, out_ptr->pointer, &bit);
        MdsFree1Dx(&tmp_xd, 0);
      }
      else
        status = (*symbol)(&nitems, prec->dscptrs[3], out_ptr->pointer, &bit);
    }
  }
  return status;
}
