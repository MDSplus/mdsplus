/*
-Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

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
/*      get_data
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

        status = tdi_get_data(&omissions, &in, &dat)
        omissions: use dtype code to omit resolving that code, list permitted.
        Use this with DTYPE_SIGNAL to get signal, unresolved.
        Use this to get RANGE or data or for WITH_UNITS or data.
        Do not use DATA(VALUE_OF(param)), use DATA(param).
        Do not use DATA(VALIDATION_OF(param)), use VALIDATION(param).
        They won't work if the _OF routine returns a function because $VALUE is
   not defined.

        Ken Klare, LANL P-4     (c)1989,1990,1991,1992,1994
        NEED to think, should "TdiImpose" convert data type?
        ASSUMES VECTOR works for any size.
*/
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include "tdithreadstatic.h"
#include <mdsplus/mdsplus.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <tdishr_messages.h>
#include <treeshr.h>

int TdiGetRecord(int nid, mdsdsc_xd_t *out);

extern int TdiEvaluate();
extern int tdi_get_ident();
extern int TdiItoX();
extern int TdiIntrinsic();
extern int TdiCall();
extern int TdiConvert();
extern int TdiData();
extern int TdiUnits();
extern int Tdi2Add();

static void fixup_dollar_nodes(int nid, mdsdsc_t *out_ptr)
{
  if (out_ptr)
  {
    switch (out_ptr->class)
    {
    default:
      break;
    case CLASS_S:
    case CLASS_D:
      if ((out_ptr->dtype == DTYPE_IDENT) && (out_ptr->length == 5) &&
          (strncasecmp((const char *)out_ptr->pointer, "$node", 5) == 0))
      {
        out_ptr->dtype = DTYPE_NID;
        out_ptr->length = 4;
        *(int *)out_ptr->pointer = nid;
      }
      break;
    case CLASS_XS:
    case CLASS_XD:
      fixup_dollar_nodes(nid, (mdsdsc_t *)out_ptr->pointer);
      break;
    case CLASS_R:
    {
      mdsdsc_r_t *ptr = (mdsdsc_r_t *)out_ptr;
      int i;
      for (i = 0; i < ptr->ndesc; i++)
        fixup_dollar_nodes(nid, (mdsdsc_t *)ptr->dscptrs[i]);
      break;
    }
    }
  }
}

int TdiImpose(mdsdsc_a_t *in_ptr, mdsdsc_xd_t *out_ptr)
{
  INIT_STATUS;
  array_bounds *pout = (array_bounds *)out_ptr->pointer;
  array_bounds arr;
  int in_size, out_size, dimct;

  if (pout == 0)
    return 1;
  switch (pout->class)
  {
  case CLASS_A:
  case CLASS_CA:
    dimct = in_ptr->dimct;
    if (dimct > MAX_DIMS)
      return TdiNDIM_OVER;
    in_size = sizeof(struct descriptor_a);
    if (in_ptr->aflags.coeff)
    {
      in_size += sizeof(void *) + dimct * sizeof(int);
      if (in_ptr->aflags.bounds)
        in_size += dimct * sizeof(int);
    }
    dimct = pout->dimct;
    memcpy(&arr, in_ptr, in_size);
    if (in_ptr->class == CLASS_APD)
    {
      arr.length = pout->length;
      arr.dtype = pout->dtype;
    }
    arr.class = pout->class;
    if (in_ptr->aflags.coeff)
    {
      /*******************************
      For CA it is a relative pointer.
      *******************************/
      if (in_ptr->class == CLASS_CA)
      {
        arr.a0 = pout->pointer + *(int *)&arr.a0;
      }
      else
        arr.a0 = pout->pointer + (arr.a0 - arr.pointer);
    }
    arr.pointer = pout->pointer;
    out_size = sizeof(struct descriptor_a);
    if (pout->aflags.coeff)
    {
      out_size += sizeof(void *) + dimct * sizeof(int);
      if (pout->aflags.bounds)
        out_size += dimct * sizeof(int);
    }
    if (in_size <= out_size && pout->class == CLASS_A)
      memcpy(pout, &arr, in_size);
    else
    {
      mdsdsc_xd_t tmp = *out_ptr;
      *out_ptr = EMPTY_XD;
      status = MdsCopyDxXd((mdsdsc_t *)&arr, out_ptr);
      MdsFree1Dx(&tmp, NULL);
    }
    break;
  case CLASS_S:
  case CLASS_D:
    if (in_ptr->class == CLASS_CA)
    {
      DESCRIPTOR_A(simple, 0, 0, 0, 0);
      simple.length = in_ptr->length;
      simple.dtype = in_ptr->dtype;
      simple.pointer = in_ptr->pointer;
      simple.arsize = in_ptr->length;
      status = TdiImpose((mdsdsc_a_t *)&simple, out_ptr);
    }
    else
    {
      DESCRIPTOR_A(simple, 0, 0, 0, 0);
      simple.length = pout->length;
      simple.dtype = pout->dtype;
      simple.pointer = pout->pointer;
      simple.arsize = pout->length;
      status = TdiImpose((mdsdsc_a_t *)&simple, out_ptr);
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
static const mdsdsc_t missing_dsc = {0, DTYPE_MISSING, CLASS_S, 0};

typedef struct get_data_cleanup
{
  mdsdsc_xd_t *out;
  mdsdsc_xd_t hold;
} get_data_cleanup_t;
static void get_data_cleanup(void *arg)
{
  get_data_cleanup_t *c = (get_data_cleanup_t *)arg;
  MdsFree1Dx(c->out, NULL);
  MdsFree1Dx(&c->hold, NULL);
}
static int get_data(const dtype_t omits[], mdsdsc_t *their_ptr,
                    mdsdsc_xd_t *out_ptr, TDITHREADSTATIC_ARG);
static int _get_data(const dtype_t omits[], mdsdsc_t *their_ptr,
                     mdsdsc_xd_t *out_ptr, TDITHREADSTATIC_ARG)
{
  int nid, *pnid;
  dtype_t dtype = 0;
  mds_signal_t *keep;
  mdsdsc_r_t *pin = (mdsdsc_r_t *)their_ptr;
  INIT_STATUS;
  TDI_GETDATA_REC++;
  if (TDI_GETDATA_REC > 1800)
  {
    return TdiRECURSIVE;
  }
  while (pin && (dtype = pin->dtype) == DTYPE_DSC)
  {
    pin = (mdsdsc_r_t *)pin->pointer;
    TDI_GETDATA_REC++;
    if (TDI_GETDATA_REC > 1800)
    {
      return TdiRECURSIVE;
    }
  }
  if (!pin)
  {
    return MdsCopyDxXd(&missing_dsc, out_ptr);
  }
  int i;
  for (i = 0; omits[i]; i++)
  {
    if (omits[i] == dtype)
    {
      return MdsCopyDxXd((mdsdsc_t *)pin, out_ptr);
    }
  }
  switch (pin->class)
  {
  case CLASS_CA:
    if (pin->pointer)
    {
      status = get_data(omits, (mdsdsc_t *)pin->pointer, out_ptr, TDITHREADSTATIC_VAR);
      /* Why is this needed????????????? */
      RETURN_IF_STATUS_NOT_OK;
      return TdiImpose((mdsdsc_a_t *)pin, out_ptr);
      /***********************************/
    }
    return MdsCopyDxXd((mdsdsc_t *)pin, out_ptr);
  case CLASS_APD:
    return TdiEvaluate(pin, out_ptr MDS_END_ARG);
  case CLASS_S:
  case CLASS_D:
  case CLASS_A:
    /***********************
     Evaluate on these types.
     ***********************/
    switch (dtype)
    {
    case DTYPE_IDENT:
      status = tdi_get_ident(pin, out_ptr);
      RETURN_IF_STATUS_NOT_OK;
      return get_data(omits, (mdsdsc_t *)out_ptr, out_ptr, TDITHREADSTATIC_VAR);
    case DTYPE_NID:
      pnid = (int *)pin->pointer;
      status = TdiGetRecord(*pnid, out_ptr);
      RETURN_IF_STATUS_NOT_OK;
      return get_data(omits, (mdsdsc_t *)out_ptr, out_ptr, TDITHREADSTATIC_VAR);
    case DTYPE_PATH:
    {
      char *path = MdsDescrToCstring((mdsdsc_t *)pin);
      status = TreeFindNode(path, &nid);
      MdsFree(path);
      RETURN_IF_STATUS_NOT_OK;
      status = TdiGetRecord(nid, out_ptr);
      RETURN_IF_STATUS_NOT_OK;
      return get_data(omits, (mdsdsc_t *)out_ptr, out_ptr, TDITHREADSTATIC_VAR);
    }
    default:
      /**************************************
       VMS types come here. Renames their XD.
       *************************************/
      if (their_ptr->class == CLASS_XD)
      {
        *out_ptr = *(mdsdsc_xd_t *)their_ptr;
        *(mdsdsc_xd_t *)their_ptr = EMPTY_XD;
        return status;
      }
      else
        return MdsCopyDxXd((mdsdsc_t *)pin, out_ptr);
    }
    break;
  case CLASS_R:
    switch (dtype)
    {
    case DTYPE_FUNCTION:
      status = TdiIntrinsic(*(opcode_t *)pin->pointer, pin->ndesc,
                            pin->dscptrs, out_ptr);
      RETURN_IF_STATUS_NOT_OK;
      return get_data(omits, (mdsdsc_t *)out_ptr, out_ptr, TDITHREADSTATIC_VAR);
    case DTYPE_CALL:
      status = TdiCall(pin->length ? *(dtype_t *)pin->pointer : DTYPE_L,
                       pin->ndesc, pin->dscptrs, out_ptr);
      RETURN_IF_STATUS_NOT_OK;
      return get_data(omits, (mdsdsc_t *)out_ptr, out_ptr, TDITHREADSTATIC_VAR);
    case DTYPE_PARAM:
      keep = (mds_signal_t *)TDI_SELF_PTR;
      TDI_SELF_PTR = (mdsdsc_xd_t *)pin;
      status = get_data(omits, (mdsdsc_t *)((mds_param_t *)pin)->value,
                        out_ptr, TDITHREADSTATIC_VAR);
      TDI_SELF_PTR = (mdsdsc_xd_t *)keep;
      return status;
    case DTYPE_SIGNAL:
      /******************************************
       We must set up for reference to our $VALUE.
       ******************************************/
      keep = (mds_signal_t *)TDI_SELF_PTR;
      TDI_SELF_PTR = (mdsdsc_xd_t *)pin;
      status = get_data(omits, (mdsdsc_t *)((mds_signal_t *)pin)->data,
                        out_ptr, TDITHREADSTATIC_VAR);
      TDI_SELF_PTR = (mdsdsc_xd_t *)keep;
      return status;
    case DTYPE_SLOPE:
    {
      /***************
       Windowless axis.
      ***************/
      int seg;
      EMPTYXD(times);
      for (seg = 0; (seg < pin->ndesc / 3); seg++)
      {
        mdsdsc_t *args[] = {*(pin->dscptrs + (seg * 3 + 1)),
                            *(pin->dscptrs + (seg * 3 + 2)),
                            *(pin->dscptrs + (seg * 3))};
        status = TdiIntrinsic(OPC_DTYPE_RANGE, 3, &args, &times);
        RETURN_IF_STATUS_NOT_OK;
        args[0] = (mdsdsc_t *)out_ptr;
        args[1] = (mdsdsc_t *)&times;
        status = TdiIntrinsic(OPC_VECTOR, 2, &args, out_ptr);
        RETURN_IF_STATUS_NOT_OK;
      }
      return get_data(omits, (mdsdsc_t *)out_ptr, out_ptr, TDITHREADSTATIC_VAR);
    }
    case DTYPE_DIMENSION:
      status = TdiItoX(pin, out_ptr MDS_END_ARG);
      RETURN_IF_STATUS_NOT_OK;
      return get_data(omits, (mdsdsc_t *)out_ptr, out_ptr, TDITHREADSTATIC_VAR);
    case DTYPE_RANGE:
      /**************************
       Range can have 2 or 3 args.
       **************************/
      status = TdiIntrinsic(OPC_DTYPE_RANGE, pin->ndesc, &pin->dscptrs[0],
                            out_ptr);
      RETURN_IF_STATUS_NOT_OK;
      return get_data(omits, (mdsdsc_t *)out_ptr, out_ptr, TDITHREADSTATIC_VAR);
    case DTYPE_WITH_UNITS:
      return get_data(omits, (mdsdsc_t *)((mds_with_units_t *)pin)->data,
                      out_ptr, TDITHREADSTATIC_VAR);
    case DTYPE_WITH_ERROR:
      return get_data(omits, (mdsdsc_t *)((mds_with_error_t *)pin)->data,
                      out_ptr, TDITHREADSTATIC_VAR);
    case DTYPE_OPAQUE:
      return get_data(omits, (mdsdsc_t *)((mds_opaque_t *)pin)->data,
                      out_ptr, TDITHREADSTATIC_VAR);
    default:
      return TdiINVCLADTY;
    }
    break;
  default:
    return TdiINVCLADSC;
  }
  return status;
}

static int get_data(const dtype_t omits[], mdsdsc_t *their_ptr,
                    mdsdsc_xd_t *out_ptr, TDITHREADSTATIC_ARG)
{
  int status;
  get_data_cleanup_t c = {out_ptr, MDSDSC_XD_INITIALIZER};
  pthread_cleanup_push(get_data_cleanup, (void *)&c);
  status = _get_data(omits, their_ptr, &c.hold, TDITHREADSTATIC_VAR);
  MdsFree1Dx(c.out, NULL);
  if (STATUS_OK)
    *out_ptr = c.hold;
  else
    MdsFree1Dx(&c.hold, NULL);
  pthread_cleanup_pop(0);
  TDI_GETDATA_REC = 0;
  return status;
}

int tdi_get_data(const dtype_t omits[], mdsdsc_t *their_ptr,
                 mdsdsc_xd_t *out_ptr)
{
  TDITHREADSTATIC_INIT;
  return get_data(omits, their_ptr, out_ptr, TDITHREADSTATIC_VAR);
}

/*----------------------------------------------------------------------------
        Get a float scalar value from an expression or whatever.
        Useful internal and external function.
        C only: status = TdiGetFloat(&in_dsc, &float)
*/
extern EXPORT int TdiGetFloat(mdsdsc_t *in_ptr, float *val_ptr)
{
  INIT_STATUS;

  if (in_ptr == 0)
    *val_ptr = (float)0.0;
  else if (in_ptr->dtype < 160 && in_ptr->dtype != DTYPE_DSC)
    switch (in_ptr->class)
    {
    case CLASS_A:
      if (((mdsdsc_a_t *)in_ptr)->arsize != in_ptr->length)
        return TdiNOT_NUMBER;
      /*********************
      WARNING falls through.
      *********************/
      MDS_ATTR_FALLTHROUGH
    case CLASS_S:
    case CLASS_D:
      switch (in_ptr->dtype)
      {
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
      default:
      {
        mdsdsc_t val_dsc = {sizeof(float), DTYPE_NATIVE_FLOAT, CLASS_S, 0};
        val_dsc.pointer = (char *)val_ptr;
        status = TdiConvert(in_ptr, &val_dsc MDS_END_ARG);
      }
      break;
      }
      break;
    default:
      status = TdiINVCLADSC;
      break;
    }
  else
  {
    mdsdsc_xd_t tmp = EMPTY_XD;
    status = TdiData(in_ptr, &tmp MDS_END_ARG);
    if (STATUS_OK)
      switch (tmp.pointer->class)
      {
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
extern EXPORT int TdiGetLong(mdsdsc_t *in_ptr, int *val_ptr)
{
  INIT_STATUS;

  if (in_ptr == 0)
    *val_ptr = 0;
  else if (in_ptr->dtype < 160 && in_ptr->dtype != DTYPE_DSC)
    switch (in_ptr->class)
    {
    case CLASS_A:
      if (((mdsdsc_a_t *)in_ptr)->arsize != in_ptr->length)
        return TdiNOT_NUMBER;
      /*********************
      WARNING falls through.
      *********************/
      MDS_ATTR_FALLTHROUGH
    case CLASS_S:
    case CLASS_D:
      switch (in_ptr->dtype)
      {
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
      default:
      {
        mdsdsc_t val_dsc = {sizeof(int), DTYPE_L, CLASS_S, 0};
        val_dsc.pointer = (char *)val_ptr;
        status = TdiConvert(in_ptr, &val_dsc MDS_END_ARG);
      }
      break;
      }
      break;
    default:
      status = TdiINVCLADSC;
      break;
    }
  else
  {
    mdsdsc_xd_t tmp = EMPTY_XD;
    status = TdiData(in_ptr, &tmp MDS_END_ARG);
    if (STATUS_OK)
      switch (tmp.pointer->class)
      {
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
extern EXPORT int TdiGetNid(mdsdsc_t *in_ptr, int *nid_ptr)
{
  mdsdsc_xd_t tmp = EMPTY_XD;
  dtype_t omits[] = {DTYPE_NID, DTYPE_PATH, 0};
  int status = tdi_get_data(omits, in_ptr, &tmp);
  if (STATUS_OK)
  {
    switch (tmp.pointer->dtype)
    {
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
        HELP_OF($THIS) gives help field of a param, for example, in the
   validation. $THIS is the dimension used by ranges in complex signal
   subscripting. WARNING: $THIS is only defined within the subexpressions.
        WARNING: Use of $THIS or $VALUE can be infinitely recursive.
*/
int Tdi1This(opcode_t opcode __attribute__((unused)),
             int narg __attribute__((unused)),
             mdsdsc_t *list[] __attribute__((unused)), mdsdsc_xd_t *out_ptr)
{
  TDITHREADSTATIC_INIT;
  if (TDI_SELF_PTR)
    return MdsCopyDxXd((mdsdsc_t *)(TDI_SELF_PTR), out_ptr);
  return TdiNO_SELF_PTR;
}

/*----------------------------------------------------------------------------
        Give value of RAW_OF(signal) for DATA(signal)
        or VALUE_OF(param) for VALIDATION(param).
        This allows the data field of signals to reference the raw field of a
   signal and the validation field of params to reference the value field of a
   param.
*/
int Tdi1Value(opcode_t opcode __attribute__((unused)),
              int narg __attribute__((unused)),
              mdsdsc_t *list[] __attribute__((unused)), mdsdsc_xd_t *out_ptr)
{
  TDITHREADSTATIC_INIT;
  if (TDI_SELF_PTR)
    switch (TDI_SELF_PTR->dtype)
    {
    case DTYPE_SIGNAL:
      return MdsCopyDxXd(((mds_signal_t *)TDI_SELF_PTR)->raw, out_ptr);
    case DTYPE_PARAM:
      return MdsCopyDxXd(((mds_param_t *)TDI_SELF_PTR)->value, out_ptr);
    default:
      return TdiINVDTYDSC;
    }
  else
    return TdiNO_SELF_PTR;
}

/*----------------------------------------------------------------------------
        Return VMS data portion of any data type in input descriptor.
        The output descriptor must be class XD, it will be and XD-DSC.
        A major entry point for evaluation of the data of expressions.
                status = TdiData(&in, &out MDS_END_ARG)
*/
static const dtype_t no_omits[] = {0};
int Tdi1Data(opcode_t opcode __attribute__((unused)),
             int narg __attribute__((unused)), mdsdsc_t *list[],
             mdsdsc_xd_t *out_ptr)
{
  return tdi_get_data(no_omits, list[0], out_ptr);
}

/*----------------------------------------------------------------------------
        Return units portion of with_units. Does not require complete
   evaluation. UNITS(dimension)        UNITS(axis) UNITS(range or slope)
   combined units of arguments UNITS(window)           UNITS(value_idx0)
                UNITS(with_units)       DATA(units)
                UNITS(other)            UNITS(DATA(other) not stripping above)
                UNITS(other)            " "     (single blank to keep IDL happy)
*/
int Tdi1Units(opcode_t opcode __attribute__((unused)), int narg,
              mdsdsc_t *list[], mdsdsc_xd_t *out_ptr)
{
  mdsdsc_r_t *rptr;
  mdsdsc_xd_t tmp = EMPTY_XD, uni[2];
  struct TdiCatStruct cats[3];
  int j;
  static const DESCRIPTOR(blank_dsc, " ");
  static const dtype_t omits[] = {DTYPE_DIMENSION, DTYPE_RANGE,
                                  DTYPE_SLOPE, DTYPE_WINDOW,
                                  DTYPE_WITH_UNITS, 0};
  int status = tdi_get_data(omits, list[0], &tmp);
  rptr = (mdsdsc_r_t *)tmp.pointer;
  if (STATUS_OK)
    switch (rptr->dtype)
    {
    case DTYPE_DIMENSION:
      status = TdiUnits(((mds_dimension_t *)rptr)->axis, out_ptr MDS_END_ARG);
      break;
    case DTYPE_RANGE:
    case DTYPE_SLOPE:
      uni[1] = uni[0] = EMPTY_XD;
      for (j = rptr->ndesc; --j >= 0 && STATUS_OK;)
      {
        if (rptr->dscptrs[j])
          status = TdiUnits(rptr->dscptrs[j], &uni[1] MDS_END_ARG);
        if (uni[1].pointer && STATUS_OK)
          status = Tdi2Add(narg, uni, 0, cats, 0);
      }
      MdsFree1Dx(&uni[1], NULL);
      if (uni[0].pointer)
      {
        MdsFree1Dx(out_ptr, NULL);
        *out_ptr = uni[0];
      }
      else if (STATUS_OK)
        status = MdsCopyDxXd((mdsdsc_t *)&blank_dsc, out_ptr);
      break;
    case DTYPE_WINDOW:
      status = TdiUnits(((struct descriptor_window *)rptr)->value_at_idx0,
                        out_ptr MDS_END_ARG);
      break;
    case DTYPE_WITH_UNITS:
      status = TdiData(((struct descriptor_with_units *)rptr)->units,
                       out_ptr MDS_END_ARG);
      break;
    case DTYPE_WITH_ERROR:
      status = TdiData(((mds_with_error_t *)rptr)->data, out_ptr MDS_END_ARG);
      break;
    default:
      status = MdsCopyDxXd((mdsdsc_t *)&blank_dsc, out_ptr);
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
int Tdi1DataWithUnits(opcode_t opcode __attribute__((unused)),
                      int narg __attribute__((unused)), mdsdsc_t *list[],
                      mdsdsc_xd_t *out_ptr)
{
  static const dtype_t omits[] = {DTYPE_WITH_UNITS, 0};
  DESCRIPTOR_WITH_UNITS(dwu, &missing_dsc, 0);
  mdsdsc_xd_t data = EMPTY_XD, units = EMPTY_XD;
  int status = tdi_get_data(omits, list[0], out_ptr);
  if (STATUS_OK)
  {
    struct descriptor_with_units *pwu =
        (struct descriptor_with_units *)out_ptr->pointer;
    if (pwu)
    {
      if (pwu->dtype == DTYPE_WITH_UNITS)
      {
        status = TdiData(pwu->data, &data MDS_END_ARG);
        if (STATUS_OK)
          status = TdiData(pwu->units, &units MDS_END_ARG);
        dwu.data = data.pointer;
        dwu.units = units.pointer;
      }
      else
      {
        data = *out_ptr;
        *out_ptr = EMPTY_XD;
        dwu.data = (mdsdsc_t *)pwu;
      }
      status = MdsCopyDxXd((mdsdsc_t *)&dwu, out_ptr);
      MdsFree1Dx(&data, NULL);
      MdsFree1Dx(&units, NULL);
    }
    else
      status = MdsCopyDxXd((mdsdsc_t *)&dwu, out_ptr);
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
int Tdi1Validation(opcode_t opcode __attribute__((unused)),
                   int narg __attribute__((unused)), mdsdsc_t *list[],
                   mdsdsc_xd_t *out_ptr)
{
  TDITHREADSTATIC_INIT;
  mdsdsc_r_t *rptr;
  mdsdsc_xd_t *keep;
  static const dtype_t omits[] = {DTYPE_PARAM, 0};

  int status = get_data(omits, list[0], out_ptr, TDITHREADSTATIC_VAR);
  rptr = (mdsdsc_r_t *)out_ptr->pointer;
  if (STATUS_OK)
    switch (rptr->dtype)
    {
    case DTYPE_PARAM:
      /******************************************
     We must set up for reference to our $VALUE.
     ******************************************/
      keep = TDI_SELF_PTR;
      TDI_SELF_PTR = (mdsdsc_xd_t *)rptr;
      status = get_data(no_omits, ((mds_param_t *)rptr)->validation, out_ptr,
                        TDITHREADSTATIC_VAR);
      TDI_SELF_PTR = keep;
      break;
    default:
      status = TdiINVDTYDSC;
      break;
    }
  return status;
}

EXPORT int TdiGetRecord(int nid, mdsdsc_xd_t *out)
{
  INIT_STATUS;
  status = TreeGetRecord(nid, (mdsdsc_xd_t *)out);
  if (STATUS_OK)
    fixup_dollar_nodes(nid, (mdsdsc_t *)out);
  return status;
}
