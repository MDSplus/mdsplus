%output  "TdiYacc.c"
%defines "tdiyacc.h"
%define api.pure full
%lex-param   { TDITHREADSTATIC_TYPE *TDITHREADSTATIC_VAR }
%parse-param { TDITHREADSTATIC_TYPE *TDITHREADSTATIC_VAR }
%code requires {
#define YYLTYPE TDITHREADSTATIC_TYPE
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
}
%{
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

/*      tdiYacc.Y
	YACC converts this to tdiYacc.C to compile TDI statements.
	Each YACC-LEX symbol has a returned token and a tdiyylval value.
	Precedence is associated with a token and is set left and right below.
	tdi_lex_... routines and TdiRefFunction depend on tokens from tdiYacc.
	Tdi0Decompile depends on TdiRefFunction and precedence.

	Josh Stillerman and Thomas W. Fredian, MIT PFC, TDI$PARSE.Y.
	Ken Klare, LANL P-4     (c)1989,1990,1991

	Note statements like    {;} {a;} {a,b,c;}
	Watch -3^4*5    Fortran says -((3^4)*5), CC/high-unary-precedence would give ((-3)^4)*5
	Watch 3^-4*5    Fortran illegal, Vax 3^(-(4*5)), CC would give (3^(-4))*5
	NEED x^float    where float=int(float) to be x^int.
	NEED x^2        square(x) and x^.5 to be sqrt(x).
	Limitations:
	        () implies no arguments. Use (*) for one. (x,*,y) is permitted, (x,,y) may be.
	        255 statements, 253 commas and arguments. NEED to check, done in RESOLVE.
	        Recursion will not work as coded, could happen in IMMEDIATE of compile.
	        (YYMAXDEPTH - something) arguments.
	        All binary stores are allowed, watch a > = b, it is not (a (>=) b), it is (a = (a > b)).

	Sneaky "neat" tricks:
	F90 style constructors: [scalars,vectors,etc] become vector or compound to become array.
	a//b//c generates CONCAT(a,b,c).
	Could NEED: Also a MIN b MIN c gives MIN(a,b,c).
	path(a) is function described at node. Not after 9/25/89.
	        May require (path)(a) if path-ness not clear.
	path[b] subscripts whatever is at node. Same problem.
	NOT and INOT of AND OR etc., form NAND or AND_NOT etc. See KNOT1 and KNOT2. Not after 9/25/89.
-*/
#include <mdsplus/mdsplus.h>

#include <stdio.h>
#include <string.h>

#include <libroutines.h>
#include <strroutines.h>
#include <tdishr_messages.h>
#include <mds_stdarg.h>

#include "tdithreadstatic.h"
#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefzone.h"
#include "tdirefstandard.h"

#define YYDEBUG 0
#define YYERROR_VERBOSE 1
#include "tdiyacc.h"
#include "tdilex.h"

extern unsigned short OpcSubscript, OpcExtFunction, OpcFun, OpcUsing;

extern int tdilex();
extern int tdi_lex_path();
extern int tdi_yacc_RESOLVE();
extern int tdi_yacc_IMMEDIATE();
extern int tdi_yacc_BUILD();
extern int tdi_yacc_ARG();
#define yyparse(...)			tdi_yacc(__VA_ARGS__)
#define yylex(VAL,TDITHREADSTATIC_VAR)	(tdiset_debug(YYDEBUG,TDI_SCANNER),tdilex(VAL, TDITHREADSTATIC_VAR, TDI_SCANNER))


int yydebug = YYDEBUG;

#define YY_ERR	1 // ensure error state : default to SYNTAX if l_status ok
#define YY_ASIS	0 // use l_status
#define yyerror(TDITHREADSTATIC_ARG,msg) {\
  if (YYDEBUG) fprintf(stderr,"yyerror '%s': %d\n", msg, TDI_REFZONE.l_status);\
  TDI_REFZONE.l_ok = yyval.mark.w_ok;\
}
//"

#define YYMAXDEPTH	250
#define __RUN(method)	do{if IS_NOT_OK(TDI_REFZONE.l_status = method) {yyerror(TDITHREADSTATIC_VAR,"method failed"); return YY_ASIS;} else TDI_REFZONE.l_ok = TDI_REFZONE.a_cur - TDI_REFZONE.a_begin;}while(0)

#define _RESOLVE(arg)				__RUN(tdi_yacc_RESOLVE(&arg.rptr, TDITHREADSTATIC_VAR))

#define _FULL1(opcode,arg1,out)                 __RUN(tdi_yacc_BUILD(255, 1, opcode, &out, &arg1, NULL , NULL , NULL , TDITHREADSTATIC_VAR))
#define _FULL2(opcode,arg1,arg2,out)            __RUN(tdi_yacc_BUILD(255, 2, opcode, &out, &arg1, &arg2, NULL , NULL , TDITHREADSTATIC_VAR))
	/*****************************
	Two args for image->routine.
	*****************************/
#define _JUST0(opcode,out)                      __RUN(tdi_yacc_BUILD(  2, 0, opcode, &out, NULL , NULL , NULL , NULL , TDITHREADSTATIC_VAR))
#define _JUST1(opcode,arg1,out)                 __RUN(tdi_yacc_BUILD(  3, 1, opcode, &out, &arg1, NULL , NULL , NULL , TDITHREADSTATIC_VAR))
#define _JUST2(opcode,arg1,arg2,out)            __RUN(tdi_yacc_BUILD(  2, 2, opcode, &out, &arg1, &arg2, NULL , NULL , TDITHREADSTATIC_VAR))
#define _JUST3(opcode,arg1,arg2,arg3,out)       __RUN(tdi_yacc_BUILD(  3, 3, opcode, &out, &arg1, &arg2, &arg3, NULL , TDITHREADSTATIC_VAR))
#define _JUST4(opcode,arg1,arg2,arg3,arg4,out)  __RUN(tdi_yacc_BUILD(  4, 4, opcode, &out, &arg1, &arg2, &arg3, &arg4, TDITHREADSTATIC_VAR))

static const struct marker _EMPTY_MARKER = { 0 };
%}
	/**********************************************************
	This becomes typedef YYSTYPE for yylval.
	We use builtin number for expressions and name for routine.
	**********************************************************/
%union	{struct marker mark;}

%start program

	/***********************************************************
	Tokens are the result of Lex.
	***********************************************************/
%nonassoc	END
%nonassoc	ELSE    ELSEW

%token	<mark>	ERROR	IDENT	POINT	TEXT	VALUE

%token  <mark>	BREAK	CASE	COND	DEFAULT	DO	ELSE    ELSEW	FOR
%token	<mark>	GOTO	IF	LABEL	RETURN	SIZEOF	SWITCH	USING	WHERE	WHILE

%token	<mark>	CAST	CONST	INC	ARG
%token	<mark>	ADD	CONCAT	IAND	IN	IOR	IXOR
%token	<mark>	POWER	PROMO	RANGE	SHIFT	BINEQ
%token	<mark>	LAND	LEQ	LGE	LOR	MUL	UNARY	LEQV
%token	<mark>	LANDS	LEQS	LGES	LORS	MULS	UNARYS	LEQVS
%token	<mark>	FUN	MODIF	VBL	AMODIF

%type	<mark>	program	stmt0	stmt	stmt_lst
%type	<mark>	slabel	ulabel	flabel	label	using	using0
%type	<mark>	fun	funvbl	modif
%type	<mark>	exp	ass	opt
%type	<mark>	unaryX	postX	primaX
%type	<mark>	bracket	paren	sub	textX

/*****************************************************************
	Precedence: lowest to highest. -=not available, ?=not explicit
		CC	F90	Vax F77	IDL
%left	STATE	//?	?	?	?	; {} IF etc.
***/
%left ','	//15	-	-	-	, name(a,b op c...) decompiles as name(a,(b,c)...) but name(a,b=c)
%right '`'	//-	-	-	-	`
%right '=' BINEQ//14	?	?	?	= *= >>= etc.
%right RANGE	//-	?	?	?	: ..	should bind more than // or IN maybe, less than +
%right '?'	//13	-	-	-	?:
%right PROMO	//-	-	-	-	@
		//-	12.bop.	-	-	defined binary	.UNION.
%left LEQV	//-	11.EQV.	8.EQV.	3 XOR	EQV NEQV
%left LOR LORS	//12	10.OR.	7.OR.	3 OR	|| OR
%left LAND LANDS//11	9.AND.	6.AND.	2 AND	&& AND
%left IOR	//10	-	(7)	3 OR	|
%left IXOR	//9 ^	-	(8)	3 XOR
%left IAND	//8	-	(6)	2 AND	& AND
%left LEQ LEQS	//7	7	4	3 EQ	== != <> EQ NE
%left LGE LGES	//6	7	4	3 LT	< > <= >= LT GT LE GE
%left IN	//-	-	-	-	IN	(subset)
%left CONCAT	//-	6	3	3 +	//
		//-	-	-	3	< >	(min max)
%left SHIFT	//5	-	-	-	<< >>
%left ADD	//4	5	3	3	+ - .binary.
%left MUL MULS '*'//3 %	3	2	2 # MOD	* /
%right POWER	//-	2	1	1	** ^

%right UNARY UNARYS//2 * &	4 + -	3	?	+ - .unary.
		//2	8.NOT.	5	?	~ ! NOT INOT SIZEOF
		//2	-	-	-	++ --
//%right CAST	//2	-	-	-	(cast)
		//-	1.uop.	-	-	defined unary .INVERSE.
//%left POST	//1. ->	?	?	?	(e) p(e) p[args] p++ p--

%%
	/***********
	Definitions.
	***********/
	/*********************************************************
	Problem, with word-form and symbolic mapped together,
	we cannot reconstruct just named ones. Would require
	reserved words: EQ GE GT LE LT NE AND OR EQV NEQV MOD NOT
	for LEQ LGE LAND LOR LEQV MUL UNARY tokens.
	Solution: symbol tokens: LEQS LGES LANDS LORS MULS UNARYS.
	NEED more if a IAND b (equal to a & b) is valid binary.
	Other tabular tokens give IDENT. $nonstandard and _names give VBL.
	*********************************************************/

slabel	: LABEL	VBL			{$$=$2;}
	;
	/*********************************
	generated shift/reduce conflicts
	but allows reuse of keywords for
	image or routine names
	*********************************/
ulabel	: IDENT	| CONST	| CAST	| GOTO	| SIZEOF
	| DO	| ELSE	| ELSEW	| LABEL	| RETURN
	| FUN	| VBL	| COND	| ARG	| DEFAULT
	| AMODIF
	;
flabel	: ulabel
	| BREAK | MODIF
	| MUL	| MULS
	| LAND	| LEQ	| LGE	| LOR	| LEQV
	| LANDS | LEQS	| LGES	| LORS	| LEQVS
	;
label	: flabel
	| WHERE	| WHILE	| CASE	| USING
	| FOR	| IF	| UNARY	| SWITCH
	;
	/*********************************************************
	CONCAT/COMMA require function with same/correct token.
	With match we can append, otherwise create a big one.
	We separate comma in expression from comma in subscript
	because subscripts permit null/full value.
	*********************************************************/
unaryX	: ADD unaryX 		{_JUST1((($1.builtin == OPC_SUBTRACT) ? OPC_UNARY_MINUS : OPC_UNARY_PLUS),$2,$$);}
	| UNARY unaryX	 	{_JUST1($1.builtin,$2,$$);}	/*UNARY*/
	| UNARYS unaryX 	{_JUST1($1.builtin,$2,$$);}	/*UNARYS*/
	| INC unaryX 		{_JUST1($1.builtin,$2,$$);}	/*preINC*/
	| postX
	;
ass	: '`'	ass			{$$.rptr=$2.rptr; $$.builtin= -2;
					__RUN(tdi_yacc_IMMEDIATE(&$$.rptr, TDITHREADSTATIC_VAR));}
	| ERROR				{yyerror(TDITHREADSTATIC_VAR, "lex error"); return YY_ERR;}
	| unaryX
	| unaryX '=' ass		{_JUST2(OPC_EQUALS,$1,$3,$$);}/*assign right-to-left*/
	| unaryX BINEQ ass		{struct marker tmp;		/*binary operation and assign*/
						_JUST2($2.builtin,$1,$3,tmp);
						_JUST1(OPC_EQUALS_FIRST,tmp,$$);}
	| ass '?' ass RANGE ass		{_JUST3(OPC_CONDITIONAL,$3,$5,$1,$$);}/*COND right-to-left*/
	| ass RANGE ass			{if ($3.rptr && $3.rptr->dtype == DTYPE_RANGE)
						if ($3.rptr->ndesc == 2)
							{$$=$3;
							$$.rptr->dscptrs[2]=$$.rptr->dscptrs[1];
							$$.rptr->dscptrs[1]=$$.rptr->dscptrs[0];
							$$.rptr->dscptrs[0]=(mdsdsc_t *)$1.rptr;
							++$$.rptr->ndesc;}
						else {TDI_REFZONE.l_status=TdiEXTRA_ARG; yyerror(TDITHREADSTATIC_VAR, "extra args"); return YY_ASIS;}
					else	{static unsigned int vmlen = sizeof(struct descriptor_range);
						LibGetVm(&vmlen, (void **)&$$.rptr, &TDI_REFZONE.l_zone);
						$$.rptr->length = 0;
						$$.rptr->dtype = DTYPE_RANGE;
						$$.rptr->class = CLASS_R;
						$$.rptr->pointer = 0;
						$$.rptr->ndesc = 2;
						$$.rptr->dscptrs[0]=(mdsdsc_t *)$1.rptr;
						$$.rptr->dscptrs[1]=(mdsdsc_t *)$3.rptr;
						_RESOLVE($$);}
					}
	| ass PROMO	ass		{_JUST2($2.builtin,$1,$3,$$);}/*PROMO right-to-left*/
	| ass LEQV	ass		{_JUST2($2.builtin,$1,$3,$$);}/*LEQV*/
	| ass LOR	ass		{_JUST2($2.builtin,$1,$3,$$);}/*LOR*/
	| ass LORS	ass		{_JUST2($2.builtin,$1,$3,$$);}/*LORS*/
	| ass LAND	ass		{_JUST2($2.builtin,$1,$3,$$);}/*LAND*/
	| ass LANDS	ass		{_JUST2($2.builtin,$1,$3,$$);}/*LANDS*/
	| ass IOR	ass		{_JUST2($2.builtin,$1,$3,$$);}/*IOR*/
	| ass IXOR	ass		{_JUST2($2.builtin,$1,$3,$$);}/*IXOR*/
	| ass IAND	ass		{_JUST2($2.builtin,$1,$3,$$);}/*IAND*/
	| ass LEQ	ass		{_JUST2($2.builtin,$1,$3,$$);}/*LEQ*/
	| ass LEQS	ass		{_JUST2($2.builtin,$1,$3,$$);}/*LEQS*/
	| ass LGE	ass		{_JUST2($2.builtin,$1,$3,$$);}/*LGE*/
	| ass LGES	ass		{_JUST2($2.builtin,$1,$3,$$);}/*LGES*/
	| ass IN	ass		{_JUST2($2.builtin,$1,$3,$$);}/*IS_IN*/
	| ass CONCAT	ass		{if ($$.rptr == 0) $$=$3;
					else if ($$.rptr->dtype == DTYPE_FUNCTION
					&&	*(opcode_t *)$$.rptr->pointer == OPC_CONCAT
					&&	$$.rptr->ndesc < 250)
						{$$.rptr->dscptrs[$$.rptr->ndesc++]=(mdsdsc_t *)$3.rptr;
						_RESOLVE($$);}
					else {_FULL2($2.builtin,$1,$3,$$);}
					}
	| ass SHIFT	ass		{_JUST2($2.builtin,$1,$3,$$);}	/*SHIFT*/
	| ass ADD	ass		{_JUST2($2.builtin,$1,$3,$$);}	/*ADD*/
	| ass MUL	ass		{_JUST2($2.builtin,$1,$3,$$);}	/*MUL*/
	| ass MULS	ass		{_JUST2($2.builtin,$1,$3,$$);}	/*MULS*/
	| ass '*'	ass		{_JUST2(OPC_MULTIPLY,$1,$3,$$);}
	| ass POWER	ass		{_JUST2($2.builtin,$1,$3,$$);}	/*POWER right-to-left*/
	| '*'				{$$=_EMPTY_MARKER;}
	;
		/********************************************************************
		Argument lists, optional or required. No arguments for empty list ().
		Use * for single missing argument. (*) is 1 missing, (,) is two.
		USING must have first argument with relative paths.
		********************************************************************/
bracket	: '[' ass		{_FULL1(OPC_VECTOR,$2,$$);}		/*constructor*/
	| '['			{_JUST0(OPC_VECTOR,$$);}		/*null constructor*/
	| bracket ',' ass 		{if ($$.rptr->ndesc >= 250) {
					_RESOLVE($1);
					_FULL1(OPC_VECTOR,$1,$$);
				}
				$$.rptr->dscptrs[$$.rptr->ndesc++] = (mdsdsc_t *)$3.rptr;
				}
	;
opt	: exp
	| %empty		{$$=_EMPTY_MARKER;}			/*null argument*/
	;
exp	: opt ',' opt	{if (	$$.rptr			/*comma is left-to-right weakest*/
			&&	$$.builtin != -2
			&&	$$.rptr->dtype == DTYPE_FUNCTION
			&&	*(opcode_t *)$$.rptr->pointer == OPC_COMMA
			&&	$$.rptr->ndesc < 250)
				$$.rptr->dscptrs[$$.rptr->ndesc++]=(mdsdsc_t *)$3.rptr;
			else _FULL2(OPC_COMMA,$1,$3,$$);}	/*first comma*/
	| ass
	| ass ERROR	{yyerror(TDITHREADSTATIC_VAR, "lex error"); return YY_ERR;}
	;
sub	: exp 		{if ($$.rptr
				&& $$.builtin != -2
				&& $$.rptr->dtype == DTYPE_FUNCTION
				&& *(opcode_t *)$$.rptr->pointer == OPC_COMMA) ;
				else _JUST1(OPC_ABORT,$1,$$);}		/*first subscript*/
       	| %empty		{_JUST0(OPC_ABORT,$$);}
	;
paren	: '(' exp ')'		{$$=$2; $$.builtin= -2;}		/*expression group*/
	| '(' stmt_lst ')'	{$$=$2; $$.builtin= -2;}		/*statement group*/
	;
using0	: USING '('		{++TDI_REFZONE.l_rel_path;}
	;
using	: using0 ass ',' ass ',' {_FULL2(OPC_ABORT,$2,$4,$$); --TDI_REFZONE.l_rel_path;}
	| using0 ass ',' ','	{_FULL2(OPC_ABORT,$2,_EMPTY_MARKER,$$); --TDI_REFZONE.l_rel_path;}
	;
	/*******************************************
	Postfix expression. NEED to understand effect of primary lvalue.
	*******************************************/
postX	: primaX
	| postX '[' sub ']'	{int j;
					$$=$3;
					$$.rptr->pointer= (uint8_t *)&OpcSubscript;
					for (j=$$.rptr->ndesc; --j>=0; )
						$$.rptr->dscptrs[j+1]=$$.rptr->dscptrs[j];
					$$.rptr->dscptrs[0]=(mdsdsc_t *)$1.rptr;
					$$.rptr->ndesc++;
					_RESOLVE($$);
				}
	| postX INC 	 {int j=$2.builtin==OPC_PRE_INC ? OPC_POST_INC :  OPC_POST_DEC;
					_JUST1(j,$1,$$);}		/*postINC*/
	| flabel '(' sub ')'	{$$=$3;
				if ($1.builtin < 0) {int j;		/*unknown today*/
					$$.rptr->pointer= (uint8_t *)&OpcExtFunction;
					for (j=$$.rptr->ndesc; --j>=0;)
						$$.rptr->dscptrs[j+2]=$$.rptr->dscptrs[j];
					$$.rptr->dscptrs[0]=0;
					$$.rptr->dscptrs[1]=(mdsdsc_t *)$1.rptr;
					$$.rptr->ndesc += 2;
				}
				else	{				/*intrinsic*/
					*(opcode_t *)$$.rptr->pointer=$1.builtin;
					_RESOLVE($$);}
				}
	| label POINT '(' sub ')' {int j;
				$$=$4;			/*external*/
				$$.rptr->dtype=DTYPE_CALL;
				$$.rptr->length=0;
				$$.rptr->pointer=0;
				for (j=$$.rptr->ndesc; --j>=0;)
					$$.rptr->dscptrs[j+2]=$$.rptr->dscptrs[j];
				$$.rptr->dscptrs[0]=(mdsdsc_t *)$1.rptr;
				$$.rptr->dscptrs[1]=(mdsdsc_t *)$2.rptr;
				$$.rptr->ndesc += 2;
				}
	| label POINT label '(' sub ')' {int j;
				$$=$5;			/*typed external*/
				StrUpcase((mdsdsc_t *)$3.rptr, (mdsdsc_t *)$3.rptr);
				for (j=TdiCAT_MAX; j-->0;)
					if (strncmp(TdiREF_CAT[j].name, (char *)$3.rptr->pointer, $3.rptr->length) == 0
					&& TdiREF_CAT[j].name[$3.rptr->length] == '\0') // exact match
						break;
				if (j<0) {TDI_REFZONE.l_status=TdiINVDTYDSC; yyerror(TDITHREADSTATIC_VAR, "invalid dtype desc"); return YY_ERR;}
				$$.rptr->dtype=DTYPE_CALL;
				$$.rptr->length=1;
				*(unsigned char *)$$.rptr->pointer=(unsigned char)j;
				for (j=$$.rptr->ndesc; --j>=0;)
					$$.rptr->dscptrs[j+2]=$$.rptr->dscptrs[j];
				$$.rptr->dscptrs[0]=(mdsdsc_t *)$1.rptr;
				$$.rptr->dscptrs[1]=(mdsdsc_t *)$2.rptr;
				$$.rptr->ndesc += 2;
				}
	| using sub ')'		{int j;	/*USING(expr,[default],[shotid],[expt])*/
					$$.rptr->pointer= (uint8_t *)&OpcUsing;
					for (j=0; j < $2.rptr->ndesc; ++j)
						$$.rptr->dscptrs[$$.rptr->ndesc++]=$2.rptr->dscptrs[j];
				}
	| using0 ass ',' ass ')' {_JUST2(OPC_USING,$2,$4,$$); --TDI_REFZONE.l_rel_path;}
	;
		/*****************************************************
		ANSI C says "..." "..." is compile-time concatenation.
		*****************************************************/
textX	: TEXT
	| textX TEXT		{MAKE_S(DTYPE_T, $1.rptr->length + $2.rptr->length, $$.rptr);
					StrConcat((mdsdsc_t *)$$.rptr, (mdsdsc_t *)$1.rptr, $2.rptr MDS_END_ARG);
				}
	;
		/********************************************************************
		We reserve all names starting with $ or _ for constants or variables.
		Wait to make path/arg/const for simple; they may be text for -> or (.
		Simple name are members of current node in tree.
		********************************************************************/
modif	: MODIF	| AMODIF ;
primaX	: modif VBL		{_JUST1($1.builtin,$2,$$);}		/*IN/INOUT/OPTIONAL/OUT/PUBLIC/PRIVATE variable*/
	| AMODIF AMODIF VBL	{struct marker tmp;			/*OPTIONAL IN/INOUT/OUT*/
					_JUST1($2.builtin,$3,tmp);
					_JUST1($1.builtin,tmp,$$);}
	| ulabel		{if (*$$.rptr->pointer == '$') {
					if($$.builtin < 0) $$.rptr->dtype=DTYPE_IDENT;
					else if ((TdiRefFunction[$$.builtin].token & LEX_M_TOKEN) == ARG)
					{__RUN(tdi_yacc_ARG(&$$, TDITHREADSTATIC_VAR));}
					else if ((TdiRefFunction[$$.builtin].token & LEX_M_TOKEN) == CONST)
						_JUST0($1.builtin,$$);
				} else	if (*$$.rptr->pointer == '_')
					$$.rptr->dtype=DTYPE_IDENT;
				else if (tdi_lex_path($1.rptr->length, $1.rptr->pointer, &$$, TDITHREADSTATIC_VAR) == ERROR)
					{yyerror(TDITHREADSTATIC_VAR, "yacc_path failed"); return YY_ERR;}
				}
	| VALUE
	| textX
	| paren								/*primary parentheses*/
	| bracket ']'		{_RESOLVE($$);}				/*constructor*/
	;
	/******************************************************
	A terminal semicolon in lex makes final exp into stmt.
	Do not build a statement list unless and until needed.
	These constructs cost little and add a lot.
	******************************************************/
funvbl	: FUN ulabel				{$$=$2;}				/* FUN vbl(list)stmt	*/
	| FUN MODIF ulabel			{_JUST1($2.builtin,$3,$$);}		/* FUN PRIVATE/PUBLIC vbl(list)stmt*/
	| MODIF FUN ulabel			{_JUST1($1.builtin,$3,$$);}		/* PRIVATE/PUBLIC FUN vbl(list)stmt*/
	;
fun	: funvbl '(' sub ')'			{int j;	$$=$3;
							$$.rptr->pointer= (uint8_t *)&OpcFun;
							for (j=$$.rptr->ndesc; --j>=0;)
								$$.rptr->dscptrs[j+2]=$$.rptr->dscptrs[j];
							$$.rptr->dscptrs[0]=(mdsdsc_t *)$1.rptr;
							$$.rptr->ndesc += 2;
							++TDI_REFZONE.l_rel_path;
						}
	;
stmt0	: '`' stmt0				{$$.rptr=$2.rptr; $$.builtin= -2;
						__RUN(tdi_yacc_IMMEDIATE(&$$.rptr, TDITHREADSTATIC_VAR));}
	| BREAK	';'				{_JUST0($1.builtin,$$);}		/* BREAK/CONTINUE;	*/
	| CASE paren stmt			{_FULL2($1.builtin,$2,$3,$$);}		/* CASE(exp) stmt	*/
	| CASE DEFAULT stmt			{_FULL1($2.builtin,$3,$$);}		/* CASE DEFAULT stmt	*/
	| DO '{' stmt_lst '}' WHILE paren ';'	{_JUST2($1.builtin,$6,$3,$$);}		/* DO stmt WHILE(exp);	*/
	| FOR '(' opt ';' opt ';' opt ')' stmt	{_JUST4($1.builtin,$3,$5,$7,$9,$$);}	/* FOR(opt;opt;opt)stmt */
	| GOTO VBL ';'				{_JUST1($1.builtin,$2,$$);}		/* GOTO label;		*/
	| IF paren stmt ELSE stmt		{_JUST3($1.builtin,$2,$3,$5,$$);}	/* IF(exp)stmtELSEstmt	*/
	| IF paren stmt	%prec END		{_JUST2($1.builtin,$2,$3,$$);}		/* IF(exp)stmt		*/
	| slabel RANGE stmt			{_FULL2(OPC_LABEL,$1,$3,$$);}		/* LABEL label:stmt	*/
	| SWITCH paren stmt0			{_JUST2($1.builtin,$2,$3,$$);}		/* SWITCH(exp)stmt	*/
	| WHERE paren stmt ELSEW stmt		{_JUST3($1.builtin,$2,$3,$5,$$);}	/* WHERE(exp)stmtELSEWHEREstmt	*/
	| WHERE paren stmt %prec END		{_JUST2($1.builtin,$2,$3,$$);}		/* WHERE(exp)stmt	*/
	| WHILE paren stmt			{_JUST2($1.builtin,$2,$3,$$);}		/* WHILE(exp)stmt	*/
	| fun stmt				{TDI_REFZONE.l_rel_path--;
						$$.rptr->dscptrs[1]=(mdsdsc_t *)$2.rptr;}
	| '{' stmt_lst '}'                      {$$=$2; _RESOLVE($$);}          /* {statement list}     */
	;
stmt	: stmt0
	| opt ';'				{$$=$1;}
stmt_lst : stmt
	| stmt_lst stmt	{short opcode;
			if ($$.rptr == 0) {$$=$2;}		/* initial null statement	*/
			else if ($2.rptr == 0) {}		/* trailing null statement	*/
			else if ($$.rptr->dtype == DTYPE_FUNCTION
			&& $$.rptr->ndesc < 250
			&& ((opcode = *(opcode_t *)$$.rptr->pointer) == OPC_STATEMENT
				|| opcode == OPC_CASE
				|| opcode == OPC_DEFAULT
				|| opcode == OPC_LABEL
			)) {$$.rptr->dscptrs[$$.rptr->ndesc++]=(mdsdsc_t *)$2.rptr;}
			else	{_FULL2(OPC_STATEMENT,$1,$2,$$);}
			}
	;
program : stmt_lst	{_RESOLVE($$);		/*statements*/
			TDI_REFZONE.a_result=(struct descriptor_d *)$$.rptr;
			TDI_REFZONE.l_status=SsSUCCESS;}/* success */
	| %empty	{$$=_EMPTY_MARKER;}				/* nothing	*/
	| ERROR		{yyerror(TDITHREADSTATIC_VAR, "lex error"); return YY_ERR;}	/* LEX_ERROR	*/
	| error		{TDI_REFZONE.l_status=TdiSYNTAX; yyerror(TDITHREADSTATIC_VAR, "syntax error"); return YY_ASIS;} /* YACC error	*/
	;
%%

#define DEFINE(NAME) const int LEX_##NAME=NAME;
#include "lexdef.h"
#undef DEFINE
