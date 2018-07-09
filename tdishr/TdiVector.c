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
/*      Tdi1Vector.C
        Converts a list of scalars and arrays into a rank-1 vector or higher array.
        Signality is lost.
        Null vectors are ignored, except lamination is not done.
        If all dimensions match exactly, the result has one more dimension.
        [[1],,[3]] [1,2,3] and [1,[2,3]] are simple vectors.
        [[1,2,3],[4,5,6]] has lamination shape [3,2].

        Ken Klare, LANL P-4     (c)1989,1990,1991
*/

#include "STATICdef.h"
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include "tdinelements.h"
#include <libroutines.h>
#include <tdishr_messages.h>
#include <stdlib.h>
#include <mdsshr.h>



extern int TdiConvert();
extern int TdiCvtArgs();
extern int TdiGetArgs();
extern int TdiMasterData();
extern int Tdi2Vector();

int Tdi1Vector(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  array miss = { sizeof(char), DTYPE_MISSING, CLASS_A, (char *)0, 0, 0, {0, 1, 1, 0,
									 0}, 1, 0
  };
  array_coeff arr = { sizeof(char), DTYPE_BU, CLASS_A, (char *)0, 0, 0, {0, 1, 1, 1, 0},
		      MAXDIM, 0, 0, {0}
  };
  struct descriptor_xd (*psig)[], (*puni)[] = 0, (*pdat)[] = 0;
  struct TdiCatStruct (*pcats)[] = 0;
  int cmode = -1, j, n, (*pnelem)[] = 0, jd, mind = MAXDIM, maxd = 0, nmiss = 0;
  int virt =
      (sizeof(struct descriptor_xd) * 3 + sizeof(int *)) * narg +
      sizeof(struct TdiCatStruct) * (narg + 1);

  if (narg == 0)
    return MdsCopyDxXd((struct descriptor *)&miss, out_ptr);
	/************************************
        Dynamic number of descriptors.
        Memory for sig[narg], uni[narg],
        dat[narg], nelem[narg], cats[narg+1].
        ************************************/

  status = (psig = malloc(virt)) != NULL;
  if STATUS_OK {
    puni = (struct descriptor_xd(*)[])&(*psig)[narg];
    pdat = (struct descriptor_xd(*)[])&(*puni)[narg];
    pnelem = (int (*)[])&(*pdat)[narg];
    pcats = (struct TdiCatStruct(*)[])&(*pnelem)[narg];	/* narg+1 of these */
  }

	/******************************************
        Fetch signals and data and data's category.
        ******************************************/
  if STATUS_OK
    status = TdiGetArgs(opcode, narg, list, (*psig), (*puni), (*pdat), (*pcats));

	/*****************************************
        Save and accumulate lengths of all inputs.
        *****************************************/
  if STATUS_OK
    for (j = narg; --j >= 0;) {
      array_coeff *pnew = (array_coeff *) (*pdat)[j].pointer;
      if ((*pcats)[j].digits > (*pcats)[narg].digits)
	(*pcats)[narg].digits = (*pcats)[j].digits;
      if (pnew->dtype == DTYPE_MISSING) {
	n = 0;
	nmiss++;
      } else {
	N_ELEMENTS(pnew, n);
	if (pnew->class == CLASS_A)
	  jd = pnew->aflags.coeff ? pnew->dimct : 1;
	else
	  jd = 0;
	if (jd < mind)
	  mind = jd;
	if (jd > maxd)
	  maxd = jd;
      }
      arr.arsize += (*pnelem)[j] = n;
    }

	/********************************
        If all dimensions match then add one.
        If all match but highest, expand it.
        Otherwise, use a simple vector.
        Shape: [[3],[3]] is [3,2].
        Shape: [[3,1],[3,4]] is [3,5].
        Shape: [[3],[3,4]] is [3,5].
        ********************************/
 if (STATUS_OK) {
  if (mind > 0 && mind >= maxd - 1 && mind < MAXDIM && nmiss == 0) {
    n = 0;
    for (j = 0; j < narg; ++j) {
      array_coeff *pnew = (array_coeff *) (*pdat)[j].pointer;
      if (pnew->aflags.coeff) {
	jd = pnew->dimct;
	n += (jd == maxd) ? pnew->m[maxd - 1] : 1;
	if (j == 0)
	  for (; --jd >= 0;)
	    arr.m[jd] = pnew->m[jd];
	else
	  for (; --jd >= 0;)
	    if (arr.m[jd] != pnew->m[jd])
	      mind = jd;
	if (mind < maxd - 1)
	  goto simple;
      } else {
	n++;
	if (j == 0)
	  arr.m[0] = (*pnelem)[j];
	else if (((int)(arr.m[0])) != (*pnelem)[j])
	  goto simple;
      }
    }
    arr.dimct = (unsigned char)(mind + 1);
    arr.aflags.coeff = 1;
    if (mind < maxd)
      arr.m[maxd - 1] = n;
    else
      arr.m[maxd] = narg;
  } else {
 simple:arr.dimct = 1;
    arr.aflags.coeff = 0;
  }
 }
  if STATUS_OK
    status = Tdi2Vector(narg, (*puni), (*pdat), (*pcats), 0);

	/*********************
        Find type conversions.
        *********************/
  if STATUS_OK
    status = TdiCvtArgs(0, (*pdat), &(*pcats)[narg]);

	/***************************
        Get an array to hold it all.
        Size is 1 so arsize = nelem.
        ***************************/
  if STATUS_OK
    status =
	MdsGet1DxA((struct descriptor_a *)&arr, &(*pcats)[narg].digits,
		   &(*pcats)[narg].out_dtype, out_ptr);

	/*********************************
        Accumulate all arrays and scalars.
        Recycle arr as temporary pointer.
        Class and flags are the same.
        *********************************/
  if STATUS_OK {
    struct descriptor *pout = out_ptr->pointer;
    arr.length = pout->length;
    arr.dtype = pout->dtype;
    arr.pointer = pout->pointer;
    for (j = 0; j < narg && STATUS_OK; ++j) {
      arr.arsize = (*pnelem)[j] * arr.length;
      status = TdiConvert((*pdat)[j].pointer, &arr MDS_END_ARG);
      arr.pointer += arr.arsize;
    }
    if (arr.length == 0 && arr.dtype == DTYPE_T && arr.dimct == 1) {
      arr.aflags.coeff = 1;
      arr.m[0] = narg;
      status = MdsCopyDxXd((struct descriptor *)&arr, out_ptr);
    }
  }

	/*************************
        Remove signal, keep units.
        *************************/
  if STATUS_OK
    status = TdiMasterData(0, (*psig), (*puni), &cmode, out_ptr);

	/********************
        Free all temporaries.
        ********************/
  for (j = narg; --j >= 0;) {
    if ((*psig)[j].pointer)
      MdsFree1Dx(&(*psig)[j], NULL);
    if ((*puni)[j].pointer)
      MdsFree1Dx(&(*puni)[j], NULL);
    if ((*pdat)[j].pointer)
      MdsFree1Dx(&(*pdat)[j], NULL);
  }
  if (psig != NULL)
    free(psig);
  return status;
}
