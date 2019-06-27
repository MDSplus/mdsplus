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
/*      TdiCall.C
	Call a routine in an image.
	A CALL has pointers to
	0       descriptor of image logical name (default SYS$SHARE:.EXE)
	1       descriptor of routine name.
	2...    descriptors of (ndesc-2) arguments.
	You must check number of arguments.
	Limit is 255 arguments.
	        result = image->entry(in1, ...)

	Or the result type may be given.
	        result = image->entry:type(in1, ...)
	where type is F for floating, B for byte, etc.,
	or DSC for a descriptor of any kind supported.
	A pointer to XD and D descriptor will free that descriptor.
	The type is given by the value at the record pointer.

	Ken Klare, LANL P-4     (c)1990,1992
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mdsplus/mdsplus.h>
#include <mdsdescrip.h>
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include <libroutines.h>
#include <strroutines.h>
#include <tdishr_messages.h>
#include <mdsshr.h>
#include <STATICdef.h>

extern int TdiConcat();
extern int TdiData();
extern int TdiEvaluate();
extern int TdiFaultHandler();
extern int TdiFindImageSymbol();
extern int TdiGetLong();
extern int TdiPutIdent();

#if defined __GNUC__ && 800 <= __GNUC__ * 100 + __GNUC_MINOR__
    _Pragma ("GCC diagnostic ignored \"-Wcast-function-type\"")
#endif

STATIC_ROUTINE int TdiInterlude(dtype_t rtype, struct descriptor **newdsc,
				int (*routine) (), unsigned int *(*called) (),
				void **result, int *max)
{
  switch (rtype) {
  case DTYPE_T:
  case DTYPE_POINTER:
  case DTYPE_DSC:
    {
      void *(*called_p) () = (void *(*)())called;
      void **result_p = (void *)result;
      *max = sizeof(void *);
      *result_p = (*called_p) (newdsc, routine);
      break;
    }
  case DTYPE_D:
  case DTYPE_G:
  case DTYPE_FC:
  case DTYPE_FSC:
  case DTYPE_Q:
  case DTYPE_QU:
    { // 8 bytes
      int64_t (*called_q) () = (int64_t (*)())called;
      int64_t *result_q = (int64_t *)result;
      *max = sizeof(int64_t);
      *result_q = (*called_q) (newdsc, routine);
      break;
    }
  //case DTYPE_F:
  //case DTYPE_FS:
  default:
    { // 4 bytes
      int32_t (*called_int) () = (int32_t (*)())called;
      int32_t *result_int = (int32_t *)result;
      *max = sizeof(int32_t);
      *result_int = (*called_int) (newdsc, routine);
    }
  }
  return 1;
}

int TdiCall(dtype_t rtype, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_function *pfun;
  struct descriptor_xd image = EMPTY_XD, entry = EMPTY_XD, tmp[255];
  int j, max = 0, ntmp = 0, (*routine) ();
  struct descriptor *result[2] = { 0, 0 };
  unsigned short code;
  struct descriptor *newdsc[256];
  struct descriptor dx = { 0, 0, CLASS_S, 0 };
  unsigned char origin[255];
  dx.dtype = rtype;
  dx.pointer = (char *)result;
  memset(newdsc, 0, sizeof(newdsc));
  if (narg > 255 + 2)
    status = TdiNDIM_OVER;
  else
    status = TdiData(list[0], &image MDS_END_ARG);
  if STATUS_OK
    status = TdiData(list[1], &entry MDS_END_ARG);
  if STATUS_OK
    status = TdiFindImageSymbol(image.pointer, entry.pointer, &routine);
  if STATUS_NOT_OK
    printf("%s\n", LibFindImageSymbolErrString());
  MdsFree1Dx(&entry, NULL);
  MdsFree1Dx(&image, NULL);
  *(int *)&newdsc[0] = narg - 2;
  for (j = 2; j < narg && STATUS_OK; ++j) {
    for (pfun = (struct descriptor_function *)list[j]; pfun && pfun->dtype == DTYPE_DSC;)
      pfun = (struct descriptor_function *)pfun->pointer;
    if (pfun && pfun->dtype == DTYPE_FUNCTION) {
      code = *(unsigned short *)pfun->pointer;
      if (code == OPC_DESCR) {
	tmp[ntmp] = EMPTY_XD;
	status = TdiData(pfun->arguments[0], &tmp[ntmp] MDS_END_ARG);
	newdsc[j - 1] = (struct descriptor *)tmp[ntmp].pointer;
	origin[ntmp++] = (unsigned char)j;
      } else if (code == OPC_REF) {
	tmp[ntmp] = EMPTY_XD;
	status = TdiData(pfun->arguments[0], &tmp[ntmp] MDS_END_ARG);
	if (tmp[ntmp].pointer) {
	  if (tmp[ntmp].pointer->dtype == DTYPE_T) {
	    DESCRIPTOR(zero, "\0");
	    TdiConcat(&tmp[ntmp], &zero, &tmp[ntmp] MDS_END_ARG);
	  }
	  newdsc[j - 1] = (struct descriptor *)tmp[ntmp].pointer->pointer;
	}
	origin[ntmp++] = (unsigned char)j;
      } else if (code == OPC_VAL) {
	int ans;
	EMPTYXD(xd);
	status = TdiData(pfun->arguments[0], &xd MDS_END_ARG);
	if (STATUS_OK && xd.pointer && xd.pointer->dtype == DTYPE_POINTER) {
	  *(void **)&newdsc[j - 1] = *(void **)xd.pointer->pointer;
	} else {
	  status = TdiGetLong(pfun->arguments[0], &ans);
	  *(long *)&newdsc[j - 1] = ans;
	}
	MdsFree1Dx(&xd, 0);
      } else if (code == OPC_XD) {
	tmp[ntmp] = EMPTY_XD;
	status = TdiEvaluate(pfun->arguments[0], newdsc[j - 1] = (struct descriptor *)&tmp[ntmp]
			     MDS_END_ARG);
	origin[ntmp++] = (unsigned char)j;
      } else
	goto fort;
    }
		/********************************************
	        Default is DESCR for text and REF for others.
	        ********************************************/
    else {
 fort:tmp[ntmp] = EMPTY_XD;
      if (list[j])
	status = TdiData(list[j], &tmp[ntmp] MDS_END_ARG);
      newdsc[j - 1] = tmp[ntmp].pointer;
      if (newdsc[j - 1]) {
	if (newdsc[j - 1]->dtype != DTYPE_T)
	  newdsc[j - 1] = (struct descriptor *)newdsc[j - 1]->pointer;
	else {
	  DESCRIPTOR(zero_dsc, "\0");
	  TdiConcat(&tmp[ntmp], &zero_dsc, &tmp[ntmp] MDS_END_ARG);
	  newdsc[j - 1] = (struct descriptor *)tmp[ntmp].pointer->pointer;
	}
      }
      origin[ntmp++] = (unsigned char)j;
    }
  }
  if STATUS_OK
    status = TdiInterlude(rtype, newdsc, routine, (void *)LibCallg, (void **)result, &max);
  if (!out_ptr)
    goto skip;
  if STATUS_OK
    switch (rtype) {
    case DTYPE_DSC:
      dx.pointer = (char *)result[0];
      if (result[0])
	switch (result[0]->class) {
	case CLASS_XD:
	  MdsFree1Dx(out_ptr, NULL);
	  *out_ptr = *(struct descriptor_xd *)result[0];
	  *(struct descriptor_xd *)result[0] = EMPTY_XD;
	  goto skip;
	case CLASS_D:
	  MdsFree1Dx(out_ptr, NULL);
	  *(struct descriptor *)out_ptr = *result[0];
	  result[0]->length = 0;
	  result[0]->pointer = 0;
	  goto skip;
	default:
	  break;
	}
      break;
    case DTYPE_T:
    case DTYPE_PATH:
    case DTYPE_EVENT:
      if (!result[0]) goto skip;
      dx.length = (unsigned short)strlen(dx.pointer = (char *)result[0]);
      break;
    case DTYPE_NID:
      dx.length = sizeof(int);
      break;
    case DTYPE_POINTER:
      dx.length = sizeof(void *);
      dx.pointer = (char *)result;
      //      if (sizeof(void *) == 8)
      //  dx.dtype = DTYPE_QU;
      //else
      //  dx.dtype = DTYPE_LU;
      break;

    default: {
      if ((int)rtype < TdiCAT_MAX) {
	if ((dx.length = TdiREF_CAT[(int)rtype].length) > max)
	  status = TdiTOO_BIG;
      } else
	status = TdiINVDTYDSC;
    }
  }
  if STATUS_OK
    status = MdsCopyDxXd(&dx, out_ptr);
 skip:
  for (j = 0; j < ntmp; ++j) {
    for (pfun = (struct descriptor_function *)list[origin[j]]; pfun && pfun->dtype == DTYPE_DSC;)
      pfun = (struct descriptor_function *)pfun->pointer;
    if (pfun && pfun->dtype == DTYPE_FUNCTION) {
      code = *(unsigned short *)pfun->pointer;
      if (code == OPC_DESCR || code == OPC_REF || code == OPC_XD)
	pfun = (struct descriptor_function *)pfun->arguments[0];
      if (pfun && pfun->dtype == DTYPE_IDENT)
	TdiPutIdent(pfun, &tmp[j]);
    }
    if (tmp[j].pointer)
      MdsFree1Dx(&tmp[j], NULL);
  }
  return status;
}
