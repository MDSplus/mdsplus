
# line 2 "TdiYacc.y"
#ifdef __VMS
#pragma module TdiYacc TdiYacc
#endif

#ifdef WIN32
#pragma warning (disable : 4244 4102 ) /* int to short conversions and unreferenced label */
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

struct TdiZoneStruct TdiRefZone = {0};

extern int TdiYacc_RESOLVE();
extern int TdiLex();
extern int TdiYacc_IMMEDIATE();
extern int TdiYacc_BUILD();
extern int TdiYacc_ARG();
extern int TdiLexPath();

#define YYMAXDEPTH	250

#define yyparse		TdiYacc
#define _RESOLVE(arg)	if (TdiYacc_RESOLVE(&arg.rptr)) {yyerror(0);}
#define _FULL1(opcode,arg1,out)			if (TdiYacc_BUILD(255, 1, opcode, &out, &arg1)) {yyerror(0);}
#define _FULL2(opcode,arg1,arg2,out)		if (TdiYacc_BUILD(255, 2, opcode, &out, &arg1, &arg2)) {yyerror(0);}
	/*****************************
	Two args for image->routine.
	*****************************/
#define _JUST0(opcode,out)			if (TdiYacc_BUILD(2, 0, opcode, &out)) {yyerror(0);}
#define _JUST1(opcode,arg1,out)			if (TdiYacc_BUILD(3, 1, opcode, &out, &arg1)) {yyerror(0);}
#define _JUST2(opcode,arg1,arg2,out)		if (TdiYacc_BUILD(2, 2, opcode, &out, &arg1, &arg2)) {yyerror(0);}
#define _JUST3(opcode,arg1,arg2,arg3,out)	if (TdiYacc_BUILD(3, 3, opcode, &out, &arg1, &arg2, &arg3)) {yyerror(0);}
#define _JUST4(opcode,arg1,arg2,arg3,arg4,out)	if (TdiYacc_BUILD(4, 4, opcode, &out, &arg1, &arg2, &arg3, &arg4)) {yyerror(0);}

static struct marker _EMPTY_MARKER = {0,0};


# line 110 "TdiYacc.y"
typedef union
#ifdef __cplusplus
	YYSTYPE
#endif
	{struct marker mark;} YYSTYPE;
# define ERROR 257
# define IDENT 258
# define POINT 259
# define TEXT 260
# define VALUE 261
# define BREAK 262
# define CASE 263
# define COND 264
# define DEFAULT 265
# define DO 266
# define ELSE 267
# define ELSEW 268
# define FOR 269
# define GOTO 270
# define IF 271
# define LABEL 272
# define RETURN 273
# define SIZEOF 274
# define SWITCH 275
# define USING 276
# define WHERE 277
# define WHILE 278
# define ARG 279
# define CAST 280
# define CONST 281
# define INC 282
# define ADD 283
# define CONCAT 284
# define IAND 285
# define IN 286
# define IOR 287
# define IXOR 288
# define LEQV 289
# define POWER 290
# define PROMO 291
# define RANGE 292
# define SHIFT 293
# define BINEQ 294
# define LAND 295
# define LEQ 296
# define LGE 297
# define LOR 298
# define MUL 299
# define UNARY 300
# define LANDS 301
# define LEQS 302
# define LGES 303
# define LORS 304
# define MULS 305
# define UNARYS 306
# define FUN 307
# define MODIF 308
# define VBL 309

#ifdef __STDC__
#include <stdlib.h>
#include <string.h>
#else
#include <malloc.h>
#include <memory.h>
#endif

#include <values.h>

#ifdef __cplusplus

#ifndef yyerror
	void yyerror(const char *);
#endif

#ifndef yylex
#ifdef __EXTERN_C__
	extern "C" { int yylex(void); }
#else
	int yylex(void);
#endif
#endif
	int yyparse(void);

#endif
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
YYSTYPE yylval;
YYSTYPE yyval;
typedef int yytabelem;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#if YYMAXDEPTH > 0
int yy_yys[YYMAXDEPTH], *yys = yy_yys;
YYSTYPE yy_yyv[YYMAXDEPTH], *yyv = yy_yyv;
#else	/* user does initial allocation */
int *yys;
YYSTYPE *yyv;
#endif
static int yymaxdepth = YYMAXDEPTH;
# define YYERRCODE 256

# line 467 "TdiYacc.y"


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
yytabelem yyexca[] ={
-1, 0,
	0, 120,
	44, 78,
	59, 78,
	-2, 0,
-1, 1,
	0, -1,
	-2, 0,
-1, 2,
	0, 119,
	-2, 78,
-1, 35,
	41, 71,
	-2, 78,
-1, 80,
	41, 71,
	-2, 78,
-1, 129,
	93, 71,
	-2, 78,
-1, 131,
	41, 71,
	-2, 78,
-1, 134,
	44, 77,
	-2, 70,
-1, 189,
	41, 71,
	-2, 78,
-1, 206,
	41, 71,
	-2, 78,
	};
# define YYNPROD 123
# define YYLAST 1236
yytabelem yyact[]={

    59,   160,    27,    98,    97,    96,    97,    96,    42,    79,
    67,   122,    85,    86,    69,    44,    87,    46,    47,    88,
    83,    89,    90,    48,    45,    91,    92,    93,    94,   121,
    43,    41,   209,   200,   199,   138,    50,    59,    65,    53,
   139,   203,   131,   221,   149,    51,    52,    54,    55,    56,
    84,    60,    59,    39,    27,     2,    17,    95,   159,    57,
    77,    63,    77,   129,    77,    68,    62,    70,    71,    72,
   223,    77,   220,    77,    76,   219,   214,   198,    34,    78,
   204,   201,   195,    18,   191,    59,   206,   141,    80,   140,
    66,    38,    40,    26,    33,   120,    32,    21,    16,    36,
    35,    12,    81,    60,    59,   196,    27,     1,    17,     0,
     0,     0,     0,     0,     0,   143,     0,     0,     0,     0,
     0,   147,   124,   126,   127,   128,     0,     0,     0,     0,
     0,     0,     0,     0,    42,    18,     0,   197,    85,    86,
     0,    44,    87,    46,    47,    88,    83,    89,    90,    48,
    45,    91,    92,    93,    94,    60,    43,    41,     0,     0,
    17,   158,    50,     0,     0,    53,     0,     0,    59,     0,
    27,    51,    52,    54,    55,    56,    84,   161,     0,     0,
     0,     0,     0,    95,    82,    57,     0,    18,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   120,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   190,     0,     0,     0,     0,     4,     3,    42,    60,
    58,    37,     6,     7,    17,    44,     8,    46,    47,     9,
    10,    11,    20,    48,    45,    13,    49,    14,    15,     0,
    43,    41,    31,    28,   123,     0,    50,     0,    59,    53,
    27,    18,   208,   155,   130,    51,    52,    54,    55,    56,
    29,   132,    64,   218,     0,     0,    30,    23,    24,    57,
    42,     0,    58,    37,     6,     7,     0,    44,     8,    46,
    47,     9,    10,    11,    20,    48,    45,    13,    49,    14,
    15,     0,    43,    41,    31,    28,     0,     0,    50,    60,
    59,    53,    27,     0,   136,     0,     0,    51,    52,    54,
    55,    56,    29,   120,     0,     0,     0,     0,    30,    23,
    24,    57,    42,     0,    58,    37,     6,     7,     0,    44,
     8,    46,    47,     9,    10,    11,    20,    48,    45,    13,
    49,    14,    15,   121,    43,    41,    31,    28,     0,     0,
    50,    60,   118,    53,     0,     0,    17,     0,   119,    51,
    52,    54,    55,    56,    29,    59,     0,    27,     0,     0,
    30,    23,    24,    57,     0,     0,     0,     0,   120,     0,
     0,     0,     0,    18,     0,     0,    42,     0,    58,    37,
     6,     7,     0,    44,     8,    46,    47,     9,    10,    11,
    20,    48,    45,    13,    49,    14,    15,     0,    43,    41,
    31,    28,     0,     0,    50,     0,    60,    53,     0,     0,
     0,   136,     0,    51,    52,    54,    55,    56,    29,     0,
     0,     0,     0,     0,    30,    23,    24,    57,     0,     0,
     0,    59,   117,   115,   109,   114,   107,   108,   102,   121,
   101,     0,   116,     0,   105,   110,   112,   103,   118,     0,
   106,   111,   113,   104,   119,     0,    42,     0,    58,    37,
    85,    86,     0,    44,    87,    46,    47,    88,    83,    89,
    90,    48,    45,    91,    49,    93,    94,     0,    43,    41,
    31,    28,    60,   189,    50,     0,     0,    53,     0,     0,
     0,     0,     0,    51,    52,    54,    55,    56,    29,     0,
     0,     0,     0,     0,    30,    95,   125,    57,    42,     0,
    58,    37,     6,     7,   120,    44,     8,    46,    47,     9,
    10,    11,    20,    48,    45,    13,    49,    14,    15,     0,
    43,    41,    31,    28,     0,   100,    50,     0,     0,    53,
   217,   120,     0,   216,   117,    51,    52,    54,    55,    56,
    29,   121,     0,     0,   116,     0,    30,    23,    24,    57,
   118,     0,   100,     0,     0,     0,   119,     0,     0,     0,
     0,     0,     0,    42,     0,    58,    37,    85,    86,   120,
    44,    87,    46,    47,    88,    83,    89,    90,    48,    45,
    91,    49,    93,    94,     0,    43,    41,    31,    28,     0,
   100,    50,     0,   133,    53,   120,     0,   193,     0,   117,
    51,    52,    54,    55,    56,    29,   121,     0,     0,     0,
     0,    30,    95,   125,    57,   118,   100,     0,   120,     0,
     0,   119,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    42,
     0,    58,    37,    85,    86,   120,    44,    87,    46,    47,
    88,    83,    89,    90,    48,    45,    91,    49,    93,    94,
     0,    43,    41,    31,    28,     0,     0,    50,   120,     0,
    53,     0,     0,     0,   157,     0,    51,    52,    54,    55,
    56,    29,     0,     0,     0,     0,     0,    30,    95,   125,
    57,    42,   120,     0,     0,    85,    86,     0,    44,    87,
    46,    47,    88,    83,    89,    90,    48,    45,    91,    92,
    93,    94,     0,    43,    41,   120,     0,     0,     0,    50,
   120,     0,    53,   187,     0,   188,     0,     0,    51,    52,
    54,    55,    56,    84,     0,     0,     0,     0,   120,     0,
    95,   159,    57,     0,     0,   117,   115,   109,   114,   107,
   108,   102,   121,   101,    99,   116,     0,   105,   110,   112,
   103,   118,   120,   106,   111,   113,   104,   119,     0,     0,
   120,     0,   117,   115,   109,   114,   107,   108,   102,   121,
   101,    99,   116,   205,   105,   110,   112,   103,   118,     0,
   106,   111,   113,   104,   119,     0,     0,     0,     0,     0,
   215,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   117,   115,   109,   114,   107,   108,   102,   121,   101,   202,
   116,     0,   105,   110,   112,   103,   118,     0,   106,   111,
   113,   104,   119,     0,     0,     0,   117,   115,   109,   114,
   107,   108,   102,   121,   101,    99,   116,     0,   105,   110,
   112,   103,   118,     0,   106,   111,   113,   104,   119,   117,
   115,   109,   114,   107,   108,     0,   121,     0,     0,   116,
     0,   105,   110,   112,   103,   118,     0,   106,   111,   113,
   104,   119,     0,     0,     0,     0,   117,   115,   109,   114,
   107,   108,     0,   121,     0,     0,   116,     0,   105,   110,
   112,     0,   118,     0,   106,   111,   113,     0,   119,   117,
   115,   109,   114,   107,   108,     0,   121,     0,     0,   116,
     0,     0,   110,   112,     0,   118,     0,     0,   111,   113,
     0,   119,     0,   117,   115,   109,   114,     0,   108,     0,
   121,     0,     0,   116,     0,     0,   110,   112,    19,   118,
     0,     0,   111,   113,    22,   119,   117,   115,   109,   114,
     0,   117,   115,   121,   114,     0,   116,     0,   121,   110,
   112,   116,   118,     0,     0,   111,   113,   118,   119,   117,
   115,     0,   114,   119,   135,     0,   121,    25,     0,   116,
   134,     5,   110,   112,    61,   118,     0,     0,   111,   113,
     0,   119,     0,   117,   115,    74,   114,     0,    73,    75,
   121,   117,   115,   116,   142,   148,     0,   112,   121,   118,
     0,   116,     0,   113,   137,   119,   156,   118,     0,   135,
     0,     0,     0,   119,     0,   134,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   144,     0,
     0,     0,     0,     0,     0,   145,   146,     0,     0,     0,
   150,   151,   152,   153,   154,     0,     0,     0,    61,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   135,     0,
   135,     0,     0,     0,   134,     0,   134,   162,   163,   164,
   165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
   175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
   185,   186,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   192,     0,     0,   194,     0,     0,
     0,     0,     0,     0,     0,    61,     0,     0,   135,    61,
     0,     0,     0,     0,   134,     0,     0,   210,     0,     0,
     0,     0,     0,     0,     0,   135,     0,     0,     0,     0,
     0,   134,     0,     0,     0,     0,     0,     0,   222,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   207,     0,     0,     0,     0,     0,     0,     0,     0,
   213,   211,   212,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   224 };
yytabelem yypact[]={

   -40,-10000000,   260,-10000000,-10000000,-10000000,     7,    -3,   -85,    50,
  -299,    45,  -278,    45,    45,    45,   260,   260,   260,    20,
  -300,    48,-10000000,  -124,  -304,   482,   -50,-10000000,   401,   401,
   401,   401,   -28,-10000000,     2,   325,   325,-10000000,  -225,-10000000,
    -4,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,    47,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,   260,
   325,-10000000,-10000000,   260,   260,   260,   325,   -15,   260,   260,
   260,   260,   260,-10000000,   482,-10000000,   128,   325,-10000000,-10000000,
   325,-10000000,  -250,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,  -308,  -250,   325,
   325,   325,   325,   325,   325,   325,   325,   325,   325,   325,
   325,   325,   325,   325,   325,   325,   325,   325,   325,   325,
   325,   325,   325,   325,-10000000,  -302,-10000000,-10000000,-10000000,   325,
-10000000,   325,   453,    43,-10000000,    27,   325,   573,-10000000,   325,
-10000000,-10000000,    41,    64,   482,-10000000,-10000000,    12,    18,-10000000,
  -233,-10000000,-10000000,  -235,-10000000,-10000000,-10000000,    40,-10000000,-10000000,
-10000000,-10000000,   482,   547,   159,   596,   623,   623,   646,   646,
   670,   693,   716,   740,   740,   698,   698,   748,   271,   336,
    53,  -261,  -261,  -261,  -261,   482,   482,   -52,    39,   325,
    46,-10000000,   482,   208,   482,-10000000,-10000000,  -246,   325,   260,
   260,-10000000,   325,-10000000,-10000000,    35,   325,   509,-10000000,    45,
    16,-10000000,-10000000,   482,-10000000,    31,-10000000,-10000000,   -16,   325,
-10000000,-10000000,    29,   260,-10000000 };
yytabelem yypgo[]={

     0,   107,    55,  1011,   101,    78,   100,    99,    98,    97,
   974,  1007,   968,    93,    96,    94,    92,    53,   613,    91 };
yytabelem yyr1[]={

     0,     4,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,    11,    11,    11,    11,    11,    11,    11,    11,
    11,    11,    11,    11,    11,    11,    11,    11,    11,    11,
    11,    11,    11,    11,    11,    11,    11,    11,    11,    11,
    13,    13,    13,    13,    13,    16,    16,    16,    10,    10,
    18,    18,    17,    17,     7,     6,     6,    12,    12,    14,
    14,    14,    14,    14,    14,    14,    14,    19,    19,    15,
    15,    15,    15,    15,    15,    15,     9,     9,     9,     8,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     2,     2,     1,
     1,     1,     1 };
yytabelem yyr2[]={

     0,     5,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     5,     7,     7,     7,    11,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     2,     3,
     5,     5,     5,     5,     2,     5,     3,     7,     7,     2,
     3,     1,     7,     7,     5,    11,     9,     2,     1,     2,
     9,     5,     9,    11,    13,     7,    11,     2,     5,     5,
     7,     3,     2,     2,     2,     5,     5,     7,     7,     9,
     5,     7,     7,    15,    19,     7,    11,     7,     7,     7,
    11,     7,     7,     5,     5,     7,     4,     2,     5,     3,
     1,     3,     3 };
yytabelem yychk[]={

-10000000,    -1,    -2,   257,   256,    -3,   262,   263,   266,   269,
   270,   271,    -4,   275,   277,   278,    -8,    96,   123,   -12,
   272,    -9,   -10,   307,   308,   -11,   -13,    42,   283,   300,
   306,   282,   -14,   -15,    -5,    -6,    -7,   261,   -19,   -17,
   -16,   281,   258,   280,   265,   274,   267,   268,   273,   276,
   286,   295,   296,   289,   297,   298,   299,   309,   260,    40,
    91,    -3,    59,   -17,   265,   123,    40,   309,   -17,   292,
   -17,   -17,   -17,    -3,   -11,    -3,    -2,    44,    59,   309,
    40,    -5,   308,   270,   300,   262,   263,   266,   269,   271,
   272,   275,   276,   277,   278,   307,   309,   308,   307,   292,
    63,   291,   289,   298,   304,   295,   301,   287,   288,   285,
   296,   302,   297,   303,   286,   284,   293,   283,   299,   305,
    42,   290,    61,   294,   -13,   308,   -13,   -13,   -13,    91,
   282,    40,   259,   -18,   -10,   -12,    96,   -11,   260,    44,
    93,    40,   -10,    -2,   -11,    -3,    -3,    -2,   -12,    59,
    -3,    -3,    -3,    -3,    -3,   125,   -12,   -18,    -5,   308,
   309,    -5,   -11,   -11,   -11,   -11,   -11,   -11,   -11,   -11,
   -11,   -11,   -11,   -11,   -11,   -11,   -11,   -11,   -11,   -11,
   -11,   -11,   -11,   -11,   -11,   -11,   -11,   -18,   -18,    40,
    -5,    41,   -11,    44,   -11,    41,    41,   125,    59,   267,
   268,    41,   292,    93,    41,   -18,    40,   -11,    44,   278,
   -12,    -3,    -3,   -11,    41,   -18,    44,    41,   -17,    59,
    41,    59,   -12,    41,    -3 };
yytabelem yydef[]={

    -2,    -2,    -2,   121,   122,   117,     9,    12,    13,    15,
     6,    16,     0,    18,    20,    21,    78,    78,    78,     0,
    17,     0,    77,    30,    29,    69,    58,    59,     0,     8,
     0,     0,    64,    79,    91,    -2,     0,    92,    93,    94,
     0,     2,     3,     4,     5,     7,    10,    11,    14,    19,
    22,    23,    24,    25,    26,    27,    28,    31,    87,    78,
    66,   118,   100,    78,    78,    78,    78,     0,    78,    78,
    78,    78,    78,   113,    32,   114,    78,    78,   116,     1,
    -2,    96,    29,     6,     8,     9,    12,    13,    15,    16,
    17,    18,    19,    20,    21,    30,    89,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    60,    29,    61,    62,    63,    -2,
    81,    -2,     0,     0,    -2,     0,     0,     0,    88,     0,
    95,    74,    77,    78,    65,   101,   102,    78,     0,   105,
   107,   108,   109,   111,   112,   115,    68,     0,    97,    29,
    90,    98,    35,     0,    37,    38,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,    57,    33,    34,     0,     0,    -2,
     0,    85,    32,     0,    67,    72,    73,     0,    78,    78,
    78,    99,     0,    80,    82,     0,    -2,     0,    76,     0,
     0,   106,   110,    36,    83,     0,    75,    86,     0,    78,
    84,   103,     0,    78,   104 };
typedef struct
#ifdef __cplusplus
	yytoktype
#endif
{ char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"ERROR",	257,
	"IDENT",	258,
	"POINT",	259,
	"TEXT",	260,
	"VALUE",	261,
	"BREAK",	262,
	"CASE",	263,
	"COND",	264,
	"DEFAULT",	265,
	"DO",	266,
	"ELSE",	267,
	"ELSEW",	268,
	"FOR",	269,
	"GOTO",	270,
	"IF",	271,
	"LABEL",	272,
	"RETURN",	273,
	"SIZEOF",	274,
	"SWITCH",	275,
	"USING",	276,
	"WHERE",	277,
	"WHILE",	278,
	"ARG",	279,
	"CAST",	280,
	"CONST",	281,
	"INC",	282,
	"ADD",	283,
	"CONCAT",	284,
	"IAND",	285,
	"IN",	286,
	"IOR",	287,
	"IXOR",	288,
	"LEQV",	289,
	"POWER",	290,
	"PROMO",	291,
	"RANGE",	292,
	"SHIFT",	293,
	"BINEQ",	294,
	"LAND",	295,
	"LEQ",	296,
	"LGE",	297,
	"LOR",	298,
	"MUL",	299,
	"UNARY",	300,
	"LANDS",	301,
	"LEQS",	302,
	"LGES",	303,
	"LORS",	304,
	"MULS",	305,
	"UNARYS",	306,
	"FUN",	307,
	"MODIF",	308,
	"VBL",	309,
	",",	44,
	"`",	96,
	"=",	61,
	"?",	63,
	"*",	42,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"slabel : LABEL VBL",
	"label : CONST",
	"label : IDENT",
	"label : CAST",
	"label : DEFAULT",
	"label : GOTO",
	"label : SIZEOF",
	"label : UNARY",
	"label : BREAK",
	"label : ELSE",
	"label : ELSEW",
	"label : CASE",
	"label : DO",
	"label : RETURN",
	"label : FOR",
	"label : IF",
	"label : LABEL",
	"label : SWITCH",
	"label : USING",
	"label : WHERE",
	"label : WHILE",
	"label : IN",
	"label : LAND",
	"label : LEQ",
	"label : LEQV",
	"label : LGE",
	"label : LOR",
	"label : MUL",
	"label : MODIF",
	"label : FUN",
	"label : VBL",
	"ass : '`' ass",
	"ass : unaryX '=' ass",
	"ass : unaryX BINEQ ass",
	"ass : ass RANGE ass",
	"ass : ass '?' ass RANGE ass",
	"ass : ass PROMO ass",
	"ass : ass LEQV ass",
	"ass : ass LOR ass",
	"ass : ass LORS ass",
	"ass : ass LAND ass",
	"ass : ass LANDS ass",
	"ass : ass IOR ass",
	"ass : ass IXOR ass",
	"ass : ass IAND ass",
	"ass : ass LEQ ass",
	"ass : ass LEQS ass",
	"ass : ass LGE ass",
	"ass : ass LGES ass",
	"ass : ass IN ass",
	"ass : ass CONCAT ass",
	"ass : ass SHIFT ass",
	"ass : ass ADD ass",
	"ass : ass MUL ass",
	"ass : ass MULS ass",
	"ass : ass '*' ass",
	"ass : ass POWER ass",
	"ass : unaryX",
	"ass : '*'",
	"unaryX : ADD unaryX",
	"unaryX : UNARY unaryX",
	"unaryX : UNARYS unaryX",
	"unaryX : INC unaryX",
	"unaryX : postX",
	"bracket : '[' ass",
	"bracket : '['",
	"bracket : bracket ',' ass",
	"exp : opt ',' opt",
	"exp : ass",
	"sub : exp",
	"sub : /* empty */",
	"paren : '(' exp ')'",
	"paren : '(' stmt_lst ')'",
	"using0 : USING '('",
	"using : using0 ass ',' ass ','",
	"using : using0 ass ',' ','",
	"opt : exp",
	"opt : /* empty */",
	"postX : primaX",
	"postX : postX '[' sub ']'",
	"postX : postX INC",
	"postX : label '(' sub ')'",
	"postX : label POINT '(' sub ')'",
	"postX : label POINT label '(' sub ')'",
	"postX : using sub ')'",
	"postX : using0 ass ',' ass ')'",
	"textX : TEXT",
	"textX : textX TEXT",
	"primaX : MODIF VBL",
	"primaX : MODIF MODIF VBL",
	"primaX : label",
	"primaX : VALUE",
	"primaX : textX",
	"primaX : paren",
	"primaX : bracket ']'",
	"funvbl : FUN label",
	"funvbl : FUN MODIF label",
	"funvbl : MODIF FUN label",
	"fun : funvbl '(' sub ')'",
	"stmt : BREAK ';'",
	"stmt : CASE paren stmt",
	"stmt : CASE DEFAULT stmt",
	"stmt : DO '{' stmt_lst '}' WHILE paren ';'",
	"stmt : FOR '(' opt ';' opt ';' opt ')' stmt",
	"stmt : GOTO VBL ';'",
	"stmt : IF paren stmt ELSE stmt",
	"stmt : IF paren stmt",
	"stmt : slabel RANGE stmt",
	"stmt : SWITCH paren stmt",
	"stmt : WHERE paren stmt ELSEW stmt",
	"stmt : WHERE paren stmt",
	"stmt : WHILE paren stmt",
	"stmt : fun stmt",
	"stmt : '`' stmt",
	"stmt : '{' stmt_lst '}'",
	"stmt : opt ';'",
	"stmt_lst : stmt",
	"stmt_lst : stmt_lst stmt",
	"program : stmt_lst",
	"program : /* empty */",
	"program : ERROR",
	"program : error",
};
#endif /* YYDEBUG */
# line	1 "/usr/ccs/bin/yaccpar"
/*
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

#pragma ident	"@(#)yaccpar	6.12	93/06/07 SMI"

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#define YYNEW(type)	malloc(sizeof(type) * yynewmax)
#define YYCOPY(to, from, type) \
	(type *) memcpy(to, (char *) from, yynewmax * sizeof(type))
#define YYENLARGE( from, type) \
	(type *) realloc((char *) from, yynewmax * sizeof(type))
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-10000000)

/*
** global variables used by the parser
*/
YYSTYPE *yypv;			/* top of value stack */
int *yyps;			/* top of state stack */

int yystate;			/* current state */
int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */
int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */



#ifdef YYNMBCHARS
#define YYLEX()		yycvtok(yylex())
/*
** yycvtok - return a token if i is a wchar_t value that exceeds 255.
**	If i<255, i itself is the token.  If i>255 but the neither 
**	of the 30th or 31st bit is on, i is already a token.
*/
#if defined(__STDC__) || defined(__cplusplus)
int yycvtok(int i)
#else
int yycvtok(i) int i;
#endif
{
	int first = 0;
	int last = YYNMBCHARS - 1;
	int mid;
	wchar_t j;

	if(i&0x60000000){/*Must convert to a token. */
		if( yymbchars[last].character < i ){
			return i;/*Giving up*/
		}
		while ((last>=first)&&(first>=0)) {/*Binary search loop*/
			mid = (first+last)/2;
			j = yymbchars[mid].character;
			if( j==i ){/*Found*/ 
				return yymbchars[mid].tvalue;
			}else if( j<i ){
				first = mid + 1;
			}else{
				last = mid -1;
			}
		}
		/*No entry in the table.*/
		return i;/* Giving up.*/
	}else{/* i is already a token. */
		return i;
	}
}
#else/*!YYNMBCHARS*/
#define YYLEX()		yylex()
#endif/*!YYNMBCHARS*/

/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
#if defined(__STDC__) || defined(__cplusplus)
int yyparse(void)
#else
int yyparse()
#endif
{
	register YYSTYPE *yypvt;	/* top of value stack for $vars */

#if defined(__cplusplus) || defined(lint)
/*
	hacks to please C++ and lint - goto's inside switch should never be
	executed; yypvt is set to 0 to avoid "used before set" warning.
*/
	static int __yaccpar_lint_hack__ = 0;
	switch (__yaccpar_lint_hack__)
	{
		case 1: goto yyerrlab;
		case 2: goto yynewstate;
	}
	yypvt = 0;
#endif

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

#if YYMAXDEPTH <= 0
	if (yymaxdepth <= 0)
	{
		if ((yymaxdepth = YYEXPAND(0)) <= 0)
		{
			yyerror("yacc initialization error");
			YYABORT;
		}
	}
#endif

	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */
	goto yystack;	/* moved from 6 lines above to here to please C++ */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ yymaxdepth ] )	/* room on stack? */
		{
			/*
			** reallocate and recover.  Note that pointers
			** have to be reset, or bad things will happen
			*/
			int yyps_index = (yy_ps - yys);
			int yypv_index = (yy_pv - yyv);
			int yypvt_index = (yypvt - yyv);
			int yynewmax;
#ifdef YYEXPAND
			yynewmax = YYEXPAND(yymaxdepth);
#else
			yynewmax = 2 * yymaxdepth;	/* double table size */
			if (yymaxdepth == YYMAXDEPTH)	/* first time growth */
			{
				char *newyys = (char *)YYNEW(int);
				char *newyyv = (char *)YYNEW(YYSTYPE);
				if (newyys != 0 && newyyv != 0)
				{
					yys = YYCOPY(newyys, yys, int);
					yyv = YYCOPY(newyyv, yyv, YYSTYPE);
				}
				else
					yynewmax = 0;	/* failed */
			}
			else				/* not first time */
			{
				yys = YYENLARGE(yys, int);
				yyv = YYENLARGE(yyv, YYSTYPE);
				if (yys == 0 || yyv == 0)
					yynewmax = 0;	/* failed */
			}
#endif
			if (yynewmax <= yymaxdepth)	/* tables not expanded */
			{
				yyerror( "yacc stack overflow" );
				YYABORT;
			}
			yymaxdepth = yynewmax;

			yy_ps = yys + yyps_index;
			yy_pv = yyv + yypv_index;
			yypvt = yyv + yypvt_index;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = YYLEX() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = YYLEX() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
			skip_init:
				yynerrs++;
				/* FALLTHRU */
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 1:
# line 183 "TdiYacc.y"
{yyval.mark=yypvt[-0].mark;} break;
case 32:
# line 198 "TdiYacc.y"
{yyval.mark.rptr=yypvt[-0].mark.rptr; yyval.mark.builtin= -2;
					TdiRefZone.l_status=TdiYacc_IMMEDIATE(&yyval.mark.rptr);
					if (!(TdiRefZone.l_status & 1)) {yyerror(0);}
					} break;
case 33:
# line 202 "TdiYacc.y"
{_JUST2(OpcEquals,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 34:
# line 203 "TdiYacc.y"
{struct marker tmp;		/*binary operation and assign*/
						_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,tmp); 
						_JUST1(OpcEqualsFirst,tmp,yyval.mark);} break;
case 35:
# line 206 "TdiYacc.y"
{if (yypvt[-0].mark.rptr && yypvt[-0].mark.rptr->dtype == DTYPE_RANGE)
						if (yypvt[-0].mark.rptr->ndesc == 2)
							{yyval.mark=yypvt[-0].mark;
							yyval.mark.rptr->dscptrs[2]=yyval.mark.rptr->dscptrs[1];
							yyval.mark.rptr->dscptrs[1]=yyval.mark.rptr->dscptrs[0];
							yyval.mark.rptr->dscptrs[0]=(struct descriptor *)yypvt[-2].mark.rptr;
							++yyval.mark.rptr->ndesc;}
						else {TdiRefZone.l_status=TdiEXTRA_ARG; return(1);}
					else	{static unsigned int vmlen = sizeof(struct descriptor_range);
						LibGetVm(&vmlen, &yyval.mark.rptr, &TdiRefZone.l_zone);
						yyval.mark.rptr->length = 0;
						yyval.mark.rptr->dtype = DTYPE_RANGE;
						yyval.mark.rptr->class = CLASS_R;
						yyval.mark.rptr->pointer = 0;
						yyval.mark.rptr->ndesc = 2;
						yyval.mark.rptr->dscptrs[0]=(struct descriptor *)yypvt[-2].mark.rptr;
						yyval.mark.rptr->dscptrs[1]=(struct descriptor *)yypvt[-0].mark.rptr;
						_RESOLVE(yyval.mark);}
					} break;
case 36:
# line 225 "TdiYacc.y"
{_JUST3(OpcConditional,yypvt[-2].mark,yypvt[-0].mark,yypvt[-4].mark,yyval.mark);} break;
case 37:
# line 226 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 38:
# line 227 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 39:
# line 228 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 40:
# line 229 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 41:
# line 230 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 42:
# line 231 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 43:
# line 232 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 44:
# line 233 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 45:
# line 234 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 46:
# line 235 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 47:
# line 236 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 48:
# line 237 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 49:
# line 238 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 50:
# line 239 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 51:
# line 240 "TdiYacc.y"
{if (yyval.mark.rptr == 0) yyval.mark=yypvt[-0].mark;
					else if (yyval.mark.rptr->dtype == DTYPE_FUNCTION
					&&	*(unsigned short *)yyval.mark.rptr->pointer == OpcConcat
					&&	yyval.mark.rptr->ndesc < 250)
						{yyval.mark.rptr->dscptrs[yyval.mark.rptr->ndesc++]=(struct descriptor *)yypvt[-0].mark.rptr;
						_RESOLVE(yyval.mark);}
					else {_FULL2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);}
					} break;
case 52:
# line 248 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 53:
# line 249 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 54:
# line 250 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 55:
# line 251 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 56:
# line 252 "TdiYacc.y"
{_JUST2(OpcMultiply,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 57:
# line 253 "TdiYacc.y"
{_JUST2(yypvt[-1].mark.builtin,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 59:
# line 255 "TdiYacc.y"
{_EMPTY_MARKER;} break;
case 60:
# line 257 "TdiYacc.y"
{int j;
					if (yypvt[-1].mark.builtin == OpcAdd)	j=OpcUnaryPlus;
					else				j=OpcUnaryMinus;
					_JUST1(j,yypvt[-0].mark,yyval.mark);} break;
case 61:
# line 261 "TdiYacc.y"
{_JUST1(yypvt[-1].mark.builtin,yypvt[-0].mark,yyval.mark);} break;
case 62:
# line 262 "TdiYacc.y"
{_JUST1(yypvt[-1].mark.builtin,yypvt[-0].mark,yyval.mark);} break;
case 63:
# line 263 "TdiYacc.y"
{_JUST1(yypvt[-1].mark.builtin,yypvt[-0].mark,yyval.mark);} break;
case 65:
# line 271 "TdiYacc.y"
{_FULL1(OpcVector,yypvt[-0].mark,yyval.mark);} break;
case 66:
# line 272 "TdiYacc.y"
{_JUST0(OpcVector,yyval.mark);} break;
case 67:
# line 273 "TdiYacc.y"
{if (yyval.mark.rptr->ndesc >= 250) {
					_RESOLVE(yypvt[-2].mark);
					_FULL1(OpcVector,yypvt[-2].mark,yyval.mark);
				}
				yyval.mark.rptr->dscptrs[yyval.mark.rptr->ndesc++] = (struct descriptor *)yypvt[-0].mark.rptr;
				} break;
case 68:
# line 280 "TdiYacc.y"
{if (yyval.mark.rptr			/*comma is left-to-right weakest*/
					&&	yyval.mark.builtin != -2
					&&	yyval.mark.rptr->dtype == DTYPE_FUNCTION
					&&	*(unsigned short *)yyval.mark.rptr->pointer == OpcComma
					&&	yyval.mark.rptr->ndesc < 250)
						yyval.mark.rptr->dscptrs[yyval.mark.rptr->ndesc++]=(struct descriptor *)yypvt[-0].mark.rptr;
					else _FULL2(OpcComma,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);/*first comma*/
					} break;
case 70:
# line 290 "TdiYacc.y"
{if (yyval.mark.rptr
				&&	yyval.mark.builtin != -2
				&& yyval.mark.rptr->dtype == DTYPE_FUNCTION
				&& *(unsigned short *)yyval.mark.rptr->pointer == OpcComma) ;
				else _JUST1(OpcAbort,yypvt[-0].mark,yyval.mark);} break;
case 71:
# line 295 "TdiYacc.y"
{_JUST0(OpcAbort,yyval.mark);} break;
case 72:
# line 297 "TdiYacc.y"
{yyval.mark=yypvt[-1].mark; yyval.mark.builtin= -2;} break;
case 73:
# line 298 "TdiYacc.y"
{yyval.mark=yypvt[-1].mark; yyval.mark.builtin= -2;} break;
case 74:
# line 300 "TdiYacc.y"
{++TdiRefZone.l_rel_path;} break;
case 75:
# line 302 "TdiYacc.y"
{_FULL2(OpcAbort,yypvt[-3].mark,yypvt[-1].mark,yyval.mark); --TdiRefZone.l_rel_path;} break;
case 76:
# line 303 "TdiYacc.y"
{_FULL2(OpcAbort,yypvt[-2].mark,_EMPTY_MARKER,yyval.mark); --TdiRefZone.l_rel_path;} break;
case 78:
# line 306 "TdiYacc.y"
{yyval.mark=_EMPTY_MARKER;} break;
case 80:
# line 312 "TdiYacc.y"
{int j;
					yyval.mark=yypvt[-1].mark;
					yyval.mark.rptr->pointer= (unsigned char *)&OpcSubscript;
					for (j=yyval.mark.rptr->ndesc; --j>=0; )
						yyval.mark.rptr->dscptrs[j+1]=yyval.mark.rptr->dscptrs[j];
					yyval.mark.rptr->dscptrs[0]=(struct descriptor *)yypvt[-3].mark.rptr;
					yyval.mark.rptr->ndesc++;
					_RESOLVE(yyval.mark);
				} break;
case 81:
# line 321 "TdiYacc.y"
{int j=yypvt[-0].mark.builtin==OpcPreInc ? OpcPostInc : OpcPostDec;
					_JUST1(j,yypvt[-1].mark,yyval.mark);} break;
case 82:
# line 323 "TdiYacc.y"
{yyval.mark=yypvt[-1].mark;
				if (yypvt[-3].mark.builtin < 0) {int j;		/*unknown today*/
					yyval.mark.rptr->pointer= (unsigned char *)&OpcExtFunction;
					for (j=yyval.mark.rptr->ndesc; --j>=0;)
						yyval.mark.rptr->dscptrs[j+2]=yyval.mark.rptr->dscptrs[j];
					yyval.mark.rptr->dscptrs[0]=0;
					yyval.mark.rptr->dscptrs[1]=(struct descriptor *)yypvt[-3].mark.rptr;
					yyval.mark.rptr->ndesc += 2;
				}
				else	{				/*intrinsic*/
					*(unsigned short *)yyval.mark.rptr->pointer=yypvt[-3].mark.builtin;
					_RESOLVE(yyval.mark);}
				} break;
case 83:
# line 336 "TdiYacc.y"
{int j;
				yyval.mark=yypvt[-1].mark;			/*external*/
				yyval.mark.rptr->dtype=DTYPE_CALL;
				yyval.mark.rptr->length=0;
				yyval.mark.rptr->pointer=0;
				for (j=yyval.mark.rptr->ndesc; --j>=0;)
					yyval.mark.rptr->dscptrs[j+2]=yyval.mark.rptr->dscptrs[j];
				yyval.mark.rptr->dscptrs[0]=(struct descriptor *)yypvt[-4].mark.rptr;
				yyval.mark.rptr->dscptrs[1]=(struct descriptor *)yypvt[-3].mark.rptr;
				yyval.mark.rptr->ndesc += 2;
				} break;
case 84:
# line 347 "TdiYacc.y"
{int j;
				yyval.mark=yypvt[-1].mark;			/*typed external*/
				StrUpcase(yypvt[-3].mark.rptr, yypvt[-3].mark.rptr);
				for (j=TdiCAT_MAX; --j>=0;)
					if (strncmp(TdiREF_CAT[j].name, (char *)yypvt[-3].mark.rptr->pointer, yypvt[-3].mark.rptr->length) == 0
					&& strlen(TdiREF_CAT[j].name) == yypvt[-3].mark.rptr->length) break;
				if (j<0) {TdiRefZone.l_status=TdiINVDTYDSC; return(1);}
				yyval.mark.rptr->dtype=DTYPE_CALL;
				yyval.mark.rptr->length=1;
				*(unsigned char *)yyval.mark.rptr->pointer=(unsigned char)j;
				for (j=yyval.mark.rptr->ndesc; --j>=0;)
					yyval.mark.rptr->dscptrs[j+2]=yyval.mark.rptr->dscptrs[j];
				yyval.mark.rptr->dscptrs[0]=(struct descriptor *)yypvt[-5].mark.rptr;
				yyval.mark.rptr->dscptrs[1]=(struct descriptor *)yypvt[-4].mark.rptr;
				yyval.mark.rptr->ndesc += 2;
				} break;
case 85:
# line 363 "TdiYacc.y"
{int j;	/*USING(expr,[default],[shotid],[expt])*/
					yyval.mark.rptr->pointer= (unsigned char *)&OpcUsing;
					for (j=0; j < yypvt[-1].mark.rptr->ndesc; ++j)
						yyval.mark.rptr->dscptrs[yyval.mark.rptr->ndesc++]=yypvt[-1].mark.rptr->dscptrs[j];
				} break;
case 86:
# line 368 "TdiYacc.y"
{_JUST2(OpcUsing,yypvt[-3].mark,yypvt[-1].mark,yyval.mark); --TdiRefZone.l_rel_path;} break;
case 88:
# line 374 "TdiYacc.y"
{MAKE_S(DTYPE_T, yypvt[-1].mark.rptr->length + yypvt[-0].mark.rptr->length, yyval.mark.rptr);
					StrConcat(yyval.mark.rptr, yypvt[-1].mark.rptr, yypvt[-0].mark.rptr MDS_END_ARG);
				} break;
case 89:
# line 383 "TdiYacc.y"
{_JUST1(yypvt[-1].mark.builtin,yypvt[-0].mark,yyval.mark);} break;
case 90:
# line 384 "TdiYacc.y"
{struct marker tmp;			/*OPTIONAL IN/INOUT/OUT*/
					_JUST1(yypvt[-1].mark.builtin,yypvt[-0].mark,tmp);
					_JUST1(yypvt[-2].mark.builtin,tmp,yyval.mark);} break;
case 91:
# line 387 "TdiYacc.y"
{if (*yyval.mark.rptr->pointer == '$') {
					if(yyval.mark.builtin < 0) yyval.mark.rptr->dtype=DTYPE_IDENT;
					else if ((TdiRefFunction[yyval.mark.builtin].token & LEX_M_TOKEN) == (unsigned int)LEX_ARG
					&& !((TdiRefZone.l_status=TdiYacc_ARG(&yyval.mark)) & 1)) {yyerror(0);}
					else if ((TdiRefFunction[yyval.mark.builtin].token & LEX_M_TOKEN) == (unsigned int)LEX_CONST)
						_JUST0(yypvt[-0].mark.builtin,yyval.mark);
				}
				else if (*yyval.mark.rptr->pointer == '_')
					yyval.mark.rptr->dtype=DTYPE_IDENT;
				else if (TdiLexPath(yypvt[-0].mark.rptr->length, yypvt[-0].mark.rptr->pointer, &yyval.mark) == ERROR)
					{yyerror(0);}
				} break;
case 95:
# line 402 "TdiYacc.y"
{_RESOLVE(yyval.mark);} break;
case 96:
# line 409 "TdiYacc.y"
{yyval.mark=yypvt[-0].mark;} break;
case 97:
# line 410 "TdiYacc.y"
{_JUST1(yypvt[-1].mark.builtin,yypvt[-0].mark,yyval.mark);} break;
case 98:
# line 411 "TdiYacc.y"
{_JUST1(yypvt[-2].mark.builtin,yypvt[-0].mark,yyval.mark);} break;
case 99:
# line 413 "TdiYacc.y"
{int j;	yyval.mark=yypvt[-1].mark;
							yyval.mark.rptr->pointer= (unsigned char *)&OpcFun;
							for (j=yyval.mark.rptr->ndesc; --j>=0;)
								yyval.mark.rptr->dscptrs[j+2]=yyval.mark.rptr->dscptrs[j];
							yyval.mark.rptr->dscptrs[0]=(struct descriptor *)yypvt[-3].mark.rptr;
							yyval.mark.rptr->ndesc += 2;
							/*++TdiRefZone.l_rel_path;*/
						} break;
case 100:
# line 422 "TdiYacc.y"
{_JUST0(yypvt[-1].mark.builtin,yyval.mark);} break;
case 101:
# line 423 "TdiYacc.y"
{_FULL2(yypvt[-2].mark.builtin,yypvt[-1].mark,yypvt[-0].mark,yyval.mark);} break;
case 102:
# line 424 "TdiYacc.y"
{_FULL1(yypvt[-1].mark.builtin,yypvt[-0].mark,yyval.mark);} break;
case 103:
# line 425 "TdiYacc.y"
{_JUST2(yypvt[-6].mark.builtin,yypvt[-1].mark,yypvt[-4].mark,yyval.mark);} break;
case 104:
# line 426 "TdiYacc.y"
{_JUST4(yypvt[-8].mark.builtin,yypvt[-6].mark,yypvt[-4].mark,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 105:
# line 427 "TdiYacc.y"
{_JUST1(yypvt[-2].mark.builtin,yypvt[-1].mark,yyval.mark);} break;
case 106:
# line 428 "TdiYacc.y"
{_JUST3(yypvt[-4].mark.builtin,yypvt[-3].mark,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 107:
# line 429 "TdiYacc.y"
{_JUST2(yypvt[-2].mark.builtin,yypvt[-1].mark,yypvt[-0].mark,yyval.mark);} break;
case 108:
# line 430 "TdiYacc.y"
{_FULL2(OpcLabel,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 109:
# line 433 "TdiYacc.y"
{_JUST2(yypvt[-2].mark.builtin,yypvt[-1].mark,yypvt[-0].mark,yyval.mark);} break;
case 110:
# line 434 "TdiYacc.y"
{_JUST3(yypvt[-4].mark.builtin,yypvt[-3].mark,yypvt[-2].mark,yypvt[-0].mark,yyval.mark);} break;
case 111:
# line 435 "TdiYacc.y"
{_JUST2(yypvt[-2].mark.builtin,yypvt[-1].mark,yypvt[-0].mark,yyval.mark);} break;
case 112:
# line 436 "TdiYacc.y"
{_JUST2(yypvt[-2].mark.builtin,yypvt[-1].mark,yypvt[-0].mark,yyval.mark);} break;
case 113:
# line 437 "TdiYacc.y"
{/*TdiRefZone.l_rel_path--;*/
						yyval.mark.rptr->dscptrs[1]=(struct descriptor *)yypvt[-0].mark.rptr;} break;
case 114:
# line 439 "TdiYacc.y"
{yyval.mark.rptr=yypvt[-0].mark.rptr; yyval.mark.builtin= -2;
						TdiRefZone.l_status=TdiYacc_IMMEDIATE(&yyval.mark.rptr);
						if (!(TdiRefZone.l_status & 1)) {yyerror(0);}
						} break;
case 115:
# line 443 "TdiYacc.y"
{yyval.mark=yypvt[-1].mark; _RESOLVE(yyval.mark);} break;
case 118:
# line 447 "TdiYacc.y"
{short opcode;
				if (yyval.mark.rptr == 0) {yyval.mark=yypvt[-0].mark;}		/* initial null statement	*/
				else if (yypvt[-0].mark.rptr == 0) {}		/* trailing null statement	*/
				else if (yyval.mark.rptr->dtype == DTYPE_FUNCTION
				&& yyval.mark.rptr->ndesc < 250
				&& ((opcode = *(unsigned short *)yyval.mark.rptr->pointer) == OpcStatement
					|| opcode == OpcCase
					|| opcode == OpcDefault
					|| opcode == OpcLabel
				)) {yyval.mark.rptr->dscptrs[yyval.mark.rptr->ndesc++]=(struct descriptor *)yypvt[-0].mark.rptr;}
				else	{_FULL2(OpcStatement,yypvt[-1].mark,yypvt[-0].mark,yyval.mark);}
			} break;
case 119:
# line 460 "TdiYacc.y"
{_RESOLVE(yyval.mark);		/*statements*/
			TdiRefZone.a_result=(struct descriptor_d *)yyval.mark.rptr;
			TdiRefZone.l_status=1;} break;
case 120:
# line 463 "TdiYacc.y"
{yyval.mark=_EMPTY_MARKER;} break;
case 121:
# line 464 "TdiYacc.y"
{} break;
case 122:
# line 465 "TdiYacc.y"
{TdiRefZone.l_status=TdiSYNTAX;} break;
# line	532 "/usr/ccs/bin/yaccpar"
	}
	goto yystack;		/* reset registers in driver code */
}

