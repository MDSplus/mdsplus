/*	Tdi1xxx_OF.C
	KIND and all xxx_OF routines in one module.

	Ken Klare, LANL P-4	(c)1989,1990,1991,1992
*/
#include <tdimessages.h>
#include "tdirefstandard.h"
#include <stdlib.h>
#include <mdsshr.h>
extern int MdsFree1Dx();
extern int MdsCopyDxXd();
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
TdiRefStandard(Tdi1ArgOf)
struct descriptor_xd	tmp = EMPTY_XD;
unsigned int			iarg = 0;
static unsigned char omits[] = {
	DTYPE_CALL,
	DTYPE_CONDITION,
	DTYPE_DEPENDENCY,
	DTYPE_FUNCTION,
	DTYPE_METHOD,
	DTYPE_PROCEDURE,
	DTYPE_ROUTINE,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1 && narg > 1) status=TdiGetLong(list[1], &iarg);
	if (status&1 && tmp.pointer->class != CLASS_R) status=TdiINVCLADSC;
	if (status&1 && iarg >= ((struct descriptor_r *)tmp.pointer)->ndesc) status=TdiBAD_INDEX;
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_CALL :
		if (iarg+2 >= ((struct descriptor_r *)tmp.pointer)->ndesc) status=TdiBAD_INDEX;
		else status=MdsCopyDxXd(((struct descriptor_call *)tmp.pointer)->arguments[iarg], out_ptr);
		break;
	case DTYPE_CONDITION :
		status=MdsCopyDxXd(((struct descriptor_condition *)tmp.pointer)->condition, out_ptr);
		break;
	case DTYPE_DEPENDENCY :
		status=MdsCopyDxXd(((struct descriptor_dependency *)tmp.pointer)->arguments[iarg], out_ptr);
		break;
	case DTYPE_FUNCTION :
		status=MdsCopyDxXd(((struct descriptor_function *)tmp.pointer)->arguments[iarg], out_ptr);
		break;
	case DTYPE_METHOD :
		status=MdsCopyDxXd(((struct descriptor_method *)tmp.pointer)->arguments[iarg], out_ptr);
		break;
	case DTYPE_PROCEDURE :
		status=MdsCopyDxXd(((struct descriptor_procedure *)tmp.pointer)->arguments[iarg], out_ptr);
		break;
	case DTYPE_ROUTINE :
		status=MdsCopyDxXd(((struct descriptor_routine *)tmp.pointer)->arguments[iarg], out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
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
TdiRefStandard(Tdi1AxisOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_DIMENSION,
	DTYPE_SLOPE,
	DTYPE_RANGE,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_DIMENSION :
		status=MdsCopyDxXd(((struct descriptor_dimension *)tmp.pointer)->axis, out_ptr);
		break;
	case DTYPE_SLOPE :
	case DTYPE_RANGE :
		MdsFree1Dx(out_ptr, NULL);
		*out_ptr = tmp;
		return status;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return start point information.
		begin_field = BEGIN_OF(range)
		begin_field = BEGIN_OF(slope, [n]) !deprecated!
		startidx_field = BEGIN_OF(window)
*/
TdiRefStandard(Tdi1BeginOf)
struct descriptor_xd	tmp = EMPTY_XD;
unsigned int			n = 0;
static unsigned char omits[] = {
	DTYPE_RANGE,
	DTYPE_SLOPE,
	DTYPE_WINDOW,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_RANGE :
		if (narg > 1) status=TdiEXTRA_ARG;
		else status=MdsCopyDxXd(((struct descriptor_range *)tmp.pointer)->begin, out_ptr);
		break;
	case DTYPE_SLOPE :
		if (narg > 1) status=TdiGetLong(list[1], &n);
		if (status&1 && 3*n+2 > ((struct descriptor_slope *)tmp.pointer)->ndesc) status=TdiBAD_INDEX;
		if (status&1) status=MdsCopyDxXd(
			((struct descriptor_slope *)tmp.pointer)->segment[n].begin, out_ptr);
		break;
	case DTYPE_WINDOW :
		if (narg > 1) status=TdiEXTRA_ARG;
		else status=MdsCopyDxXd(((struct descriptor_window *)tmp.pointer)->startidx, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return the class of its argument, but not of the XD.
		byte = CLASS_OF(any)
*/
TdiRefStandard(Tdi1ClassOf)
struct descriptor	*px = list[0];
unsigned char	class;

	while (px && px->dtype == DTYPE_DSC) px = (struct descriptor *)px->pointer;
	if (px) class = px->class;
	else class = DTYPE_MISSING;
	status = TdiPutLogical(class, out_ptr);
	return status;
}
/*--------------------------------------------------------------
	Evaluate and then return the class of an argument.
		byte = CLASS(any)
*/
TdiRefStandard(Tdi1Class)

	status = TdiEvaluate(list[0], out_ptr MDS_END_ARG);
	if (status & 1) status = Tdi1ClassOf(opcode, narg, &out_ptr->pointer, out_ptr);
	return status;
}
/*--------------------------------------------------------------
	Return completion in dispatch information.
		completion = COMPLETION_OF(dispatch)
*/
TdiRefStandard(Tdi1CompletionOf)
struct descriptor_xd	tmp = EMPTY_XD;

	status=TdiDispatchOf(list[0], &tmp MDS_END_ARG);
	if (status&1) status=MdsCopyDxXd(((struct descriptor_dispatch *)tmp.pointer)->completion, out_ptr);
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return completion message in action information.
		completion_message = COMPLETION_MESSAGE_OF(action)
*/
TdiRefStandard(Tdi1CompletionMessageOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_ACTION,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_ACTION :
		status=MdsCopyDxXd(((struct descriptor_action *)tmp.pointer)->completion_message, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return record information.
		condition = CONDITION_OF(condition)
*/
TdiRefStandard(Tdi1ConditionOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_CONDITION,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_CONDITION :
		status=MdsCopyDxXd(((struct descriptor_condition *)tmp.pointer)->condition, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return a dimension of a signal or array.
	Dimension number is 1 to MAXDIM, default is first.
		subscript_range = DIM_OF(array, [dim_num])
		dimension_field = DIM_OF(signal, [dim_num])
		same = DIM_OF(dimension)
*/
TdiRefStandard(Tdi1DimOf)
array_bounds                    *pa;
struct descriptor_xd	tmp = EMPTY_XD;
int				dimct;
int				l, u;
int			index = 0;
static unsigned char omits[] = {
	DTYPE_SIGNAL,
	DTYPE_DIMENSION,
	0};

	/**************************************
	For array of signals, give array range.
	**************************************/
	status=TdiGetData(omits, list[0], &tmp);
	if (status&1 && narg > 1) status=TdiGetLong(list[1], &index);

	pa = (array_bounds *)tmp.pointer;
	if (status&1) switch (pa->class) {
	case CLASS_A :
		dimct = pa->aflags.coeff ? pa->dimct : 1;
		if (index >= dimct) status=TdiBAD_INDEX;
		/*******************************************
		After arsize is a0, multipliers, and bounds.
		NEED we reverse for non-FORTRAN?
		*******************************************/
		else if (pa->aflags.bounds) {
			l = pa->m[dimct + 2*index];
			u = pa->m[dimct + 2*index + 1];
		}
		else if (pa->aflags.coeff) {
			l = 0;
			u = pa->m[index] - 1;
		}
		else {
			l = 0;
			u = (int)pa->arsize / (int)pa->length - 1;
		}
		if (status&1) {
		struct descriptor	ldsc = {sizeof(int),DTYPE_L,CLASS_S,0};
		struct descriptor	udsc = {sizeof(int),DTYPE_L,CLASS_S,0};
                        ldsc.pointer = (char *)&l;
                        udsc.pointer = (char *)&u;
			status=TdiBuildRange(&ldsc, &udsc, out_ptr MDS_END_ARG);
		}
		break;
	case CLASS_R :
		switch (pa->dtype) {
		case DTYPE_DIMENSION :
			if (index > 0) status=TdiBAD_INDEX;
			else status=MdsCopyDxXd((struct descriptor *)&tmp, out_ptr);
			break;
		case DTYPE_SIGNAL :
			if (index >= ((struct descriptor_signal *)pa)->ndesc+2
			|| ((struct descriptor_signal *)pa)->dimensions[index] == 0) {
                                struct descriptor_s index_dsc = {sizeof(int), DTYPE_L, CLASS_S, 0};
                                index_dsc.pointer = (char *)&index;
				status=TdiGetData(&omits[1], &tmp, &tmp);
				if (status&1) status=TdiDimOf(&tmp, &index_dsc, out_ptr MDS_END_ARG);
			}
			else status=MdsCopyDxXd(((struct descriptor_signal *)pa)->dimensions[index], out_ptr);
			break;
		default : status=TdiINVDTYDSC; break;
		}
		break;
	default : status=TdiINVCLADSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return dispatch information.
		dispatch = DISPATCH_OF(action)
		same = DISPATCH_OF(dispatch)
*/
TdiRefStandard(Tdi1DispatchOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_ACTION,
	DTYPE_DISPATCH,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_ACTION :
		status=TdiDispatchOf(((struct descriptor_action *)tmp.pointer)->dispatch, out_ptr MDS_END_ARG);
		break;
	case DTYPE_DISPATCH :
		MdsFree1Dx(out_ptr,NULL);
		*out_ptr = tmp;
		return status;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return one of the class-R descriptor pointers.
		descriptor = DSCPTR_OF(classR, [number])
		descriptor = DSCPTR_OF(classAPD, [number])
*/
TdiRefStandard(Tdi1DscptrOf)
struct descriptor_r	*pr = (struct descriptor_r *)list[0];
unsigned int		iarg = 0;

	while (pr && pr->dtype == DTYPE_DSC) pr = (struct descriptor_r *)pr->pointer;
	if (pr) {
		if (narg > 1) status=TdiGetLong(list[1], &iarg);
		if (status&1) switch (pr->class) {
		case CLASS_R :
			if (iarg >= pr->ndesc) status=TdiBAD_INDEX;
			else status=MdsCopyDxXd(pr->dscptrs[iarg], out_ptr);
			break;
		case CLASS_APD :
			{array_desc *pa = (array_desc *)pr;
				if (iarg >= pa->arsize/pa->length) status=TdiBAD_INDEX;
				else status=MdsCopyDxXd(pa->pointer[iarg], out_ptr);
			}
			break;
		default : status=TdiINVCLADSC; break;
		}
	}
	return status;
}
/*--------------------------------------------------------------
	Evaluate and then return a descriptor pointer of an argument.
		byte = DSCPTR(any)
*/
TdiRefStandard(Tdi1Dscptr)
struct descriptor_xd tmp = EMPTY_XD, *new[2];
        new[0] = &tmp;
        new[1] = narg>1 ? (struct descriptor_xd *)list[1] : (struct descriptor_xd *)0;
	status = TdiEvaluate(list[0], &tmp MDS_END_ARG);
	if (status & 1) status = Tdi1DscptrOf(opcode, narg, (struct descriptor **)new, out_ptr);
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return the data type of its argument, but not of the XD.
		byte = KIND_OF(any)
*/
TdiRefStandard(Tdi1KindOf)
struct descriptor	*px = list[0];
unsigned char	dtype;

	while (px && px->dtype == DTYPE_DSC) px = (struct descriptor *)px->pointer;
	if (px) dtype = px->dtype;
	else dtype = DTYPE_MISSING;
	status = TdiPutLogical(dtype, out_ptr);
	return status;
}
/*--------------------------------------------------------------
	Evaluate and then return the dtype of an argument.
		byte = KIND(any)
*/
TdiRefStandard(Tdi1Kind)

	status = TdiEvaluate(list[0], out_ptr MDS_END_ARG);
	if (status & 1) status = Tdi1KindOf(opcode, narg, &out_ptr->pointer, out_ptr);
	return status;
}
/*--------------------------------------------------------------
	Return end point information.
		end_field = END_OF(&range)
		end_field = END_OF(&slope,[n]) !deprecated!
		endidx_field = END_OF(&window)
*/
TdiRefStandard(Tdi1EndOf)
struct descriptor_xd	tmp = EMPTY_XD;
unsigned int			n = 0;
static unsigned char omits[] = {
	DTYPE_RANGE,
	DTYPE_SLOPE,
	DTYPE_WINDOW,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_RANGE :
		if (narg > 1) status=TdiEXTRA_ARG;
		else status=MdsCopyDxXd(((struct descriptor_range *)tmp.pointer)->ending, out_ptr);
		break;
	case DTYPE_SLOPE :
		if (narg > 1) status=TdiGetLong(list[1], &n);
		if (status&1 && 3*n+3>((struct descriptor_slope *)tmp.pointer)->ndesc) status=TdiBAD_INDEX;
		if (status&1) status=MdsCopyDxXd(
			((struct descriptor_slope *)tmp.pointer)->segment[n].ending, out_ptr);
		break;
	case DTYPE_WINDOW :
		if (narg > 1) status=TdiEXTRA_ARG;
		else status=MdsCopyDxXd(((struct descriptor_window *)tmp.pointer)->endingidx, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return error bar associated with value.
		error = ERROR_OF(with_error)
*/
TdiRefStandard(Tdi1ErrorOf)
static DESCRIPTOR(none, "");
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_WITH_ERROR,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_WITH_ERROR :
		status=MdsCopyDxXd(((struct descriptor_with_error *)tmp.pointer)->error, out_ptr);
		break;
	default :
		status=MdsCopyDxXd((struct descriptor *)&none, out_ptr);
		break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return errorlog in action information.
		errorlogs_field = ERRORLOGS_OF(action)
*/
TdiRefStandard(Tdi1ErrorlogsOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_ACTION,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_ACTION :
                {
                   struct descriptor_action *act = (struct descriptor_action *)tmp.pointer;
                   if ((act->ndesc >= 3) &&  act->errorlogs)
                     status=MdsCopyDxXd(act->errorlogs, out_ptr);
                   else
                     status=TdiINVDTYDSC;
                }
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return help portion of a parameter.
		help_field = HELP_OF(param)
*/
TdiRefStandard(Tdi1HelpOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_PARAM,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_PARAM :
		status=MdsCopyDxXd(((struct descriptor_param *)tmp.pointer)->help, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return dispatch identification information.
		ident_field = IDENT_OF(dispatch)
*/
TdiRefStandard(Tdi1IdentOf)
struct descriptor_xd	tmp = EMPTY_XD;

	status=TdiDispatchOf(list[0], &tmp MDS_END_ARG);
	if (status&1) status=MdsCopyDxXd(((struct descriptor_dispatch *)tmp.pointer)->ident, out_ptr);
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return image (.exe filename) portion of a record.
		text = IMAGE_OF(call)
		text = IMAGE_OF(conglom)
		text = IMAGE_OF(routine)
*/
TdiRefStandard(Tdi1ImageOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_CALL,
	DTYPE_CONGLOM,
	DTYPE_ROUTINE,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_CALL :
		status=MdsCopyDxXd(((struct descriptor_call *)tmp.pointer)->image, out_ptr);
		break;
	case DTYPE_CONGLOM :
		status=MdsCopyDxXd(((struct descriptor_conglom *)tmp.pointer)->image, out_ptr);
		break;
	case DTYPE_ROUTINE :
		status=MdsCopyDxXd(((struct descriptor_routine *)tmp.pointer)->image, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return interrupt in dispatch information, only allowed for asynchronous.
		when = INTERRUPT_OF(action or from dispatch)
*/
TdiRefStandard(Tdi1InterruptOf)
struct descriptor_xd	tmp = EMPTY_XD;
struct descriptor_dispatch	*pd;
char	omits[] = {
	DTYPE_T,
	0};

	status=TdiDispatchOf(list[0], &tmp MDS_END_ARG);
	if (status&1) {
		pd = (struct descriptor_dispatch *)tmp.pointer;
		if (*(char *)pd->pointer != TreeSCHED_ASYNC) {
			status=TdiGetData(omits, pd->when, out_ptr);
			if (status&1) {
				if (out_ptr->pointer == 0
				|| out_ptr->pointer->dtype != DTYPE_T) status = TdiINVDTYDSC;
			}
		}
		else	status = TdiINV_OPC;
	}
	else	status=TdiINVDTYDSC;
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return language used by procedure.
		language_field = LANGUAGE_OF(procedure)
*/
TdiRefStandard(Tdi1LanguageOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_PROCEDURE,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_PROCEDURE :
		status=MdsCopyDxXd(((struct descriptor_procedure *)tmp.pointer)->language, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return method used on object.
		method_field = METHOD_OF(method)
*/
TdiRefStandard(Tdi1MethodOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_METHOD,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_METHOD :
		status=MdsCopyDxXd(((struct descriptor_method *)tmp.pointer)->method, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return conglomerate information.
		model_field = MODEL_OF(conglom)
*/
TdiRefStandard(Tdi1ModelOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_CONGLOM,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_CONGLOM :
		status=MdsCopyDxXd(((struct descriptor_conglom *)tmp.pointer)->model, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return conglomerate information.
		name_field = NAME_OF(conglom)
*/
TdiRefStandard(Tdi1NameOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_CONGLOM,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_CONGLOM :
		status=MdsCopyDxXd(((struct descriptor_conglom *)tmp.pointer)->name, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return the number of class-R descriptor pointers.
		byte = NDESC_OF(&classR)
*/
TdiRefStandard(Tdi1NdescOf)
struct descriptor_r		*pr = (struct descriptor_r *)list[0];
int				iarg = 0;
unsigned char	ndesc;

	while (pr && pr->dtype == DTYPE_DSC) pr = (struct descriptor_r *)pr->pointer;
	if (pr) switch (pr->class) {
	case CLASS_R :
		ndesc = pr->ndesc;
		status = TdiPutLogical(ndesc, out_ptr);
		break;
	default : status=TdiINVCLADSC; break;
	}
	else status=TdiNULL_PTR;
	return status;
}
/*--------------------------------------------------------------
	Evaluate and then return the number of descriptors of an argument.
	WARNING, will not work for DTYPE_FUNCTION.
		byte = NDESC(any)
*/
TdiRefStandard(Tdi1Ndesc)

	status = TdiEvaluate(list[0], out_ptr MDS_END_ARG);
	if (status & 1) status = Tdi1NdescOf(opcode, narg, &out_ptr->pointer, out_ptr);
	return status;
}
/*--------------------------------------------------------------
	Return object used by method.
		object_field = OBJECT_OF(method)
*/
TdiRefStandard(Tdi1ObjectOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_METHOD,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_METHOD :
		status=MdsCopyDxXd(((struct descriptor_method *)tmp.pointer)->object, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return performance in action information.
		performance_field = PERFORMANCE_OF(action)
*/
TdiRefStandard(Tdi1PerformanceOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_ACTION,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_ACTION :
		status=MdsCopyDxXd((struct descriptor *)
                             ((struct descriptor_action *)tmp.pointer)->performance, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return phase in dispatch information.
		phase_field = PHASE_OF(dispatch)
*/
TdiRefStandard(Tdi1PhaseOf)
struct descriptor_xd	tmp = EMPTY_XD;

	status=TdiDispatchOf(list[0], &tmp MDS_END_ARG);
	if (status&1) status=MdsCopyDxXd(((struct descriptor_dispatch *)tmp.pointer)->phase, out_ptr);
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return procedure using a language.
		procedure_field = PROCEDURE_OF(procedure)
*/
TdiRefStandard(Tdi1ProcedureOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_PROCEDURE,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_PROCEDURE :
		status=MdsCopyDxXd(((struct descriptor_procedure *)tmp.pointer)->procedure, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return program used by program.
		program_field = PROGRAM_OF(program)
*/
TdiRefStandard(Tdi1ProgramOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_PROGRAM,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_PROGRAM :
		status=MdsCopyDxXd(((struct descriptor_program *)tmp.pointer)->program, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
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
TdiRefStandard(Tdi1QualifiersOf)
struct descriptor_xd	tmp = EMPTY_XD;
struct descriptor		*pd;
static unsigned char omits[] = {
	DTYPE_CALL,
	DTYPE_CONDITION,
	DTYPE_CONGLOM,
	DTYPE_DEPENDENCY,
	DTYPE_DISPATCH,
	DTYPE_FUNCTION,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	pd = tmp.pointer;
	if (status&1) switch (pd->dtype) {
	case DTYPE_CONGLOM :
		status=MdsCopyDxXd(((struct descriptor_conglom *)pd)->qualifiers, out_ptr);
		break;
	case DTYPE_CALL :
	case DTYPE_CONDITION :
	case DTYPE_DEPENDENCY :
	case DTYPE_DISPATCH :
	case DTYPE_FUNCTION :
	{struct descriptor x = {0,0,CLASS_S,0};
                x.length = pd->length;
                x.dtype = (char) (pd->length==1 ? DTYPE_BU : DTYPE_WU);
		x.pointer = pd->pointer;
		status=MdsCopyDxXd(&x, out_ptr);
		break;
	}
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return raw data portion of a SIGNAL.
		raw_field = RAW_OF(signal)
		data = RAW_OF(other)
*/
TdiRefStandard(Tdi1RawOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_SIGNAL,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_SIGNAL :
		status=MdsCopyDxXd(((struct descriptor_signal *)tmp.pointer)->raw, out_ptr);
		break;
	default :
		MdsFree1Dx(out_ptr,NULL);
		*out_ptr = tmp;
		return status;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return routine (entry-point name) portion of a record.
		routine_field = ROUTINE_OF(call)
		routine_field = ROUTINE_OF(routine)
*/
TdiRefStandard(Tdi1RoutineOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_CALL,
	DTYPE_ROUTINE,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_CALL :
		status=MdsCopyDxXd(((struct descriptor_call *)tmp.pointer)->routine, out_ptr);
		break;
	case DTYPE_ROUTINE :
		status=MdsCopyDxXd(((struct descriptor_routine *)tmp.pointer)->routine, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return slope field information.
		step_field = SLOPE_OF(range)
		slope_field = SLOPE_OF(slope,[n]) !deprecated!
*/
TdiRefStandard(Tdi1SlopeOf)
static unsigned char one_val = 1;
static struct descriptor one = {sizeof(unsigned char),DTYPE_BU,CLASS_S,(char *)&one_val};

struct descriptor_xd	tmp = EMPTY_XD;
unsigned int			n = 0;
static unsigned char omits[] = {
	DTYPE_RANGE,
	DTYPE_SLOPE,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1 && narg > 1) status=TdiGetLong(list[1], &n);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_RANGE :
		if (n > 0) status=TdiBAD_INDEX;
		if(((struct descriptor_range *)tmp.pointer)->ndesc < 3) MdsCopyDxXd(&one, out_ptr);
		else MdsCopyDxXd(((struct descriptor_r *)tmp.pointer)->dscptrs[2], out_ptr);
		break;
	case DTYPE_SLOPE :
		if (3*n+1 > ((struct descriptor_r *)tmp.pointer)->ndesc) status=TdiBAD_INDEX;
		if (status&1) status=MdsCopyDxXd(
			((struct descriptor_slope *)tmp.pointer)->segment[n].slope, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
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
TdiRefStandard(Tdi1TaskOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_ACTION,
	DTYPE_METHOD,
	DTYPE_PROCEDURE,
	DTYPE_PROGRAM,
	DTYPE_ROUTINE,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_ACTION :
		status=TdiTaskOf(((struct descriptor_action *)tmp.pointer)->task, out_ptr MDS_END_ARG);
		break;
	case DTYPE_METHOD :
	case DTYPE_PROCEDURE :
	case DTYPE_PROGRAM :
	case DTYPE_ROUTINE :
        case DTYPE_L :
		MdsFree1Dx(out_ptr,NULL);
		*out_ptr = tmp;
		return status;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return TIME_OUT of program, procedure, routine, or method.
		time_out_field = TIME_OUT_OF(program)
		time_out_field = TIME_OUT_OF(procedure)
		time_out_field = TIME_OUT_OF(routine)
		time_out_field = TIME_OUT_OF(method)
*/
TdiRefStandard(Tdi1TimeoutOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_METHOD,
	DTYPE_PROCEDURE,
	DTYPE_PROGRAM,
	DTYPE_ROUTINE,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_METHOD :
		status=MdsCopyDxXd(((struct descriptor_method *)tmp.pointer)->time_out, out_ptr);
		break;
	case DTYPE_PROCEDURE :
		status=MdsCopyDxXd(((struct descriptor_procedure *)tmp.pointer)->time_out, out_ptr);
		break;
	case DTYPE_PROGRAM :
		status=MdsCopyDxXd(((struct descriptor_program *)tmp.pointer)->time_out, out_ptr);
		break;
	case DTYPE_ROUTINE :
		status=MdsCopyDxXd(((struct descriptor_routine *)tmp.pointer)->time_out, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return units portion of with_units.
		units_field = UNITS_OF(with_units)
		" " = UNITS_OF(other)
	NEED thought about rescale of units, parameters...
*/
TdiRefStandard(Tdi1UnitsOf)
static DESCRIPTOR(none, " ");
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_WITH_UNITS,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_WITH_UNITS :
		status=MdsCopyDxXd(((struct descriptor_with_units *)tmp.pointer)->units, out_ptr);
		break;
	default :
		status=MdsCopyDxXd((struct descriptor *)&none, out_ptr);
		break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return validation field of a parameter.
		validation_field = VALIDATION_OF(param)
*/
TdiRefStandard(Tdi1ValidationOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_PARAM,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_PARAM :
		status=MdsCopyDxXd(((struct descriptor_param *)tmp.pointer)->validation, out_ptr);
		break;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
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
TdiRefStandard(Tdi1ValueOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_DIMENSION,
	DTYPE_PARAM,
	DTYPE_SIGNAL,
	DTYPE_WINDOW,
	DTYPE_WITH_UNITS,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_DIMENSION :
		status=TdiValueOf(((struct descriptor_dimension *)tmp.pointer)->window, out_ptr MDS_END_ARG);
		break;
	case DTYPE_PARAM :
		status=MdsCopyDxXd(((struct descriptor_param *)tmp.pointer)->value, out_ptr);
		break;
	case DTYPE_SIGNAL :
		status=MdsCopyDxXd(((struct descriptor_signal *)tmp.pointer)->data, out_ptr);
		break;
	case DTYPE_WINDOW :
		status=MdsCopyDxXd(((struct descriptor_window *)tmp.pointer)->value_at_idx0, out_ptr);
		break;
	case DTYPE_WITH_UNITS :
		status=MdsCopyDxXd(((struct descriptor_with_units *)tmp.pointer)->data, out_ptr);
		break;
	default :
		MdsFree1Dx(out_ptr,NULL);
		*out_ptr = tmp;
		return status;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return dispatch information.
		when_field = WHEN_OF(dispatch)
*/
TdiRefStandard(Tdi1WhenOf)
struct descriptor_xd	tmp = EMPTY_XD;

	status=TdiDispatchOf(list[0], &tmp MDS_END_ARG);
	if (status&1) status=MdsCopyDxXd(((struct descriptor_dispatch *)tmp.pointer)->when, out_ptr);
	MdsFree1Dx(&tmp,NULL);
	return status;
}
/*--------------------------------------------------------------
	Return window portion of a dimension.
		window_field = WINDOW_OF(dimension)
		same = WINDOW_OF(window)
*/
TdiRefStandard(Tdi1WindowOf)
struct descriptor_xd	tmp = EMPTY_XD;
static unsigned char omits[] = {
	DTYPE_DIMENSION,
	DTYPE_WINDOW,
	0};

	status=TdiGetData(omits, list[0], &tmp);
	if (status&1) switch (tmp.pointer->dtype) {
	case DTYPE_DIMENSION :
		status=MdsCopyDxXd(((struct descriptor_dimension *)tmp.pointer)->window, out_ptr);
		break;
	case DTYPE_WINDOW :
		MdsFree1Dx(out_ptr,NULL);
		*out_ptr = tmp;
		return status;
	default : status=TdiINVDTYDSC; break;
	}
	MdsFree1Dx(&tmp,NULL);
	return status;
}
