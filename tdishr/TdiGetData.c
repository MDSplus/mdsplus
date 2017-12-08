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
/*      TdiGetData
        Evaluate and get data.
        For use by TDI$$GET_ARG, Tdi1Data, ... .
        Generally we have expressions including conversion or scaling.
                DATA(dimension)         list of axis points
                DATA(slope)             list of axis points without window
                DATA(signal)            data scalar or array
                DATA(param)             value scalar or array
                DATA(range)             list of numbers
                DATA(VMS)               scalar or array
                DATA(with_units)        data scalar or array

        status = TdiGetData(&omissions, &in, &dat)
        omissions: use dtype code to omit resolving that code, list permitted.
        Use this with DTYPE_SIGNAL to get signal, unresolved.
        Use this to get RANGE or data or for WITH_UNITS or data.
        Do not use DATA(VALUE_OF(param)), use DATA(param).
        Do not use DATA(VALIDATION_OF(param)), use VALIDATION(param).
        They won't work if the _OF routine returns a function because $VALUE is not defined.

        Ken Klare, LANL P-4     (c)1989,1990,1991,1992,1994
        NEED to think, should "TdiImpose" convert data type?
        ASSUMES VECTOR works for any size.
*/
#include <mdsplus/mdsplus.h>
#include <STATICdef.h>
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include <tdishr_messages.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <string.h>
#include <strings.h>
#include "tdithreadsafe.h"



#define _MOVC3(a,b,c) memcpy(c,b,a)
int TdiGetRecord(int nid, struct descriptor_xd *out);

extern unsigned short OpcDtypeRange;

extern int TdiEvaluate();
extern int TdiGetIdent();
extern int TdiItoX();
extern int TdiIntrinsic();
extern int TdiCall();
extern int TdiConvert();
extern int TdiData();
extern int TdiUnits();
extern int Tdi2Add();

int TdiImpose();

static void FixupDollarNodes(int nid, struct descriptor *out_ptr) {
  if (out_ptr) {
    switch (out_ptr->class) {
      case CLASS_S:
      case CLASS_D:
        if ((out_ptr->dtype == DTYPE_IDENT) &&
            (out_ptr->length == 5) &&
            (strncasecmp((const char *)out_ptr->pointer,"$node",5) == 0)) {
          out_ptr->dtype=DTYPE_NID;
          out_ptr->length=4;
          *(int *)out_ptr->pointer=nid;
        }
        break;
      case CLASS_XS:
      case CLASS_XD:
        FixupDollarNodes(nid, (struct descriptor *)out_ptr->pointer);
        break;
      case CLASS_R: {
        struct descriptor_r *ptr = (struct descriptor_r *)out_ptr;
        unsigned char i;
        for (i=0;i<ptr->ndesc; i++)
          FixupDollarNodes(nid, (struct descriptor *)ptr->dscptrs[i]);
        break;
      }
    }
  }
}

int TdiImpose(struct descriptor_a *in_ptr, struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  array_bounds *pout = (array_bounds *) out_ptr->pointer;
  array_bounds arr;
  int in_size, out_size, dimct;

  if (pout == 0)
    return 1;
  switch (pout->class) {
  case CLASS_A:
  case CLASS_CA:
    dimct = in_ptr->dimct;
    if (dimct > MAXDIM)
      return TdiNDIM_OVER;
    in_size = sizeof(struct descriptor_a);
    if (in_ptr->aflags.coeff) {
      in_size += sizeof(void *) + dimct * sizeof(int);
      if (in_ptr->aflags.bounds)
	in_size += dimct * sizeof(int);
    }
    dimct = pout->dimct;
    _MOVC3((short)in_size, (char *)in_ptr, (char *)&arr);
    if (in_ptr->class == CLASS_APD) {
      arr.length = pout->length;
      arr.dtype = pout->dtype;
    }
    arr.class = pout->class;
    if (in_ptr->aflags.coeff) {
			/*******************************
                        For CA it is a relative pointer.
                        *******************************/
      if (in_ptr->class == CLASS_CA) {
	arr.a0 = pout->pointer + *(int *)&arr.a0;
      } else
	arr.a0 = pout->pointer + (arr.a0 - arr.pointer);
    }
    arr.pointer = pout->pointer;
    out_size = sizeof(struct descriptor_a);
    if (pout->aflags.coeff) {
      out_size += sizeof(void *) + dimct * sizeof(int);
      if (pout->aflags.bounds)
	out_size += dimct * sizeof(int);
    }
    if (in_size <= out_size && pout->class == CLASS_A)
      _MOVC3((short)in_size, (char *)&arr, (char *)pout);
    else {
      struct descriptor_xd tmp = *out_ptr;
      *out_ptr = EMPTY_XD;
      status = MdsCopyDxXd((struct descriptor *)&arr, out_ptr);
      MdsFree1Dx(&tmp, NULL);
    }
    break;
  case CLASS_S:
  case CLASS_D:
    if (in_ptr->class == CLASS_CA) {
      DESCRIPTOR_A(simple, 0, 0, 0, 0);
      simple.length = in_ptr->length;
      simple.dtype = in_ptr->dtype;
      simple.pointer = in_ptr->pointer;
      simple.arsize = in_ptr->length;
      status = TdiImpose((struct descriptor_a *)&simple, out_ptr);
    } else {
      DESCRIPTOR_A(simple, 0, 0, 0, 0);
      simple.length = pout->length;
      simple.dtype = pout->dtype;
      simple.pointer = pout->pointer;
      simple.arsize = pout->length;
      status = TdiImpose((struct descriptor_a *)&simple, out_ptr);
    }
    break;
  default:

    status = TdiINVCLADSC;
    break;
  }
  return status;
}

/*----------------------------------------------------------------------------
*/
STATIC_CONSTANT struct descriptor missing_dsc = { 0, DTYPE_MISSING, CLASS_S, 0 };

int TdiGetData(unsigned char omits[], struct descriptor *their_ptr, struct descriptor_xd *out_ptr)
{
  int nid, *pnid;
  unsigned char *optr, dtype = 0;
  struct descriptor_signal *keep;
  struct descriptor_xd hold = EMPTY_XD;
  struct descriptor_r *pin = (struct descriptor_r *)their_ptr;
  INIT_STATUS;
  GET_TDITHREADSTATIC_P;
  int *recursion_count = &(TdiThreadStatic_p->TdiGetData_recursion_count);
  *recursion_count = (*recursion_count + 1);
  if ((*recursion_count) > 1800)
    return TdiRECURSIVE;
  while (pin && (dtype = pin->dtype) == DTYPE_DSC) {
    pin = (struct descriptor_r *)pin->pointer;
    *recursion_count = (*recursion_count + 1);
    if ((*recursion_count) > 1800)
      return TdiRECURSIVE;
  }
  if (!pin)
    status = MdsCopyDxXd(&missing_dsc, &hold);
  else {
    for (optr = &omits[0]; *optr != 0; ++optr)
      if (*optr == dtype)
	break;
    if (*optr)
      status = MdsCopyDxXd((struct descriptor *)pin, &hold);
    else
      switch (pin->class) {
      case CLASS_CA:
	if (pin->pointer) {
	  status = TdiGetData(omits, (struct descriptor *)pin->pointer, &hold);
/********************* Why is this needed????????????? *************************************/
	  if STATUS_OK
	    status = TdiImpose((struct descriptor_a *)pin, &hold);
/***************************************************************************************/
	} else
	  status = MdsCopyDxXd((struct descriptor *)pin, &hold);
	break;
      case CLASS_APD:
	status = TdiEvaluate(pin, &hold MDS_END_ARG);
	break;
      case CLASS_S:
      case CLASS_D:
      case CLASS_A:
	switch (dtype) {
		/***********************
                Evaluate on these types.
                ***********************/
	case DTYPE_IDENT:
	  status = TdiGetIdent(pin, &hold);
 redo:	  if STATUS_OK
	    status = TdiGetData(omits, (struct descriptor *)&hold, &hold);
	  break;
	case DTYPE_NID:
	  pnid = (int *)pin->pointer;
	  status = TdiGetRecord(*pnid, &hold);
	  goto redo;
	case DTYPE_PATH:
	  {
	    char *path = MdsDescrToCstring((struct descriptor *)pin);
	    status = TreeFindNode(path, &nid);
	    MdsFree(path);
	    if STATUS_OK
	      status = TdiGetRecord(nid, &hold);
	  }
	  goto redo;
		/*******************
                VMS types come here.
                Renames their XD.
                *******************/
	default:
	  if (their_ptr->class == CLASS_XD) {
	    hold = *(struct descriptor_xd *)their_ptr;
	    *(struct descriptor_xd *)their_ptr = EMPTY_XD;
	  } else
	    status = MdsCopyDxXd((struct descriptor *)pin, &hold);
	  break;
	}
	break;
      case CLASS_R:
	switch (dtype) {
	case DTYPE_FUNCTION:
	  status = TdiIntrinsic(*(unsigned short *)pin->pointer, pin->ndesc, pin->dscptrs, &hold);
	  goto redo;
	case DTYPE_CALL:
	  status =
	      TdiCall(pin->length ? *(unsigned char *)pin->pointer : DTYPE_L, pin->ndesc,
		      pin->dscptrs, &hold);
	  goto redo;
	case DTYPE_PARAM:
	  keep = (struct descriptor_signal *)TdiThreadStatic_p->TdiSELF_PTR;
	  TdiThreadStatic_p->TdiSELF_PTR = (struct descriptor_xd *)pin;
	  status =
	      TdiGetData(omits,
			 (struct descriptor *)((struct descriptor_param *)pin)->value, &hold);
	  TdiThreadStatic_p->TdiSELF_PTR = (struct descriptor_xd *)keep;
	  break;
	case DTYPE_SIGNAL:
			/******************************************
                        We must set up for reference to our $VALUE.
                        ******************************************/
	  keep = (struct descriptor_signal *)TdiThreadStatic_p->TdiSELF_PTR;
	  TdiThreadStatic_p->TdiSELF_PTR = (struct descriptor_xd *)pin;
	  status =
	      TdiGetData(omits,
			 (struct descriptor *)((struct descriptor_signal *)pin)->data, &hold);
	  TdiThreadStatic_p->TdiSELF_PTR = (struct descriptor_xd *)keep;
	  break;
		/***************
                Windowless axis.
                ***************/
	case DTYPE_SLOPE:
	case DTYPE_DIMENSION:
	  status = TdiItoX(pin, &hold MDS_END_ARG);
	  goto redo;
		/**************************
                Range can have 2 or 3 args.
                **************************/
	case DTYPE_RANGE:
	  status = TdiIntrinsic(OpcDtypeRange, pin->ndesc, &pin->dscptrs[0], &hold);
	  goto redo;
	case DTYPE_WITH_UNITS:
	  status = TdiGetData(omits, (struct descriptor *)((struct descriptor_with_units *)
							   pin)->data, &hold);
	  break;
	case DTYPE_WITH_ERROR:
	  status = TdiGetData(omits, (struct descriptor *)((struct descriptor_with_error *)
							   pin)->data, &hold);
	  break;
	case DTYPE_OPAQUE:
	  status = TdiGetData(omits, (struct descriptor *)((struct descriptor_opaque *)
							   pin)->data, &hold);
	  break;
	default:
	  status = TdiINVCLADTY;
	  break;
	}
	break;
      default:
	status = TdiINVCLADSC;
	break;
      }
  }
  *recursion_count = 0;
	/**********************************
        Watch out for input same as output.
        **********************************/
  MdsFree1Dx(out_ptr, NULL);
  if STATUS_OK
    *out_ptr = hold;
  else
    MdsFree1Dx(&hold, NULL);
  return status;
}

/*----------------------------------------------------------------------------
        Get a float scalar value from an expression or whatever.
        Useful internal and external function.
        C only: status = TdiGetFloat(&in_dsc, &float)
*/
extern EXPORT int TdiGetFloat(struct descriptor *in_ptr, float *val_ptr)
{
  INIT_STATUS;

  if (in_ptr == 0)
    *val_ptr = (float)0.0;
  else if (in_ptr->dtype < 160 && in_ptr->dtype != DTYPE_DSC)
    switch (in_ptr->class) {
    case CLASS_A:
      if (((struct descriptor_a *)in_ptr)->arsize != in_ptr->length)
	return TdiNOT_NUMBER;
	/*********************
        WARNING falls through.
        *********************/
      MDS_ATTR_FALLTHROUGH
    case CLASS_S:
    case CLASS_D:
      switch (in_ptr->dtype) {
      case DTYPE_BU:
	*val_ptr = (float)*(unsigned char *)in_ptr->pointer;
	break;
      case DTYPE_B:
	*val_ptr = (float)*(char *)in_ptr->pointer;
	break;
      case DTYPE_WU:
	*val_ptr = (float)*(unsigned short *)in_ptr->pointer;
	break;
      case DTYPE_W:
	*val_ptr = (float)*(short *)in_ptr->pointer;
	break;
      case DTYPE_LU:
	*val_ptr = (float)*(unsigned int *)in_ptr->pointer;
	break;
      case DTYPE_L:
	*val_ptr = (float)*(int *)in_ptr->pointer;
	break;
      default:{
	  struct descriptor val_dsc = { sizeof(float), DTYPE_NATIVE_FLOAT, CLASS_S, 0 };
	  val_dsc.pointer = (char *)val_ptr;
	  status = TdiConvert(in_ptr, &val_dsc MDS_END_ARG);
	} break;
      }
      break;
    default:
      status = TdiINVCLADSC;
      break;
  } else {
    struct descriptor_xd tmp = EMPTY_XD;
    status = TdiData(in_ptr, &tmp MDS_END_ARG);
    if STATUS_OK
      switch (tmp.pointer->class) {
      case CLASS_S:
      case CLASS_D:
      case CLASS_A:
	status = TdiGetFloat(tmp.pointer, val_ptr);
	break;
      default:
	status = TdiINVCLADSC;
	break;
      }
    MdsFree1Dx(&tmp, NULL);
  }
  return status;
}

/*----------------------------------------------------------------------------
        Get a long scalar value from an expression or whatever.
        Useful internal and external function.
        C only: status = TdiGetLong(&in_dsc, &long)
*/
extern EXPORT int TdiGetLong(struct descriptor *in_ptr, int *val_ptr)
{
  INIT_STATUS;

  if (in_ptr == 0)
    *val_ptr = 0;
  else if (in_ptr->dtype < 160 && in_ptr->dtype != DTYPE_DSC)
    switch (in_ptr->class) {
    case CLASS_A:
      if (((struct descriptor_a *)in_ptr)->arsize != in_ptr->length)
	return TdiNOT_NUMBER;
	/*********************
        WARNING falls through.
        *********************/
      MDS_ATTR_FALLTHROUGH
    case CLASS_S:
    case CLASS_D:
      switch (in_ptr->dtype) {
      case DTYPE_BU:
	*val_ptr = (int)*(unsigned char *)in_ptr->pointer;
	break;
      case DTYPE_B:
	*val_ptr = (int)*(char *)in_ptr->pointer;
	break;
      case DTYPE_WU:
	*val_ptr = (int)*(unsigned short *)in_ptr->pointer;
	break;
      case DTYPE_W:
	*val_ptr = (int)*(short *)in_ptr->pointer;
	break;
      case DTYPE_L:
      case DTYPE_LU:
	*val_ptr = *(int *)in_ptr->pointer;
	break;
      default:{
	  struct descriptor val_dsc = { sizeof(int), DTYPE_L, CLASS_S, 0 };
	  val_dsc.pointer = (char *)val_ptr;
	  status = TdiConvert(in_ptr, &val_dsc MDS_END_ARG);
	}
	break;
      }
      break;
    default:
      status = TdiINVCLADSC;
      break;
  } else {
    struct descriptor_xd tmp = EMPTY_XD;
    status = TdiData(in_ptr, &tmp MDS_END_ARG);
    if STATUS_OK
      switch (tmp.pointer->class) {
      case CLASS_S:
      case CLASS_D:
      case CLASS_A:
	status = TdiGetLong(tmp.pointer, val_ptr);
	break;
      default:
	status = TdiINVCLADSC;
	break;
      }
    MdsFree1Dx(&tmp, NULL);
  }
  return status;
}

/*----------------------------------------------------------------------------
        Get the Node ID for NID, PATH, or TEXT types including expressions.
        Useful internal and external function.
        C only: status = TdiGetNid(&in_dsc, &nid)
*/
extern EXPORT int TdiGetNid(struct descriptor *in_ptr, int *nid_ptr)
{
  INIT_STATUS;
  struct descriptor_xd tmp = EMPTY_XD;
  unsigned char omits[] = {
    DTYPE_NID,
    DTYPE_PATH,
    0
  };

  status = TdiGetData(omits, in_ptr, &tmp);
  if STATUS_OK {
    switch (tmp.pointer->dtype) {
    case DTYPE_T:
    case DTYPE_PATH:
      {
	char *path = MdsDescrToCstring(tmp.pointer);
	status = TreeFindNode(path, nid_ptr);
	MdsFree(path);
      }
      break;
    case DTYPE_NID:
      *nid_ptr = *(unsigned int *)tmp.pointer->pointer;
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
    MdsFree1Dx(&tmp, NULL);
  }
  return status;
}

/*----------------------------------------------------------------------------
        Give signal, dimension, or parameter for use by lower level routines.
        RAW_OF($THIS) = $VALUE is raw field of a signal.
        DIM_OF($THIS, [dim]) gives a dimension of a signal.
        VALUE_OF($THIS) = $VALUE is value field of param.
        HELP_OF($THIS) gives help field of a param, for example, in the validation.
        $THIS is the dimension used by ranges in complex signal subscripting.
        WARNING: $THIS is only defined within the subexpressions.
        WARNING: Use of $THIS or $VALUE can be infinitely recursive.
*/
int Tdi1This(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)),
	     struct descriptor *list[] __attribute__ ((unused)), struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  GET_TDITHREADSTATIC_P;
  if (TdiThreadStatic_p->TdiSELF_PTR)
    status = MdsCopyDxXd((struct descriptor *)(TdiThreadStatic_p->TdiSELF_PTR), out_ptr);
  else
    status = TdiNO_SELF_PTR;
  return status;
}

/*----------------------------------------------------------------------------
        Give value of RAW_OF(signal) for DATA(signal)
        or VALUE_OF(param) for VALIDATION(param).
        This allows the data field of signals to reference the raw field of a signal
        and the validation field of params to reference the value field of a param.
*/
int Tdi1Value(int opcode __attribute__ ((unused)) , int narg __attribute__ ((unused)),
	      struct descriptor *list[] __attribute__ ((unused)), struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  GET_TDITHREADSTATIC_P;
  if (TdiThreadStatic_p->TdiSELF_PTR)
    switch (TdiThreadStatic_p->TdiSELF_PTR->dtype) {
    case DTYPE_SIGNAL:
      status =
	  MdsCopyDxXd(((struct descriptor_signal *)TdiThreadStatic_p->TdiSELF_PTR)->raw, out_ptr);
      break;
    case DTYPE_PARAM:
      status =
	  MdsCopyDxXd(((struct descriptor_param *)TdiThreadStatic_p->TdiSELF_PTR)->value, out_ptr);
      break;
    default:
      status = TdiINVDTYDSC;
      break;
  } else
    status = TdiNO_SELF_PTR;
  return status;
}

/*----------------------------------------------------------------------------
        Return VMS data portion of any data type in input descriptor.
        The output descriptor must be class XD, it will be and XD-DSC.
        A major entry point for evaluation of the data of expressions.
                status = TdiData(&in, &out MDS_END_ARG)
*/
int Tdi1Data(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)),
	     struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;

  unsigned char omits[] = { 0 };
  status = TdiGetData(omits, list[0], out_ptr);
  return status;
}

/*----------------------------------------------------------------------------
        Return units portion of with_units. Does not require complete evaluation.
                UNITS(dimension)        UNITS(axis)
                UNITS(range or slope)   combined units of arguments
                UNITS(window)           UNITS(value_idx0)
                UNITS(with_units)       DATA(units)
                UNITS(other)            UNITS(DATA(other) not stripping above)
                UNITS(other)            " "     (single blank to keep IDL happy)
*/
int Tdi1Units(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_r *rptr;
  struct descriptor_xd tmp = EMPTY_XD, uni[2];
  struct TdiCatStruct cats[3];
  int j;
  STATIC_CONSTANT DESCRIPTOR(blank_dsc, " ");
  STATIC_CONSTANT unsigned char omits[] = {
    DTYPE_DIMENSION,
    DTYPE_RANGE,
    DTYPE_SLOPE,
    DTYPE_WINDOW,
    DTYPE_WITH_UNITS,
    0
  };

  status = TdiGetData((unsigned char *)omits, list[0], &tmp);
  rptr = (struct descriptor_r *)tmp.pointer;
  if STATUS_OK
    switch (rptr->dtype) {
    case DTYPE_DIMENSION:
      status = TdiUnits(((struct descriptor_dimension *)rptr)->axis, out_ptr MDS_END_ARG);
      break;
    case DTYPE_RANGE:
    case DTYPE_SLOPE:
      uni[1] = uni[0] = EMPTY_XD;
      for (j = rptr->ndesc; --j >= 0 && STATUS_OK;) {
	if (rptr->dscptrs[j])
	  status = TdiUnits(rptr->dscptrs[j], &uni[1] MDS_END_ARG);
	if (uni[1].pointer && STATUS_OK)
	  status = Tdi2Add(narg, uni, 0, cats, 0);
      }
      MdsFree1Dx(&uni[1], NULL);
      if (uni[0].pointer) {
	MdsFree1Dx(out_ptr, NULL);
	*out_ptr = uni[0];
      } else if STATUS_OK
	status = MdsCopyDxXd((struct descriptor *)&blank_dsc, out_ptr);
      break;
    case DTYPE_WINDOW:
      status = TdiUnits(((struct descriptor_window *)rptr)->value_at_idx0, out_ptr MDS_END_ARG);
      break;
    case DTYPE_WITH_UNITS:
      status = TdiData(((struct descriptor_with_units *)rptr)->units, out_ptr MDS_END_ARG);
      break;
    case DTYPE_WITH_ERROR:
      status = TdiData(((struct descriptor_with_error *)rptr)->data, out_ptr MDS_END_ARG);
      break;
    default:
      status = MdsCopyDxXd((struct descriptor *)&blank_dsc, out_ptr);
      break;
    }
  MdsFree1Dx(&tmp, NULL);
  return status;
}

/*----------------------------------------------------------------------------
        Return VMS data portion and the units of the input descriptor.
        Always returns a with_units descriptor with data, assuming no error.
        Caution. The units field may be null.
                status = TdiDataWithUnits(&in, &out MDS_END_ARG)
*/
STATIC_CONSTANT DESCRIPTOR_WITH_UNITS(null, 0, 0);
int Tdi1DataWithUnits(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)),
		      struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  STATIC_CONSTANT unsigned char omits[] = { DTYPE_WITH_UNITS, 0 };
  struct descriptor_with_units dwu = null;
  struct descriptor_xd data = EMPTY_XD, units = EMPTY_XD;
  dwu.data = null.data = (struct descriptor *)&missing_dsc;
  status = TdiGetData((unsigned char *)omits, list[0], out_ptr);
  if STATUS_OK {
    struct descriptor_with_units *pwu = (struct descriptor_with_units *)out_ptr->pointer;
    if (pwu) {
      if (pwu->dtype == DTYPE_WITH_UNITS) {
	status = TdiData(pwu->data, &data MDS_END_ARG);
	if STATUS_OK
	  status = TdiData(pwu->units, &units MDS_END_ARG);
	dwu.data = data.pointer;
	dwu.units = units.pointer;
      } else {
	data = *out_ptr;
	*out_ptr = EMPTY_XD;
	dwu.data = (struct descriptor *)pwu;
      }
      status = MdsCopyDxXd((struct descriptor *)&dwu, out_ptr);
      MdsFree1Dx(&data, NULL);
      MdsFree1Dx(&units, NULL);
    } else
      status = MdsCopyDxXd((struct descriptor *)&null, out_ptr);
  }
  return status;
}

/*----------------------------------------------------------------------------
        Evaluate and get validation field of a parameter.
        Do not use DATA(VALIDATION_OF(param)), use VALIDATION(param).
        Note VALIDATION_OF may return a function.
        Note that $VALUE would not be defined in that form.
        Need we check that result is logical scalar?
*/
int Tdi1Validation(int opcode __attribute__ ((unused)), int narg __attribute__ ((unused)),
		   struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  GET_TDITHREADSTATIC_P;
  struct descriptor_r *rptr;
  struct descriptor_xd *keep;
  STATIC_CONSTANT unsigned char omits[] = { DTYPE_PARAM, 0 };
  STATIC_CONSTANT unsigned char noomits[] = { 0 };

  status = TdiGetData((unsigned char *)omits, list[0], out_ptr);
  rptr = (struct descriptor_r *)out_ptr->pointer;
  if STATUS_OK
    switch (rptr->dtype) {
    case DTYPE_PARAM:
		/******************************************
                We must set up for reference to our $VALUE.
                ******************************************/
      keep = TdiThreadStatic_p->TdiSELF_PTR;
      TdiThreadStatic_p->TdiSELF_PTR = (struct descriptor_xd *)rptr;
      status = TdiGetData(noomits, ((struct descriptor_param *)rptr)->validation, out_ptr);
      TdiThreadStatic_p->TdiSELF_PTR = keep;
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  return status;
}

static int use_get_record_fun = 0;
EXPORT int TdiGetRecord(int nid, struct descriptor_xd *out)
{
  INIT_STATUS;
  if (use_get_record_fun) {
    int stat;
    short opcode = 162;		/* external function */
    DESCRIPTOR_LONG(stat_d, &stat);
    static DESCRIPTOR(getrec_d, "TdiGetRecord---not-used");
    DESCRIPTOR_LONG(nid_d, (char *)&nid);
    DESCRIPTOR(var_d, "_out");
    DESCRIPTOR_R(getrec, DTYPE_FUNCTION, 4);
    getrec.pointer = (unsigned char *)&opcode;
    getrec.dscptrs[0] = 0;
    getrec.dscptrs[1] = (struct descriptor *)&getrec_d;
    getrec.dscptrs[2] = (struct descriptor *)&nid_d;
    getrec.dscptrs[3] = (struct descriptor *)&var_d;
    var_d.dtype = DTYPE_IDENT;
    status = TdiEvaluate(&getrec, &stat_d MDS_END_ARG);
    if (status == TdiUNKNOWN_VAR || status == TdiSYNTAX)
      use_get_record_fun = 0;
    else if STATUS_OK {
      status = stat;
      if IS_OK(stat)
	status = TdiGetIdent(&var_d, out);
    }
  }
  if (!use_get_record_fun)
    status = TreeGetRecord(nid, (struct descriptor_xd *)out);
  if STATUS_OK
    FixupDollarNodes(nid,(struct descriptor *)out);
  return status;
}

void TdiResetGetRecord()
{
  use_get_record_fun = 1;
}
