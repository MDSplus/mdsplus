/*	Tdi1Compile.C
	The interface to compiler.

	Ken Klare, LANL CTR-7	(c)1989,1990
*/

extern unsigned short OpcCompile;

#include "tdirefzone.h"
#include "tdirefstandard.h"
#include <libroutines.h>
#include <tdimessages.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <string.h>

extern int TdiEvaluate();
extern int TdiYacc();
extern int TdiIntrinsic();
extern void TdiYyReset();

/*-------------------------------------------------------
	Interface to compiler/parser.
		expression = COMPILE(string, [arg1,...])
	YACC converts TdiYacc.Y to TdiYacc.C to parse TDI statements. Also YACC subroutines.
	LEX converts TdiLex.X to TdiLex.C for lexical analysis. Also LEX subroutines.

	Limitations:
	For recursion must pass LEX:
		zone status bol cur end
		yylval yyval
		yytext[YYLMAX] yyleng yymorfg yytchar yyin? yyout?
	For recursion must pass YACC also:
		yydebug? yyv[YYMAXDEPTH] yychar yynerrs yyerrflag
	Thus no recursion because the yy's are built into LEX and YACC.
	IMMEDIATE (`) must never call COMPILE. NEED to prevent this.
*/
TdiRefStandard(Tdi1Compile)
EMPTYXD(tmp);
struct descriptor		*text_ptr;
static DESCRIPTOR(compile_zone,"TDI Compile Zone");

	status = TdiEvaluate(list[0],&tmp MDS_END_ARG);
	text_ptr = tmp.pointer;
	if (status & 1 && text_ptr->dtype != DTYPE_T) status = TdiINVDTYDSC;
	if (status & 1) {
		if (!TdiRefZone.l_zone) status = LibCreateVmZone(&TdiRefZone.l_zone,0,0,0,0,0,0,0,0,0,&compile_zone);

		/****************************************
		In case we bomb out, probably not needed.
		****************************************/
		TdiRefZone.l_status = TdiBOMB;
                if (TdiRefZone.a_begin) free(TdiRefZone.a_begin);
                TdiRefZone.a_begin = TdiRefZone.a_cur = memcpy(malloc(text_ptr->length),text_ptr->pointer,text_ptr->length);
		TdiRefZone.a_end = TdiRefZone.a_cur + text_ptr->length;
		TdiRefZone.l_ok = 0;
		TdiRefZone.l_narg = narg - 1;
		TdiRefZone.l_iarg = 0;
		TdiRefZone.a_list = &list[0];
		if (status & 1) {
                        TdiYyReset();
			if (TdiYacc() && TdiRefZone.l_status & 1) status = TdiSYNTAX;
			else status = TdiRefZone.l_status;
		}

		/************************
		Move from temporary zone.
		************************/
		if (status & 1) {
			if (TdiRefZone.a_result == 0) MdsFree1Dx(out_ptr, NULL);
			else status = MdsCopyDxXd((struct descriptor *)TdiRefZone.a_result, out_ptr);
		}
		LibResetVmZone(&TdiRefZone.l_zone);
	}
	MdsFree1Dx(&tmp, NULL);
	return(status);
}
/*-------------------------------------------------------
	Compile and evaluate an expression.
		result = EXECUTE(string, [arg1,...])
*/
TdiRefStandard(Tdi1Execute)
struct descriptor_xd	tmp = EMPTY_XD;

	status = TdiIntrinsic(OpcCompile, narg, list, &tmp);
	if (status & 1) status = TdiEvaluate(tmp.pointer, out_ptr MDS_END_ARG);
	MdsFree1Dx(&tmp, NULL);
	return status;
}
