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
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <libroutines.h>
#include <strroutines.h>
#include <librtl_messages.h>

#define _MOVC3(a,b,c) memcpy(c,b,a)
typedef ARRAY_COEFF(char, 1) array_coef;
typedef RECORD(4) record_four;


  static unsigned short opcode = OpcDECOMPRESS;
  static record_four rec0 = {sizeof(opcode), DTYPE_FUNCTION, CLASS_R, (unsigned char *) &opcode, 4, 0, 0, 0, 0};
  static    DESCRIPTOR_A(dat0, 1, DTYPE_BU, 0, 0);
  static struct descriptor EMPTY_D = {0, DTYPE_T, CLASS_D, 0};
  static    EMPTYXD(EMPTY_XD);
/*--------------------------------------------------------------------------
	The inner routine scans some classes and tries to compress arrays.
	If successful returns 1, if unsuccessful returns NORMAL.
*/
  int       compress(
		               struct descriptor *pcimage,
		               struct descriptor *pcentry,
		               int delta,
		               struct descriptor *pwork)
{
  int       j,
              stat1,
              status = 1;
  int       bit = 0,
              asize,
              nitems,
              (*symbol) ();
  char     *pcmp,
             *plim;
  array_coef *pca0,
             *pca1;
  struct descriptor_a *pdat,
             *porig;
  record_four *prec;
  struct descriptor dximage,
              dxentry,
             *pd0,
             *pd1,
            **ppd;
  if (pwork)
    switch (pwork->class)
    {
     case CLASS_APD:
      pd1 = (struct descriptor *) pwork->pointer;
      j = (long) ((struct descriptor_a *) pwork)->arsize / (long) pwork->length;
      for (; --j >= 0 && status & 1;)
	if ((stat1 = compress(pcimage, pcentry, delta, pd1++)) != 1)
	  status = stat1;
      break;
     case CLASS_CA:
      if (pwork->pointer)
	status = compress(pcimage, pcentry, delta, (struct descriptor *) pwork->pointer);
      break;
     case CLASS_R:
      ppd = &((struct descriptor_r *) pwork)->dscptrs[0];
      j = ((struct descriptor_r *) pwork)->ndesc;
      for (; --j >= 0 && status & 1;)
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
      porig = (struct descriptor_a *) ((char *) pwork + delta);
      asize = sizeof(struct descriptor_a) + 
	      (porig->aflags.coeff ? sizeof(void *) + porig->dimct * sizeof(int) : 0) +
	      (porig->aflags.bounds ? porig->dimct * 2 * sizeof(int) : 0);
      asize = ((asize + asize - 1)/sizeof(void *)) * sizeof(void *);
    /**************************************************************
    Check if we have minimum requirements.
    Make two CLASS_CA descriptors with a0=offset.
    First points to function descriptor.
    Second is dummy for expansion function.
    ASSUME compressor fails gracefully and only changes *pdat data.
    **************************************************************/
      prec = (record_four *) ((char *) pwork + asize);
      pca1 = (array_coef *) ((char *) prec + sizeof(rec0));
      pdat = (struct descriptor_a *) ((char *) pca1 + asize);
      pcmp = (char *) pdat + sizeof(struct descriptor_a);
      plim = porig->pointer + porig->arsize - sizeof(opcode);
      if (pcmp >= plim)
	break;

    /************************
    Use data from saved copy.
    ************************/
      pwork->pointer += delta;

      *prec = rec0;
      *pdat = *(struct descriptor_a *) & dat0;
      pdat->pointer = pcmp;
      pdat->arsize = plim - pcmp;

      nitems = (int) porig->arsize / (int) porig->length;
      if (pcentry)
      {
	dximage = EMPTY_D;
	dxentry = EMPTY_D;
	status = LibFindImageSymbol(pcimage, pcentry, &symbol);
	if (status & 1)
	  status = (*symbol) (&nitems, pwork, pdat, &bit, &dximage, &dxentry);
	pdat->arsize = (bit + 7) / 8;
	pd0 = (struct descriptor *) (pdat->pointer + pdat->arsize);
	if (dximage.pointer)
	{
	  pd1 = &pd0[1] + dximage.length;
	  if ((char *) pd1 < (char *) plim)
	  {
	    prec->dscptrs[0] = pd0;
	    *pd0 = dximage;
	    _MOVC3(dximage.length, dximage.pointer, pd0->pointer = (char *) &pd0[1]);
	  }
	  pd0 = pd1;
	  StrFree1Dx(&dximage);
	}
	if (dxentry.pointer)
	{
	  pd1 = &pd0[1] + dxentry.length;
	  if ((char *) pd1 < (char *) plim)
	  {
	    prec->dscptrs[1] = pd0;
	    *pd0 = dxentry;
	    _MOVC3(dxentry.length, dxentry.pointer, pd0->pointer = (char *) &pd0[1]);
	  }
	  pd0 = pd1;
	  StrFree1Dx(&dxentry);
	}
	if (status & 1 && status != LibSTRTRU && (char *) pd0 < (char *) plim)
	  goto good;
      /**************************************************
      If it doesn't fit then must restore old data field.
      **************************************************/
	bit = 0;
	prec->dscptrs[0] = 0;
	prec->dscptrs[1] = 0;
	_MOVC3(pdat->arsize = plim - pcmp, pcmp + delta, pcmp);
      }

    /********************
    Standard compression.
    ********************/
      status = MdsCmprs(&nitems, (struct descriptor_a *) pwork, pdat, &bit);
      pdat->arsize = (bit + 7) / 8;
      if (status & 1 && status != LibSTRTRU)
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
	pca0->a0 -= (int) porig->pointer;
      _MOVC3((short) asize, (char *) pca0, (char *) pca1);
      pca0->pointer = (char *) prec;
      pca1->pointer = 0;

      prec->dscptrs[2] = (struct descriptor *) pca1;
      prec->dscptrs[3] = (struct descriptor *) pdat;
      break;
     default:
      break;
    }
  return status;
}
/*--------------------------------------------------------------------------
	The outside routine.
*/
int       MdsCompress(
		                 struct descriptor *cimage_ptr,
		                 struct descriptor *centry_ptr,
		                 struct descriptor *in_ptr,
		                 struct descriptor_xd *out_ptr)
{
  int       status = 1;
  struct descriptor_xd work;
  static unsigned char dsc_dtype = DTYPE_DSC;
  if (in_ptr == 0)
    return MdsFree1Dx(out_ptr,NULL);
  switch (in_ptr->class)
  {
   case CLASS_XD:
    work = *(struct descriptor_xd *) in_ptr;
    *(struct descriptor_xd *) in_ptr = EMPTY_XD;
    break;
   case CLASS_R:
    if (((struct descriptor_r *) in_ptr)->ndesc == 0)
      return MdsCopyDxXd(in_ptr, out_ptr);
   case CLASS_A:
   case CLASS_APD:
   case CLASS_CA:
    work = EMPTY_XD;
    status = MdsCopyDxXd(in_ptr, &work);
    if (status == MdsCOMPRESSIBLE)
      break;
    MdsFree1Dx(out_ptr,NULL);
    *out_ptr = work;
    return status;
   default:
    return MdsCopyDxXd(in_ptr, out_ptr);
  }
/********************************
Copy original to output and work.
Compact/copy from work.
********************************/
  if (status & 1)
    status = MdsGet1Dx(&work.l_length, &dsc_dtype, out_ptr, NULL);
  if (status & 1)
  {
    _MOVC3(work.l_length, work.pointer, out_ptr->pointer);
    status = compress(cimage_ptr, centry_ptr, (char *) out_ptr->pointer - (char *) work.pointer, work.pointer);
    if (status & 1)
      status = MdsCopyDxXd(work.pointer, out_ptr);
    MdsFree1Dx(&work,NULL);
  }
  return status;
}
/*--------------------------------------------------------------------------
	Expansion of compressed data.

		status = MdsDecompress(%ref(class_ca or r_function), %ref(output_xd))
*/
int       MdsDecompress(
			           struct descriptor_r *rec_ptr,
			           struct descriptor_xd *out_ptr)
{
  struct descriptor_r *prec = rec_ptr;
  int       status,
              (*symbol) ();
  if (prec == 0)
  {
    MdsFree1Dx(out_ptr, NULL);
    return 1;
  }
  if (prec->class == CLASS_CA && prec->pointer)
    prec = (struct descriptor_r *) prec->pointer;
  if (prec->class != CLASS_R
      || prec->dtype != DTYPE_FUNCTION
      || prec->pointer == 0
      || *(unsigned short *) prec->pointer != OpcDECOMPRESS)
    status = MdsCopyDxXd( (struct descriptor *)prec, out_ptr );
  else
  {
    struct descriptor_a *pa = (struct descriptor_a *) prec->dscptrs[2];
    int       nitems = (int) pa->arsize / (int) pa->length;
    int      bit = 0;
    if (prec->dscptrs[1])
    {
      status = LibFindImageSymbol(prec->dscptrs[0], prec->dscptrs[1], &symbol);
      if (!(status & 1))
	return status;
    }
    else
    {
      symbol = MdsXpand;
      status = 1;
    }
    if (status & 1)
      status = MdsGet1DxA(pa, &pa->length, &pa->dtype, out_ptr);
    if (status & 1)
      status = (*symbol) (&nitems, prec->dscptrs[3], out_ptr->pointer, &bit);
  }
  return status;
}
