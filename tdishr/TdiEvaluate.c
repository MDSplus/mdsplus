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
/*      Tdi1Evaluate.C
        Resolve NID, PATH, and FUNCTION data types and return a dynamic block.
        The output descriptor must be class XD, it will be and XD-DSC.
        The major entry point for evaluation of expressions.
                status = TdiEvaluate(&in_dsc, &out_dsc)

        Thomas W. Fredian, MIT PFC      19-Sep-1988     copyrighted
        Ken Klare, LANL P-4     (c)1989,1990,1991

RULES OF THE GAME:
        If "in" is a descriptor and an XD or D, it is freed.
        To avoid an XD release use its data pointer.

        "out" must be an XD. It will be an XD-DSC, usually.
*/

#include <STATICdef.h>
#include "tdirefstandard.h"
#include "tdishrp.h"
#include <tdishr_messages.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <stdlib.h>
#include <string.h>



STATIC_CONSTANT struct descriptor missing = { 0, DTYPE_MISSING, CLASS_S, 0 };

extern int TdiGetIdent();
extern int TdiEvaluate();
extern int TdiIntrinsic();
extern int TdiCall();
extern int TdiImpose();
extern int Tdi1Vector();

int Tdi1Evaluate(int opcode __attribute__ ((unused)),
		 int narg __attribute__ ((unused)),
		 struct descriptor *list[], struct descriptor_xd *out_ptr)
// SsINTERNAL: requires MdsCopyDxXd
{
  INIT_STATUS;
  struct descriptor_function *pfun;
  int nid, *pnid;

  /*****************************
  Cannot evaluate a null pointer
  *****************************/
  if (list[0] == 0)
    return MdsCopyDxXd(&missing, out_ptr);

  switch (list[0]->class) {
  case CLASS_XD:
    /***************************************************
    If input is an class XD and dtype DSC and points to
    real stuff, we can just copy its descriptor.
    Release any lingering output unless it is our input.
    ***************************************************/
    switch (list[0]->dtype) {
    case DTYPE_DSC:
      if (list[0]->pointer == 0)
	return TdiNULL_PTR;
      switch (((struct descriptor *)(list[0]->pointer))->dtype) {
      case DTYPE_DSC:
      case DTYPE_IDENT:
      case DTYPE_NID:
      case DTYPE_PATH:
      case DTYPE_FUNCTION:
      case DTYPE_CALL:
	break;
      default:
	switch (((struct descriptor *)(list[0]->pointer))->class) {
	case CLASS_APD:
	  break;
	default:
	  if (out_ptr->l_length && (out_ptr->pointer != (struct descriptor *)list[0]->pointer))
	    MdsFree1Dx(out_ptr, NULL);
	  *out_ptr = *(struct descriptor_xd *)list[0];
	  *(struct descriptor_xd *)list[0] = EMPTY_XD;
	  return status;
	}
      }
      break;
    }
  /*****************************************************************************
  WARNING falls through if an XD but not DSC of usable data (no known examples).
  *****************************************************************************/
  case CLASS_S:
  case CLASS_D:
  case CLASS_XS:
    switch (list[0]->dtype) {
    case DTYPE_DSC:
      status = TdiEvaluate(list[0]->pointer, out_ptr MDS_END_ARG);
      break;
    case DTYPE_IDENT:
      status = TdiGetIdent(list[0], out_ptr);
      break;
    case DTYPE_NID:
      pnid = (int *)list[0]->pointer;
      status = TdiGetRecord(*pnid, out_ptr);
      if STATUS_OK
	status = TdiEvaluate(out_ptr, out_ptr MDS_END_ARG);
      break;
    case DTYPE_PATH:
      {
	char *path = MdsDescrToCstring(list[0]);
	status = TreeFindNode(path, &nid);
	MdsFree(path);
	if STATUS_OK
	  status = TdiGetRecord(nid, out_ptr);
	if STATUS_OK
	  status = TdiEvaluate(out_ptr, out_ptr MDS_END_ARG);
      }
      break;
    default:
      if (list[0]->dtype < 160)
	status = SsINTERNAL;
      else
	status = TdiINVCLADTY;
      break;
    }
    break;
  case CLASS_R:
    switch (list[0]->dtype) {
    case DTYPE_FUNCTION:
      pfun = (struct descriptor_function *)list[0];
      status = TdiIntrinsic(*(unsigned short *)pfun->pointer,
          pfun->ndesc, pfun->arguments, out_ptr);
      break;
    case DTYPE_PARAM:
    case DTYPE_SIGNAL:
    case DTYPE_DIMENSION:
    case DTYPE_WINDOW:
    case DTYPE_SLOPE:
    case DTYPE_CONGLOM:
    case DTYPE_RANGE:
    case DTYPE_ACTION:
    case DTYPE_DISPATCH:
    case DTYPE_PROGRAM:
    case DTYPE_ROUTINE:
    case DTYPE_PROCEDURE:
    case DTYPE_METHOD:
    case DTYPE_DEPENDENCY:
    case DTYPE_CONDITION:
    case DTYPE_EVENT:
    case DTYPE_WITH_UNITS:
      status = SsINTERNAL;
      break;
    case DTYPE_CALL:
      pfun = (struct descriptor_function *)list[0];
      status =
	  TdiCall(pfun->length ? *(unsigned char *)pfun->pointer : DTYPE_L, pfun->ndesc,
		  pfun->arguments, out_ptr);
      break;
    default:
      /***********************
      NEED error if full list.
      ***********************/
      if (list[0]->dtype < DTYPE_PARAM)
	status = TdiINVCLADTY;
      else
	status = SsINTERNAL;
      break;
    }
    break;
  case CLASS_CA:
    /***************************************
    Must expand compressed data. 24-Apr-1991
    ***************************************/
    status = TdiEvaluate(list[0]->pointer, out_ptr MDS_END_ARG);
    if STATUS_OK
      status = TdiImpose(list[0], out_ptr);
    break;
  case CLASS_A:
    /*************************
    Arrays of most types.
    NEED array(dsc/nid/path)=?
    *************************/
    status = SsINTERNAL;
    break;
  case CLASS_APD:
    if (list[0]->dtype == DTYPE_DICTIONARY || list[0]->dtype == DTYPE_TUPLE
	|| list[0]->dtype == DTYPE_LIST || list[0]->dtype == DTYPE_OPAQUE) {
      status = SsINTERNAL;
    } else {
      status =
	  Tdi1Vector(0,
		     (int)((struct descriptor_a *)list[0])->arsize /
		     (int)list[0]->length, list[0]->pointer, out_ptr);
      if STATUS_OK
	status = TdiImpose(list[0], out_ptr);
    }
    break;
  default:
    status = TdiINVCLADSC;
    break;
  }
  /****************************
  VMS type or MDS special type.
  Make a descriptor, class XD.
  ****************************/
  if (status == SsINTERNAL)
    status = MdsCopyDxXd(list[0], out_ptr);
  if (list[0]->pointer)
    MdsFree1Dx((struct descriptor_xd *)list[0], NULL);
  return status;
}
