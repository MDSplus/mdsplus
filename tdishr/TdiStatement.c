/*	Tdi1Statement.C
	The C-like statements allowed.
	Extension allows multiple statements in call format for:
		CASE DEFAULT DO FOR LABEL SWITCH WHILE
	Unwritten: WHERE
	Ken Klare, LANL P-24	(c)1989,1990,1995
*/
extern unsigned short OpcCase;
extern unsigned short OpcDefault;
extern unsigned short OpcLabel;
extern unsigned short OpcStatement;
extern int TdiON_ERROR;

#include "tdirefstandard.h"
#include <strroutines.h>
#include <tdimessages.h>
#include <stdlib.h>
#include <mdsshr.h>

extern int MdsFree1Dx();
extern int TdiIntrinsic();
extern int TdiEvaluate();
extern int TdiGetLong();
extern int TdiGe();
extern int TdiLe();
extern int TdiEq();

static int goto1(int, struct descriptor *[], struct descriptor_xd *);

/*-----------------------------------------------------------------
	ABORT processing.
		ABORT()
*/
TdiRefStandard(Tdi1Abort)
	return TdiABORT;
}
/*-----------------------------------------------------------------
	BREAK from DO...WHILE FOR or WHILE loops or SWITCH.
		BREAK;
*/
TdiRefStandard(Tdi1Break)
	return TdiBREAK;
}
/*-----------------------------------------------------------------
	CASE within SWITCH.
*/
TdiRefStandard(Tdi1Case)
	status = TdiIntrinsic(OpcStatement, narg-1, &list[1], out_ptr);
	return status;
}
/*-----------------------------------------------------------------
	Evaluates all arguments but returns last only.
		result = evaluation,...evaluation
*/
TdiRefStandard(Tdi1Comma)
int	j;

	for (j = 0; j < narg && status & 1; j++)
		if (list[j] != 0) status = TdiEvaluate(list[j],out_ptr MDS_END_ARG);
	return status;
}
/*-----------------------------------------------------------------
	CONTINUE from DO...WHILE FOR or WHILE loops.
		CONTINUE;
*/
TdiRefStandard(Tdi1Continue)
	return TdiCONTINUE;
}
/*-----------------------------------------------------------------
	CASE DEFAULT within SWITCH.
*/
TdiRefStandard(Tdi1Default)
	status = TdiIntrinsic(OpcStatement, narg, &list[0], out_ptr);
	return status;
}
/*-----------------------------------------------------------------
	Loops until expression is false, doing a statement.
		DO {statement} WHILE (expression);
*/
TdiRefStandard(Tdi1Do)
int	test;

	do {
		status = TdiIntrinsic(OpcStatement, narg-1, &list[1], out_ptr);
		if (status & 1 || status == TdiCONTINUE) status = TdiGetLong(list[0], &test);
	} while (status & 1 && test & 1);
	if (status == TdiBREAK) status = 1;
	return status;
}
/*-----------------------------------------------------------------
	Loops FOR expression true and doing a statement.
		FOR ([init]; [test]; [update]) statement
*/
TdiRefStandard(Tdi1For)
int	test;

	if (list[0]) status = TdiEvaluate(list[0], out_ptr MDS_END_ARG);
	while (status & 1) {
		if (list[1]) {
			status = TdiGetLong(list[1], &test);
			if (!(status & 1 && test & 1)) break;
		}
		status = TdiIntrinsic(OpcStatement, narg-3, &list[3], out_ptr);
		if (status & 1 || status == TdiCONTINUE) status = TdiEvaluate(list[2], out_ptr MDS_END_ARG);
	}
	if (status == TdiBREAK) status = 1;
	return status;
}
/*-----------------------------------------------------------------
	GOTO a label in active area of code.
		GOTO label;
*/
TdiRefStandard(Tdi1Goto)

	status = TdiEvaluate(list[0], out_ptr MDS_END_ARG);
	if (status & 1) {
		if	(out_ptr->pointer->class != CLASS_S) status = TdiINVCLADSC;
		else if	(out_ptr->pointer->dtype != DTYPE_T) status = TdiINVDTYDSC;
		else	status = TdiGOTO;
	}
	return status;
}
/*-----------------------------------------------------------------
	Tests IF expression and does a statement if true or possibly another if false.
		IF (expression) statement ELSE statement
*/
TdiRefStandard(Tdi1If)
int	test = 0;

	status = TdiGetLong(list[0], &test);
	if (test & 1) {
		if (status & 1)		status = TdiEvaluate(list[1], out_ptr MDS_END_ARG);
		while (status == TdiGOTO) status = goto1(1, &list[1], out_ptr);
	}
	else if (narg > 2) {
		if (status & 1)		status = TdiEvaluate(list[2], out_ptr MDS_END_ARG);
		while (status == TdiGOTO) status = goto1(1, &list[2], out_ptr);
	}
	if (status == 0) status = TdiGOTO;
	return status;
}
/*-----------------------------------------------------------------
	If error on first evaluation, try next.
	If last fails, return EMPTY descriptor and success.
		first_good = IF_ERROR(a,...)
*/
TdiRefStandard(Tdi1IfError)
int	keep, j;

	keep = TdiON_ERROR;
	TdiON_ERROR = 0;
	for (j = 0; j < narg; ++j) {
		status = TdiEvaluate(list[j], out_ptr MDS_END_ARG);
		if (status & 1) break;
	}
	TdiON_ERROR = keep;
	if (j >= narg) status = MdsFree1Dx(out_ptr, NULL);
	return status;
}
/*-----------------------------------------------------------------
	Hold LABEL for GOTO.
		label : stmt
*/
TdiRefStandard(Tdi1Label)

	status = TdiIntrinsic(OpcStatement, narg-1, &list[1], out_ptr);
	return status;
}
/*-----------------------------------------------------------------
	Remark embedded in expression, a no-operation.
*/
TdiRefStandard(Tdi1Rem)
	return 1;
}
/*-----------------------------------------------------------------
	RETURN from FUN.
	FUN name(arg...) {
		...
		RETURN (expr);
		...
	}
*/
TdiRefStandard(Tdi1Return)

	if (narg > 0 && list[0]) status = TdiEvaluate(list[0], out_ptr MDS_END_ARG);
	else status = MdsFree1Dx(out_ptr, NULL);
	if (status & 1) status = TdiRETURN;
	return status;
}
/*-----------------------------------------------------------------
	Handle GOTO search for DO...WHILE, FOR, and WHILE loops
	and STATEMENT and SWITCH blocks.

	You must never branch into DO FOR or WHILE loops
	nor into IF ELSE SWITCH WHERE or ELSEWHERE blocks!!!!,
	but you can branch within these constructs.

	Limitation: label must be in currently active code.
	Limitation: destination label must be text. It is not evaluated.
	Note that GOTO label may be an expression.
		label : stmt
		...
		GOTO label;

	Assumes text is explicit.
	Must handle statements:
	LABEL label : stmt...
	CASE (value) stmt...
	CASE DEFAULT stmt...
	DO {stmt...} WHILE (exp);
	FOR (exp;exp;exp) stmt...
	IF (exp) stmt ELSE stmt
	SWITCH (exp) stmt...
	WHERE (exp) stmt ELSEWHERE stmt
	{stmt ...}
*/
static int			goto1(
int				narg,
struct descriptor		*list[],
struct descriptor_xd	*out_ptr) {
int	j=0, off, opcode, status = 0;
struct descriptor	*pstr = (struct descriptor *)out_ptr;

	/***************************
	Watch for D converted to XD.
	***************************/
	while (pstr && pstr->dtype == DTYPE_DSC) pstr = (struct descriptor *)pstr->pointer;
	if (pstr && pstr->dtype == DTYPE_T) for (j = 0; status == 0 && j < narg; ++j) {
	struct descriptor_function *pfun = (struct descriptor_function *)list[j];

		if (pfun && pfun->dtype == DTYPE_DSC) pfun = (struct descriptor_function *)pfun->pointer;
		if (pfun && pfun->dtype == DTYPE_FUNCTION) {
			off = -1;
			opcode = *(unsigned short *)pfun->pointer;
			if (opcode == OpcLabel) {
				if (StrCompare(pfun->arguments[0], pstr) == 0) break;
				off = 1;
			}
			else if (opcode == OpcDefault
			||	opcode == OpcStatement) off = 0;
			else if (opcode == OpcCase) off = 1;
			if (off >= 0) status = goto1(pfun->ndesc-off, &pfun->arguments[off], out_ptr);
		}
	}
	if (status & 1 && j < narg) status = TdiIntrinsic(OpcStatement, narg-j, &list[j], out_ptr);
	return status;
}
/*-----------------------------------------------------------------
	Evaluates all arguments but returns last only.
	Note statements end with a semicolon or right brace.
		{statement ... statement}
*/
TdiRefStandard(Tdi1Statement)
int	j;

	for (j = 0; status & 1 && j < narg; ++j) status = TdiEvaluate(list[j], out_ptr MDS_END_ARG);
	while (status == TdiGOTO) status = goto1(narg, list, out_ptr);
	if (status == 0) status = TdiGOTO;
	return status;
}
/*-----------------------------------------------------------------
	Switch between case types.
		SWITCH (expression) {
		CASE DEFAULT statement
		CASE (expression) statement
		...
		}
	It is more efficient to have CASE DEFAULT near the top.
	Note that a CASE cannot be defined inside of
		FOR IF ELSE SWITCH WHERE ELSEWHERE or WHILE.
	NEED to handle vector cases and stepped ranges, NEED IS_IN.
*/
static int			switch1(
struct descriptor		*ptest,
int				*jdefault,
struct descriptor_function	***pdefault,
int				narg,
struct descriptor_function	*list[],
struct descriptor_xd	*out_ptr) {
int	j, ndesc, off, test, status = 1;
struct descriptor_range	*pr;
struct descriptor_xd	tmp = EMPTY_XD;

	for (j = 0; status & 1 && j < narg; ++j) if (list[j] && list[j]->dtype == DTYPE_FUNCTION) {
	unsigned short opcode = *(unsigned short *)list[j]->pointer;

		off = -1;
		if (opcode == OpcStatement) off = 0;
		else if (opcode == OpcCase) {
			ndesc = list[j]->ndesc;
			if (ndesc == 0) {status = TdiMISS_ARG; break;}
			status = TdiEvaluate(list[j]->arguments[0], out_ptr MDS_END_ARG);
			if (status & 1)
                        {
                          pr = (struct descriptor_range *)out_ptr->pointer;
  			  if (pr && pr->dtype == DTYPE_RANGE) {
				if (pr->begin) {
					status = TdiGe(ptest, pr->begin, &tmp MDS_END_ARG);
					if (status & 1) status = TdiGetLong(&tmp, &test);
				}
				else test = 1;
				if (status & 1 && test & 1 && pr->ending) {
					status = TdiLe(ptest, pr->ending, &tmp MDS_END_ARG);
					if (status & 1) status = TdiGetLong(&tmp, &test);
				}
			  }
			  else {
				status = TdiEq(ptest, pr, &tmp MDS_END_ARG);
				if (status & 1) status = TdiGetLong(&tmp, &test);
			  }
                        }
			if (status & 1 && test & 1) {
				*jdefault = 0;
				break;
			}
			off = 1;
		}
		else if (opcode == OpcDefault) {
			*jdefault = narg - j;
			*pdefault = &list[j];
			off = 0;
		}
		if (off >= 0 && status & 1) {
			status = switch1(ptest, jdefault, pdefault, list[j]->ndesc-off, 
                                        (struct descriptor_function **)&list[j]->arguments[off], out_ptr);
			if (status & 1) {++j; break;}
			if (status == TdiCASE) status = 1;
		}
	}
	MdsFree1Dx(&tmp, NULL);
	if (status & 1 && j >= narg) status = TdiCASE;
	else {
		*jdefault = 0;		/*suppress the default because it is a match*/
		for (; status & 1 && j < narg; ++j) status = TdiEvaluate(list[j], out_ptr MDS_END_ARG);
	}
	return status;
}

TdiRefStandard(Tdi1Switch)
int				jdefault = 0;
struct descriptor_function	**pdefault = NULL;
struct descriptor_xd	tmp = EMPTY_XD;

	status = TdiEvaluate(list[0], &tmp MDS_END_ARG);
	if (status & 1) status = switch1(tmp.pointer, &jdefault, &pdefault, narg-1,
                                        (struct descriptor_function **)&list[1], out_ptr);
	MdsFree1Dx(&tmp, NULL);
	if (status == TdiCASE) {
		status = 1;
		for (; jdefault > 0 && status & 1; --jdefault, ++pdefault) {
			status = TdiEvaluate(*(struct descriptor **)pdefault, out_ptr MDS_END_ARG);
		}
	}
	if (status == TdiBREAK) status = 1;
	else {
		while (status == TdiGOTO) status = goto1(narg-1, &list[1], out_ptr);
		if (status == 0) status = TdiGOTO;
	}
	return status;
}
/*-----------------------------------------------------------------
	Loops while expression is true and does a statement.
		WHILE (expression) statement
*/
TdiRefStandard(Tdi1While)
int	test;

	while (status & 1 || status == TdiCONTINUE) {
		status = TdiGetLong(list[0], &test);
		if (!(status & 1 && test & 1)) break;
		status = TdiIntrinsic(OpcStatement, narg-1, &list[1], out_ptr);
	}
	if (status == TdiBREAK) status = 1;
	return status;
}
