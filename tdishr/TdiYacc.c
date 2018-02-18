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

//#line 2 "TdiYacc.y"
/*      TdiYacc.Y
        YACC converts this to TdiYacc.C to compile TDI statements.
        Each YACC-LEX symbol has a returned token and a tdiyylval value.
        Precedence is associated with a token and is set left and right below.
        TdiLex_... routines and TdiRefFunction depend on tokens from TdiYacc.
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
#include <STATICdef.h>
#include <stdio.h>
#include <string.h>
#include "tdithreadsafe.h"
#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefzone.h"
#include "tdirefstandard.h"

#include <libroutines.h>
#include <strroutines.h>
#include <tdishr_messages.h>
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

extern int TdiYacc_RESOLVE();
extern int TdiLex();
extern int TdiYacc_IMMEDIATE();
extern int TdiYacc_BUILD();
extern int TdiYacc_ARG();
extern int TdiLexPath();

#define YYMAXDEPTH      250

#define _RESOLVE(arg)   			if IS_NOT_OK(TdiYacc_RESOLVE(&arg.rptr)) {tdiyyerror(0);}
#define _FULL1(opcode,arg1,out)                 if IS_NOT_OK(TdiYacc_BUILD(255, 1, opcode, &out, &arg1)) {tdiyyerror(0);}
#define _FULL2(opcode,arg1,arg2,out)            if IS_NOT_OK(TdiYacc_BUILD(255, 2, opcode, &out, &arg1, &arg2)) {tdiyyerror(0);}
	/*****************************
        Two args for image->routine.
        *****************************/
#define _JUST0(opcode,out)                      if IS_NOT_OK(TdiYacc_BUILD(2, 0, opcode, &out)) {tdiyyerror(0);}
#define _JUST1(opcode,arg1,out)                 if IS_NOT_OK(TdiYacc_BUILD(3, 1, opcode, &out, &arg1)) {tdiyyerror(0);}
#define _JUST2(opcode,arg1,arg2,out)            if IS_NOT_OK(TdiYacc_BUILD(2, 2, opcode, &out, &arg1, &arg2)) {tdiyyerror(0);}
#define _JUST3(opcode,arg1,arg2,arg3,out)       if IS_NOT_OK(TdiYacc_BUILD(3, 3, opcode, &out, &arg1, &arg2, &arg3)) {tdiyyerror(0);}
#define _JUST4(opcode,arg1,arg2,arg3,arg4,out)  if IS_NOT_OK(TdiYacc_BUILD(4, 4, opcode, &out, &arg1, &arg2, &arg3, &arg4)) {tdiyyerror(0);}

STATIC_THREADSAFE struct marker _EMPTY_MARKER = { 0 };

#include "tdiyacc.h"

#define tdiyyclearin tdiyychar = -1
#define tdiyyerrok tdiyyerrflag = 0
extern int tdiyychar;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif

/* __YYSCLASS defines the scoping/storage class for global objects
 * that are NOT renamed by the -p option.  By default these names
 * are going to be 'STATIC_THREADSAFE' so that multi-definition errors
 * will not occur with multiple parsers.
 * If you want (unsupported) access to internal names you need
 * to define this to be null so it implies 'extern' scope.
 * This should not be used in conjunction with -p.
 */
#ifndef __YYSCLASS
#define __YYSCLASS STATIC_THREADSAFE
#endif
YYSTYPE tdiyylval;
__YYSCLASS YYSTYPE tdiyyval;
typedef int tdiyytabelem;
#define YYERRCODE 256

//#line 468 "TdiYacc.y"

const int
    LEX_ERROR = ERROR,
    LEX_IDENT = IDENT,
    LEX_POINT = POINT,
    LEX_TEXT = TEXT,
    LEX_VALUE = VALUE,
    LEX_ARG = ARG,
    LEX_BREAK = BREAK,
    LEX_CASE = CASE,
    LEX_COND = COND,
    LEX_DEFAULT = DEFAULT,
    LEX_DO = DO,
    LEX_ELSE = ELSE,
    LEX_ELSEW = ELSEW,
    LEX_FOR = FOR,
    LEX_GOTO = GOTO,
    LEX_IF = IF,
    LEX_LABEL = LABEL,
    LEX_RETURN = RETURN,
    LEX_SIZEOF = SIZEOF,
    LEX_SWITCH = SWITCH,
    LEX_USING = USING,
    LEX_WHERE = WHERE,
    LEX_WHILE = WHILE,
    LEX_CAST = CAST,
    LEX_CONST = CONST,
    LEX_INC = INC,
    LEX_ADD = ADD,
    LEX_BINEQ = BINEQ,
    LEX_CONCAT = CONCAT,
    LEX_IAND = IAND,
    LEX_IN = IN,
    LEX_IOR = IOR,
    LEX_IXOR = IXOR,
    LEX_LEQV = LEQV,
    LEX_POWER = POWER,
    LEX_PROMO = PROMO,
    LEX_RANGE = RANGE,
    LEX_SHIFT = SHIFT,
    LEX_LAND = LAND,
    LEX_LEQ = LEQ,
    LEX_LGE = LGE,
    LEX_LOR = LOR,
    LEX_MUL = MUL,
    LEX_UNARY = UNARY,
    LEX_LANDS = LANDS,
    LEX_LEQS = LEQS,
    LEX_LGES = LGES,
    LEX_LORS = LORS,
    LEX_MULS = MULS,
    LEX_UNARYS = UNARYS,
    LEX_FUN = FUN,
    LEX_VBL = VBL,
    LEX_MODIF = MODIF;

YYSTYPE *TdiYylvalPtr = &tdiyylval;
__YYSCLASS tdiyytabelem tdiyyexca[] = {
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

#define YYNPROD 123
#define YYLAST 1236
__YYSCLASS tdiyytabelem tdiyyact[] = {

  59, 160, 27, 98, 97, 96, 97, 96, 42, 79,
  67, 122, 85, 86, 69, 44, 87, 46, 47, 88,
  83, 89, 90, 48, 45, 91, 92, 93, 94, 121,
  43, 41, 209, 200, 199, 138, 50, 59, 65, 53,
  139, 203, 131, 221, 149, 51, 52, 54, 55, 56,
  84, 60, 59, 39, 27, 2, 17, 95, 159, 57,
  77, 63, 77, 129, 77, 68, 62, 70, 71, 72,
  223, 77, 220, 77, 76, 219, 214, 198, 34, 78,
  204, 201, 195, 18, 191, 59, 206, 141, 80, 140,
  66, 38, 40, 26, 33, 120, 32, 21, 16, 36,
  35, 12, 81, 60, 59, 196, 27, 1, 17, 0,
  0, 0, 0, 0, 0, 143, 0, 0, 0, 0,
  0, 147, 124, 126, 127, 128, 0, 0, 0, 0,
  0, 0, 0, 0, 42, 18, 0, 197, 85, 86,
  0, 44, 87, 46, 47, 88, 83, 89, 90, 48,
  45, 91, 92, 93, 94, 60, 43, 41, 0, 0,
  17, 158, 50, 0, 0, 53, 0, 0, 59, 0,
  27, 51, 52, 54, 55, 56, 84, 161, 0, 0,
  0, 0, 0, 95, 82, 57, 0, 18, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 120, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 190, 0, 0, 0, 0, 4, 3, 42, 60,
  58, 37, 6, 7, 17, 44, 8, 46, 47, 9,
  10, 11, 20, 48, 45, 13, 49, 14, 15, 0,
  43, 41, 31, 28, 123, 0, 50, 0, 59, 53,
  27, 18, 208, 155, 130, 51, 52, 54, 55, 56,
  29, 132, 64, 218, 0, 0, 30, 23, 24, 57,
  42, 0, 58, 37, 6, 7, 0, 44, 8, 46,
  47, 9, 10, 11, 20, 48, 45, 13, 49, 14,
  15, 0, 43, 41, 31, 28, 0, 0, 50, 60,
  59, 53, 27, 0, 136, 0, 0, 51, 52, 54,
  55, 56, 29, 120, 0, 0, 0, 0, 30, 23,
  24, 57, 42, 0, 58, 37, 6, 7, 0, 44,
  8, 46, 47, 9, 10, 11, 20, 48, 45, 13,
  49, 14, 15, 121, 43, 41, 31, 28, 0, 0,
  50, 60, 118, 53, 0, 0, 17, 0, 119, 51,
  52, 54, 55, 56, 29, 59, 0, 27, 0, 0,
  30, 23, 24, 57, 0, 0, 0, 0, 120, 0,
  0, 0, 0, 18, 0, 0, 42, 0, 58, 37,
  6, 7, 0, 44, 8, 46, 47, 9, 10, 11,
  20, 48, 45, 13, 49, 14, 15, 0, 43, 41,
  31, 28, 0, 0, 50, 0, 60, 53, 0, 0,
  0, 136, 0, 51, 52, 54, 55, 56, 29, 0,
  0, 0, 0, 0, 30, 23, 24, 57, 0, 0,
  0, 59, 117, 115, 109, 114, 107, 108, 102, 121,
  101, 0, 116, 0, 105, 110, 112, 103, 118, 0,
  106, 111, 113, 104, 119, 0, 42, 0, 58, 37,
  85, 86, 0, 44, 87, 46, 47, 88, 83, 89,
  90, 48, 45, 91, 49, 93, 94, 0, 43, 41,
  31, 28, 60, 189, 50, 0, 0, 53, 0, 0,
  0, 0, 0, 51, 52, 54, 55, 56, 29, 0,
  0, 0, 0, 0, 30, 95, 125, 57, 42, 0,
  58, 37, 6, 7, 120, 44, 8, 46, 47, 9,
  10, 11, 20, 48, 45, 13, 49, 14, 15, 0,
  43, 41, 31, 28, 0, 100, 50, 0, 0, 53,
  217, 120, 0, 216, 117, 51, 52, 54, 55, 56,
  29, 121, 0, 0, 116, 0, 30, 23, 24, 57,
  118, 0, 100, 0, 0, 0, 119, 0, 0, 0,
  0, 0, 0, 42, 0, 58, 37, 85, 86, 120,
  44, 87, 46, 47, 88, 83, 89, 90, 48, 45,
  91, 49, 93, 94, 0, 43, 41, 31, 28, 0,
  100, 50, 0, 133, 53, 120, 0, 193, 0, 117,
  51, 52, 54, 55, 56, 29, 121, 0, 0, 0,
  0, 30, 95, 125, 57, 118, 100, 0, 120, 0,
  0, 119, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 42,
  0, 58, 37, 85, 86, 120, 44, 87, 46, 47,
  88, 83, 89, 90, 48, 45, 91, 49, 93, 94,
  0, 43, 41, 31, 28, 0, 0, 50, 120, 0,
  53, 0, 0, 0, 157, 0, 51, 52, 54, 55,
  56, 29, 0, 0, 0, 0, 0, 30, 95, 125,
  57, 42, 120, 0, 0, 85, 86, 0, 44, 87,
  46, 47, 88, 83, 89, 90, 48, 45, 91, 92,
  93, 94, 0, 43, 41, 120, 0, 0, 0, 50,
  120, 0, 53, 187, 0, 188, 0, 0, 51, 52,
  54, 55, 56, 84, 0, 0, 0, 0, 120, 0,
  95, 159, 57, 0, 0, 117, 115, 109, 114, 107,
  108, 102, 121, 101, 99, 116, 0, 105, 110, 112,
  103, 118, 120, 106, 111, 113, 104, 119, 0, 0,
  120, 0, 117, 115, 109, 114, 107, 108, 102, 121,
  101, 99, 116, 205, 105, 110, 112, 103, 118, 0,
  106, 111, 113, 104, 119, 0, 0, 0, 0, 0,
  215, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  117, 115, 109, 114, 107, 108, 102, 121, 101, 202,
  116, 0, 105, 110, 112, 103, 118, 0, 106, 111,
  113, 104, 119, 0, 0, 0, 117, 115, 109, 114,
  107, 108, 102, 121, 101, 99, 116, 0, 105, 110,
  112, 103, 118, 0, 106, 111, 113, 104, 119, 117,
  115, 109, 114, 107, 108, 0, 121, 0, 0, 116,
  0, 105, 110, 112, 103, 118, 0, 106, 111, 113,
  104, 119, 0, 0, 0, 0, 117, 115, 109, 114,
  107, 108, 0, 121, 0, 0, 116, 0, 105, 110,
  112, 0, 118, 0, 106, 111, 113, 0, 119, 117,
  115, 109, 114, 107, 108, 0, 121, 0, 0, 116,
  0, 0, 110, 112, 0, 118, 0, 0, 111, 113,
  0, 119, 0, 117, 115, 109, 114, 0, 108, 0,
  121, 0, 0, 116, 0, 0, 110, 112, 19, 118,
  0, 0, 111, 113, 22, 119, 117, 115, 109, 114,
  0, 117, 115, 121, 114, 0, 116, 0, 121, 110,
  112, 116, 118, 0, 0, 111, 113, 118, 119, 117,
  115, 0, 114, 119, 135, 0, 121, 25, 0, 116,
  134, 5, 110, 112, 61, 118, 0, 0, 111, 113,
  0, 119, 0, 117, 115, 74, 114, 0, 73, 75,
  121, 117, 115, 116, 142, 148, 0, 112, 121, 118,
  0, 116, 0, 113, 137, 119, 156, 118, 0, 135,
  0, 0, 0, 119, 0, 134, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 144, 0,
  0, 0, 0, 0, 0, 145, 146, 0, 0, 0,
  150, 151, 152, 153, 154, 0, 0, 0, 61, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 135, 0,
  135, 0, 0, 0, 134, 0, 134, 162, 163, 164,
  165, 166, 167, 168, 169, 170, 171, 172, 173, 174,
  175, 176, 177, 178, 179, 180, 181, 182, 183, 184,
  185, 186, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 192, 0, 0, 194, 0, 0,
  0, 0, 0, 0, 0, 61, 0, 0, 135, 61,
  0, 0, 0, 0, 134, 0, 0, 210, 0, 0,
  0, 0, 0, 0, 0, 135, 0, 0, 0, 0,
  0, 134, 0, 0, 0, 0, 0, 0, 222, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 207, 0, 0, 0, 0, 0, 0, 0, 0,
  213, 211, 212, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 224
};

__YYSCLASS tdiyytabelem tdiyypact[] = {

  -40, -3000, 260, -3000, -3000, -3000, 7, -3, -85, 50,
  -299, 45, -278, 45, 45, 45, 260, 260, 260, 20,
  -300, 48, -3000, -124, -304, 482, -50, -3000, 401, 401,
  401, 401, -28, -3000, 2, 325, 325, -3000, -225, -3000,
  -4, -3000, -3000, -3000, -3000, -3000, -3000, -3000, -3000, 47,
  -3000, -3000, -3000, -3000, -3000, -3000, -3000, -3000, -3000, 260,
  325, -3000, -3000, 260, 260, 260, 325, -15, 260, 260,
  260, 260, 260, -3000, 482, -3000, 128, 325, -3000, -3000,
  325, -3000, -250, -3000, -3000, -3000, -3000, -3000, -3000, -3000,
  -3000, -3000, -3000, -3000, -3000, -3000, -3000, -308, -250, 325,
  325, 325, 325, 325, 325, 325, 325, 325, 325, 325,
  325, 325, 325, 325, 325, 325, 325, 325, 325, 325,
  325, 325, 325, 325, -3000, -302, -3000, -3000, -3000, 325,
  -3000, 325, 453, 43, -3000, 27, 325, 573, -3000, 325,
  -3000, -3000, 41, 64, 482, -3000, -3000, 12, 18, -3000,
  -233, -3000, -3000, -235, -3000, -3000, -3000, 40, -3000, -3000,
  -3000, -3000, 482, 547, 159, 596, 623, 623, 646, 646,
  670, 693, 716, 740, 740, 698, 698, 748, 271, 336,
  53, -261, -261, -261, -261, 482, 482, -52, 39, 325,
  46, -3000, 482, 208, 482, -3000, -3000, -246, 325, 260,
  260, -3000, 325, -3000, -3000, 35, 325, 509, -3000, 45,
  16, -3000, -3000, 482, -3000, 31, -3000, -3000, -16, 325,
  -3000, -3000, 29, 260, -3000
};

__YYSCLASS tdiyytabelem tdiyypgo[] = {

  0, 107, 55, 1011, 101, 78, 100, 99, 98, 97,
  974, 1007, 968, 93, 96, 94, 92, 53, 613, 91
};

__YYSCLASS tdiyytabelem tdiyyr1[] = {

  0, 4, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  13, 13, 13, 13, 13, 16, 16, 16, 10, 10,
  18, 18, 17, 17, 7, 6, 6, 12, 12, 14,
  14, 14, 14, 14, 14, 14, 14, 19, 19, 15,
  15, 15, 15, 15, 15, 15, 9, 9, 9, 8,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 2, 2, 1,
  1, 1, 1
};

__YYSCLASS tdiyytabelem tdiyyr2[] = {

  0, 5, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 5, 7, 7, 7, 11, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 2, 3,
  5, 5, 5, 5, 2, 5, 3, 7, 7, 2,
  3, 1, 7, 7, 5, 11, 9, 2, 1, 2,
  9, 5, 9, 11, 13, 7, 11, 2, 5, 5,
  7, 3, 2, 2, 2, 5, 5, 7, 7, 9,
  5, 7, 7, 15, 19, 7, 11, 7, 7, 7,
  11, 7, 7, 5, 5, 7, 4, 2, 5, 3,
  1, 3, 3
};

__YYSCLASS tdiyytabelem tdiyychk[] = {

  -3000, -1, -2, 257, 256, -3, 262, 263, 266, 269,
  270, 271, -4, 275, 277, 278, -8, 96, 123, -12,
  272, -9, -10, 307, 308, -11, -13, 42, 283, 300,
  306, 282, -14, -15, -5, -6, -7, 261, -19, -17,
  -16, 281, 258, 280, 265, 274, 267, 268, 273, 276,
  286, 295, 296, 289, 297, 298, 299, 309, 260, 40,
  91, -3, 59, -17, 265, 123, 40, 309, -17, 292,
  -17, -17, -17, -3, -11, -3, -2, 44, 59, 309,
  40, -5, 308, 270, 300, 262, 263, 266, 269, 271,
  272, 275, 276, 277, 278, 307, 309, 308, 307, 292,
  63, 291, 289, 298, 304, 295, 301, 287, 288, 285,
  296, 302, 297, 303, 286, 284, 293, 283, 299, 305,
  42, 290, 61, 294, -13, 308, -13, -13, -13, 91,
  282, 40, 259, -18, -10, -12, 96, -11, 260, 44,
  93, 40, -10, -2, -11, -3, -3, -2, -12, 59,
  -3, -3, -3, -3, -3, 125, -12, -18, -5, 308,
  309, -5, -11, -11, -11, -11, -11, -11, -11, -11,
  -11, -11, -11, -11, -11, -11, -11, -11, -11, -11,
  -11, -11, -11, -11, -11, -11, -11, -18, -18, 40,
  -5, 41, -11, 44, -11, 41, 41, 125, 59, 267,
  268, 41, 292, 93, 41, -18, 40, -11, 44, 278,
  -12, -3, -3, -11, 41, -18, 44, 41, -17, 59,
  41, 59, -12, 41, -3
};

__YYSCLASS tdiyytabelem tdiyydef[] = {

  -2, -2, -2, 121, 122, 117, 9, 12, 13, 15,
  6, 16, 0, 18, 20, 21, 78, 78, 78, 0,
  17, 0, 77, 30, 29, 69, 58, 59, 0, 8,
  0, 0, 64, 79, 91, -2, 0, 92, 93, 94,
  0, 2, 3, 4, 5, 7, 10, 11, 14, 19,
  22, 23, 24, 25, 26, 27, 28, 31, 87, 78,
  66, 118, 100, 78, 78, 78, 78, 0, 78, 78,
  78, 78, 78, 113, 32, 114, 78, 78, 116, 1,
  -2, 96, 29, 6, 8, 9, 12, 13, 15, 16,
  17, 18, 19, 20, 21, 30, 89, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 60, 29, 61, 62, 63, -2,
  81, -2, 0, 0, -2, 0, 0, 0, 88, 0,
  95, 74, 77, 78, 65, 101, 102, 78, 0, 105,
  107, 108, 109, 111, 112, 115, 68, 0, 97, 29,
  90, 98, 35, 0, 37, 38, 39, 40, 41, 42,
  43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
  53, 54, 55, 56, 57, 33, 34, 0, 0, -2,
  0, 85, 32, 0, 67, 72, 73, 0, 78, 78,
  78, 99, 0, 80, 82, 0, -2, 0, 76, 0,
  0, 106, 110, 36, 83, 0, 75, 86, 0, 78,
  84, 103, 0, 78, 104
};

typedef struct {
  char *t_name;
  int t_val;
} tdiyytoktype;


//#define YYDEBUG
#ifndef YYDEBUG
#define YYDEBUG_STATE
#define YYDEBUG_(msg)
#else
#define YYDEBUG_TOKEN \
  if (tdiyychar == 0)\
    printf("end-of-file\n");\
  else if (tdiyychar < 0)\
    printf("-none-\n");\
  else {\
    int i;\
    for (i = 0; tdiyytoks[i].t_val >= 0; i++)\
      if (tdiyytoks[i].t_val == tdiyychar)\
        break;\
    printf("%s\n", tdiyytoks[i].t_name);\
  }
/*
 ** if debugging, look up token value in list of value vs.
 ** name pairs.  0 and negative (-1) are special values.
 ** Note: linear search is used since time is not a real
 ** consideration while debugging.
 */
#define YYDEBUG_STATE {\
  printf("State %d, token ", tdiyy_state);\
  YYDEBUG_TOKEN;\
}
#define YYDEBUG_(msg) {\
  printf(msg);\
  YYDEBUG_TOKEN;\
}
//"
__YYSCLASS tdiyytoktype tdiyytoks[] = {
 {"ERROR", 257},
 {"IDENT", 258},
 {"POINT", 259},
 {"TEXT", 260},
 {"VALUE", 261},
 {"BREAK", 262},
 {"CASE", 263},
 {"COND", 264},
 {"DEFAULT", 265},
 {"DO", 266},
 {"ELSE", 267},
 {"ELSEW", 268},
 {"FOR", 269},
 {"GOTO", 270},
 {"IF", 271},
 {"LABEL", 272},
 {"RETURN", 273},
 {"SIZEOF", 274},
 {"SWITCH", 275},
 {"USING", 276},
 {"WHERE", 277},
 {"WHILE", 278},
 {"ARG", 279},
 {"CAST", 280},
 {"CONST", 281},
 {"INC", 282},
 {"ADD", 283},
 {"CONCAT", 284},
 {"IAND", 285},
 {"IN", 286},
 {"IOR", 287},
 {"IXOR", 288},
 {"LEQV", 289},
 {"POWER", 290},
 {"PROMO", 291},
 {"RANGE", 292},
 {"SHIFT", 293},
 {"BINEQ", 294},
 {"LAND", 295},
 {"LEQ", 296},
 {"LGE", 297},
 {"LOR", 298},
 {"MUL", 299},
 {"UNARY", 300},
 {"LANDS", 301},
 {"LEQS", 302},
 {"LGES", 303},
 {"LORS", 304},
 {"MULS", 305},
 {"UNARYS", 306},
 {"FUN", 307},
 {"MODIF", 308},
 {"VBL", 309},
 {",", 44},
 {"`", 96},
 {"=", 61},
 {"?", 63},
 {"*", 42},
 {"-unknown-", -1}		/* ends search */
};

__YYSCLASS char *tdiyyreds[] = {
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
#endif				/* YYDEBUG */

#define YYFLAG  (-3000)
/* @(#) $Revision$ */

/*
** Skeleton parser driver for yacc output
*/

#if defined(NLS) && !defined(NL_SETN)
#include <msgbuf.h>
#endif

#ifndef nl_msg
#define nl_msg(i,s) (s)
#endif

/*
** yacc user known macros and defines
*/
#define YYERROR         goto tdiyyerrlab

#ifndef __RUNTIME_YYMAXDEPTH
#define YYACCEPT        {return MDSplusSUCCESS;}
#define YYABORT         {return MDSplusERROR;}
#else
#define YYACCEPT        {free_stacks(); return MDSplusSUCCESS;}
#define YYABORT         {free_stacks(); return MDSplusERROR;}
#endif

#define YYBACKUP( newtoken, newvalue ) {\
  if ( tdiyychar >= 0 || ( tdiyyr2[ tdiyytmp ] >> 1 ) != 1 ) {\
    tdiyyerror( (nl_msg(30001,"syntax error - cannot backup")) );\
    goto tdiyyerrlab;\
  }\
  tdiyychar = newtoken;\
  tdiyystate = *tdiyyps;\
  tdiyylval = newvalue;\
  goto tdiyynewstate;\
}
//"
#define YYRECOVERING()  (!!tdiyyerrflag)

/*
** driver internal defines
*/
/* define for YYFLAG now generated by yacc program. */
/*#define YYFLAG                (FLAGVAL)*/

/*
** global variables used by the parser
*/
#ifndef __RUNTIME_YYMAXDEPTH
__YYSCLASS YYSTYPE tdiyyv[YYMAXDEPTH];	/* value stack */
__YYSCLASS int tdiyys[YYMAXDEPTH];	/* state stack */
#else
__YYSCLASS YYSTYPE *tdiyyv;	/* pointer to malloc'ed value stack */
__YYSCLASS int *tdiyys;		/* pointer to malloc'ed stack stack */

#if defined(__STDC__) || defined (__cplusplus)
#include <stdlib.h>
#else
extern char *malloc();
extern char *realloc();
extern void free();
#endif				/* __STDC__ or __cplusplus */

STATIC_ROUTINE int allocate_stacks();
STATIC_ROUTINE void free_stacks();
#ifndef YYINCREMENT
#define YYINCREMENT (YYMAXDEPTH/2) + 10
#endif
#endif				/* __RUNTIME_YYMAXDEPTH */
long tdiyymaxdepth = YYMAXDEPTH;

__YYSCLASS YYSTYPE *tdiyypv;	/* top of value stack */
__YYSCLASS int *tdiyyps;		/* top of state stack */

__YYSCLASS int tdiyystate;		/* current state */
__YYSCLASS int tdiyytmp;		/* extra var (lasts between blocks) */

int tdiyynerrs;			/* number of errors */
__YYSCLASS int tdiyyerrflag;	/* error recovery flag */
int tdiyychar;			/* current input token number */

/*
** tdiyyparse - return 0 if worked, 1 if syntax error not recovered from
*/
// TdiYacc aka tdiyyparse         TdiYacc

int TdiYacc(){
  GET_TDITHREADSTATIC_P;
  YYSTYPE *tdiyypvt;	/* top of value stack for $vars */

  /*
   ** Initialize externals - tdiyyparse may be called more than once
   */
#ifdef __RUNTIME_YYMAXDEPTH
  if (allocate_stacks())
    YYABORT;
#endif
  tdiyyval.mark.w_ok=0;
  //  tdiyypv = &tdiyyv[-1];
  tdiyypv = tdiyyv - 1;
  //  tdiyyps = &tdiyys[-1];
  tdiyyps = tdiyys - 1;
  tdiyystate = 0;
  tdiyytmp = 0;
  tdiyynerrs = 0;
  tdiyyerrflag = 0;
  tdiyychar = -1;

  {
    YYSTYPE *tdiyy_pv;	/* top of value stack */
    int *tdiyy_ps;	/* top of state stack */
    int tdiyy_state;	/* current state */
    int tdiyy_n;	/* internal state number info */

    /*
     ** get globals into registers.
     ** either we just started, or we just finished a reduction
     */
 tdiyy_stack_reset:
    tdiyy_pv = tdiyypv;
    tdiyy_ps = tdiyyps;
    tdiyy_state = tdiyystate;

    /*
     ** top of for (;;) loop while no reductions done
     */
 tdiyy_stack:
    /*
     ** put a state and value onto the stacks
     */
    YYDEBUG_STATE
    if (++tdiyy_ps >= &tdiyys[tdiyymaxdepth]) {	/* room on stack? */
#ifndef __RUNTIME_YYMAXDEPTH
      tdiyyerror((nl_msg(30002, "yacc stack overflow")));
      YYABORT;
#else
      /* save old stack bases to recalculate pointers */
      YYSTYPE *tdiyyv_old = tdiyyv;
      int *tdiyys_old = tdiyys;
      tdiyymaxdepth += YYINCREMENT;
      tdiyys = (int *)realloc(tdiyys, tdiyymaxdepth * sizeof(int));
      tdiyyv = (YYSTYPE *) realloc(tdiyyv, tdiyymaxdepth * sizeof(YYSTYPE));
      if (tdiyys == 0 || tdiyyv == 0) {
	tdiyyerror((nl_msg(30002, "yacc stack overflow")));
	YYABORT;
      }
      /* Reset pointers into stack */
      tdiyy_ps = (tdiyy_ps - tdiyys_old) + tdiyys;
      tdiyyps = (tdiyyps - tdiyys_old) + tdiyys;
      tdiyy_pv = (tdiyy_pv - tdiyyv_old) + tdiyyv;
      tdiyypv = (tdiyypv - tdiyyv_old) + tdiyyv;
#endif

    }
    *tdiyy_ps = tdiyy_state;
    *++tdiyy_pv = tdiyyval;

    /*
     ** we have a new state - find out what to do
     */
 tdiyy_newstate:
    if ((tdiyy_n = tdiyypact[tdiyy_state]) <= YYFLAG)
      goto tdiyy_default;		/* simple state */
    if ((tdiyychar < 0) && ((tdiyychar = TdiLex()) < 0))
      tdiyychar = 0;		/* reached EOF */
    YYDEBUG_("Received token ")
    if (((tdiyy_n += tdiyychar) < 0) || (tdiyy_n >= YYLAST))
      goto tdiyy_default;
    if (tdiyychk[tdiyy_n = tdiyyact[tdiyy_n]] == tdiyychar) {	/*valid shift */
      tdiyychar = -1;
      tdiyyval = tdiyylval;
      tdiyy_state = tdiyy_n;
      if (tdiyyerrflag > 0)
	tdiyyerrflag--;
      goto tdiyy_stack;
    }

 tdiyy_default:
    if ((tdiyy_n = tdiyydef[tdiyy_state]) == -2) {
      if ((tdiyychar < 0) && ((tdiyychar = TdiLex()) < 0))
	tdiyychar = 0;		/* reached EOF */
      YYDEBUG_("received token ")
      /*
       ** look through exception table
       */
      {
	int *tdiyyxi;
	for (tdiyyxi=tdiyyexca ; tdiyyxi[0]!=-1 || tdiyyxi[1]!=tdiyy_state ; tdiyyxi+=2);
	while ((*(tdiyyxi += 2) >= 0) && (*tdiyyxi != tdiyychar)) ;
	if ((tdiyy_n = tdiyyxi[1]) < 0)
	  YYACCEPT;
      }
    }

    /*
     ** check for syntax error
     */
    if (tdiyy_n == 0) {		/* have an error */
      /* no worry about speed here! */
      switch (tdiyyerrflag) {
      case 0:			/* new error */
	tdiyyerror((nl_msg(30003, "syntax error")));
	// tdiyyerrlab:
	/*
	 ** get globals into registers.
	 ** we have a user generated syntax type error
	tdiyy_pv = tdiyypv;
	tdiyy_ps = tdiyyps;
	tdiyy_state = tdiyystate;
	 */
	tdiyynerrs++;
	MDS_ATTR_FALLTHROUGH
      case 1:
      case 2:			/* incompletely recovered error */
	/* try again... */
	tdiyyerrflag = 3;
	/*
	 ** find state where "error" is a legal
	 ** shift action
	 */
	while (tdiyy_ps >= tdiyys) {
	  tdiyy_n = tdiyypact[*tdiyy_ps] + YYERRCODE;
	  if (tdiyy_n >= 0 && tdiyy_n < YYLAST && tdiyychk[tdiyyact[tdiyy_n]] == YYERRCODE) {
	    /*
	     ** simulate shift of "error"
	     */
	    tdiyy_state = tdiyyact[tdiyy_n];
	    goto tdiyy_stack;
	  }
	  /*
	   ** current state has no shift on
	   ** "error", pop stack
	   */
#ifdef YYDEBUG
	  printf("Error recovery pops state %d, uncovers state %d\n", *tdiyy_ps, tdiyy_ps[-1]);
#endif
	  tdiyy_ps--;
	  tdiyy_pv--;
	}
	/*
	 ** there is no state on stack with "error" as
	 ** a valid shift.  give up.
	 */
	YYABORT;
      case 3:			/* no shift yet; eat a token */
	YYDEBUG_("Error recovery discards token ")
	if (tdiyychar == 0)	/* reached EOF. quit */
	  YYABORT;
	tdiyychar = -1;
	goto tdiyy_newstate;
      }
    }
    /* end if ( tdiyy_n == 0 ) */
    /*
     ** reduction by production tdiyy_n
     ** put stack tops, etc. so things right after switch
     */
#ifdef YYDEBUG
    /*
     ** if debugging, print the string that is the user's
     ** specification of the reduction which is just about
     ** to be done.
     */
    printf("Reduce by (%d) \"%s\"\n", tdiyy_n, tdiyyreds[tdiyy_n]);
#endif
    tdiyytmp = tdiyy_n;		/* value to switch over */
    tdiyypvt = tdiyy_pv;	/* $vars top of value stack */
    /*
     ** Look in goto table for next state
     ** Sorry about using tdiyy_state here as temporary
     ** register variable, but why not, if it works...
     ** If tdiyyr2[ tdiyy_n ] doesn't have the low order bit
     ** set, then there is no action to be done for
     ** this reduction.  So, no saving & unsaving of
     ** registers done.  The only difference between the
     ** code just after the if and the body of the if is
     ** the goto tdiyy_stack in the body.  This way the test
     ** can be made before the choice of what to do is needed.
     */
    {
      /* length of production doubled with extra bit */
      int tdiyy_len = tdiyyr2[tdiyy_n];
      int back_to_stack = !(tdiyy_len & 1);
      tdiyy_len >>= 1;
      tdiyyval = (tdiyy_pv -= tdiyy_len)[1];	/* $$ = $1 */
      tdiyy_state = tdiyypgo[tdiyy_n = tdiyyr1[tdiyy_n]] + *(tdiyy_ps -= tdiyy_len) + 1;
      if (tdiyy_state >= YYLAST || tdiyychk[tdiyy_state = tdiyyact[tdiyy_state]] != -tdiyy_n)
	tdiyy_state = tdiyyact[tdiyypgo[tdiyy_n]];
      if (back_to_stack)
	goto tdiyy_stack;
    }
    /* save until reenter driver code */
    tdiyystate = tdiyy_state;
    tdiyyps = tdiyy_ps;
    tdiyypv = tdiyy_pv;
  }
  /*
   ** code supplied by user is placed in this switch
   */
  switch (tdiyytmp) {

  case 1:
//#line 184 "TdiYacc.y"
    {
      tdiyyval.mark = tdiyypvt[-0].mark;
    }
    break;
  case 32:
//#line 199 "TdiYacc.y"
    {
      tdiyyval.mark.rptr = tdiyypvt[-0].mark.rptr;
      tdiyyval.mark.builtin = -2;
      TdiRefZone.l_status = TdiYacc_IMMEDIATE(&tdiyyval.mark.rptr);
      if (!(TdiRefZone.l_status & 1)) {
	tdiyyerror(0);
      }
    }
    break;
  case 33:
//#line 203 "TdiYacc.y"
    {
      _JUST2(OpcEquals, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 34:
//#line 204 "TdiYacc.y"
    {
      struct marker tmp;	/*binary operation and assign */
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tmp);
      _JUST1(OpcEqualsFirst, tmp, tdiyyval.mark);
    } break;
  case 35:
//#line 207 "TdiYacc.y"
    {
      if (tdiyypvt[-0].mark.rptr && tdiyypvt[-0].mark.rptr->dtype == DTYPE_RANGE)
	if (tdiyypvt[-0].mark.rptr->ndesc == 2) {
	  tdiyyval.mark = tdiyypvt[-0].mark;
	  tdiyyval.mark.rptr->dscptrs[2] = tdiyyval.mark.rptr->dscptrs[1];
	  tdiyyval.mark.rptr->dscptrs[1] = tdiyyval.mark.rptr->dscptrs[0];
	  tdiyyval.mark.rptr->dscptrs[0] = (struct descriptor *)tdiyypvt[-2].mark.rptr;
	  ++tdiyyval.mark.rptr->ndesc;
	} else {
	  TdiRefZone.l_status = TdiEXTRA_ARG;
	  return MDSplusERROR;
      } else {
	unsigned int vmlen = sizeof(struct descriptor_range);
	LibGetVm(&vmlen, (void **)&tdiyyval.mark.rptr, &TdiRefZone.l_zone);
	tdiyyval.mark.rptr->length = 0;
	tdiyyval.mark.rptr->dtype = DTYPE_RANGE;
	tdiyyval.mark.rptr->class = CLASS_R;
	tdiyyval.mark.rptr->pointer = 0;
	tdiyyval.mark.rptr->ndesc = 2;
	tdiyyval.mark.rptr->dscptrs[0] = (struct descriptor *)tdiyypvt[-2].mark.rptr;
	tdiyyval.mark.rptr->dscptrs[1] = (struct descriptor *)tdiyypvt[-0].mark.rptr;
	_RESOLVE(tdiyyval.mark);
      }
    } break;
  case 36:
//#line 226 "TdiYacc.y"
    {
      _JUST3(OpcConditional, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyypvt[-4].mark, tdiyyval.mark);
    }
    break;
  case 37:
//#line 227 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 38:
//#line 228 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 39:
//#line 229 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 40:
//#line 230 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 41:
//#line 231 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 42:
//#line 232 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 43:
//#line 233 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 44:
//#line 234 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 45:
//#line 235 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 46:
//#line 236 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 47:
//#line 237 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 48:
//#line 238 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 49:
//#line 239 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 50:
//#line 240 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 51:
//#line 241 "TdiYacc.y"
    {
      if (tdiyyval.mark.rptr == 0)
	tdiyyval.mark = tdiyypvt[-0].mark;
      else if (tdiyyval.mark.rptr->dtype == DTYPE_FUNCTION
	       && *(unsigned short *)tdiyyval.mark.rptr->pointer == OpcConcat
	       && tdiyyval.mark.rptr->ndesc < 250) {
	tdiyyval.mark.rptr->dscptrs[tdiyyval.mark.rptr->ndesc++] =
	    (struct descriptor *)tdiyypvt[-0].mark.rptr;
	_RESOLVE(tdiyyval.mark);
      } else {
	_FULL2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
      }
    }
    break;
  case 52:
//#line 249 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 53:
//#line 250 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 54:
//#line 251 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 55:
//#line 252 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 56:
//#line 253 "TdiYacc.y"
    {
      _JUST2(OpcMultiply, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 57:
//#line 254 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-1].mark.builtin, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 59:
//#line 256 "TdiYacc.y"
    {
      tdiyyval.mark = _EMPTY_MARKER;
    }
    break;
  case 60:
//#line 258 "TdiYacc.y"
    {
      int j;
      if (tdiyypvt[-1].mark.builtin == OpcAdd)
	j = OpcUnaryPlus;
      else
	j = OpcUnaryMinus;
      _JUST1(j, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 61:
//#line 262 "TdiYacc.y"
    {
      _JUST1(tdiyypvt[-1].mark.builtin, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 62:
//#line 263 "TdiYacc.y"
    {
      _JUST1(tdiyypvt[-1].mark.builtin, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 63:
//#line 264 "TdiYacc.y"
    {
      _JUST1(tdiyypvt[-1].mark.builtin, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 65:
//#line 272 "TdiYacc.y"
    {
      _FULL1(OpcVector, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 66:
//#line 273 "TdiYacc.y"
    {
      _JUST0(OpcVector, tdiyyval.mark);
    }
    break;
  case 67:
//#line 274 "TdiYacc.y"
    {
      if (tdiyyval.mark.rptr->ndesc >= 250) {
	_RESOLVE(tdiyypvt[-2].mark);
	_FULL1(OpcVector, tdiyypvt[-2].mark, tdiyyval.mark);
      }
      tdiyyval.mark.rptr->dscptrs[tdiyyval.mark.rptr->ndesc++] = (struct descriptor *)tdiyypvt[-0].mark.rptr;
    } break;
  case 68:
//#line 281 "TdiYacc.y"
    {
      if (tdiyyval.mark.rptr	/*comma is left-to-right weakest */
	  && tdiyyval.mark.builtin != -2
	  && tdiyyval.mark.rptr->dtype == DTYPE_FUNCTION
	  && *(unsigned short *)tdiyyval.mark.rptr->pointer == OpcComma
	  && tdiyyval.mark.rptr->ndesc < 250)
	tdiyyval.mark.rptr->dscptrs[tdiyyval.mark.rptr->ndesc++] =
	    (struct descriptor *)tdiyypvt[-0].mark.rptr;
      else
	_FULL2(OpcComma, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);	/*first comma */
    }
    break;
  case 70:
//#line 291 "TdiYacc.y"
    {
      if (tdiyyval.mark.rptr
	  && tdiyyval.mark.builtin != -2
	  && tdiyyval.mark.rptr->dtype == DTYPE_FUNCTION
	  && *(unsigned short *)tdiyyval.mark.rptr->pointer == OpcComma) ;
      else
	_JUST1(OpcAbort, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 71:
//#line 296 "TdiYacc.y"
    {
      _JUST0(OpcAbort, tdiyyval.mark);
    }
    break;
  case 72:
//#line 298 "TdiYacc.y"
    {
      tdiyyval.mark = tdiyypvt[-1].mark;
      tdiyyval.mark.builtin = -2;
    }
    break;
  case 73:
//#line 299 "TdiYacc.y"
    {
      tdiyyval.mark = tdiyypvt[-1].mark;
      tdiyyval.mark.builtin = -2;
    }
    break;
  case 74:
//#line 301 "TdiYacc.y"
    {
      ++TdiRefZone.l_rel_path;
    }
    break;
  case 75:
//#line 303 "TdiYacc.y"
    {
      _FULL2(OpcAbort, tdiyypvt[-3].mark, tdiyypvt[-1].mark, tdiyyval.mark);
      --TdiRefZone.l_rel_path;
    }
    break;
  case 76:
//#line 304 "TdiYacc.y"
    {
      _FULL2(OpcAbort, tdiyypvt[-2].mark, _EMPTY_MARKER, tdiyyval.mark);
      --TdiRefZone.l_rel_path;
    }
    break;
  case 78:
//#line 307 "TdiYacc.y"
    {
      tdiyyval.mark = _EMPTY_MARKER;
    }
    break;
  case 80:
//#line 313 "TdiYacc.y"
    {
      int j;
      tdiyyval.mark = tdiyypvt[-1].mark;
      tdiyyval.mark.rptr->pointer = (unsigned char *)&OpcSubscript;
      for (j = tdiyyval.mark.rptr->ndesc; --j >= 0;)
	tdiyyval.mark.rptr->dscptrs[j + 1] = tdiyyval.mark.rptr->dscptrs[j];
      tdiyyval.mark.rptr->dscptrs[0] = (struct descriptor *)tdiyypvt[-3].mark.rptr;
      tdiyyval.mark.rptr->ndesc++;
      _RESOLVE(tdiyyval.mark);
    } break;
  case 81:
//#line 322 "TdiYacc.y"
    {
      int j = tdiyypvt[-0].mark.builtin == OpcPreInc ? OpcPostInc : OpcPostDec;
      _JUST1(j, tdiyypvt[-1].mark, tdiyyval.mark);
    } break;
  case 82:
//#line 324 "TdiYacc.y"
    {
      tdiyyval.mark = tdiyypvt[-1].mark;
      if (tdiyypvt[-3].mark.builtin < 0) {
	int j;			/*unknown today */
	tdiyyval.mark.rptr->pointer = (unsigned char *)&OpcExtFunction;
	for (j = tdiyyval.mark.rptr->ndesc; --j >= 0;)
	  tdiyyval.mark.rptr->dscptrs[j + 2] = tdiyyval.mark.rptr->dscptrs[j];
	tdiyyval.mark.rptr->dscptrs[0] = 0;
	tdiyyval.mark.rptr->dscptrs[1] = (struct descriptor *)tdiyypvt[-3].mark.rptr;
	tdiyyval.mark.rptr->ndesc += 2;
      } else {			/*intrinsic */
	*(unsigned short *)tdiyyval.mark.rptr->pointer = tdiyypvt[-3].mark.builtin;
	_RESOLVE(tdiyyval.mark);
      }
    } break;
  case 83:
//#line 337 "TdiYacc.y"
    {
      int j;
      tdiyyval.mark = tdiyypvt[-1].mark;	/*external */
      tdiyyval.mark.rptr->dtype = DTYPE_CALL;
      tdiyyval.mark.rptr->length = 0;
      tdiyyval.mark.rptr->pointer = 0;
      for (j = tdiyyval.mark.rptr->ndesc; --j >= 0;)
	tdiyyval.mark.rptr->dscptrs[j + 2] = tdiyyval.mark.rptr->dscptrs[j];
      tdiyyval.mark.rptr->dscptrs[0] = (struct descriptor *)tdiyypvt[-4].mark.rptr;
      tdiyyval.mark.rptr->dscptrs[1] = (struct descriptor *)tdiyypvt[-3].mark.rptr;
      tdiyyval.mark.rptr->ndesc += 2;
    } break;
  case 84:
//#line 348 "TdiYacc.y"
    {
      int j;
      tdiyyval.mark = tdiyypvt[-1].mark;	/*typed external */
      StrUpcase((struct descriptor *)tdiyypvt[-3].mark.rptr, (struct descriptor *)tdiyypvt[-3].mark.rptr);
      for (j = TdiCAT_MAX; --j >= 0;)
	if (strncmp
	    (TdiREF_CAT[j].name, (char *)tdiyypvt[-3].mark.rptr->pointer,
	     tdiyypvt[-3].mark.rptr->length) == 0
	    && strlen(TdiREF_CAT[j].name) == tdiyypvt[-3].mark.rptr->length)
	  break;
      if (j < 0) {
	TdiRefZone.l_status = TdiINVDTYDSC;
	return MDSplusERROR;
      }
      tdiyyval.mark.rptr->dtype = DTYPE_CALL;
      tdiyyval.mark.rptr->length = 1;
      *(unsigned char *)tdiyyval.mark.rptr->pointer = (unsigned char)j;
      for (j = tdiyyval.mark.rptr->ndesc; --j >= 0;)
	tdiyyval.mark.rptr->dscptrs[j + 2] = tdiyyval.mark.rptr->dscptrs[j];
      tdiyyval.mark.rptr->dscptrs[0] = (struct descriptor *)tdiyypvt[-5].mark.rptr;
      tdiyyval.mark.rptr->dscptrs[1] = (struct descriptor *)tdiyypvt[-4].mark.rptr;
      tdiyyval.mark.rptr->ndesc += 2;
    } break;
  case 85:
//#line 364 "TdiYacc.y"
    {
      int j;			/*USING(expr,[default],[shotid],[expt]) */
      tdiyyval.mark.rptr->pointer = (unsigned char *)&OpcUsing;
      for (j = 0; j < tdiyypvt[-1].mark.rptr->ndesc; ++j)
	tdiyyval.mark.rptr->dscptrs[tdiyyval.mark.rptr->ndesc++] = tdiyypvt[-1].mark.rptr->dscptrs[j];
    }
    break;
  case 86:
//#line 369 "TdiYacc.y"
    {
      _JUST2(OpcUsing, tdiyypvt[-3].mark, tdiyypvt[-1].mark, tdiyyval.mark);
      --TdiRefZone.l_rel_path;
    }
    break;
  case 88:
//#line 375 "TdiYacc.y"
    {
      MAKE_S(DTYPE_T, tdiyypvt[-1].mark.rptr->length + tdiyypvt[-0].mark.rptr->length, tdiyyval.mark.rptr);
      StrConcat((struct descriptor *)tdiyyval.mark.rptr,
		(struct descriptor *)tdiyypvt[-1].mark.rptr, tdiyypvt[-0].mark.rptr MDS_END_ARG);
    } break;
  case 89:
//#line 384 "TdiYacc.y"
    {
      _JUST1(tdiyypvt[-1].mark.builtin, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 90:
//#line 385 "TdiYacc.y"
    {
      struct marker tmp;	/*OPTIONAL IN/INOUT/OUT */
      _JUST1(tdiyypvt[-1].mark.builtin, tdiyypvt[-0].mark, tmp);
      _JUST1(tdiyypvt[-2].mark.builtin, tmp, tdiyyval.mark);
    } break;
  case 91:
//#line 388 "TdiYacc.y"
    {
      if (*tdiyyval.mark.rptr->pointer == '$') {
	if (tdiyyval.mark.builtin < 0) {
	  tdiyyval.mark.rptr->dtype = DTYPE_IDENT;
	} else {
	  if ((TdiRefFunction[tdiyyval.mark.builtin].token & LEX_M_TOKEN) == (unsigned int)LEX_ARG) {
	    if (!((TdiRefZone.l_status = TdiYacc_ARG(&tdiyyval.mark)) & 1)) {
	      tdiyyerror(0);
	    }
	  } else {
	    if ((TdiRefFunction[tdiyyval.mark.builtin].token & LEX_M_TOKEN) == (unsigned int)LEX_CONST)
	      _JUST0(tdiyypvt[-0].mark.builtin, tdiyyval.mark);
	  }
	}
      } else if (*tdiyyval.mark.rptr->pointer == '_')
	tdiyyval.mark.rptr->dtype = DTYPE_IDENT;
      else if (TdiLexPath
	       (tdiyypvt[-0].mark.rptr->length, tdiyypvt[-0].mark.rptr->pointer, &tdiyyval.mark) == ERROR) {
	tdiyyerror(0);
      }
    }
    break;
  case 95:
//#line 403 "TdiYacc.y"
    {
      _RESOLVE(tdiyyval.mark);
    }
    break;
  case 96:
//#line 410 "TdiYacc.y"
    {
      tdiyyval.mark = tdiyypvt[-0].mark;
    }
    break;
  case 97:
//#line 411 "TdiYacc.y"
    {
      _JUST1(tdiyypvt[-1].mark.builtin, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 98:
//#line 412 "TdiYacc.y"
    {
      _JUST1(tdiyypvt[-2].mark.builtin, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 99:
//#line 414 "TdiYacc.y"
    {
      int j;
      tdiyyval.mark = tdiyypvt[-1].mark;
      tdiyyval.mark.rptr->pointer = (unsigned char *)&OpcFun;
      for (j = tdiyyval.mark.rptr->ndesc; --j >= 0;)
	tdiyyval.mark.rptr->dscptrs[j + 2] = tdiyyval.mark.rptr->dscptrs[j];
      tdiyyval.mark.rptr->dscptrs[0] = (struct descriptor *)tdiyypvt[-3].mark.rptr;
      tdiyyval.mark.rptr->ndesc += 2;
      /*++TdiRefZone.l_rel_path; */
    } break;
  case 100:
//#line 423 "TdiYacc.y"
    {
      _JUST0(tdiyypvt[-1].mark.builtin, tdiyyval.mark);
    }
    break;
  case 101:
//#line 424 "TdiYacc.y"
    {
      _FULL2(tdiyypvt[-2].mark.builtin, tdiyypvt[-1].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 102:
//#line 425 "TdiYacc.y"
    {
      _FULL1(tdiyypvt[-1].mark.builtin, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 103:
//#line 426 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-6].mark.builtin, tdiyypvt[-1].mark, tdiyypvt[-4].mark, tdiyyval.mark);
    }
    break;
  case 104:
//#line 427 "TdiYacc.y"
    {
      _JUST4(tdiyypvt[-8].mark.builtin, tdiyypvt[-6].mark, tdiyypvt[-4].mark,
	     tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 105:
//#line 428 "TdiYacc.y"
    {
      _JUST1(tdiyypvt[-2].mark.builtin, tdiyypvt[-1].mark, tdiyyval.mark);
    }
    break;
  case 106:
//#line 429 "TdiYacc.y"
    {
      _JUST3(tdiyypvt[-4].mark.builtin, tdiyypvt[-3].mark, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 107:
//#line 430 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-2].mark.builtin, tdiyypvt[-1].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 108:
//#line 431 "TdiYacc.y"
    {
      _FULL2(OpcLabel, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 109:
//#line 434 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-2].mark.builtin, tdiyypvt[-1].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 110:
//#line 435 "TdiYacc.y"
    {
      _JUST3(tdiyypvt[-4].mark.builtin, tdiyypvt[-3].mark, tdiyypvt[-2].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 111:
//#line 436 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-2].mark.builtin, tdiyypvt[-1].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 112:
//#line 437 "TdiYacc.y"
    {
      _JUST2(tdiyypvt[-2].mark.builtin, tdiyypvt[-1].mark, tdiyypvt[-0].mark, tdiyyval.mark);
    }
    break;
  case 113:
//#line 438 "TdiYacc.y"
    {				/*TdiRefZone.l_rel_path--; */
      tdiyyval.mark.rptr->dscptrs[1] = (struct descriptor *)tdiyypvt[-0].mark.rptr;
    } break;
  case 114:
//#line 440 "TdiYacc.y"
    {
      tdiyyval.mark.rptr = tdiyypvt[-0].mark.rptr;
      tdiyyval.mark.builtin = -2;
      TdiRefZone.l_status = TdiYacc_IMMEDIATE(&tdiyyval.mark.rptr);
      if (!(TdiRefZone.l_status & 1)) {
	tdiyyerror(0);
      }
    }
    break;
  case 115:
//#line 444 "TdiYacc.y"
    {
      tdiyyval.mark = tdiyypvt[-1].mark;
      _RESOLVE(tdiyyval.mark);
    }
    break;
  case 118:
//#line 448 "TdiYacc.y"
    {
      short opcode;
      if (tdiyyval.mark.rptr == 0) {
	tdiyyval.mark = tdiyypvt[-0].mark;
      } /* initial null statement   */
      else if (tdiyypvt[-0].mark.rptr == 0) {
      } /* trailing null statement  */
      else if (tdiyyval.mark.rptr->dtype == DTYPE_FUNCTION
	       && tdiyyval.mark.rptr->ndesc < 250
	       && ((opcode = *(unsigned short *)tdiyyval.mark.rptr->pointer)
		   == OpcStatement || opcode == OpcCase
		   || opcode == OpcDefault || opcode == OpcLabel)) {
	tdiyyval.mark.rptr->dscptrs[tdiyyval.mark.rptr->ndesc++] =
	    (struct descriptor *)tdiyypvt[-0].mark.rptr;
      } else {
	_FULL2(OpcStatement, tdiyypvt[-1].mark, tdiyypvt[-0].mark, tdiyyval.mark);
      }
    }
    break;
  case 119:
//#line 461 "TdiYacc.y"
    {
      _RESOLVE(tdiyyval.mark);	/*statements */
      TdiRefZone.a_result = (struct descriptor_d *)tdiyyval.mark.rptr;
      TdiRefZone.l_status = 1;
    } break;
  case 120:
//#line 464 "TdiYacc.y"
    {
      tdiyyval.mark = _EMPTY_MARKER;
    }
    break;
  case 121:
//#line 465 "TdiYacc.y"
    {
    }
    break;
  case 122:
//#line 466 "TdiYacc.y"
    {
      TdiRefZone.l_status = TdiSYNTAX;
    }
    break;
  }
  goto tdiyy_stack_reset;			/* reset registers in driver code */
}

#ifdef __RUNTIME_YYMAXDEPTH

STATIC_ROUTINE int allocate_stacks()
{
  GET_THREADSTATIC_P;
  /* allocate the tdiyys and tdiyyv stacks */
  tdiyys = (int *)malloc(tdiyymaxdepth * sizeof(int));
  tdiyyv = (YYSTYPE *) malloc(tdiyymaxdepth * sizeof(YYSTYPE));

  if (tdiyys == 0 || tdiyyv == 0) {
    tdiyyerror((nl_msg(30004, "unable to allocate space for yacc stacks")));
    return MDSplusERROR;
  } else
    return MDSplusSUCCESS;

}

STATIC_ROUTINE void free_stacks()
{
  if (tdiyys != 0)
    free((char *)tdiyys);
  if (tdiyyv != 0)
    free((char *)tdiyyv);
}

#endif				/* defined(__RUNTIME_YYMAXDEPTH) */
