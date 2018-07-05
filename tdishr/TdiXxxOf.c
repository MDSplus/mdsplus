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
/*      Tdi1xxx_OF.C
        KIND and all xxx_OF routines in one module.

        Ken Klare, LANL P-4     (c)1989,1990,1991,1992
*/
#include "STATICdef.h"
#include <tdishr_messages.h>
#include "tdirefstandard.h"
#include <stdlib.h>
#include <mdsshr.h>



extern int TdiGetData();
extern int TdiGetLong();
extern int TdiPutLogical();
extern int TdiEvaluate();
extern int TdiDispatchOf();
extern int TdiBuildRange();
extern int TdiDimOf();
extern int TdiValueOf();
extern int TdiTaskOf();

/*--------------------------------------------------------------
        Return one of the class-R descriptor arguments.
                argument = ARG_OF(classR)
        NEED to remember to use AS_IS to prevent FUNCTION evaluation.
*/
int Tdi1ArgOf(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  unsigned int iarg = 0;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_CALL,
    DTYPE_CONDITION,
    DTYPE_DEPENDENCY,
    DTYPE_FUNCTION,
    DTYPE_METHOD,
    DTYPE_PROCEDURE,
    DTYPE_ROUTINE,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if (STATUS_OK && narg > 1)
    status = TdiGetLong(list[1], &iarg);
  if (STATUS_OK && tmp.pointer->class != CLASS_R)
    status = TdiINVCLADSC;
  if (STATUS_OK && iarg >= ((struct descriptor_r *)tmp.pointer)->ndesc)
    status = TdiBAD_INDEX;
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_CALL:
      if (iarg + 2 >= ((struct descriptor_r *)tmp.pointer)->ndesc)
	status = TdiBAD_INDEX;
      else
	status = MdsCopyDxXd(((struct descriptor_call *)tmp.pointer)->arguments[iarg], out_ptr);
      break;
    case DTYPE_CONDITION:
      status = MdsCopyDxXd(((struct descriptor_condition *)tmp.pointer)->condition, out_ptr);
      break;
    case DTYPE_DEPENDENCY:
      status = MdsCopyDxXd(((struct descriptor_dependency *)tmp.pointer)->arguments[iarg], out_ptr);
      break;
    case DTYPE_FUNCTION:
      status = MdsCopyDxXd(((struct descriptor_function *)tmp.pointer)->arguments[iarg], out_ptr);
      break;
    case DTYPE_METHOD:
      status = MdsCopyDxXd(((struct descriptor_method *)tmp.pointer)->arguments[iarg], out_ptr);
      break;
    case DTYPE_PROCEDURE:
      status = MdsCopyDxXd(((struct descriptor_procedure *)tmp.pointer)->arguments[iarg], out_ptr);
      break;
    case DTYPE_ROUTINE:
      status = MdsCopyDxXd(((struct descriptor_routine *)tmp.pointer)->arguments[iarg], out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return axis portion of a dimension.
                axis_field = AXIS_OF(dimension)
                same = AXIS_OF(slope) !deprecated!
                axis_field = AXIS_OF(DIM_OF(signal))
*/
int Tdi1AxisOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_DIMENSION,
    DTYPE_SLOPE,
    DTYPE_RANGE,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_DIMENSION:
      status = MdsCopyDxXd(((struct descriptor_dimension *)tmp.pointer)->axis, out_ptr);
      break;
    case DTYPE_SLOPE:
    case DTYPE_RANGE:
      MdsFree1Dx(out_ptr, NULL);
      *out_ptr = tmp;
      return status;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return start point information.
                begin_field = BEGIN_OF(range)
                begin_field = BEGIN_OF(slope, [n]) !deprecated!
                startidx_field = BEGIN_OF(window)
*/
int Tdi1BeginOf(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  unsigned int n = 0;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_RANGE,
    DTYPE_SLOPE,
    DTYPE_WINDOW,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_RANGE:
      if (narg > 1)
	status = TdiEXTRA_ARG;
      else
	status = MdsCopyDxXd(((struct descriptor_range *)tmp.pointer)->begin, out_ptr);
      break;
    case DTYPE_SLOPE:
      if (narg > 1)
	status = TdiGetLong(list[1], &n);
      if (STATUS_OK && 3 * n + 2 > ((struct descriptor_slope *)tmp.pointer)->ndesc)
	status = TdiBAD_INDEX;
      if STATUS_OK
	status = MdsCopyDxXd(((struct descriptor_slope *)tmp.pointer)->segment[n].begin, out_ptr);
      break;
    case DTYPE_WINDOW:
      if (narg > 1)
	status = TdiEXTRA_ARG;
      else
	status = MdsCopyDxXd(((struct descriptor_window *)tmp.pointer)->startidx, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return the class of its argument, but not of the XD.
                byte = CLASS_OF(any)
*/
int Tdi1ClassOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor *px = list[0];
  unsigned char class;

  while (px && px->dtype == DTYPE_DSC)
    px = (struct descriptor *)px->pointer;
  if (px)
    class = px->class;
  else
    class = DTYPE_MISSING;
  status = TdiPutLogical(class, out_ptr);
  return status;
}

/*--------------------------------------------------------------
        Evaluate and then return the class of an argument.
                byte = CLASS(any)
*/
int Tdi1Class(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;

  status = TdiEvaluate(list[0], out_ptr MDS_END_ARG);
  if STATUS_OK
    status = Tdi1ClassOf(opcode, narg, &out_ptr->pointer, out_ptr);
  return status;
}

/*--------------------------------------------------------------
        Return completion in dispatch information.
                completion = COMPLETION_OF(dispatch)
*/
int Tdi1CompletionOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;

  status = TdiDispatchOf(list[0], &tmp MDS_END_ARG);
  if STATUS_OK
    status = MdsCopyDxXd(((struct descriptor_dispatch *)tmp.pointer)->completion, out_ptr);
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return completion message in action information.
                completion_message = COMPLETION_MESSAGE_OF(action)
*/
int Tdi1CompletionMessageOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[],
			    struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_ACTION,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_ACTION:
      if (((struct descriptor_action *)tmp.pointer)->ndesc<4)
	status = MdsFree1Dx(out_ptr, NULL);
      else
	status = MdsCopyDxXd(((struct descriptor_action *)tmp.pointer)->completion_message, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return record information.
                condition = CONDITION_OF(condition)
*/
int Tdi1ConditionOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_CONDITION,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_CONDITION:
      status = MdsCopyDxXd(((struct descriptor_condition *)tmp.pointer)->condition, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return a dimension of a signal or array.
        Dimension number is 1 to MAXDIM, default is first.
                subscript_range = DIM_OF(array, [dim_num])
                dimension_field = DIM_OF(signal, [dim_num])
                same = DIM_OF(dimension)
*/
int Tdi1DimOf(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  array_bounds *pa;
  struct descriptor_xd tmp = EMPTY_XD;
  int dimct;
  int l, u;
  int index = 0;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_SIGNAL,
    DTYPE_DIMENSION,
    0
  };

	/**************************************
        For array of signals, give array range.
        **************************************/
  status = TdiGetData(omits, list[0], &tmp);
  if (STATUS_OK && narg > 1)
    status = TdiGetLong(list[1], &index);

  pa = (array_bounds *) tmp.pointer;
  if STATUS_OK
    switch (pa->class) {
    case CLASS_A:
      dimct = pa->aflags.coeff ? pa->dimct : 1;
      if (index >= dimct)
	status = TdiBAD_INDEX;
		/*******************************************
                After arsize is a0, multipliers, and bounds.
                NEED we reverse for non-FORTRAN?
                *******************************************/
      else if (pa->aflags.bounds) {
	l = pa->m[dimct + 2 * index];
	u = pa->m[dimct + 2 * index + 1];
      } else if (pa->aflags.coeff) {
	l = 0;
	u = pa->m[index] - 1;
      } else {
	l = 0;
	u = (int)pa->arsize / (int)pa->length - 1;
      }
      if STATUS_OK {
	struct descriptor ldsc = { sizeof(int), DTYPE_L, CLASS_S, 0 };
	struct descriptor udsc = { sizeof(int), DTYPE_L, CLASS_S, 0 };
	ldsc.pointer = (char *)&l;
	udsc.pointer = (char *)&u;
	status = TdiBuildRange(&ldsc, &udsc, out_ptr MDS_END_ARG);
      }
      break;
    case CLASS_R:
      switch (pa->dtype) {
      case DTYPE_DIMENSION:
	if (index > 0)
	  status = TdiBAD_INDEX;
	else
	  status = MdsCopyDxXd((struct descriptor *)&tmp, out_ptr);
	break;
      case DTYPE_SIGNAL:
	if (index > ((struct descriptor_signal *)pa)->ndesc - 3
	    || ((struct descriptor_signal *)pa)->dimensions[index] == 0) {
	  struct descriptor_s index_dsc = { sizeof(int), DTYPE_L, CLASS_S, 0 };
	  index_dsc.pointer = (char *)&index;
	  status = TdiGetData(&omits[1], &tmp, &tmp);
	  if STATUS_OK
	    status = TdiDimOf(&tmp, &index_dsc, out_ptr MDS_END_ARG);
	} else
	  status = MdsCopyDxXd(((struct descriptor_signal *)pa)->dimensions[index], out_ptr);
	break;
      default:
	status = TdiINVDTYDSC;
	break;
      }
      break;
    default:
      status = TdiINVCLADSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return dispatch information.
                dispatch = DISPATCH_OF(action)
                same = DISPATCH_OF(dispatch)
*/
int Tdi1DispatchOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_ACTION,
    DTYPE_DISPATCH,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_ACTION:
      status = TdiDispatchOf(((struct descriptor_action *)tmp.pointer)->dispatch, out_ptr MDS_END_ARG);
      break;
    case DTYPE_DISPATCH:
      MdsFree1Dx(out_ptr, NULL);
      *out_ptr = tmp;
      return status;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return one of the class-R descriptor pointers.
                descriptor = DSCPTR_OF(classR, [number])
                descriptor = DSCPTR_OF(classAPD, [number])
*/
int Tdi1DscptrOf(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_r *pr = (struct descriptor_r *)list[0];
  unsigned int iarg = 0;

  while (pr && pr->dtype == DTYPE_DSC)
    pr = (struct descriptor_r *)pr->pointer;
  if (pr) {
    if (narg > 1)
      status = TdiGetLong(list[1], &iarg);
    if STATUS_OK
      switch (pr->class) {
      case CLASS_R:
	if (iarg >= pr->ndesc)
	  status = TdiBAD_INDEX;
	else
	  status = MdsCopyDxXd(pr->dscptrs[iarg], out_ptr);
	break;
      case CLASS_APD:
	{
	  array_desc *pa = (array_desc *) pr;
	  if (iarg >= pa->arsize / pa->length)
	    status = TdiBAD_INDEX;
	  else
	    status = MdsCopyDxXd(pa->pointer[iarg], out_ptr);
	}
	break;
      default:
	status = TdiINVCLADSC;
	break;
      }
  }
  return status;
}

/*--------------------------------------------------------------
        Evaluate and then return a descriptor pointer of an argument.
                byte = DSCPTR(any)
*/
int Tdi1Dscptr(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD, *new[2];
  new[0] = &tmp;
  new[1] = narg > 1 ? (struct descriptor_xd *)list[1] : (struct descriptor_xd *)0;
  status = TdiEvaluate(list[0], &tmp MDS_END_ARG);
  if STATUS_OK
    status = Tdi1DscptrOf(opcode, narg, (struct descriptor **)new, out_ptr);
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return the data type of its argument, but not of the XD.
                byte = KIND_OF(any)
*/
int Tdi1KindOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor *px = list[0];
  unsigned char dtype;

  while (px && px->dtype == DTYPE_DSC)
    px = (struct descriptor *)px->pointer;
  if (px)
    dtype = px->dtype;
  else
    dtype = DTYPE_MISSING;
  status = TdiPutLogical(dtype, out_ptr);
  return status;
}

/*--------------------------------------------------------------
        Evaluate and then return the dtype of an argument.
                byte = KIND(any)
*/
int Tdi1Kind(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;

  status = TdiEvaluate(list[0], out_ptr MDS_END_ARG);
  if STATUS_OK
    status = Tdi1KindOf(opcode, narg, &out_ptr->pointer, out_ptr);
  return status;
}

/*--------------------------------------------------------------
        Return end point information.
                end_field = END_OF(&range)
                end_field = END_OF(&slope,[n]) !deprecated!
                endidx_field = END_OF(&window)
*/
int Tdi1EndOf(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  unsigned int n = 0;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_RANGE,
    DTYPE_SLOPE,
    DTYPE_WINDOW,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_RANGE:
      if (narg > 1)
	status = TdiEXTRA_ARG;
      else
	status = MdsCopyDxXd(((struct descriptor_range *)tmp.pointer)->ending, out_ptr);
      break;
    case DTYPE_SLOPE:
      if (narg > 1)
	status = TdiGetLong(list[1], &n);
      if (STATUS_OK && 3 * n + 3 > ((struct descriptor_slope *)tmp.pointer)->ndesc)
	status = TdiBAD_INDEX;
      if STATUS_OK
	status = MdsCopyDxXd(((struct descriptor_slope *)tmp.pointer)->segment[n].ending, out_ptr);
      break;
    case DTYPE_WINDOW:
      if (narg > 1)
	status = TdiEXTRA_ARG;
      else
	status = MdsCopyDxXd(((struct descriptor_window *)tmp.pointer)->endingidx, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return error bar associated with value.
                error = ERROR_OF(with_error)
*/
int Tdi1ErrorOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  STATIC_CONSTANT DESCRIPTOR(none, "");
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_WITH_ERROR,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_WITH_ERROR:
      status = MdsCopyDxXd(((struct descriptor_with_error *)tmp.pointer)->error, out_ptr);
      break;
    default:
      status = MdsCopyDxXd((struct descriptor *)&none, out_ptr);
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return errorlog in action information.
                errorlogs_field = ERRORLOGS_OF(action)
*/
int Tdi1ErrorlogsOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_ACTION,
    0
  };
  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_ACTION:
      if (((struct descriptor_action *)tmp.pointer)->ndesc<3)
	status = MdsFree1Dx(out_ptr, NULL);
      else
	status = MdsCopyDxXd(((struct descriptor_action *)tmp.pointer)->errorlogs, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return help portion of a parameter.
                help_field = HELP_OF(param)
*/
int Tdi1HelpOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_PARAM,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_PARAM:
      status = MdsCopyDxXd(((struct descriptor_param *)tmp.pointer)->help, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return dispatch identification information.
                ident_field = IDENT_OF(dispatch)
*/
int Tdi1IdentOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;

  status = TdiDispatchOf(list[0], &tmp MDS_END_ARG);
  if STATUS_OK
    status = MdsCopyDxXd(((struct descriptor_dispatch *)tmp.pointer)->ident, out_ptr);
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return image (.exe filename) portion of a record.
                text = IMAGE_OF(call)
                text = IMAGE_OF(conglom)
                text = IMAGE_OF(routine)
*/
int Tdi1ImageOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_CALL,
    DTYPE_CONGLOM,
    DTYPE_ROUTINE,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_CALL:
      status = MdsCopyDxXd(((struct descriptor_call *)tmp.pointer)->image, out_ptr);
      break;
    case DTYPE_CONGLOM:
      status = MdsCopyDxXd(((struct descriptor_conglom *)tmp.pointer)->image, out_ptr);
      break;
    case DTYPE_ROUTINE:
      status = MdsCopyDxXd(((struct descriptor_routine *)tmp.pointer)->image, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return interrupt in dispatch information, only allowed for asynchronous.
                when = INTERRUPT_OF(action or from dispatch)
*/
int Tdi1InterruptOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  struct descriptor_dispatch *pd;
  char omits[] = {
    DTYPE_T,
    0
  };

  status = TdiDispatchOf(list[0], &tmp MDS_END_ARG);
  if STATUS_OK {
    pd = (struct descriptor_dispatch *)tmp.pointer;
    if (*(char *)pd->pointer != TreeSCHED_ASYNC) {
      status = TdiGetData(omits, pd->when, out_ptr);
      if STATUS_OK {
	if (out_ptr->pointer == 0 || out_ptr->pointer->dtype != DTYPE_T)
	  status = TdiINVDTYDSC;
      }
    } else
      status = TdiINV_OPC;
  } else
    status = TdiINVDTYDSC;
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return language used by procedure.
                language_field = LANGUAGE_OF(procedure)
*/
int Tdi1LanguageOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_PROCEDURE,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_PROCEDURE:
      status = MdsCopyDxXd(((struct descriptor_procedure *)tmp.pointer)->language, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return method used on object.
                method_field = METHOD_OF(method)
*/
int Tdi1MethodOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_METHOD,
    DTYPE_OPAQUE,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_METHOD:
      status = MdsCopyDxXd(((struct descriptor_method *)tmp.pointer)->method, out_ptr);
      break;
    case DTYPE_OPAQUE:
      status = MdsCopyDxXd(((struct descriptor_opaque *)tmp.pointer)->opaque_type, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return conglomerate information.
                model_field = MODEL_OF(conglom)
*/
int Tdi1ModelOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_CONGLOM,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_CONGLOM:
      status = MdsCopyDxXd(((struct descriptor_conglom *)tmp.pointer)->model, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return conglomerate information.
                name_field = NAME_OF(conglom)
*/
int Tdi1NameOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_CONGLOM,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_CONGLOM:
      status = MdsCopyDxXd(((struct descriptor_conglom *)tmp.pointer)->name, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return the number of class-R descriptor pointers.
                byte = NDESC_OF(&classR)
*/
int Tdi1NdescOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_r *pr = (struct descriptor_r *)list[0];
  unsigned char ndesc;

  while (pr && pr->dtype == DTYPE_DSC)
    pr = (struct descriptor_r *)pr->pointer;
  if (pr)
    switch (pr->class) {
    case CLASS_R:
      ndesc = pr->ndesc;
      status = TdiPutLogical(ndesc, out_ptr);
      break;
    default:
      status = TdiINVCLADSC;
      break;
  } else
    status = TdiNULL_PTR;
  return status;
}

/*--------------------------------------------------------------
        Evaluate and then return the number of descriptors of an argument.
        WARNING, will not work for DTYPE_FUNCTION.
                byte = NDESC(any)
*/
int Tdi1Ndesc(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;

  status = TdiEvaluate(list[0], out_ptr MDS_END_ARG);
  if STATUS_OK
    status = Tdi1NdescOf(opcode, narg, &out_ptr->pointer, out_ptr);
  return status;
}

/*--------------------------------------------------------------
        Return object used by method.
                object_field = OBJECT_OF(method)
*/
int Tdi1ObjectOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_METHOD,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_METHOD:
      status = MdsCopyDxXd(((struct descriptor_method *)tmp.pointer)->object, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return performance in action information.
                performance_field = PERFORMANCE_OF(action)
*/
int Tdi1PerformanceOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[],
		      struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_ACTION,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_ACTION:
      if (((struct descriptor_action *)tmp.pointer)->ndesc<5)
        status = MdsFree1Dx(out_ptr, NULL);
      else
	status = MdsCopyDxXd((struct descriptor *)((struct descriptor_action *)tmp.pointer)->performance, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return phase in dispatch information.
                phase_field = PHASE_OF(dispatch)
*/
int Tdi1PhaseOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;

  status = TdiDispatchOf(list[0], &tmp MDS_END_ARG);
  if STATUS_OK
    status = MdsCopyDxXd(((struct descriptor_dispatch *)tmp.pointer)->phase, out_ptr);
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return procedure using a language.
                procedure_field = PROCEDURE_OF(procedure)
*/
int Tdi1ProcedureOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_PROCEDURE,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_PROCEDURE:
      status = MdsCopyDxXd(((struct descriptor_procedure *)tmp.pointer)->procedure, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return program used by program.
                program_field = PROGRAM_OF(program)
*/
int Tdi1ProgramOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_PROGRAM,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_PROGRAM:
      status = MdsCopyDxXd(((struct descriptor_program *)tmp.pointer)->program, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return conglomerate information.
                type = QUALIFIERS_OF(call)
                modifier = QUALIFIERS_OF(condition)
                qualifier_field = QUALIFIERS_OF(conglom)
                opcode = QUALIFIERS_OF(dependency)
                type = QUALIFIERS_OF(dispatch)
                opcode = QUALIFIERS_OF(function)
*/
int Tdi1QualifiersOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  struct descriptor *pd;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_CALL,
    DTYPE_CONDITION,
    DTYPE_CONGLOM,
    DTYPE_DEPENDENCY,
    DTYPE_DISPATCH,
    DTYPE_FUNCTION,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  pd = tmp.pointer;
  if STATUS_OK
    switch (pd->dtype) {
    case DTYPE_CONGLOM:
      status = MdsCopyDxXd(((struct descriptor_conglom *)pd)->qualifiers, out_ptr);
      break;
    case DTYPE_CALL:
    case DTYPE_CONDITION:
    case DTYPE_DEPENDENCY:
    case DTYPE_DISPATCH:
    case DTYPE_FUNCTION:
      {
	struct descriptor x = { 0, 0, CLASS_S, 0 };
	x.length = pd->length;
	x.dtype = (char)(pd->length == 1 ? DTYPE_BU : DTYPE_WU);
	x.pointer = pd->pointer;
	status = MdsCopyDxXd(&x, out_ptr);
	break;
      }
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return raw data portion of a SIGNAL.
                raw_field = RAW_OF(signal)
                data = RAW_OF(other)
*/
int Tdi1RawOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_SIGNAL,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_SIGNAL:
      status = MdsCopyDxXd(((struct descriptor_signal *)tmp.pointer)->raw, out_ptr);
      break;
    default:
      MdsFree1Dx(out_ptr, NULL);
      *out_ptr = tmp;
      return status;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return routine (entry-point name) portion of a record.
                routine_field = ROUTINE_OF(call)
                routine_field = ROUTINE_OF(routine)
*/
int Tdi1RoutineOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_CALL,
    DTYPE_ROUTINE,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_CALL:
      status = MdsCopyDxXd(((struct descriptor_call *)tmp.pointer)->routine, out_ptr);
      break;
    case DTYPE_ROUTINE:
      status = MdsCopyDxXd(((struct descriptor_routine *)tmp.pointer)->routine, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return slope field information.
                step_field = SLOPE_OF(range)
                slope_field = SLOPE_OF(slope,[n]) !deprecated!
*/
int Tdi1SlopeOf(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  STATIC_CONSTANT unsigned char one_val = 1;
  STATIC_CONSTANT struct descriptor one =
      { sizeof(unsigned char), DTYPE_BU, CLASS_S, (char *)&one_val };

  struct descriptor_xd tmp = EMPTY_XD;
  unsigned int n = 0;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_RANGE,
    DTYPE_SLOPE,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if (STATUS_OK && narg > 1)
    status = TdiGetLong(list[1], &n);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_RANGE:
      if (n > 0)
	status = TdiBAD_INDEX;
      if (((struct descriptor_range *)tmp.pointer)->ndesc < 3)
	MdsCopyDxXd(&one, out_ptr);
      else
	MdsCopyDxXd(((struct descriptor_r *)tmp.pointer)->dscptrs[2], out_ptr);
      break;
    case DTYPE_SLOPE:
      if (3 * n + 1 > ((struct descriptor_r *)tmp.pointer)->ndesc)
	status = TdiBAD_INDEX;
      if STATUS_OK
	status = MdsCopyDxXd(((struct descriptor_slope *)tmp.pointer)->segment[n].slope, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return taskule information.
                task_field = TASK_OF(action)
                same = TASK_OF(program)
                same = TASK_OF(procedure)
                same = TASK_OF(routine)
                same = TASK_OF(method)
*/
int Tdi1TaskOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_ACTION,
    DTYPE_METHOD,
    DTYPE_PROCEDURE,
    DTYPE_PROGRAM,
    DTYPE_ROUTINE,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_ACTION:
      status = TdiTaskOf(((struct descriptor_action *)tmp.pointer)->task, out_ptr MDS_END_ARG);
      break;
    case DTYPE_METHOD:
    case DTYPE_PROCEDURE:
    case DTYPE_PROGRAM:
    case DTYPE_ROUTINE:
    case DTYPE_L:
    case DTYPE_LU:
    case DTYPE_W:
    case DTYPE_WU:
    case DTYPE_B:
    case DTYPE_BU:
      MdsFree1Dx(out_ptr, NULL);
      *out_ptr = tmp;
      return status;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return TIME_OUT of program, procedure, routine, or method.
                time_out_field = TIME_OUT_OF(program)
                time_out_field = TIME_OUT_OF(procedure)
                time_out_field = TIME_OUT_OF(routine)
                time_out_field = TIME_OUT_OF(method)
*/
int Tdi1TimeoutOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_METHOD,
    DTYPE_PROCEDURE,
    DTYPE_PROGRAM,
    DTYPE_ROUTINE,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_METHOD:
      status = MdsCopyDxXd(((struct descriptor_method *)tmp.pointer)->time_out, out_ptr);
      break;
    case DTYPE_PROCEDURE:
      status = MdsCopyDxXd(((struct descriptor_procedure *)tmp.pointer)->time_out, out_ptr);
      break;
    case DTYPE_PROGRAM:
      status = MdsCopyDxXd(((struct descriptor_program *)tmp.pointer)->time_out, out_ptr);
      break;
    case DTYPE_ROUTINE:
      status = MdsCopyDxXd(((struct descriptor_routine *)tmp.pointer)->time_out, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return units portion of with_units.
                units_field = UNITS_OF(with_units)
                " " = UNITS_OF(other)
        NEED thought about rescale of units, parameters...
*/
int Tdi1UnitsOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  STATIC_CONSTANT DESCRIPTOR(none, " ");
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_WITH_UNITS,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_WITH_UNITS:
      status = MdsCopyDxXd(((struct descriptor_with_units *)tmp.pointer)->units, out_ptr);
      break;
    default:
      status = MdsCopyDxXd((struct descriptor *)&none, out_ptr);
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return validation field of a parameter.
                validation_field = VALIDATION_OF(param)
*/
int Tdi1ValidationOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_PARAM,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_PARAM:
      status = MdsCopyDxXd(((struct descriptor_param *)tmp.pointer)->validation, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return data portion of signal or parameter.
                value_field = VALUE_OF(param)   
                data_field = VALUE_OF(signal)
                value_at_idx0_field = VALUE_OF(window or dimension)
                data_field = VALUE_OF(with_units)
                data = VALUE_OF(other)  
*/
int Tdi1ValueOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_DIMENSION,
    DTYPE_PARAM,
    DTYPE_SIGNAL,
    DTYPE_WINDOW,
    DTYPE_WITH_UNITS,
    DTYPE_OPAQUE,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_DIMENSION:
      status =
	  TdiValueOf(((struct descriptor_dimension *)tmp.pointer)->window, out_ptr MDS_END_ARG);
      break;
    case DTYPE_PARAM:
      status = MdsCopyDxXd(((struct descriptor_param *)tmp.pointer)->value, out_ptr);
      break;
    case DTYPE_SIGNAL:
      status = MdsCopyDxXd(((struct descriptor_signal *)tmp.pointer)->data, out_ptr);
      break;
    case DTYPE_WINDOW:
      status = MdsCopyDxXd(((struct descriptor_window *)tmp.pointer)->value_at_idx0, out_ptr);
      break;
    case DTYPE_WITH_UNITS:
      status = MdsCopyDxXd(((struct descriptor_with_units *)tmp.pointer)->data, out_ptr);
      break;
    case DTYPE_OPAQUE:
      status = MdsCopyDxXd(((struct descriptor_opaque *)tmp.pointer)->data, out_ptr);
      break;
    default:
      MdsFree1Dx(out_ptr, NULL);
      *out_ptr = tmp;
      return status;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return dispatch information.
                when_field = WHEN_OF(dispatch)
*/
int Tdi1WhenOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;

  status = TdiDispatchOf(list[0], &tmp MDS_END_ARG);
  if STATUS_OK
    status = MdsCopyDxXd(((struct descriptor_dispatch *)tmp.pointer)->when, out_ptr);
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*--------------------------------------------------------------
        Return window portion of a dimension.
                window_field = WINDOW_OF(dimension)
                same = WINDOW_OF(window)
*/
int Tdi1WindowOf(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)), struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_DIMENSION,
    DTYPE_WINDOW,
    0
  };

  status = TdiGetData(omits, list[0], &tmp);
  if STATUS_OK
    switch (tmp.pointer->dtype) {
    case DTYPE_DIMENSION:
      status = MdsCopyDxXd(((struct descriptor_dimension *)tmp.pointer)->window, out_ptr);
      break;
    case DTYPE_WINDOW:
      MdsFree1Dx(out_ptr, NULL);
      *out_ptr = tmp;
      return status;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}
