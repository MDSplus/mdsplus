/*	TdiGetData
	Evaluate and get data.
	For use by TDI$$GET_ARG, Tdi1Data, ... .
	Generally we have expressions including conversion or scaling.
		DATA(dimension)		list of axis points
		DATA(slope)		list of axis points without window
		DATA(signal)		data scalar or array
		DATA(param)		value scalar or array
		DATA(range)		list of numbers
		DATA(VMS)		scalar or array
		DATA(with_units)	data scalar or array

	status = TdiGetData(&omissions, &in, &dat)
	omissions: use dtype code to omit resolving that code, list permitted.
	Use this with DTYPE_SIGNAL to get signal, unresolved.
	Use this to get RANGE or data or for WITH_UNITS or data.
	Do not use DATA(VALUE_OF(param)), use DATA(param).
	Do not use DATA(VALIDATION_OF(param)), use VALIDATION(param).
	They won't work if the _OF routine returns a function because $VALUE is not defined.

	Ken Klare, LANL P-4	(c)1989,1990,1991,1992,1994
	NEED to think, should "TdiImpose" convert data type?
	ASSUMES VECTOR works for any size.
*/
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include <tdimessages.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <string.h>

#define _MOVC3(a,b,c) memcpy(c,b,a)

struct descriptor *TdiSELF_PTR = 0;

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
extern int MdsCopyDxXd();
extern int MdsFree1Dx();

int TdiImpose();

int				TdiImpose(
struct descriptor_a		*in_ptr,
struct descriptor_xd	*out_ptr)
{
array_bounds *pout = (array_bounds *)out_ptr->pointer;
array_bounds arr;
int	in_size, out_size, dimct, status = 1;

	if (pout == 0) return 1;
	switch (pout->class) {
	case CLASS_A :
	case CLASS_CA :
		dimct = in_ptr->dimct;
		if (dimct > MAXDIM) return TdiNDIM_OVER;
		in_size = sizeof(struct descriptor_a)
			+ ((in_ptr->aflags.coeff ? 1 + dimct : 0)
			+ (in_ptr->aflags.bounds ? dimct : 0)) * sizeof(int);
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
			if (in_ptr->class == CLASS_CA) arr.a0 += (int)pout->pointer;
			else arr.a0 = pout->pointer + (arr.a0 - arr.pointer);
		}
		arr.pointer = pout->pointer;
		out_size = sizeof(struct descriptor_a)
			+ ((pout->aflags.coeff ? 1 + dimct : 0)
			+ (pout->aflags.bounds ? dimct : 0)) * sizeof(int);
		if (in_size <= out_size && pout->class == CLASS_A)
			_MOVC3((short)in_size, (char *)&arr, (char *)pout);
		else {
		struct descriptor_xd tmp = *out_ptr;
			*out_ptr = EMPTY_XD;
			status = MdsCopyDxXd((struct descriptor *)&arr, out_ptr);
			MdsFree1Dx(&tmp,NULL);
		}
		break;
	case CLASS_S :
	case CLASS_D :
		if (in_ptr->class == CLASS_CA) {
			DESCRIPTOR_A(simple, 0, 0, 0, 0);
	                simple.length = in_ptr->length;
	                simple.dtype = in_ptr->dtype;
			simple.pointer = in_ptr->pointer;
			simple.arsize = in_ptr->length;
			status = TdiImpose((struct descriptor_a *)&simple, out_ptr);
		}
		else {
			DESCRIPTOR_A(simple, 0, 0, 0, 0);
	                simple.length = pout->length;
	                simple.dtype = pout->dtype;
			simple.pointer = pout->pointer;
			simple.arsize = pout->length;
			status = TdiImpose((struct descriptor_a *)&simple, out_ptr);
		}
		break;
	default :

		status = TdiINVCLADSC;
		break;
	}
	return status;
}
/*----------------------------------------------------------------------------
*/
static struct descriptor missing_dsc = {0,DTYPE_MISSING,CLASS_S,0};
int				TdiGetData(
unsigned char			omits[],
struct descriptor		*their_ptr,
struct descriptor_xd	*out_ptr)
{
int				nid, *pnid;
unsigned char			*optr, dtype=0;
struct descriptor_signal	*keep;
struct descriptor_xd	hold = EMPTY_XD;
struct descriptor_r		*pin = (struct descriptor_r *)their_ptr;
int				status = 1;
static unsigned char OMIT_WINDOW[] = {DTYPE_WINDOW,0};
static unsigned char OMIT_AXIS[] = {DTYPE_SLOPE,0};
static recursion_count = 0;

	if (++recursion_count > 1800) return TdiRECURSIVE;
	while (pin && (dtype = pin->dtype) == DTYPE_DSC) {
		pin = (struct descriptor_r *)pin->pointer;
		if (++recursion_count > 1800) return TdiRECURSIVE;
	}
 if (!pin) status = MdsCopyDxXd(&missing_dsc, &hold);
 else {
	for (optr = &omits[0]; *optr != 0; ++optr) if (*optr == dtype) break;
	if (*optr) status = MdsCopyDxXd((struct descriptor *)pin, &hold);
	else switch (pin->class) {
	case CLASS_CA :
		if (pin->pointer) {
			status = TdiGetData(omits, (struct descriptor *)pin->pointer, &hold);
/********************* Why is this needed????????????? *************************************/
			if (status & 1) status = TdiImpose((struct descriptor_a *)pin, &hold);
/***************************************************************************************/
		}
		else status = MdsCopyDxXd((struct descriptor *)pin, &hold);
		break;
	case CLASS_APD :
		status = TdiEvaluate(pin, &hold MDS_END_ARG);
		break;
	case CLASS_S :
	case CLASS_D :
	case CLASS_A :
		switch (dtype) {
		/***********************
		Evaluate on these types.
		***********************/
		case DTYPE_IDENT :
			status = TdiGetIdent(pin, &hold);
redo:			if (status & 1) status = TdiGetData(omits, (struct descriptor *)&hold, &hold);
			break;
		case DTYPE_NID :
			pnid = (int *)pin->pointer;
			status = TreeGetRecord(*pnid, &hold);
			goto redo;
		case DTYPE_PATH :
			{
				char *path = MdsDescrToCstring((struct descriptor *)pin);
				status = TreeFindNode(path, &nid);
				MdsFree(path);
				if (status & 1) status = TreeGetRecord(nid, &hold);
			}
			goto redo;
		/*******************
		VMS types come here.
		Renames their XD.
		*******************/
		default :
			if (their_ptr->class == CLASS_XD) {
				hold = *(struct descriptor_xd *)their_ptr;
				*(struct descriptor_xd *)their_ptr = EMPTY_XD;
			}
			else status = MdsCopyDxXd((struct descriptor *)pin, &hold);
			break;
		}
		break;
	case CLASS_R :
		switch (dtype) {
		case DTYPE_FUNCTION :
			status = TdiIntrinsic(*(unsigned short *)pin->pointer,
				pin->ndesc, pin->dscptrs, &hold);
			goto redo;
		case DTYPE_CALL :
			status = TdiCall(pin->length ? *(unsigned char *)pin->pointer : DTYPE_L,
				pin->ndesc, pin->dscptrs, &hold);
			goto redo;
		case DTYPE_PARAM :
			keep = (struct descriptor_signal *)TdiSELF_PTR;
			TdiSELF_PTR = (struct descriptor *)pin;
			status = TdiGetData(omits, 
                              (struct descriptor *)((struct descriptor_param *)pin)->value, &hold);
			TdiSELF_PTR = (struct descriptor *)keep;
			break;
		case DTYPE_SIGNAL :
			/******************************************
			We must set up for reference to our $VALUE.
			******************************************/
			keep = (struct descriptor_signal *)TdiSELF_PTR;
			TdiSELF_PTR = (struct descriptor *)pin;
			status = TdiGetData(omits, 
                              (struct descriptor *)((struct descriptor_signal *)pin)->data, &hold);
			TdiSELF_PTR = (struct descriptor *)keep;
			break;
		/***************
		Windowless axis.
		***************/
		case DTYPE_SLOPE :
		case DTYPE_DIMENSION :
			status = TdiItoX(pin, &hold MDS_END_ARG);
			goto redo;
		/**************************
		Range can have 2 or 3 args.
		**************************/
		case DTYPE_RANGE :
			status = TdiIntrinsic(OpcDtypeRange, pin->ndesc, &pin->dscptrs[0], &hold);
			goto redo;
		case DTYPE_WITH_UNITS :
			status = TdiGetData(omits, 
                              (struct descriptor *)((struct descriptor_with_units *)pin)->data, &hold);
			break;
		case DTYPE_WITH_ERROR :
			status = TdiGetData(omits, 
                              (struct descriptor *)((struct descriptor_with_error *)pin)->data, &hold);
			break;
		default :
			status = TdiINVCLADTY;
			break;
		}
		break;
	default :
		status = TdiINVCLADSC;
		break;
	}
 }
	recursion_count = 0;
	/**********************************
	Watch out for input same as output.
	**********************************/
	MdsFree1Dx(out_ptr,NULL);
	if (status & 1) *out_ptr = hold;
	else	MdsFree1Dx(&hold,NULL);
	return status;
}
/*----------------------------------------------------------------------------
	Get a float scalar value from an expression or whatever.
	Useful internal and external function.
	C only:	status = TdiGetFloat(&in_dsc, &float)
*/
int				TdiGetFloat(
struct descriptor		*in_ptr,
float				*val_ptr)
{
int				status = 1;

	if (in_ptr == 0) *val_ptr = (float)0.0;
	else if (in_ptr->dtype < 160 && in_ptr->dtype != DTYPE_DSC) switch (in_ptr->class) {
	case CLASS_A :
		if (((struct descriptor_a *)in_ptr)->arsize != in_ptr->length) return TdiNOT_NUMBER;
	/*********************
	WARNING falls through.
	*********************/
	case CLASS_S :
	case CLASS_D :
		switch (in_ptr->dtype) {
		case DTYPE_BU :			*val_ptr = (float) *(unsigned char *)	in_ptr->pointer; break;
		case DTYPE_B :			*val_ptr = (float) *(char *)		in_ptr->pointer; break;
		case DTYPE_WU :			*val_ptr = (float) *(unsigned short *)	in_ptr->pointer; break;
		case DTYPE_W :			*val_ptr = (float) *(short *)		in_ptr->pointer; break;
		case DTYPE_LU :			*val_ptr = (float) *(unsigned int *)	in_ptr->pointer; break;
		case DTYPE_L :			*val_ptr = (float) *(int *)		in_ptr->pointer; break;
		default : {struct descriptor val_dsc = {sizeof(float),DTYPE_FLOAT,CLASS_S,0};
                           val_dsc.pointer = (char *)val_ptr;
			   status = TdiConvert(in_ptr, &val_dsc MDS_END_ARG);
			  } break;
		}
		break;
	default : status = TdiINVCLADSC; break;
	}
	else {
	struct descriptor_xd	tmp = EMPTY_XD;
		status = TdiData(in_ptr, &tmp MDS_END_ARG);
		if (status & 1) switch (tmp.pointer->class) {
		case CLASS_S :
		case CLASS_D :
		case CLASS_A :
			status = TdiGetFloat(tmp.pointer, val_ptr);
			break;
		default :
			status = TdiINVCLADSC;
			break;
		}
		MdsFree1Dx(&tmp,NULL);
	}
	return status;
}
/*----------------------------------------------------------------------------
	Get a long scalar value from an expression or whatever.
	Useful internal and external function.
	C only:	status = TdiGetLong(&in_dsc, &long)
*/
int				TdiGetLong(
struct descriptor		*in_ptr,
int				*val_ptr)
{
int				status = 1;

	if (in_ptr == 0) *val_ptr = 0;
	else if (in_ptr->dtype < 160 && in_ptr->dtype != DTYPE_DSC) switch (in_ptr->class) {
	case CLASS_A :
		if (((struct descriptor_a *)in_ptr)->arsize != in_ptr->length) return TdiNOT_NUMBER;
	/*********************
	WARNING falls through.
	*********************/
	case CLASS_S :
	case CLASS_D :
		switch (in_ptr->dtype) {
		case DTYPE_BU :			*val_ptr = (int) *(unsigned char *)	in_ptr->pointer; break;
		case DTYPE_B :			*val_ptr = (int) *(char *)		in_ptr->pointer; break;
		case DTYPE_WU :			*val_ptr = (int) *(unsigned short *)	in_ptr->pointer; break;
		case DTYPE_W :			*val_ptr = (int) *(short *)		in_ptr->pointer; break;
		case DTYPE_L : case DTYPE_LU : *val_ptr = *(int *)		in_ptr->pointer; break;
		default : {struct descriptor val_dsc = {sizeof(int),DTYPE_L,CLASS_S,0};
			        val_dsc.pointer = (char *)val_ptr;
				status = TdiConvert(in_ptr, &val_dsc MDS_END_ARG);
			}
			break;
		}
		break;
	default : status = TdiINVCLADSC; break;
	}
	else {
	struct descriptor_xd	tmp = EMPTY_XD;
		status = TdiData(in_ptr, &tmp MDS_END_ARG);
		if (status & 1) switch (tmp.pointer->class) {
		case CLASS_S :
		case CLASS_D :
		case CLASS_A :
			status = TdiGetLong(tmp.pointer, val_ptr);
			break;
		default :
			status = TdiINVCLADSC;
			break;
		}
		MdsFree1Dx(&tmp,NULL);
	}
	return status;
}
/*----------------------------------------------------------------------------
	Get the Node ID for NID, PATH, or TEXT types including expressions.
	Useful internal and external function.
	C only:	status = TdiGetNid(&in_dsc, &nid)
*/
int			TdiGetNid(
struct descriptor	*in_ptr,
int		*nid_ptr)
{
int	status = 1;
struct descriptor_xd tmp = EMPTY_XD;
unsigned char	omits[] = {
	DTYPE_NID,
	DTYPE_PATH,
	0};

	status = TdiGetData(omits, in_ptr, &tmp);
	if (status & 1) switch (tmp.pointer->dtype) {
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
	default :
		status = TdiINVDTYDSC;
		break;
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
TdiRefStandard(Tdi1This)
	if (TdiSELF_PTR) status = MdsCopyDxXd(TdiSELF_PTR, out_ptr);
	else status = TdiNO_SELF_PTR;
	return status;
}
/*----------------------------------------------------------------------------
	Give value of RAW_OF(signal) for DATA(signal)
	or VALUE_OF(param) for VALIDATION(param).
	This allows the data field of signals to reference the raw field of a signal
	and the validation field of params to reference the value field of a param.
*/
TdiRefStandard(Tdi1Value)

	if (TdiSELF_PTR) switch (TdiSELF_PTR->dtype) {
	case DTYPE_SIGNAL :
		status = MdsCopyDxXd(((struct descriptor_signal *)TdiSELF_PTR)->raw, out_ptr);
		break;
	case DTYPE_PARAM :
		status = MdsCopyDxXd(((struct descriptor_param *)TdiSELF_PTR)->value, out_ptr);
		break;
	default :
		status = TdiINVDTYDSC;
		break;
	}
	else status = TdiNO_SELF_PTR;
	return status;
}
/*----------------------------------------------------------------------------
	Return VMS data portion of any data type in input descriptor.
	The output descriptor must be class XD, it will be and XD-DSC.
	A major entry point for evaluation of the data of expressions.
		status = TdiData(&in, &out MDS_END_ARG)
*/
TdiRefStandard(Tdi1Data)

        unsigned char omits[] = {0};
	status = TdiGetData(omits, list[0], out_ptr);
	return status;
}
/*----------------------------------------------------------------------------
	Return units portion of with_units. Does not require complete evaluation.
		UNITS(dimension)	UNITS(axis)
		UNITS(range or slope)	combined units of arguments
		UNITS(window)		UNITS(value_idx0)
		UNITS(with_units)	DATA(units)
		UNITS(other)		UNITS(DATA(other) not stripping above)
		UNITS(other)		" "	(single blank to keep IDL happy)
*/
TdiRefStandard(Tdi1Units)
struct descriptor_r		*rptr;
struct descriptor_xd	tmp = EMPTY_XD, uni[2];
struct TdiCatStruct		cats[3];
int				j;
static DESCRIPTOR(blank_dsc, " ");
static unsigned char omits[] = {
	DTYPE_DIMENSION,
	DTYPE_RANGE,
	DTYPE_SLOPE,
	DTYPE_WINDOW,
	DTYPE_WITH_UNITS,
	0};

	status = TdiGetData((unsigned char *)omits, list[0], &tmp);
	rptr = (struct descriptor_r *)tmp.pointer;
	if (status & 1) switch (rptr->dtype) {
	case DTYPE_DIMENSION :
		status = TdiUnits(((struct descriptor_dimension *)rptr)->axis, out_ptr MDS_END_ARG);
		break;
	case DTYPE_RANGE :
	case DTYPE_SLOPE :
		uni[1] = uni[0] = EMPTY_XD;
		for (j = rptr->ndesc; --j >= 0 && status & 1;) {
			if (rptr->dscptrs[j]) status = TdiUnits(rptr->dscptrs[j], &uni[1] MDS_END_ARG);
			if (uni[1].pointer && status & 1) status = Tdi2Add(narg, uni, 0, cats, 0);
		}
		MdsFree1Dx(&uni[1],NULL);
		if (uni[0].pointer) {
			MdsFree1Dx(out_ptr,NULL);
			*out_ptr = uni[0];
		}
		else if (status & 1) status = MdsCopyDxXd((struct descriptor *)&blank_dsc, out_ptr);
		break;
	case DTYPE_WINDOW :
		status = TdiUnits(((struct descriptor_window *)rptr)->value_at_idx0, out_ptr MDS_END_ARG);
		break;
	case DTYPE_WITH_UNITS :
		status = TdiData(((struct descriptor_with_units *)rptr)->units, out_ptr MDS_END_ARG);
		break;
	case DTYPE_WITH_ERROR :
		status = TdiData(((struct descriptor_with_error *)rptr)->data, out_ptr MDS_END_ARG);
		break;
	default :
		status = MdsCopyDxXd((struct descriptor *)&blank_dsc, out_ptr);
		break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*----------------------------------------------------------------------------
	Return VMS data portion and the units of the input descriptor.
	Always returns a with_units descriptor with data, assuming no error.
	Caution. The units field may be null.
		status = TdiDataWithUnits(&in, &out MDS_END_ARG)
*/
static DESCRIPTOR_WITH_UNITS(null,0,0);
TdiRefStandard(Tdi1DataWithUnits)
static unsigned char omits[] = {DTYPE_WITH_UNITS,0};
struct descriptor_with_units dwu = null;
struct descriptor_xd data = EMPTY_XD, units = EMPTY_XD;
        dwu.data = null.data = (struct descriptor *)&missing_dsc;
	status = TdiGetData((unsigned char *)omits, list[0], out_ptr);
	if (status & 1) {
	struct descriptor_with_units *pwu = (struct descriptor_with_units *)out_ptr->pointer;
		if (pwu) {
			if (pwu->dtype == DTYPE_WITH_UNITS) {
				status = TdiData(pwu->data, &data MDS_END_ARG);
				if (status & 1) status = TdiData(pwu->units, &units MDS_END_ARG);
				dwu.data = data.pointer;
				dwu.units = units.pointer;
			}
			else {
				data = *out_ptr;
				*out_ptr = EMPTY_XD;
				dwu.data = (struct descriptor *)pwu;
			}
			status = MdsCopyDxXd((struct descriptor *)&dwu, out_ptr);
			MdsFree1Dx(&data,NULL);
			MdsFree1Dx(&units,NULL);
		}
		else status = MdsCopyDxXd((struct descriptor *)&null, out_ptr);
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
TdiRefStandard(Tdi1Validation)
struct descriptor_r		*rptr;
struct descriptor		*keep;
static unsigned char omits[] = {DTYPE_PARAM,0};
static unsigned char noomits[] = {0};

	status = TdiGetData((unsigned char *)omits, list[0], out_ptr);
	rptr = (struct descriptor_r *)out_ptr->pointer;
	if (status & 1) switch (rptr->dtype) {
	case DTYPE_PARAM :
		/******************************************
		We must set up for reference to our $VALUE.
		******************************************/
		keep = TdiSELF_PTR;
		TdiSELF_PTR = (struct descriptor *)rptr;
		status = TdiGetData(noomits, ((struct descriptor_param *)rptr)->validation, out_ptr);
		TdiSELF_PTR = keep;
		break;
	default : status = TdiINVDTYDSC; break;
	}
	return status;
}
