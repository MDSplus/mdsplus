/* /harpodka400/sys0/syscommon/posix$bin/yacc -d /alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y */
#ifdef YYTRACE
#define YYDEBUG 1
#else
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#endif
/*
 * Portable way of defining ANSI C prototypes
 */
#ifndef YY_ARGS
#if defined(__STDC__) || defined(__cplusplus)  
#define YY_ARGS(x)	x
#define YY_FNARG1(type, name) (type name)       
#define	YY_FNARG4(t1, n1, t2, n2, t3, n3, t4, n4) \
		(t1 n1, t2 n2, t3 n3, t4 n4)
#else
#define YY_ARGS(x)	()
#define YY_FNARG1(type, name) (name) type name; 
#define YY_FNARG4(t1, n1, t2, n2, t3, n3, t4, n4) \
		(n1, n2, n3, n4) t1 n1; t2 n2; t3 n3; t4 n4;
#endif
#endif

#if YYDEBUG
typedef struct yyNamedType_tag {	/* Tokens */
	char	* name;		/* printable name */
	short	token;		/* token # */
	short	type;		/* token type */
} yyNamedType;
typedef struct yyTypedRules_tag {	/* Typed rule table */
	char	* name;		/* compressed rule string */
	short	type;		/* rule result type */
} yyTypedRules;

#endif

#line 1 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"

#ifdef __DECC
#pragma module TdiYacc TdiYacc
#endif
/*	TdiYacc.Y
	YACC converts this to TdiYacc.C to compile TDI statements.
	Each YACC-LEX symbol has a returned token and a yylval value.
	Precedence is associated with a token and is set left and right below.
	TdiLex_... routines and TdiRefFunction depend on tokens from TdiYacc.
	Tdi0Decompile depends on TdiRefFunction and precedence.

	Josh Stillerman and Thomas W. Fredian, MIT PFC, TDI$PARSE.Y.
	Ken Klare, LANL P-4	(c)1989,1990,1991

	Note statements like	{;} {a;} {a,b,c;}
	Watch -3^4*5	Fortran says -((3^4)*5), CC/high-unary-precedence would give ((-3)^4)*5
	Watch 3^-4*5	Fortran illegal, Vax 3^(-(4*5)), CC would give (3^(-4))*5
	NEED x^float	where float=int(float) to be x^int.
	NEED x^2	square(x) and x^.5 to be sqrt(x).
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
*/
#ifdef _MSC_VER
#pragma warning (disable : 4244 4102 ) /* int to short conversions and unreferenced label */
#endif

#include <mdsdescrip.h>
#include <mdsdescrip.h>
#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefzone.h"
#include <libroutines.h>
#include <strroutines.h>
#include <stdio.h>
#include <string.h>
#include <tdimessages.h>
#include <mds_stdarg.h>

extern unsigned short
	OpcAbort,
	OpcAdd,
	OpcCase,
	OpcComma,
	OpcConditional,
	OpcConcat,
	OpcDefault,
	OpcEquals,
	OpcEqualsFirst,
	OpcExtFunction,
	OpcFun,
	OpcInT,
	OpcInT_UNSIGNED,
	OpcLabel,
	OpcMultiply,
	OpcPostDec,
	OpcPostInc,
	OpcPreInc,
	OpcStatement,
	OpcSubscript,
	OpcUnaryMinus,
	OpcUnaryPlus,
	OpcUsing,
	OpcVector;

#define OPC_ABORT OpcAbort
#define OPC_ADD OpcAdd
#define OPC_CASE OpcCase
#define OPC_COMMA OpcComma
#define OPC_CONDITIONAL OpcConditional
#define OPC_CONCAT OpcConcat
#define OPC_DEFAULT OpcDefault
#define OPC_EQUALS OpcEquals
#define OPC_EQUALS_FIRST OpcEqualsFirst
#define OPC_EXT_FUNCTION OpcExtFunction
#define OPC_FUN OpcFun
#define OPC_INT OpcInT
#define OPC_INT_UNSIGNED OpcInT_UNSIGNED
#define OPC_LABEL OpcLabel
#define OPC_MULTIPLY OpcMultiply
#define OPC_POST_DEC OpcPostDec
#define OPC_POST_INC OpcPostInc
#define OPC_PRE_INC OpcPreInc
#define OPC_STATEMENT OpcStatement
#define OPC_SUBSCRIPT OpcSubscript
#define OPC_UNARY_MINUS OpcUnaryMinus
#define OPC_UNARY_PLUS OpcUnaryPlus
#define OPC_USING OpcUsing
#define OPC_VECTOR OpcVector

#define str_upcase StrUpcase
#define str_concat StrConcat

#define dsc_descriptor descriptor
#define dsc_descriptor_d descriptor_d
#define dsc_descriptor_range descriptor_range

#define dsc_w_length length
#define dsc_b_dtype dtype
#define dsc_b_class class
#define dsc_b_ndesc ndesc
#define dsc_a_pointer pointer
#define dsc_a_dscptrs dscptrs

#define DSC_K_DTYPE_IDENT DTYPE_IDENT
#define DSC_K_DTYPE_T DTYPE_T
#define DSC_K_DTYPE_CALL DTYPE_CALL
#define DSC_K_DTYPE_RANGE DTYPE_RANGE
#define DSC_K_DTYPE_FUNCTION DTYPE_FUNCTION

#define DSC_K_CLASS_R CLASS_R

#define TDI__SYNTAX TdiSYNTAX
#define TDI__EXTRA_ARG TdiEXTRA_ARG
#define TDI_K_CAT_MAX TdiCAT_MAX
#define TDI__INVDTYDSC TdiINVDTYDSC

#define lib_get_vm LibGetVm

#define _MAKE_S MAKE_S
struct TdiZoneStruct TdiRefZone = {0};

extern int TdiYacc_RESOLVE();
extern int TdiLex();
extern int TdiYacc_IMMEDIATE();
extern int TdiYacc_BUILD();
extern int TdiYacc_ARG();
extern int TdiLexPath();

#define YYMAXDEPTH	250
#define _LEX_PATH	TdiLexPath
#define _REF_CAT	TdiREF_CAT
#define _REF_FUNCTION	TdiRefFunction
#define _ref_zone	TdiRefZone
#define _arg		TdiYacc_ARG
#define _build		TdiYacc_BUILD
#define _immediate	TdiYacc_IMMEDIATE
/*define _knot1		TdiYacc_KNOT1 removed 9/25/89*/
/*define _knot2		TdiYacc_KNOT2 removed 9/25/89*/
#define _knot1(arg)
#define _knot2(arg)
#define yyparse		TdiYacc
#define _RESOLVE(arg)	if (TdiYacc_RESOLVE(&arg.rptr)) {yyerror(0);}
#define _FULL1(opcode,arg1,out)			if (_build(255, 1, opcode, &out, &arg1)) {yyerror(0);}
#define _FULL2(opcode,arg1,arg2,out)		if (_build(255, 2, opcode, &out, &arg1, &arg2)) {yyerror(0);}
	/*****************************
	Two args for image->routine.
	*****************************/
#define _JUST0(opcode,out)			if (_build(2, 0, opcode, &out)) {yyerror(0);}
#define _JUST1(opcode,arg1,out)			if (_build(3, 1, opcode, &out, &arg1)) {yyerror(0);}
#define _JUST2(opcode,arg1,arg2,out)		if (_build(2, 2, opcode, &out, &arg1, &arg2)) {yyerror(0);}
#define _JUST3(opcode,arg1,arg2,arg3,out)	if (_build(3, 3, opcode, &out, &arg1, &arg2, &arg3)) {yyerror(0);}
#define _JUST4(opcode,arg1,arg2,arg3,arg4,out)	if (_build(4, 4, opcode, &out, &arg1, &arg2, &arg3, &arg4)) {yyerror(0);}

static struct marker _EMPTY_MARKER = {0,0};

typedef union {struct marker mark;} YYSTYPE;
#define ERROR	257
#define IDENT	258
#define POINT	259
#define TEXT	260
#define VALUE	261
#define BREAK	262
#define CASE	263
#define COND	264
#define DEFAULT	265
#define DO	266
#define ELSE	267
#define ELSEW	268
#define FOR	269
#define GOTO	270
#define IF	271
#define LABEL	272
#define RETURN	273
#define SIZEOF	274
#define SWITCH	275
#define USING	276
#define WHERE	277
#define WHILE	278
#define ARG	279
#define CAST	280
#define CONST	281
#define INC	282
#define ADD	283
#define CONCAT	284
#define IAND	285
#define IN	286
#define IOR	287
#define IXOR	288
#define LEQV	289
#define POWER	290
#define PROMO	291
#define RANGE	292
#define SHIFT	293
#define BINEQ	294
#define LAND	295
#define LEQ	296
#define LGE	297
#define LOR	298
#define MUL	299
#define UNARY	300
#define LANDS	301
#define LEQS	302
#define LGES	303
#define LORS	304
#define MULS	305
#define UNARYS	306
#define FUN	307
#define MODIF	308
#define VBL	309
extern int yychar, yyerrflag;
extern YYSTYPE yylval;
#if YYDEBUG
yyTypedRules yyRules[] = {
	{ "&00: %01 &00",  0},
	{ "%04: &17 &54",  1},
	{ "%05: &26",  1},
	{ "%05: &03",  1},
	{ "%05: &25",  1},
	{ "%05: &10",  1},
	{ "%05: &15",  1},
	{ "%05: &19",  1},
	{ "%05: &45",  1},
	{ "%05: &07",  1},
	{ "%05: &12",  1},
	{ "%05: &13",  1},
	{ "%05: &08",  1},
	{ "%05: &11",  1},
	{ "%05: &18",  1},
	{ "%05: &14",  1},
	{ "%05: &16",  1},
	{ "%05: &17",  1},
	{ "%05: &20",  1},
	{ "%05: &21",  1},
	{ "%05: &22",  1},
	{ "%05: &23",  1},
	{ "%05: &31",  1},
	{ "%05: &40",  1},
	{ "%05: &41",  1},
	{ "%05: &34",  1},
	{ "%05: &42",  1},
	{ "%05: &43",  1},
	{ "%05: &44",  1},
	{ "%05: &53",  1},
	{ "%05: &52",  1},
	{ "%05: &54",  1},
	{ "%11: &56 %11",  1},
	{ "%11: %13 &57 %11",  1},
	{ "%11: %13 &39 %11",  1},
	{ "%11: %11 &37 %11",  1},
	{ "%11: %11 &58 %11 &37 %11",  1},
	{ "%11: %11 &36 %11",  1},
	{ "%11: %11 &34 %11",  1},
	{ "%11: %11 &43 %11",  1},
	{ "%11: %11 &49 %11",  1},
	{ "%11: %11 &40 %11",  1},
	{ "%11: %11 &46 %11",  1},
	{ "%11: %11 &32 %11",  1},
	{ "%11: %11 &33 %11",  1},
	{ "%11: %11 &30 %11",  1},
	{ "%11: %11 &41 %11",  1},
	{ "%11: %11 &47 %11",  1},
	{ "%11: %11 &42 %11",  1},
	{ "%11: %11 &48 %11",  1},
	{ "%11: %11 &31 %11",  1},
	{ "%11: %11 &29 %11",  1},
	{ "%11: %11 &38 %11",  1},
	{ "%11: %11 &28 %11",  1},
	{ "%11: %11 &44 %11",  1},
	{ "%11: %11 &50 %11",  1},
	{ "%11: %11 &59 %11",  1},
	{ "%11: %11 &35 %11",  1},
	{ "%11: %13",  1},
	{ "%11: &59",  1},
	{ "%13: &28 %13",  1},
	{ "%13: &45 %13",  1},
	{ "%13: &51 %13",  1},
	{ "%13: &27 %13",  1},
	{ "%13: %14",  1},
	{ "%16: &60 %11",  1},
	{ "%16: &60",  1},
	{ "%16: %16 &55 %11",  1},
	{ "%10: %12 &55 %12",  1},
	{ "%10: %11",  1},
	{ "%18: %10",  1},
	{ "%18:",  1},
	{ "%17: &61 %10 &62",  1},
	{ "%17: &61 %02 &62",  1},
	{ "%07: &21 &61",  1},
	{ "%06: %07 %11 &55 %11 &55",  1},
	{ "%06: %07 %11 &55 &55",  1},
	{ "%12: %10",  1},
	{ "%12:",  1},
	{ "%14: %15",  1},
	{ "%14: %14 &60 %18 &63",  1},
	{ "%14: %14 &27",  1},
	{ "%14: %05 &61 %18 &62",  1},
	{ "%14: %05 &04 &61 %18 &62",  1},
	{ "%14: %05 &04 %05 &61 %18 &62",  1},
	{ "%14: %06 %18 &62",  1},
	{ "%14: %07 %11 &55 %11 &62",  1},
	{ "%19: &05",  1},
	{ "%19: %19 &05",  1},
	{ "%15: &53 &54",  1},
	{ "%15: &53 &53 &54",  1},
	{ "%15: %05",  1},
	{ "%15: &06",  1},
	{ "%15: %19",  1},
	{ "%15: %17",  1},
	{ "%15: %16 &63",  1},
	{ "%09: &52 %05",  1},
	{ "%09: &52 &53 %05",  1},
	{ "%09: &53 &52 %05",  1},
	{ "%08: %09 &61 %18 &62",  1},
	{ "%03: &07 &64",  1},
	{ "%03: &08 %17 %03",  1},
	{ "%03: &08 &10 %03",  1},
	{ "%03: &11 &65 %02 &66 &23 %17 &64",  1},
	{ "%03: &14 &61 %12 &64 %12 &64 %12 &62 %03",  1},
	{ "%03: &15 &54 &64",  1},
	{ "%03: &16 %17 %03 &12 %03",  1},
	{ "%03: &16 %17 %03",  1},
	{ "%03: %04 &37 %03",  1},
	{ "%03: &20 %17 %03",  1},
	{ "%03: &22 %17 %03 &13 %03",  1},
	{ "%03: &22 %17 %03",  1},
	{ "%03: &23 %17 %03",  1},
	{ "%03: %08 %03",  1},
	{ "%03: &56 %03",  1},
	{ "%03: &65 %02 &66",  1},
	{ "%03: %12 &64",  1},
	{ "%02: %03",  1},
	{ "%02: %02 %03",  1},
	{ "%01: %02",  1},
	{ "%01:",  1},
	{ "%01: &02",  1},
	{ "%01: &01",  1},
{ "$accept",  0},{ "error",  0}
};
yyNamedType yyTokenTypes[] = {
	{ "$end",  0,  0},
	{ "error",  256,  0},
	{ "ERROR",  257,  1},
	{ "IDENT",  258,  1},
	{ "POINT",  259,  1},
	{ "TEXT",  260,  1},
	{ "VALUE",  261,  1},
	{ "BREAK",  262,  1},
	{ "CASE",  263,  1},
	{ "COND",  264,  1},
	{ "DEFAULT",  265,  1},
	{ "DO",  266,  1},
	{ "ELSE",  267,  1},
	{ "ELSEW",  268,  1},
	{ "FOR",  269,  1},
	{ "GOTO",  270,  1},
	{ "IF",  271,  1},
	{ "LABEL",  272,  1},
	{ "RETURN",  273,  1},
	{ "SIZEOF",  274,  1},
	{ "SWITCH",  275,  1},
	{ "USING",  276,  1},
	{ "WHERE",  277,  1},
	{ "WHILE",  278,  1},
	{ "ARG",  279,  1},
	{ "CAST",  280,  1},
	{ "CONST",  281,  1},
	{ "INC",  282,  1},
	{ "ADD",  283,  1},
	{ "CONCAT",  284,  1},
	{ "IAND",  285,  1},
	{ "IN",  286,  1},
	{ "IOR",  287,  1},
	{ "IXOR",  288,  1},
	{ "LEQV",  289,  1},
	{ "POWER",  290,  1},
	{ "PROMO",  291,  1},
	{ "RANGE",  292,  1},
	{ "SHIFT",  293,  1},
	{ "BINEQ",  294,  1},
	{ "LAND",  295,  1},
	{ "LEQ",  296,  1},
	{ "LGE",  297,  1},
	{ "LOR",  298,  1},
	{ "MUL",  299,  1},
	{ "UNARY",  300,  1},
	{ "LANDS",  301,  1},
	{ "LEQS",  302,  1},
	{ "LGES",  303,  1},
	{ "LORS",  304,  1},
	{ "MULS",  305,  1},
	{ "UNARYS",  306,  1},
	{ "FUN",  307,  1},
	{ "MODIF",  308,  1},
	{ "VBL",  309,  1},
	{ "','",  44,  0},
	{ "'`'",  96,  0},
	{ "'='",  61,  0},
	{ "'?'",  63,  0},
	{ "'*'",  42,  0},
	{ "'['",  91,  0},
	{ "'('",  40,  0},
	{ "')'",  41,  0},
	{ "']'",  93,  0},
	{ "';'",  59,  0},
	{ "'{'",  123,  0},
	{ "'}'",  125,  0}

};
#endif
static short yydef[] = {

	  -1,   -9,  -17,  -23,  121,  -29,   35,  -35,   30,  123, 
	 122,  124,  -41,  112,  111,  103,  102,  101,  100,   98, 
	  97,   95,   94,   91,   90,   88,   99,  -47,  -51,  -57, 
	 -63,  -69,  111,  -77,  -83,  114,   34,   29,  -89,    3, 
	 111,  -95, -101, -107, -113, -119, -125, -131, -137,    3, 
	-143,   31,   28,   27,   26,   25,   24,   23,   22,   21, 
	  20,   19,   18,   17,   16,   15,   14,   13,   12,   11, 
	  10,    9,    8,    6,    5,    4,   37,   36, -149, -155, 
	-161, -167, -173,    7, -179, -185
};
static short yyex[] = {

	   0,   39,   44,   34,   59,   34,   -1,    1,    0,   38, 
	  44,   34,   59,   34,   -1,    1,   44,   34,   59,   34, 
	  -1,    1,   44,   34,   59,   34,   -1,    1,   44,   34, 
	  41,   33,   -1,    1,   44,   34,   59,   34,   -1,    1, 
	  44,   34,   59,   34,   -1,    1,    0,    0,   -1,    1, 
	  44,   34,   59,   34,   -1,    1,   44,   34,   59,   34, 
	  -1,    1,   44,   34,   41,   33,   -1,    1,   44,  114, 
	  41,   32,   93,   32,   -1,    1,   44,   34,   41,   33, 
	  -1,    1,   44,   34,   59,   34,   -1,    1,   44,   34, 
	  93,   33,   -1,    1,   44,   34,   59,   34,   -1,    1, 
	  44,   34,   59,   34,   -1,    1,   44,   34,   59,   34, 
	  -1,    1,   44,   34,   59,   34,   -1,    1,   44,   34, 
	  59,   34,   -1,    1,   44,   34,   59,   34,   -1,    1, 
	  44,   34,   59,   34,   -1,    1,   44,   34,   59,   34, 
	  -1,    1,   44,   34,   41,   33,   -1,    1,   44,   34, 
	  59,   34,   -1,    1,   44,   34,   41,   33,   -1,    1, 
	  44,   34,   59,   34,   -1,    1,   44,   34,   59,   34, 
	  -1,    1,   44,   34,   59,   34,   -1,    1,   44,   34, 
	  41,   34,   -1,    1,   44,   34,   59,   34,   -1,    1
};
static short yyact[] = {

	  -8, -187,   -9,  -13,   -3, -145, -146, -190, -221, -222, 
	 -24,  -23, -192,  -22, -197, -198,  -21,  -26,  -20,  -27, 
	-201, -194,  -19,  -18,  -17,  -16, -191, -189, -139, -137, 
	-209, -212, -210, -211, -213, -214, -215,  -25, -138,  -14, 
	 -15, -218,  309,  308,  307,  306,  300,  299,  298,  297, 
	 296,  295,  289,  286,  283,  282,  281,  280,  278,  277, 
	 276,  275,  274,  273,  272,  271,  270,  269,  268,  267, 
	 266,  265,  263,  262,  261,  260,  258,  257,  256,  123, 
	  96,   91,   42,   40,   -8, -187,   -9,  -13,   -3, -190, 
	-221, -222,  -24,  -23, -192,  -22, -197, -198,  -21,  -26, 
	 -20,  -27, -201, -194,  -19,  -18,  -17,  -16, -191, -189, 
	-139, -137, -209, -212, -210, -211, -213, -214, -215,  -25, 
	-138,  -14,  -15, -218,  309,  308,  307,  306,  300,  299, 
	 298,  297,  296,  295,  289,  286,  283,  282,  281,  280, 
	 278,  277,  276,  275,  274,  273,  272,  271,  270,  269, 
	 268,  267,  266,  265,  263,  262,  261,  260,  258,  123, 
	  96,   91,   42,   40,  -30,  292,  -31,   40, -169,  260, 
	  -8, -187,   -9, -134, -190, -221, -222, -196, -199, -192, 
	-200, -197, -198, -202, -193, -203, -204, -201, -194, -205, 
	 -18, -207, -208, -191, -189, -139, -137, -209, -212, -210, 
	-211, -213, -214, -215,  -25, -138, -217,  -33, -218,  309, 
	 308,  307,  306,  300,  299,  298,  297,  296,  295,  289, 
	 286,  283,  282,  281,  280,  278,  277,  276,  275,  274, 
	 273,  272,  271,  270,  269,  268,  267,  266,  265,  263, 
	 262,  261,  260,  258,   96,   91,   42,   40,  -34, -133, 
	 259,   40,  -37, -224,   59,   44, -131, -166,   93,   44, 
	 -39, -175,  282,   91,   -8,   -9, -190, -221, -222, -196, 
	-199, -192, -200, -197, -198, -202, -193, -203, -204, -201, 
	-194, -205,  -18, -207, -208, -191, -189, -139, -137, -209, 
	-212, -210, -211, -213, -214, -215,  -25, -138, -217,  -33, 
	-218,  309,  308,  307,  306,  300,  299,  298,  297,  296, 
	 295,  289,  286,  283,  282,  281,  280,  278,  277,  276, 
	 275,  274,  273,  272,  271,  270,  269,  268,  267,  266, 
	 265,  263,  262,  261,  260,  258,   91,   40, -129, -109, 
	-126, -124, -118, -123, -116, -117, -111, -130, -110, -108, 
	-125, -114, -119, -121, -112, -127, -115, -120, -122, -113, 
	-128,  305,  304,  303,  302,  301,  299,  298,  297,  296, 
	 295,  293,  292,  291,  290,  289,  288,  287,  286,  285, 
	 284,  283,   63,   42, -106, -107,  294,   61, -190, -196, 
	-199, -192, -200, -197, -198, -202, -193, -203, -204, -201, 
	-194, -205, -206, -207, -208, -191, -189, -209, -212, -210, 
	-211, -213, -214, -215, -195, -217,  -41, -218,  309,  308, 
	 307,  300,  299,  298,  297,  296,  295,  289,  286,  281, 
	 280,  278,  277,  276,  275,  274,  273,  272,  271,  270, 
	 269,  268,  267,  266,  265,  263,  262,  258, -105, -104, 
	-168,  309,  308,  307,   -8,   40, -179,   40,  -46,   40, 
	 -47,  123,   -8,  -48,  265,   40, -161,   59, -103,  309, 
	-188,  309,   -8, -187,   -9,  -13,   -3, -148, -190, -221, 
	-222,  -24,  -23, -192,  -22, -197, -198,  -21,  -26,  -20, 
	 -27, -201, -194,  -19,  -18,  -17,  -16, -191, -189, -139, 
	-137, -209, -212, -210, -211, -213, -214, -215,  -25, -138, 
	 -14,  -15, -218,  309,  308,  307,  306,  300,  299,  298, 
	 297,  296,  295,  289,  286,  283,  282,  281,  280,  278, 
	 277,  276,  275,  274,  273,  272,  271,  270,  269,  268, 
	 267,  266,  265,  263,  262,  261,  260,  258,  125,  123, 
	  96,   91,   42,   40, -171,   41,  -37,   44, -104, -168, 
	 309,  308,  -51, -190, -196, -199, -192, -200, -197, -198, 
	-202, -193, -203, -204, -201, -194, -205, -206, -207, -208, 
	-191, -189, -209, -212, -210, -211, -213, -214, -215, -195, 
	-217, -216, -218,  309,  308,  307,  300,  299,  298,  297, 
	 296,  295,  289,  286,  281,  280,  278,  277,  276,  275, 
	 274,  273,  272,  271,  270,  269,  268,  267,  266,  265, 
	 263,  262,  258,   40, -129,  -99, -109, -126, -124, -118, 
	-123, -116, -117, -111, -130, -110, -108, -125, -114, -119, 
	-121, -112, -127, -115, -120, -122, -113, -128,  305,  304, 
	 303,  302,  301,  299,  298,  297,  296,  295,  293,  292, 
	 291,  290,  289,  288,  287,  286,  285,  284,  283,   63, 
	  44,   42,   -8, -180, -187,   -9,  -13,   -3, -190, -221, 
	-222,  -24,  -23, -192,  -22, -197, -198,  -21,  -26,  -20, 
	 -27, -201, -194,  -19,  -18,  -17,  -16, -191, -189, -139, 
	-137, -209, -212, -210, -211, -213, -214, -215,  -25, -138, 
	 -14,  -15, -218,  309,  308,  307,  306,  300,  299,  298, 
	 297,  296,  295,  289,  286,  283,  282,  281,  280,  278, 
	 277,  276,  275,  274,  273,  272,  271,  270,  269,  268, 
	 267,  266,  265,  263,  262,  261,  260,  258,  123,   96, 
	  91,   42,   41,   40, -181,   41, -190, -196, -199, -192, 
	-200, -197, -198, -202, -193, -203, -204, -201, -194, -205, 
	-206, -207, -208, -191, -189, -209, -212, -210, -211, -213, 
	-214, -215, -195, -217, -216, -218,  309,  308,  307,  300, 
	 299,  298,  297,  296,  295,  289,  286,  281,  280,  278, 
	 277,  276,  275,  274,  273,  272,  271,  270,  269,  268, 
	 267,  266,  265,  263,  262,  258, -167,  309, -156,   59, 
	-162,   41,  -80,   40, -174,   41,   -8, -187, -177,   -9, 
	-134, -190, -221, -222, -196, -199, -192, -200, -197, -198, 
	-202, -193, -203, -204, -201, -194, -205,  -18, -207, -208, 
	-191, -189, -139, -137, -209, -212, -210, -211, -213, -214, 
	-215,  -25, -138, -217,  -33, -218,  309,  308,  307,  306, 
	 300,  299,  298,  297,  296,  295,  289,  286,  283,  282, 
	 281,  280,  278,  277,  276,  275,  274,  273,  272,  271, 
	 270,  269,  268,  267,  266,  265,  263,  262,  261,  260, 
	 258,   96,   91,   44,   42,   40, -176,   93, -130,  290, 
	-129, -130, -127, -128,  305,  299,  290,   42, -129, -126, 
	-130, -127, -128,  305,  299,  290,  283,   42, -129, -126, 
	-130, -125, -127, -128,  305,  299,  293,  290,  283,   42, 
	-129, -126, -124, -130, -125, -127, -128,  305,  299,  293, 
	 290,  284,  283,   42, -129, -126, -124, -123, -130, -125, 
	-127, -128,  305,  299,  293,  290,  286,  284,  283,   42, 
	-129, -126, -124, -123, -130, -125, -121, -127, -122, -128, 
	 305,  303,  299,  297,  293,  290,  286,  284,  283,   42, 
	-129, -126, -124, -123, -130, -125, -119, -121, -127, -120, 
	-122, -128,  305,  303,  302,  299,  297,  296,  293,  290, 
	 286,  284,  283,   42, -129, -126, -124, -118, -123, -130, 
	-125, -119, -121, -127, -120, -122, -128,  305,  303,  302, 
	 299,  297,  296,  293,  290,  286,  285,  284,  283,   42, 
	-129, -126, -124, -118, -123, -117, -130, -125, -119, -121, 
	-127, -120, -122, -128,  305,  303,  302,  299,  297,  296, 
	 293,  290,  288,  286,  285,  284,  283,   42, -129, -126, 
	-124, -118, -123, -116, -117, -130, -125, -119, -121, -127, 
	-120, -122, -128,  305,  303,  302,  299,  297,  296,  293, 
	 290,  288,  287,  286,  285,  284,  283,   42, -129, -126, 
	-124, -118, -123, -116, -117, -130, -125, -114, -119, -121, 
	-127, -115, -120, -122, -128,  305,  303,  302,  301,  299, 
	 297,  296,  295,  293,  290,  288,  287,  286,  285,  284, 
	 283,   42, -129, -126, -124, -118, -123, -116, -117, -130, 
	-125, -114, -119, -121, -112, -127, -115, -120, -122, -113, 
	-128,  305,  304,  303,  302,  301,  299,  298,  297,  296, 
	 295,  293,  290,  288,  287,  286,  285,  284,  283,   42, 
	-129, -126, -124, -118, -123, -116, -117, -111, -130, -110, 
	-125, -114, -119, -121, -112, -127, -115, -120, -122, -113, 
	-128,  305,  304,  303,  302,  301,  299,  298,  297,  296, 
	 295,  293,  291,  290,  289,  288,  287,  286,  285,  284, 
	 283,   42, -129, -109, -126, -124, -118, -123, -116, -117, 
	-111, -130, -110,  -93, -125, -114, -119, -121, -112, -127, 
	-115, -120, -122, -113, -128,  305,  304,  303,  302,  301, 
	 299,  298,  297,  296,  295,  293,  292,  291,  290,  289, 
	 288,  287,  286,  285,  284,  283,   63,   42,  -81,  268, 
	 -82,  267,  -37,  -83,   59,   44,   -8, -187,   -9,  -13, 
	  -3,  -92, -190, -221, -222,  -24,  -23, -192,  -22, -197, 
	-198,  -21,  -26,  -20,  -27, -201, -194,  -19,  -18,  -17, 
	 -16, -191, -189, -139, -137, -209, -212, -210, -211, -213, 
	-214, -215,  -25, -138,  -14,  -15, -218,  309,  308,  307, 
	 306,  300,  299,  298,  297,  296,  295,  289,  286,  283, 
	 282,  281,  280,  278,  277,  276,  275,  274,  273,  272, 
	 271,  270,  269,  268,  267,  266,  265,  263,  262,  261, 
	 260,  258,  125,  123,   96,   91,   42,   40, -173,   41, 
	-170, -129, -178, -109, -126, -124, -118, -123, -116, -117, 
	-111, -130, -110, -108, -125, -114, -119, -121, -112, -127, 
	-115, -120, -122, -113, -128,  305,  304,  303,  302,  301, 
	 299,  298,  297,  296,  295,  293,  292,  291,  290,  289, 
	 288,  287,  286,  285,  284,  283,   63,   44,   42,   41, 
	 -89,  278, -172,   41,  -37,  -85,   59,   44, -158,   59, 
	 -86,  -37,   44,   41,   -1
};
static short yypact[] = {

	  42,  124,  124,  124,  169,  209,  250,  124,  209,  262, 
	 361,  386,  124,  418,  451,  455,  455,  457,  455,  455, 
	 459,  461,  464,  467,  301,  469,  471,  472,  513,  124, 
	 209,  556,  560,  209,  713,  755,  209,  361,  209,  361, 
	 786,  124,  124,  124,  124,  209,  124,  124,  124,  361, 
	 209,  361,  909,  909,  909,  909,  914,  923,  934,  947, 
	 962,  962,  980,  980, 1002, 1027, 1054, 1083, 1083, 1115, 
	1115, 1151, 1191,  361,  361,  361, 1259, 1261, 1307,  209, 
	 124,  124,  209,  361,  209,  124, 1412, 1409,  455, 1406, 
	1403, 1401,  209, 1375, 1349, 1264, 1235,  907,  866,  825, 
	 823,  821,  819,  817,  786,  209,  209,  209,  209,  209, 
	 209,  209,  209,  209,  209,  209,  209,  209,  209,  209, 
	 209,  209,  209,  209,  209,  209,  209,  209,  209,  209, 
	 209,  648,  593,  209,  557,  555,  301,  301,  301,  258, 
	 254,  209,  167,  165
};
static short yygo[] = {

	 -28,  -29,  -35,  -79,   -2,   46,    7,    2, -147, -150, 
	-149, -147, -154, -147, -151,  -77, -153,  -78, -159, -160, 
	-147, -152, -155, -157, -225,   85,   81,   80,   78,   48, 
	  47,   44,   43,   42,   41,   34,   29,   28,   12,    3, 
	   1, -144, -165, -164, -163, -101,   -7,  132,  104,   40, 
	  13,   -6, -142,   -4, -143,  -32,  -36,  -32,  -32,  -32, 
	 -32,  -32, -219,   79,   50,   38,   33,   30,    7,    5, 
	 -38,  -40,  -84,  -94,  -76,  -75,  -74,  -97,  -73,  -72, 
	 -71,  -70,  -69,  -68,  -67,  -66,  -65,  -64,  -63,  -62, 
	 -61,  -60,  -59,  -58,  -57,  -56,  -55,  -54,  -53,  -52, 
	 -50, -132,  -11,  141,  133,  130,  129,  128,  127,  126, 
	 125,  124,  123,  122,  121,  120,  119,  118,  117,  116, 
	 115,  114,  113,  112,  111,  110,  109,  108,  107,  106, 
	 105,   98,   92,   12,    8, -135, -135, -135, -182, -135, 
	 -96, -135, -135,  -90,  -87, -141,   84,   82,   79,   50, 
	  45,   38,   36,   33,   30,    5, -185, -186, -184, -183, 
	 -12,  138,  137,  136,   24,  -10, -220, -140,  -42,  -43, 
	 -44,  -45,  -49,  -88, -223,   88,   22,   19,   18,   16, 
	  15, -102, -100,  -98,  -95,  -91, -136,   79,   50,   38, 
	  33,   30,   -5,   -1
};
static short yypgo[] = {

	   0,    0,    0,  102,  102,  102,  102,  102,  102,  102, 
	 102,  102,  102,  102,  102,  102,  102,  102,  102,  102, 
	 102,  102,  102,  102,  102,  102,  102,  102,  102,  167, 
	 167,  167,  186,  186,  145,  166,   24,   24,    0,    0, 
	   0,    0,    4,   24,   24,   24,   24,   24,   24,   24, 
	  24,   24,   24,   24,   24,   24,   24,   53,   54,   54, 
	  54,  166,  166,  166,  192,  165,  165,  165,  165,  165, 
	 165,  165,   51,   51,   52,  174,  174,   62,  160,  160, 
	 160,  160,  102,   41,   46,   46,   46,   46,   46,   46, 
	  46,   46,   46,   46,   46,   46,   46,   46,   46,   46, 
	  46,   46,   46,   46,   46,   46,   46,   46,   46,   46, 
	  46,   46,   46,   46,  145,  165,  192,  166,  166,   24, 
	   4,  166,   62,  160,  102,    0
};
static short yyrlen[] = {

	   0,    0,    0,    2,    3,    3,    3,    5,    3,    3, 
	   3,    3,    3,    3,    3,    3,    3,    3,    3,    3, 
	   3,    3,    3,    3,    3,    3,    3,    3,    3,    2, 
	   1,    3,    1,    0,    0,    1,    3,    3,    1,    0, 
	   1,    1,    2,    3,    2,    2,    3,    5,    3,    3, 
	   5,    3,    9,    7,    3,    3,    2,    4,    3,    3, 
	   2,    2,    3,    2,    2,    5,    3,    6,    5,    4, 
	   2,    4,    4,    5,    2,    3,    3,    3,    2,    2, 
	   2,    2,    1,    2,    1,    1,    1,    1,    1,    1, 
	   1,    1,    1,    1,    1,    1,    1,    1,    1,    1, 
	   1,    1,    1,    1,    1,    1,    1,    1,    1,    1, 
	   1,    1,    1,    1,    1,    1,    1,    1,    1,    2, 
	   1,    1,    1,    1,    1,    2
};
#define YYS0	0
#define YYDELTA	104
#define YYNPACT	144
#define YYNDEF	86

#define YYr123	0
#define YYr124	1
#define YYr125	2
#define YYr32	3
#define YYr33	4
#define YYr34	5
#define YYr35	6
#define YYr36	7
#define YYr37	8
#define YYr38	9
#define YYr39	10
#define YYr40	11
#define YYr41	12
#define YYr42	13
#define YYr43	14
#define YYr44	15
#define YYr45	16
#define YYr46	17
#define YYr47	18
#define YYr48	19
#define YYr49	20
#define YYr50	21
#define YYr51	22
#define YYr52	23
#define YYr53	24
#define YYr54	25
#define YYr55	26
#define YYr56	27
#define YYr57	28
#define YYr65	29
#define YYr66	30
#define YYr67	31
#define YYr70	32
#define YYr71	33
#define YYr78	34
#define YYr91	35
#define YYr107	36
#define YYr111	37
#define YYr119	38
#define YYr120	39
#define YYr122	40
#define YYr121	41
#define YYr118	42
#define YYr115	43
#define YYr114	44
#define YYr113	45
#define YYr112	46
#define YYr110	47
#define YYr109	48
#define YYr108	49
#define YYr106	50
#define YYr105	51
#define YYr104	52
#define YYr103	53
#define YYr102	54
#define YYr101	55
#define YYr100	56
#define YYr99	57
#define YYr98	58
#define YYr97	59
#define YYr96	60
#define YYr95	61
#define YYr90	62
#define YYr89	63
#define YYr88	64
#define YYr86	65
#define YYr85	66
#define YYr84	67
#define YYr83	68
#define YYr82	69
#define YYr81	70
#define YYr80	71
#define YYr76	72
#define YYr75	73
#define YYr74	74
#define YYr73	75
#define YYr72	76
#define YYr68	77
#define YYr63	78
#define YYr62	79
#define YYr61	80
#define YYr60	81
#define YYr59	82
#define YYr1	83
#define YYrACCEPT	YYr123
#define YYrERROR	YYr124
#define YYrLR2	YYr125
#if YYDEBUG
char * yystoken[] = {

	"error",
	"ERROR",
	"IDENT",
	"POINT",
	"TEXT",
	"VALUE",
	"BREAK",
	"CASE",
	"COND",
	"DEFAULT",
	"DO",
	"ELSE",
	"ELSEW",
	"FOR",
	"GOTO",
	"IF",
	"LABEL",
	"RETURN",
	"SIZEOF",
	"SWITCH",
	"USING",
	"WHERE",
	"WHILE",
	"ARG",
	"CAST",
	"CONST",
	"INC",
	"ADD",
	"CONCAT",
	"IAND",
	"IN",
	"IOR",
	"IXOR",
	"LEQV",
	"POWER",
	"PROMO",
	"RANGE",
	"SHIFT",
	"BINEQ",
	"LAND",
	"LEQ",
	"LGE",
	"LOR",
	"MUL",
	"UNARY",
	"LANDS",
	"LEQS",
	"LGES",
	"LORS",
	"MULS",
	"UNARYS",
	"FUN",
	"MODIF",
	"VBL",
	0
};
char * yysvar[] = {
	"$accept",
	"program",
	"stmt_lst",
	"stmt",
	"slabel",
	"label",
	"using",
	"using0",
	"fun",
	"funvbl",
	"exp",
	"ass",
	"opt",
	"unaryX",
	"postX",
	"primaX",
	"bracket",
	"paren",
	"sub",
	"textX",
	0
};
short yyrmap[] = {

	 123,  124,  125,   32,   33,   34,   35,   36,   37,   38, 
	  39,   40,   41,   42,   43,   44,   45,   46,   47,   48, 
	  49,   50,   51,   52,   53,   54,   55,   56,   57,   65, 
	  66,   67,   70,   71,   78,   91,  107,  111,  119,  120, 
	 122,  121,  118,  115,  114,  113,  112,  110,  109,  108, 
	 106,  105,  104,  103,  102,  101,  100,   99,   98,   97, 
	  96,   95,   90,   89,   88,   86,   85,   84,   83,   82, 
	  81,   80,   76,   75,   74,   73,   72,   68,   63,   62, 
	  61,   60,   59,    1,    2,    3,    4,    5,    6,    7, 
	   8,    9,   10,   11,   12,   13,   14,   15,   16,   17, 
	  18,   19,   20,   21,   22,   23,   24,   25,   26,   27, 
	  28,   29,   30,   31,   77,   79,   87,   92,   94,  116, 
	 117,   93,   69,   64,   58,    0
};
short yysmap[] = {

	   0,    3,    5,    6,   11,   13,   14,   18,   21,   22, 
	  27,   28,   29,   31,   32,   40,   41,   42,   43,   44, 
	  45,   47,   48,   51,   52,   54,   59,   60,   62,   64, 
	  65,   68,   72,   84,   86,   87,   89,   92,   94,  124, 
	 126,  132,  133,  135,  136,  137,  138,  139,  140,  149, 
	 151,  158,  160,  161,  162,  163,  164,  165,  166,  167, 
	 168,  169,  170,  171,  172,  173,  174,  175,  176,  177, 
	 178,  179,  180,  182,  183,  184,  189,  191,  193,  198, 
	 205,  206,  207,  213,  219,  223,  221,  220,  217,  216, 
	 209,  208,  204,  202,  199,  192,  181,  159,  154,  153, 
	 150,  147,  143,  130,  129,  122,  121,  120,  119,  118, 
	 117,  116,  115,  114,  113,  112,  111,  110,  109,  108, 
	 107,  106,  105,  104,  103,  102,  101,  100,   99,   98, 
	  91,   85,   83,   70,   69,   67,   25,   24,   23,   20, 
	  19,   17,    8,    7,    1,    2,   61,  145,  123,   63, 
	 188,  214,  190,  146,  215,  196,  224,  222,  194,  195, 
	 141,  197,  186,  185,  125,   90,  187,  131,   66,  211, 
	 148,  218,  210,  200,   93,  203,  201,  212,  134,  155, 
	 156,  157,   95,   96,  142,   97,   26,  144,   58,   57, 
	  56,   55,   82,   53,  128,   81,   50,   49,   80,   79, 
	  46,   78,   77,   76,   75,  127,   74,   73,   39,   38, 
	  37,   36,   35,   34,   33,  152,   71,   30,   16,   15, 
	  12,   10,    9,   88,    4
};
int yyntoken = 67;
int yynvar = 20;
int yynstate = 225;
int yynrule = 126;
#endif

#if YYDEBUG
/*
 * Package up YACC context for tracing
 */
typedef struct yyTraceItems_tag {
	int	state, lookahead, errflag, done;
	int	rule, npop;
	short	* states;
	int	nstates;
	YYSTYPE * values;
	int	nvalues;
	short	* types;
} yyTraceItems;
#endif

#line 2 "/etc/yyparse.c"

/*
 * Copyright 1985, 1990 by Mortice Kern Systems Inc.  All rights reserved.
 * 
 * Automaton to interpret LALR(1) tables.
 *
 * Macros:
 *	yyclearin - clear the lookahead token.
 *	yyerrok - forgive a pending error
 *	YYERROR - simulate an error
 *	YYACCEPT - halt and return 0
 *	YYABORT - halt and return 1
 *	YYRETURN(value) - halt and return value.  You should use this
 *		instead of return(value).
 *	YYREAD - ensure yychar contains a lookahead token by reading
 *		one if it does not.  See also YYSYNC.
 *	YYRECOVERING - 1 if syntax error detected and not recovered
 *		yet; otherwise, 0.
 *
 * Preprocessor flags:
 *	YYDEBUG - includes debug code if 1.  The parser will print
 *		 a travelogue of the parse if this is defined as 1
 *		 and yydebug is non-zero.
 *		yacc -t sets YYDEBUG to 1, but not yydebug.
 *	YYTRACE - turn on YYDEBUG, and undefine default trace functions
 *		so that the interactive functions in 'ytrack.c' will
 *		be used.
 *	YYSSIZE - size of state and value stacks (default 150).
 *	YYSTATIC - By default, the state stack is an automatic array.
 *		If this is defined, the stack will be static.
 *		In either case, the value stack is static.
 *	YYALLOC - Dynamically allocate both the state and value stacks
 *		by calling malloc() and free().
 *	YYSYNC - if defined, yacc guarantees to fetch a lookahead token
 *		before any action, even if it doesnt need it for a decision.
 *		If YYSYNC is defined, YYREAD will never be necessary unless
 *		the user explicitly sets yychar = -1
 *
 * Copyright (c) 1983, by the University of Waterloo
 */
/*
 * Prototypes
 */

extern int yylex YY_ARGS((void));

#if YYDEBUG

#include <stdlib.h>		/* common prototypes */
#include <string.h>
#if defined(__cplusplus)
extern "C"  {
#endif
extern char *	yyValue YY_ARGS((YYSTYPE, int));	/* print yylval */
extern void yyShowState YY_ARGS((yyTraceItems *));
extern void yyShowReduce YY_ARGS((yyTraceItems *));
extern void yyShowGoto YY_ARGS((yyTraceItems *));
extern void yyShowShift YY_ARGS((yyTraceItems *));
extern void yyShowErrRecovery YY_ARGS((yyTraceItems *));
extern void yyShowErrDiscard YY_ARGS((yyTraceItems *));

extern void yyShowRead YY_ARGS((int));
#if defined(__cplusplus)
}
#endif
#endif

/*
 * If YYDEBUG defined and yydebug set,
 * tracing functions will be called at appropriate times in yyparse()
 * Pass state of YACC parse, as filled into yyTraceItems yyx
 * If yyx.done is set by the tracing function, yyparse() will terminate
 * with a return value of -1
 */
#define YY_TRACE(fn) { \
	yyx.state = yystate; yyx.lookahead = yychar; yyx.errflag =yyerrflag; \
	yyx.states = yys+1; yyx.nstates = yyps-yys; \
	yyx.values = yyv+1; yyx.nvalues = yypv-yyv; \
	yyx.types = yytypev+1; yyx.done = 0; \
	yyx.rule = yyi; yyx.npop = yyj; \
	fn(&yyx); \
	if (yyx.done) YYRETURN(-1); }

#ifndef I18N
#define m_textmsg(id, str, cls)	(str)
#else /*I18N*/
extern	char* m_textmsg YY_ARGS((int id, const char* str, char* cls));
#endif/*I18N*/

#ifndef YYSSIZE
# define YYSSIZE	150
#endif

#define YYERROR		goto yyerrlabel
#define yyerrok		yyerrflag = 0
#if YYDEBUG
#define yyclearin	{ if (yydebug) yyShowRead(-1); yychar = -1; }
#else
#define yyclearin	yychar = -1
#endif
#define YYACCEPT	YYRETURN(0)
#define YYABORT		YYRETURN(1)
#define YYRECOVERING()	(yyerrflag != 0)
#ifdef YYALLOC
# define YYRETURN(val)	{ retval = (val); goto yyReturn; }
#else
# define YYRETURN(val)	return(val)
#endif
#if YYDEBUG
/* The if..else makes this macro behave exactly like a statement */
# define YYREAD	if (yychar < 0) {					\
			if ((yychar = yylex()) < 0)			\
				yychar = 0;				\
			if (yydebug)					\
				yyShowRead(yychar);			\
		} else
#else
# define YYREAD	if (yychar < 0) {					\
			if ((yychar = yylex()) < 0)			\
				yychar = 0;				\
		} else
#endif

#define YYERRCODE	256		/* value of `error' */
#define	YYQYYP	yyq[yyq-yyp]

YYSTYPE	yyval;				/* $ */
YYSTYPE	*yypvt;				/* $n */
YYSTYPE	yylval;				/* yylex() sets this */

int	yychar,				/* current token */
	yyerrflag,			/* error flag */
	yynerrs;			/* error count */

#if YYDEBUG
int yydebug = 0;		/* debug if this flag is set */
extern char	*yysvar[];	/* table of non-terminals (aka 'variables') */
extern yyNamedType yyTokenTypes[];	/* table of terminals & their types */
extern short	yyrmap[], yysmap[];	/* map internal rule/states */
extern int	yynstate, yynvar, yyntoken, yynrule;

extern int	yyGetType YY_ARGS((int));	/* token type */
extern char	*yyptok YY_ARGS((int));	/* printable token string */
extern int	yyExpandName YY_ARGS((int, int, char *, int));
				  /* expand yyRules[] or yyStates[] */
static char *	yygetState YY_ARGS((int));

#define yyassert(condition, msg, arg) \
	if (!(condition)) { \
		printf(m_textmsg(2824, "\nyacc bug: ", "1")); \
		printf(msg, arg); \
		YYABORT; }
#else /* !YYDEBUG */
#define yyassert(condition, msg, arg)
#endif

#line 533 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
const
LEX_ERROR	= ERROR,
LEX_IDENT	= IDENT,
LEX_POINT	= POINT,
LEX_TEXT	= TEXT,
LEX_VALUE	= VALUE,
LEX_ARG		= ARG,
LEX_BREAK	= BREAK,
LEX_CASE	= CASE,
LEX_COND	= COND,
LEX_DEFAULT	= DEFAULT,
LEX_DO		= DO,
LEX_ELSE	= ELSE,
LEX_ELSEW	= ELSEW,
LEX_FOR		= FOR,
LEX_GOTO	= GOTO,
LEX_IF		= IF,
LEX_LABEL	= LABEL,
LEX_RETURN	= RETURN,
LEX_SIZEOF	= SIZEOF,
LEX_SWITCH	= SWITCH,
LEX_USING	= USING,
LEX_WHERE	= WHERE,
LEX_WHILE	= WHILE,
LEX_CAST	= CAST,
LEX_CONST	= CONST,
LEX_INC		= INC,
LEX_ADD		= ADD,
LEX_BINEQ	= BINEQ,
LEX_CONCAT	= CONCAT,
LEX_IAND	= IAND,
LEX_IN		= IN,
LEX_IOR		= IOR,
LEX_IXOR	= IXOR,
LEX_LEQV	= LEQV,
LEX_POWER	= POWER,
LEX_PROMO	= PROMO,
LEX_RANGE	= RANGE,
LEX_SHIFT	= SHIFT,
LEX_LAND	= LAND,
LEX_LEQ		= LEQ,
LEX_LGE		= LGE,
LEX_LOR		= LOR,
LEX_MUL		= MUL,
LEX_UNARY	= UNARY,
LEX_LANDS	= LANDS,
LEX_LEQS	= LEQS,
LEX_LGES	= LGES,
LEX_LORS	= LORS,
LEX_MULS	= MULS,
LEX_UNARYS	= UNARYS,
LEX_FUN		= FUN,
LEX_VBL		= VBL,
LEX_MODIF	= MODIF;

YYSTYPE *TdiYylvalPtr = &yylval;

yyparse()
{
	register short		yyi, *yyp;	/* for table lookup */
	register short		*yyps;		/* top of state stack */
	register short		yystate;	/* current state */
	register YYSTYPE	*yypv;		/* top of value stack */
	register short		*yyq;
	register int		yyj;
#if YYDEBUG
	yyTraceItems	yyx;			/* trace block */
	short	* yytp;
	int	yyruletype = 0;
#endif
#ifdef YYSTATIC
	static short	yys[YYSSIZE + 1];
	static YYSTYPE	yyv[YYSSIZE + 1];
#if YYDEBUG
	static short	yytypev[YYSSIZE+1];	/* type assignments */
#endif
#else /* ! YYSTATIC */
#ifdef YYALLOC
	YYSTYPE *yyv;
	short	*yys;
#if YYDEBUG
	short	*yytypev;
#endif
	YYSTYPE save_yylval;
	YYSTYPE save_yyval;
	YYSTYPE *save_yypvt;
	int save_yychar, save_yyerrflag, save_yynerrs;
	int retval;
#else
	short		yys[YYSSIZE + 1];
	static YYSTYPE	yyv[YYSSIZE + 1];	/* historically static */
#if YYDEBUG
	short	yytypev[YYSSIZE+1];		/* mirror type table */
#endif
#endif /* ! YYALLOC */
#endif /* ! YYSTATIC */


#ifdef YYALLOC
	yys = (short *) malloc((YYSSIZE + 1) * sizeof(short));
	yyv = (YYSTYPE *) malloc((YYSSIZE + 1) * sizeof(YYSTYPE));
#if YYDEBUG
	yytypev = (short *) malloc((YYSSIZE+1) * sizeof(short));
#endif
	if (yys == (short *)0 || yyv == (YYSTYPE *)0
#if YYDEBUG
		|| yytypev == (short *) 0
#endif
	) {
		yyerror("Not enough space for parser stacks");
		return 1;
	}
	save_yylval = yylval;
	save_yyval = yyval;
	save_yypvt = yypvt;
	save_yychar = yychar;
	save_yyerrflag = yyerrflag;
	save_yynerrs = yynerrs;
#endif

	yynerrs = 0;
	yyerrflag = 0;
	yyclearin;
	yyps = yys;
	yypv = yyv;
	*yyps = yystate = YYS0;		/* start state */
#if YYDEBUG
	yytp = yytypev;
	yyi = yyj = 0;			/* silence compiler warnings */
#endif

yyStack:
	yyassert((unsigned)yystate < yynstate, m_textmsg(587, "state %d\n", "1"), yystate);
	if (++yyps > &yys[YYSSIZE]) {
		yyerror("Parser stack overflow");
		YYABORT;
	}
	*yyps = yystate;	/* stack current state */
	*++yypv = yyval;	/* ... and value */
#if YYDEBUG
	*++yytp = yyruletype;	/* ... and type */

	if (yydebug)
		YY_TRACE(yyShowState)
#endif

	/*
	 *	Look up next action in action table.
	 */
yyEncore:
#ifdef YYSYNC
	YYREAD;
#endif
	if (yystate >= sizeof yypact/sizeof yypact[0]) 	/* simple state */
		yyi = yystate - YYDELTA;	/* reduce in any case */
	else {
		if(*(yyp = &yyact[yypact[yystate]]) >= 0) {
			/* Look for a shift on yychar */
#ifndef YYSYNC
			YYREAD;
#endif
			yyq = yyp;
			yyi = yychar;
			while (yyi < *yyp++)
				;
			if (yyi == yyp[-1]) {
				yystate = ~YYQYYP;
#if YYDEBUG
				if (yydebug) {
					yyruletype = yyGetType(yychar);
					YY_TRACE(yyShowShift)
				}
#endif
				yyval = yylval;	/* stack what yylex() set */
				yyclearin;		/* clear token */
				if (yyerrflag)
					yyerrflag--;	/* successful shift */
				goto yyStack;
			}
		}

		/*
	 	 *	Fell through - take default action
	 	 */

		if (yystate >= sizeof yydef /sizeof yydef[0])
			goto yyError;
		if ((yyi = yydef[yystate]) < 0)	 { /* default == reduce? */
			/* Search exception table */
			yyassert((unsigned)~yyi < sizeof yyex/sizeof yyex[0],
				m_textmsg(2825, "exception %d\n", "1"), yystate);
			yyp = &yyex[~yyi];
#ifndef YYSYNC
			YYREAD;
#endif
			while((yyi = *yyp) >= 0 && yyi != yychar)
				yyp += 2;
			yyi = yyp[1];
			yyassert(yyi >= 0,
				 m_textmsg(2826, "Ex table not reduce %d\n", "1"), yyi);
		}
	}

	yyassert((unsigned)yyi < yynrule, m_textmsg(2827, "reduce %d\n", "1"), yyi);
	yyj = yyrlen[yyi];
#if YYDEBUG
	if (yydebug)
		YY_TRACE(yyShowReduce)
	yytp -= yyj;
#endif
	yyps -= yyj;		/* pop stacks */
	yypvt = yypv;		/* save top */
	yypv -= yyj;
	yyval = yypv[1];	/* default action $ = $1 */
#if YYDEBUG
	yyruletype = yyRules[yyrmap[yyi]].type;
#endif

	switch (yyi) {		/* perform semantic action */
		
case YYr1: {	/* slabel :  LABEL VBL */
#line 247 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
yyval.mark=yypvt[0].mark;
} break;

case YYr32: {	/* ass :  '`' ass */
#line 262 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
yyval.mark.rptr=yypvt[0].mark.rptr; yyval.mark.builtin= -2;
					_ref_zone.l_status=_immediate(&yyval.mark.rptr);
					if (!(_ref_zone.l_status & 1)) {yyerror(0);}
					
} break;

case YYr33: {	/* ass :  unaryX '=' ass */
#line 266 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(OPC_EQUALS,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr34: {	/* ass :  unaryX BINEQ ass */
#line 267 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
struct marker tmp;		
						_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,tmp); _knot2(&tmp);
						_JUST1(OPC_EQUALS_FIRST,tmp,yyval.mark);
} break;

case YYr35: {	/* ass :  ass RANGE ass */
#line 270 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
if (yypvt[0].mark.rptr && yypvt[0].mark.rptr->dsc_b_dtype == DSC_K_DTYPE_RANGE)
						if (yypvt[0].mark.rptr->dsc_b_ndesc == 2)
							{yyval.mark=yypvt[0].mark;
							yyval.mark.rptr->dsc_a_dscptrs[2]=yyval.mark.rptr->dsc_a_dscptrs[1];
							yyval.mark.rptr->dsc_a_dscptrs[1]=yyval.mark.rptr->dsc_a_dscptrs[0];
							yyval.mark.rptr->dsc_a_dscptrs[0]=(struct dsc_descriptor *)yypvt[-2].mark.rptr;
							++yyval.mark.rptr->dsc_b_ndesc;}
						else {_ref_zone.l_status=TDI__EXTRA_ARG; return(1);}
					else	{static unsigned long vmlen = sizeof(struct dsc_descriptor_range);
						lib_get_vm(&vmlen, &yyval.mark.rptr, &_ref_zone.l_zone);
						yyval.mark.rptr->dsc_w_length = 0;
						yyval.mark.rptr->dsc_b_dtype = DSC_K_DTYPE_RANGE;
						yyval.mark.rptr->dsc_b_class = DSC_K_CLASS_R;
						yyval.mark.rptr->dsc_a_pointer = 0;
						yyval.mark.rptr->dsc_b_ndesc = 2;
						yyval.mark.rptr->dsc_a_dscptrs[0]=(struct dsc_descriptor *)yypvt[-2].mark.rptr;
						yyval.mark.rptr->dsc_a_dscptrs[1]=(struct dsc_descriptor *)yypvt[0].mark.rptr;
						_RESOLVE(yyval.mark);}
					
} break;

case YYr36: {	/* ass :  ass '?' ass RANGE ass */
#line 289 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST3(OPC_CONDITIONAL,yypvt[-2].mark,yypvt[0].mark,yypvt[-4].mark,yyval.mark);
} break;

case YYr37: {	/* ass :  ass PROMO ass */
#line 290 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr38: {	/* ass :  ass LEQV ass */
#line 291 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark); _knot2(&yyval.mark.rptr);
} break;

case YYr39: {	/* ass :  ass LOR ass */
#line 292 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark); _knot2(&yyval.mark.rptr);
} break;

case YYr40: {	/* ass :  ass LORS ass */
#line 293 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark); _knot2(&yyval.mark.rptr);
} break;

case YYr41: {	/* ass :  ass LAND ass */
#line 294 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark); _knot2(&yyval.mark.rptr);
} break;

case YYr42: {	/* ass :  ass LANDS ass */
#line 295 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark); _knot2(&yyval.mark.rptr);
} break;

case YYr43: {	/* ass :  ass IOR ass */
#line 296 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark); _knot2(&yyval.mark.rptr);
} break;

case YYr44: {	/* ass :  ass IXOR ass */
#line 297 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark); _knot2(&yyval.mark.rptr);
} break;

case YYr45: {	/* ass :  ass IAND ass */
#line 298 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark); _knot2(&yyval.mark.rptr);
} break;

case YYr46: {	/* ass :  ass LEQ ass */
#line 299 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr47: {	/* ass :  ass LEQS ass */
#line 300 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr48: {	/* ass :  ass LGE ass */
#line 301 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr49: {	/* ass :  ass LGES ass */
#line 302 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr50: {	/* ass :  ass IN ass */
#line 303 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr51: {	/* ass :  ass CONCAT ass */
#line 304 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
if (yyval.mark.rptr == 0) yyval.mark=yypvt[0].mark;
					else if (yyval.mark.rptr->dsc_b_dtype == DSC_K_DTYPE_FUNCTION
					&&	*(unsigned short *)yyval.mark.rptr->dsc_a_pointer == OPC_CONCAT
					&&	yyval.mark.rptr->dsc_b_ndesc < 250)
						{yyval.mark.rptr->dsc_a_dscptrs[yyval.mark.rptr->dsc_b_ndesc++]=(struct dsc_descriptor *)yypvt[0].mark.rptr;
						_RESOLVE(yyval.mark);}
					else {_FULL2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark);}
					
} break;

case YYr52: {	/* ass :  ass SHIFT ass */
#line 312 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr53: {	/* ass :  ass ADD ass */
#line 313 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr54: {	/* ass :  ass MUL ass */
#line 314 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr55: {	/* ass :  ass MULS ass */
#line 315 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr56: {	/* ass :  ass '*' ass */
#line 316 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(OPC_MULTIPLY,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr57: {	/* ass :  ass POWER ass */
#line 317 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr59: {	/* ass :  '*' */
#line 319 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_EMPTY_MARKER;
} break;

case YYr60: {	/* unaryX :  ADD unaryX */
#line 321 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
int j;
					if (yypvt[-1].mark.builtin == OPC_ADD)	j=OPC_UNARY_PLUS;
					else				j=OPC_UNARY_MINUS;
					_JUST1(j,yypvt[0].mark,yyval.mark);
} break;

case YYr61: {	/* unaryX :  UNARY unaryX */
#line 325 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST1(yypvt[-1].mark.builtin,yypvt[0].mark,yyval.mark); _knot1(&yyval.mark.rptr);
} break;

case YYr62: {	/* unaryX :  UNARYS unaryX */
#line 326 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST1(yypvt[-1].mark.builtin,yypvt[0].mark,yyval.mark); _knot1(&yyval.mark.rptr);
} break;

case YYr63: {	/* unaryX :  INC unaryX */
#line 327 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST1(yypvt[-1].mark.builtin,yypvt[0].mark,yyval.mark);
} break;

case YYr65: {	/* bracket :  '[' ass */
#line 335 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_FULL1(OPC_VECTOR,yypvt[0].mark,yyval.mark);
} break;

case YYr66: {	/* bracket :  '[' */
#line 336 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST0(OPC_VECTOR,yyval.mark);
} break;

case YYr67: {	/* bracket :  bracket ',' ass */
#line 337 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
if (yyval.mark.rptr->dsc_b_ndesc >= 250) {
					_RESOLVE(yypvt[-2].mark);
					_FULL1(OPC_VECTOR,yypvt[-2].mark,yyval.mark);
				}
				yyval.mark.rptr->dsc_a_dscptrs[yyval.mark.rptr->dsc_b_ndesc++] = (struct dsc_descriptor *)yypvt[0].mark.rptr;
				
} break;

case YYr68: {	/* exp :  opt ',' opt */
#line 344 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
if (yyval.mark.rptr			
					&&	yyval.mark.builtin != -2
					&&	yyval.mark.rptr->dsc_b_dtype == DSC_K_DTYPE_FUNCTION
					&&	*(unsigned short *)yyval.mark.rptr->dsc_a_pointer == OPC_COMMA
					&&	yyval.mark.rptr->dsc_b_ndesc < 250)
						yyval.mark.rptr->dsc_a_dscptrs[yyval.mark.rptr->dsc_b_ndesc++]=(struct dsc_descriptor *)yypvt[0].mark.rptr;
					else _FULL2(OPC_COMMA,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
					
} break;

case YYr70: {	/* sub :  exp */
#line 354 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
if (yyval.mark.rptr
				&&	yyval.mark.builtin != -2
				&& yyval.mark.rptr->dsc_b_dtype == DSC_K_DTYPE_FUNCTION
				&& *(unsigned short *)yyval.mark.rptr->dsc_a_pointer == OPC_COMMA) ;
				else _JUST1(OPC_ABORT,yypvt[0].mark,yyval.mark);
} break;

case YYr71: {	/* sub :  */
#line 359 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST0(OPC_ABORT,yyval.mark);
} break;

case YYr72: {	/* paren :  '(' exp ')' */
#line 361 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
yyval.mark=yypvt[-1].mark; yyval.mark.builtin= -2;
} break;

case YYr73: {	/* paren :  '(' stmt_lst ')' */
#line 362 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
yyval.mark=yypvt[-1].mark; yyval.mark.builtin= -2;
} break;

case YYr74: {	/* using0 :  USING '(' */
#line 364 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
++_ref_zone.l_rel_path;
} break;

case YYr75: {	/* using :  using0 ass ',' ass ',' */
#line 366 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_FULL2(OPC_ABORT,yypvt[-3].mark,yypvt[-1].mark,yyval.mark); --_ref_zone.l_rel_path;
} break;

case YYr76: {	/* using :  using0 ass ',' ',' */
#line 367 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_FULL2(OPC_ABORT,yypvt[-2].mark,_EMPTY_MARKER,yyval.mark); --_ref_zone.l_rel_path;
} break;

case YYr78: {	/* opt :  */
#line 370 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
yyval.mark=_EMPTY_MARKER;
} break;

case YYr80: {	/* postX :  postX '[' sub ']' */
#line 376 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
int j;
					yyval.mark=yypvt[-1].mark;
					yyval.mark.rptr->dsc_a_pointer= (unsigned char *)&OPC_SUBSCRIPT;
					for (j=yyval.mark.rptr->dsc_b_ndesc; --j>=0; )
						yyval.mark.rptr->dsc_a_dscptrs[j+1]=yyval.mark.rptr->dsc_a_dscptrs[j];
					yyval.mark.rptr->dsc_a_dscptrs[0]=(struct dsc_descriptor *)yypvt[-3].mark.rptr;
					yyval.mark.rptr->dsc_b_ndesc++;
					_RESOLVE(yyval.mark);
				
} break;

case YYr81: {	/* postX :  postX INC */
#line 385 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
int j=yypvt[0].mark.builtin==OPC_PRE_INC ? OPC_POST_INC : OPC_POST_DEC;
					_JUST1(j,yypvt[-1].mark,yyval.mark);
} break;

case YYr82: {	/* postX :  label '(' sub ')' */
#line 387 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
yyval.mark=yypvt[-1].mark;
				if (yypvt[-3].mark.builtin < 0) {int j;		
					yyval.mark.rptr->dsc_a_pointer= (unsigned char *)&OPC_EXT_FUNCTION;
					for (j=yyval.mark.rptr->dsc_b_ndesc; --j>=0;)
						yyval.mark.rptr->dsc_a_dscptrs[j+2]=yyval.mark.rptr->dsc_a_dscptrs[j];
					yyval.mark.rptr->dsc_a_dscptrs[0]=0;
					yyval.mark.rptr->dsc_a_dscptrs[1]=(struct dsc_descriptor *)yypvt[-3].mark.rptr;
					yyval.mark.rptr->dsc_b_ndesc += 2;
				}
				else	{				
					*(unsigned short *)yyval.mark.rptr->dsc_a_pointer=yypvt[-3].mark.builtin;
					_RESOLVE(yyval.mark);}
				
} break;

case YYr83: {	/* postX :  label POINT '(' sub ')' */
#line 400 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
int j;
				yyval.mark=yypvt[-1].mark;			
				yyval.mark.rptr->dsc_b_dtype=DSC_K_DTYPE_CALL;
				yyval.mark.rptr->dsc_w_length=0;
				yyval.mark.rptr->dsc_a_pointer=0;
				for (j=yyval.mark.rptr->dsc_b_ndesc; --j>=0;)
					yyval.mark.rptr->dsc_a_dscptrs[j+2]=yyval.mark.rptr->dsc_a_dscptrs[j];
				yyval.mark.rptr->dsc_a_dscptrs[0]=(struct dsc_descriptor *)yypvt[-4].mark.rptr;
				yyval.mark.rptr->dsc_a_dscptrs[1]=(struct dsc_descriptor *)yypvt[-3].mark.rptr;
				yyval.mark.rptr->dsc_b_ndesc += 2;
				
} break;

case YYr84: {	/* postX :  label POINT label '(' sub ')' */
#line 411 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
int j;
				yyval.mark=yypvt[-1].mark;			
				str_upcase(yypvt[-3].mark.rptr, yypvt[-3].mark.rptr);
				for (j=TDI_K_CAT_MAX; --j>=0;)
					if (strncmp(_REF_CAT[j].name, (char *)yypvt[-3].mark.rptr->dsc_a_pointer, yypvt[-3].mark.rptr->dsc_w_length) == 0
					&& strlen(_REF_CAT[j].name) == yypvt[-3].mark.rptr->dsc_w_length) break;
				if (j<0) {_ref_zone.l_status=TDI__INVDTYDSC; return(1);}
				yyval.mark.rptr->dsc_b_dtype=DSC_K_DTYPE_CALL;
				yyval.mark.rptr->dsc_w_length=1;
				*(unsigned char *)yyval.mark.rptr->dsc_a_pointer=(unsigned char)j;
				for (j=yyval.mark.rptr->dsc_b_ndesc; --j>=0;)
					yyval.mark.rptr->dsc_a_dscptrs[j+2]=yyval.mark.rptr->dsc_a_dscptrs[j];
				yyval.mark.rptr->dsc_a_dscptrs[0]=(struct dsc_descriptor *)yypvt[-5].mark.rptr;
				yyval.mark.rptr->dsc_a_dscptrs[1]=(struct dsc_descriptor *)yypvt[-4].mark.rptr;
				yyval.mark.rptr->dsc_b_ndesc += 2;
				
} break;

case YYr85: {	/* postX :  using sub ')' */
#line 427 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
int j;	
					yyval.mark.rptr->dsc_a_pointer= (unsigned char *)&OPC_USING;
					for (j=0; j < yypvt[-1].mark.rptr->dsc_b_ndesc; ++j)
						yyval.mark.rptr->dsc_a_dscptrs[yyval.mark.rptr->dsc_b_ndesc++]=yypvt[-1].mark.rptr->dsc_a_dscptrs[j];
				
} break;

case YYr86: {	/* postX :  using0 ass ',' ass ')' */
#line 432 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(OPC_USING,yypvt[-3].mark,yypvt[-1].mark,yyval.mark); --_ref_zone.l_rel_path;
} break;

case YYr88: {	/* textX :  textX TEXT */
#line 438 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_MAKE_S(DSC_K_DTYPE_T, yypvt[-1].mark.rptr->dsc_w_length + yypvt[0].mark.rptr->dsc_w_length, yyval.mark.rptr);
					str_concat(yyval.mark.rptr, yypvt[-1].mark.rptr, yypvt[0].mark.rptr MDS_END_ARG);
				
} break;

case YYr89: {	/* primaX :  MODIF VBL */
#line 447 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST1(yypvt[-1].mark.builtin,yypvt[0].mark,yyval.mark);
} break;

case YYr90: {	/* primaX :  MODIF MODIF VBL */
#line 448 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
struct marker tmp;			
					_JUST1(yypvt[-1].mark.builtin,yypvt[0].mark,tmp);
					_JUST1(yypvt[-2].mark.builtin,tmp,yyval.mark);
} break;

case YYr91: {	/* primaX :  label */
#line 451 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
if (*yyval.mark.rptr->dsc_a_pointer == '$') {
					if(yyval.mark.builtin < 0) yyval.mark.rptr->dsc_b_dtype=DSC_K_DTYPE_IDENT;
					else if ((_REF_FUNCTION[yyval.mark.builtin].token & LEX_M_TOKEN) == (unsigned long)LEX_ARG
					&& !((_ref_zone.l_status=_arg(&yyval.mark)) & 1)) {yyerror(0);}
					else if ((_REF_FUNCTION[yyval.mark.builtin].token & LEX_M_TOKEN) == (unsigned long)LEX_CONST)
						_JUST0(yypvt[0].mark.builtin,yyval.mark);
				}
				else if (*yyval.mark.rptr->dsc_a_pointer == '_')
					yyval.mark.rptr->dsc_b_dtype=DSC_K_DTYPE_IDENT;
				else if (_LEX_PATH(yypvt[0].mark.rptr->dsc_w_length, yypvt[0].mark.rptr->dsc_a_pointer, &yyval.mark) == ERROR)
					{yyerror(0);}
				
} break;

case YYr95: {	/* primaX :  bracket ']' */
#line 466 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_RESOLVE(yyval.mark);
} break;

case YYr96: {	/* funvbl :  FUN label */
#line 473 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
yyval.mark=yypvt[0].mark;
} break;

case YYr97: {	/* funvbl :  FUN MODIF label */
#line 474 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST1(yypvt[-1].mark.builtin,yypvt[0].mark,yyval.mark);
} break;

case YYr98: {	/* funvbl :  MODIF FUN label */
#line 475 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST1(yypvt[-2].mark.builtin,yypvt[0].mark,yyval.mark);
} break;

case YYr99: {	/* fun :  funvbl '(' sub ')' */
#line 477 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
int j;	yyval.mark=yypvt[-1].mark;
							yyval.mark.rptr->dsc_a_pointer= (unsigned char *)&OPC_FUN;
							for (j=yyval.mark.rptr->dsc_b_ndesc; --j>=0;)
								yyval.mark.rptr->dsc_a_dscptrs[j+2]=yyval.mark.rptr->dsc_a_dscptrs[j];
							yyval.mark.rptr->dsc_a_dscptrs[0]=(struct dsc_descriptor *)yypvt[-3].mark.rptr;
							yyval.mark.rptr->dsc_b_ndesc += 2;
							
						
} break;

case YYr100: {	/* stmt :  BREAK ';' */
#line 486 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST0(yypvt[-1].mark.builtin,yyval.mark);
} break;

case YYr101: {	/* stmt :  CASE paren stmt */
#line 487 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_FULL2(yypvt[-2].mark.builtin,yypvt[-1].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr102: {	/* stmt :  CASE DEFAULT stmt */
#line 488 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_FULL1(yypvt[-1].mark.builtin,yypvt[0].mark,yyval.mark);
} break;

case YYr103: {	/* stmt :  DO '{' stmt_lst '}' WHILE paren ';' */
#line 489 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-6].mark.builtin,yypvt[-1].mark,yypvt[-4].mark,yyval.mark);
} break;

case YYr104: {	/* stmt :  FOR '(' opt ';' opt ';' opt ')' stmt */
#line 490 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST4(yypvt[-8].mark.builtin,yypvt[-6].mark,yypvt[-4].mark,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr105: {	/* stmt :  GOTO VBL ';' */
#line 491 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST1(yypvt[-2].mark.builtin,yypvt[-1].mark,yyval.mark);
} break;

case YYr106: {	/* stmt :  IF paren stmt ELSE stmt */
#line 492 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST3(yypvt[-4].mark.builtin,yypvt[-3].mark,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr107: {	/* stmt :  IF paren stmt */
#line 493 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-2].mark.builtin,yypvt[-1].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr108: {	/* stmt :  slabel RANGE stmt */
#line 494 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_FULL2(OPC_LABEL,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr109: {	/* stmt :  SWITCH paren stmt */
#line 497 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-2].mark.builtin,yypvt[-1].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr110: {	/* stmt :  WHERE paren stmt ELSEW stmt */
#line 498 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST3(yypvt[-4].mark.builtin,yypvt[-3].mark,yypvt[-2].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr111: {	/* stmt :  WHERE paren stmt */
#line 499 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-2].mark.builtin,yypvt[-1].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr112: {	/* stmt :  WHILE paren stmt */
#line 500 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_JUST2(yypvt[-2].mark.builtin,yypvt[-1].mark,yypvt[0].mark,yyval.mark);
} break;

case YYr113: {	/* stmt :  fun stmt */
#line 501 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"

						yyval.mark.rptr->dsc_a_dscptrs[1]=(struct dsc_descriptor *)yypvt[0].mark.rptr;
} break;

case YYr114: {	/* stmt :  '`' stmt */
#line 503 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
yyval.mark.rptr=yypvt[0].mark.rptr; yyval.mark.builtin= -2;
						_ref_zone.l_status=_immediate(&yyval.mark.rptr);
						if (!(_ref_zone.l_status & 1)) {yyerror(0);}
						
} break;

case YYr115: {	/* stmt :  '{' stmt_lst '}' */
#line 507 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
yyval.mark=yypvt[-1].mark; _RESOLVE(yyval.mark);
} break;

case YYr118: {	/* stmt_lst :  stmt_lst stmt */
#line 511 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
short opcode;
				if (yyval.mark.rptr == 0) {yyval.mark=yypvt[0].mark;}		
				else if (yypvt[0].mark.rptr == 0) {}		
				else if (yyval.mark.rptr->dsc_b_dtype == DSC_K_DTYPE_FUNCTION
				&& yyval.mark.rptr->dsc_b_ndesc < 250
				&& ((opcode = *(unsigned short *)yyval.mark.rptr->dsc_a_pointer) == OPC_STATEMENT
					|| opcode == OPC_CASE
					|| opcode == OPC_DEFAULT
					|| opcode == OPC_LABEL
				)) {yyval.mark.rptr->dsc_a_dscptrs[yyval.mark.rptr->dsc_b_ndesc++]=(struct dsc_descriptor *)yypvt[0].mark.rptr;}
				else	{_FULL2(OPC_STATEMENT,yypvt[-1].mark,yypvt[0].mark,yyval.mark);}
			
} break;

case YYr119: {	/* program :  stmt_lst */
#line 524 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_RESOLVE(yyval.mark);		
			_ref_zone.a_result=(struct dsc_descriptor_d *)yyval.mark.rptr;
			_ref_zone.l_status=1;
} break;

case YYr120: {	/* program :  */
#line 527 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
yyval.mark=_EMPTY_MARKER;
} break;

case YYr121: {	/* program :  ERROR */
#line 528 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"

} break;

case YYr122: {	/* program :  error */
#line 529 "/alpha1dka600/mds_source_roots/mds$source/tdishr/work/tdi$$yacc.y"
_ref_zone.l_status=TDI__SYNTAX;
} break;
#line 314 "/etc/yyparse.c"
	case YYrACCEPT:
		YYACCEPT;
	case YYrERROR:
		goto yyError;
	}

	/*
	 *	Look up next state in goto table.
	 */

	yyp = &yygo[yypgo[yyi]];
	yyq = yyp++;
	yyi = *yyps;
	while (yyi < *yyp++)
		;

	yystate = ~(yyi == *--yyp? YYQYYP: *yyq);
#if YYDEBUG
	if (yydebug)
		YY_TRACE(yyShowGoto)
#endif
	goto yyStack;

yyerrlabel:	;		/* come here from YYERROR	*/
/*
#pragma used yyerrlabel
 */
	yyerrflag = 1;
	if (yyi == YYrERROR) {
		yyps--;
		yypv--;
#if YYDEBUG
		yytp--;
#endif
	}

yyError:
	switch (yyerrflag) {

	case 0:		/* new error */
		yynerrs++;
		yyi = yychar;
		yyerror("Syntax error");
		if (yyi != yychar) {
			/* user has changed the current token */
			/* try again */
			yyerrflag++;	/* avoid loops */
			goto yyEncore;
		}

	case 1:		/* partially recovered */
	case 2:
		yyerrflag = 3;	/* need 3 valid shifts to recover */
			
		/*
		 *	Pop states, looking for a
		 *	shift on `error'.
		 */

		for ( ; yyps > yys; yyps--, yypv--
#if YYDEBUG
					, yytp--
#endif
		) {
			if (*yyps >= sizeof yypact/sizeof yypact[0])
				continue;
			yyp = &yyact[yypact[*yyps]];
			yyq = yyp;
			do
				;
			while (YYERRCODE < *yyp++);

			if (YYERRCODE == yyp[-1]) {
				yystate = ~YYQYYP;
				goto yyStack;
			}
				
			/* no shift in this state */
#if YYDEBUG
			if (yydebug && yyps > yys+1)
				YY_TRACE(yyShowErrRecovery)
#endif
			/* pop stacks; try again */
		}
		/* no shift on error - abort */
		break;

	case 3:
		/*
		 *	Erroneous token after
		 *	an error - discard it.
		 */

		if (yychar == 0)  /* but not EOF */
			break;
#if YYDEBUG
		if (yydebug)
			YY_TRACE(yyShowErrDiscard)
#endif
		yyclearin;
		goto yyEncore;	/* try again in same state */
	}
	YYABORT;

#ifdef YYALLOC
yyReturn:
	yylval = save_yylval;
	yyval = save_yyval;
	yypvt = save_yypvt;
	yychar = save_yychar;
	yyerrflag = save_yyerrflag;
	yynerrs = save_yynerrs;
	free((char *)yys);
	free((char *)yyv);
	return(retval);
#endif
}

		
#if YYDEBUG
/*
 * Return type of token
 */
int
yyGetType YY_FNARG1(int, tok)
{
	yyNamedType * tp;
	for (tp = &yyTokenTypes[yyntoken-1]; tp > yyTokenTypes; tp--)
		if (tp->token == tok)
			return tp->type;
	return 0;
}
/*
 * Print a token legibly.
 */
char *
yyptok YY_FNARG1(int, tok)
{
	yyNamedType * tp;
	for (tp = &yyTokenTypes[yyntoken-1]; tp > yyTokenTypes; tp--)
		if (tp->token == tok)
			return tp->name;
	return "";
}

/*
 * Read state 'num' from YYStatesFile
 */
#ifdef YYTRACE
static FILE *yyStatesFile = (FILE *) 0;
static char yyReadBuf[YYMAX_READ+1];

static char *
yygetState YY_FNARG1(int, num)
{
	int	size;

	if (yyStatesFile == (FILE *) 0
	 && (yyStatesFile = fopen(YYStatesFile, "r")) == (FILE *) 0)
		return "yyExpandName: cannot open states file";

	if (num < yynstate - 1)
		size = (int)(yyStates[num+1] - yyStates[num]);
	else {
		/* length of last item is length of file - ptr(last-1) */
		if (fseek(yyStatesFile, 0L, 2) < 0)
			goto cannot_seek;
		size = (int) (ftell(yyStatesFile) - yyStates[num]);
	}
	if (size < 0 || size > YYMAX_READ)
		return "yyExpandName: bad read size";
	if (fseek(yyStatesFile, yyStates[num], 0) < 0) {
	cannot_seek:
		return "yyExpandName: cannot seek in states file";
	}

	(void) fread(yyReadBuf, 1, size, yyStatesFile);
	yyReadBuf[size] = '\0';
	return yyReadBuf;
}
#endif /* YYTRACE */
/*
 * Expand encoded string into printable representation
 * Used to decode yyStates and yyRules strings.
 * If the expansion of 's' fits in 'buf', return 1; otherwise, 0.
 */
int
yyExpandName YY_FNARG4( int, num, int, isrule, char*, buf, int, len)
{
	int	i, n, cnt, type;
	char	* endp, * cp;
	char	*s;

	if (isrule)
		s = yyRules[num].name;
	else
#ifdef YYTRACE
		s = yygetState(num);
#else
		s = "*no states*";
#endif

	for (endp = buf + len - 8; *s; s++) {
		if (buf >= endp) {		/* too large: return 0 */
		full:	(void) strcpy(buf, " ...\n");
			return 0;
		} else if (*s == '%') {		/* nonterminal */
			type = 0;
			cnt = yynvar;
			goto getN;
		} else if (*s == '&') {		/* terminal */
			type = 1;
			cnt = yyntoken;
		getN:
			if (cnt < 100)
				i = 2;
			else if (cnt < 1000)
				i = 3;
			else
				i = 4;
			for (n = 0; i-- > 0; )
				n = (n * 10) + *++s - '0';
			if (type == 0) {
				if (n >= yynvar)
					goto too_big;
				cp = yysvar[n];
			} else if (n >= yyntoken) {
			    too_big:
				cp = "<range err>";
			} else
				cp = yyTokenTypes[n].name;

			if ((i = strlen(cp)) + buf > endp)
				goto full;
			(void) strcpy(buf, cp);
			buf += i;
		} else
			*buf++ = *s;
	}
	*buf = '\0';
	return 1;
}
#ifndef YYTRACE
/*
 * Show current state of yyparse
 */
void
yyShowState YY_FNARG1(yyTraceItems*, tp)
{
	short * p;
	YYSTYPE * q;

	printf(
	    m_textmsg(2828, "state %d (%d), char %s (%d)\n", "1"),
	      yysmap[tp->state], tp->state,
	      yyptok(tp->lookahead), tp->lookahead);
}
/*
 * show results of reduction
 */
void
yyShowReduce YY_FNARG1(yyTraceItems*, tp)
{
	printf("reduce %d (%d), pops %d (%d)\n",
		yyrmap[tp->rule], tp->rule,
		tp->states[tp->nstates - tp->npop],
		yysmap[tp->states[tp->nstates - tp->npop]]);
}
void
yyShowRead YY_FNARG1(int, val)
{
	printf(m_textmsg(2829, "read %s (%d)\n", "1"), yyptok(val), val);
}
void
yyShowGoto YY_FNARG1(yyTraceItems*, tp)
{
	printf(m_textmsg(2830, "goto %d (%d)\n", "1"), yysmap[tp->state], tp->state);
}
void
yyShowShift YY_FNARG1(yyTraceItems*, tp)
{
	printf(m_textmsg(2831, "shift %d (%d)\n", "1"), yysmap[tp->state], tp->state);
}
void
yyShowErrRecovery YY_FNARG1(yyTraceItems*, tp)
{
	short	* top = tp->states + tp->nstates - 1;

	printf(
	m_textmsg(2832, "Error recovery pops state %d (%d), uncovers %d (%d)\n", "1"),
		yysmap[*top], *top, yysmap[*(top-1)], *(top-1));
}
void
yyShowErrDiscard YY_FNARG1(yyTraceItems*, tp)
{
	printf(m_textmsg(2833, "Error recovery discards %s (%d), ", "1"),
		yyptok(tp->lookahead), tp->lookahead);
}
#endif	/* ! YYTRACE */
#endif	/* YYDEBUG */
