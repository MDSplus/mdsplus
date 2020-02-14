/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 14 "yylex/TdiYacc.y" /* yacc.c:339  */

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

#ifndef _MACOSX
extern char *stpcpy(char *dest, const char *src);
#endif

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

#line 189 "TdiYacc.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "tdiyacc.h".  */
#ifndef YY_YY_TDIYACC_H_INCLUDED
# define YY_YY_TDIYACC_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 7 "yylex/TdiYacc.y" /* yacc.c:355  */

#define YYLTYPE TDITHREADSTATIC_TYPE
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

#line 227 "TdiYacc.c" /* yacc.c:355  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LEX_END = 258,
    LEX_ELSE = 259,
    LEX_ELSEW = 260,
    LEX_ERROR = 261,
    LEX_IDENT = 262,
    LEX_POINT = 263,
    LEX_TEXT = 264,
    LEX_VALUE = 265,
    LEX_BREAK = 266,
    LEX_CASE = 267,
    LEX_COND = 268,
    LEX_DEFAULT = 269,
    LEX_DO = 270,
    LEX_FOR = 271,
    LEX_GOTO = 272,
    LEX_IF = 273,
    LEX_LABEL = 274,
    LEX_RETURN = 275,
    LEX_SWITCH = 276,
    LEX_USING = 277,
    LEX_WHERE = 278,
    LEX_WHILE = 279,
    LEX_CAST = 280,
    LEX_CONST = 281,
    LEX_INC = 282,
    LEX_ARG = 283,
    LEX_SIZEOF = 284,
    LEX_ADD = 285,
    LEX_CONCAT = 286,
    LEX_IAND = 287,
    LEX_IN = 288,
    LEX_IOR = 289,
    LEX_IXOR = 290,
    LEX_POWER = 291,
    LEX_PROMO = 292,
    LEX_RANGE = 293,
    LEX_SHIFT = 294,
    LEX_BINEQ = 295,
    LEX_LAND = 296,
    LEX_LEQ = 297,
    LEX_LGE = 298,
    LEX_LOR = 299,
    LEX_MUL = 300,
    LEX_UNARY = 301,
    LEX_LEQV = 302,
    LEX_LANDS = 303,
    LEX_LEQS = 304,
    LEX_LGES = 305,
    LEX_LORS = 306,
    LEX_MULS = 307,
    LEX_UNARYS = 308,
    LEX_LEQVS = 309,
    LEX_FUN = 310,
    LEX_MODIF = 311,
    LEX_VBL = 312,
    LEX_AMODIF = 313
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 140 "yylex/TdiYacc.y" /* yacc.c:355  */
struct marker mark;

#line 301 "TdiYacc.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (TDITHREADSTATIC_TYPE *TDITHREADSTATIC_VAR);

#endif /* !YY_YY_TDIYACC_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 317 "TdiYacc.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  158
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1396

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  71
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  24
/* YYNRULES -- Number of rules.  */
#define YYNRULES  138
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  255

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   313

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      65,    66,    63,     2,    59,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    68,
       2,    61,     2,    62,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    64,     2,    67,     2,     2,    60,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    69,     2,    70,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   218,   218,   225,   225,   225,   225,   225,   226,   226,
     226,   226,   226,   227,   227,   227,   227,   227,   228,   230,
     231,   231,   232,   232,   233,   233,   233,   233,   233,   234,
     234,   234,   234,   234,   236,   237,   237,   237,   237,   238,
     238,   238,   238,   246,   247,   248,   249,   250,   252,   254,
     255,   256,   257,   260,   261,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     302,   303,   304,   305,   306,   307,   308,   315,   316,   317,
     324,   325,   327,   334,   335,   337,   342,   344,   345,   347,
     349,   350,   355,   356,   365,   367,   380,   391,   408,   413,
     418,   419,   428,   428,   429,   430,   433,   444,   445,   446,
     447,   454,   455,   456,   458,   467,   469,   470,   471,   472,
     473,   474,   475,   476,   477,   478,   479,   480,   481,   482,
     484,   486,   487,   488,   489,   502,   505,   506,   507
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "END", "ELSE", "ELSEW", "ERROR", "IDENT",
  "POINT", "TEXT", "VALUE", "BREAK", "CASE", "COND", "DEFAULT", "DO",
  "FOR", "GOTO", "IF", "LABEL", "RETURN", "SWITCH", "USING", "WHERE",
  "WHILE", "CAST", "CONST", "INC", "ARG", "SIZEOF", "ADD", "CONCAT",
  "IAND", "IN", "IOR", "IXOR", "POWER", "PROMO", "RANGE", "SHIFT", "BINEQ",
  "LAND", "LEQ", "LGE", "LOR", "MUL", "UNARY", "LEQV", "LANDS", "LEQS",
  "LGES", "LORS", "MULS", "UNARYS", "LEQVS", "FUN", "MODIF", "VBL",
  "AMODIF", "','", "'`'", "'='", "'?'", "'*'", "'['", "'('", "')'", "']'",
  "';'", "'{'", "'}'", "$accept", "slabel", "ulabel", "flabel", "label",
  "unaryX", "ass", "bracket", "opt", "exp", "sub", "paren", "using0",
  "using", "postX", "textX", "modif", "primaX", "funvbl", "fun", "stmt0",
  "stmt", "stmt_lst", "program", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,    44,
      96,    61,    63,    42,    91,    40,    41,    93,    59,   123,
     125
};
# endif

#define YYPACT_NINF -53

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-53)))

#define YYTABLE_NINF -138

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     227,   -53,   -53,   -53,    18,   -53,   -53,   -53,   -37,    -4,
     -53,   -53,   -19,   -38,   -34,   -21,    -1,   -53,   -21,    -2,
     -21,   -21,   -53,   -53,   876,   -53,   -53,   876,   -53,   -53,
     -53,   -53,   -53,   876,   -53,   -53,   -53,   -53,   -53,   -53,
     876,   -53,  1039,   -25,   -53,    -5,   559,   -53,   814,   559,
     559,    20,    -6,     0,    61,   -20,   978,   -52,   -14,   -53,
     -53,   814,   689,   -22,    62,    13,   -53,    34,   559,   -53,
     -53,   293,   101,   -53,   559,   559,   559,   814,    35,   559,
     -53,   982,   -53,   559,   559,   -53,   -53,   -53,   -53,   -53,
     -53,   -53,   -53,   -53,   -53,   -53,    49,   -53,   -53,   -53,
     -53,  1056,   -53,   -53,  1056,    50,   -53,  1156,   -53,   814,
    1156,    43,   493,   359,   559,   689,   931,   814,   814,   -53,
     814,   814,   814,   814,   814,   814,   814,   814,   814,   814,
     814,   814,   814,   814,   814,   814,   814,   814,   814,   814,
     814,   814,   814,   814,   -53,   814,   -53,  1122,    51,    53,
      47,   -53,   625,   -53,   -53,   689,   -53,   -53,   -53,   -53,
     -53,   426,   -11,   -53,   112,   -37,    -4,   -19,   -38,   -34,
     -21,    -1,   -21,   -21,   -21,  1039,    63,   982,   -53,   121,
     -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,    64,   -53,
     -53,   -53,   689,   -53,   -53,    78,  1156,  1156,   -28,    97,
     160,  1333,  1293,  1316,    93,  1224,  1156,   152,    89,   123,
    1047,  1270,    93,  1247,    89,   123,  1047,  1270,    93,  1190,
      93,  1156,   -53,   752,   -53,    77,    80,   126,   814,   559,
     559,   -53,    81,   689,   814,   -53,  1085,   -53,   -53,   -21,
      -8,   -53,   -53,   -53,    92,  1156,   -53,   -53,    87,   814,
     -53,   -53,   -40,   559,   -53
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,   138,     9,    10,    49,     3,   100,   107,    20,    37,
      15,    17,     8,    39,     6,    40,    11,    12,    42,    38,
      35,    36,     4,     5,     0,    16,     7,     0,    24,    25,
      26,    27,    22,    41,    28,    29,    30,    31,    32,    23,
       0,    33,    13,    21,    14,    18,     0,    76,    78,    81,
      81,     0,   106,    34,     0,    50,    83,     0,     0,    80,
     109,     0,    81,    47,   108,     0,    92,     0,    81,   131,
     133,    81,     0,   116,    81,    81,    81,    81,     0,    81,
       2,     0,    89,    81,    81,    20,    37,     8,    39,     6,
      40,    11,    42,    35,    36,    13,    21,    46,    43,    44,
      45,     0,    18,   111,     0,     0,    49,    48,   115,     0,
      77,    80,    81,    81,    81,    81,     0,     0,     0,    84,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   110,    81,   132,     0,     0,    85,
       0,    94,    81,   101,   104,    81,   129,   134,     1,   118,
     117,    81,     0,   121,   123,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   125,   127,
     128,   112,   113,   105,    87,    88,   130,   124,     0,    38,
      41,    21,    81,    19,    34,     0,    52,    51,    71,    69,
      63,    68,    61,    62,    75,    55,    54,    70,    59,    64,
      66,    57,    72,    56,    60,    65,    67,    58,    73,     0,
      74,    79,    82,     0,    98,     0,     0,     0,    81,    81,
      81,    95,     0,    81,     0,    91,     0,    93,   114,     0,
       0,   122,   126,    96,     0,    53,    90,    99,     0,    81,
      97,   119,     0,    81,   120
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -53,   -53,     1,    54,    55,    22,   -45,   -53,   -41,   -15,
      -7,    -9,   -53,   -53,   -53,   -53,   -53,   -53,   -53,   -53,
     -42,   -46,   -36,   -53
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
     150,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      75,   107,   -19,   110,   108,   151,    79,   143,   126,    81,
      74,    83,    84,   112,   113,   144,   147,   134,  -137,   145,
     117,   148,   156,    78,   140,   157,   253,    77,   159,   160,
     104,    73,  -102,   164,   111,   142,   162,   179,   180,   178,
     161,   118,   152,   103,    49,   145,    97,   149,   145,    98,
      76,   145,  -103,   105,   146,    99,    80,   228,   114,   -19,
     249,    49,   100,    82,   107,   115,   157,   157,   187,   116,
     154,   153,   196,   197,   148,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   155,
     149,   158,   181,   163,   222,   182,  -102,   183,   188,   184,
     145,   148,   -80,   224,   148,   157,   229,   193,   104,   120,
     121,   122,   123,   124,   125,   126,   230,   120,   129,   126,
     231,   131,   132,   126,   134,   108,   129,   149,   137,   138,
     149,   140,   134,   233,   237,   225,   238,   243,   226,   140,
     239,   148,   142,   120,   121,   251,   123,    75,   250,   126,
     142,    79,   129,    81,    83,    84,   132,     0,   134,     0,
     194,   195,     0,   138,     0,   140,   103,   149,   236,     0,
       0,     0,   120,   241,   242,   232,   142,   240,   126,   245,
     120,   121,   148,   123,     0,     0,   126,   134,     0,   129,
       0,     0,   131,   132,   140,   134,     0,   254,   252,   137,
     138,     0,   140,     0,     0,   142,     0,     0,   149,     0,
       0,     0,     0,   142,     0,     0,   244,  -136,     1,     0,
     248,     2,     3,     4,     5,     0,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,   -81,    46,     0,     0,
      47,    48,    49,  -135,     0,   -81,    50,     2,     3,   106,
       5,     0,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,     0,    46,     0,     0,    47,    48,    49,     0,
       0,     0,    50,     2,     3,   106,     5,     0,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,     0,    46,
       0,     0,    47,    48,    49,     0,     0,     0,    50,   186,
       2,     3,   106,     5,     0,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,     0,    46,     0,     0,    47,
      48,    49,     0,     0,     0,    50,   227,     2,     3,   106,
       5,     0,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,     0,    46,     0,     0,    47,    48,    49,   185,
       0,     0,    50,     2,     3,   106,     5,     0,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,     0,    46,
       0,     0,    47,    48,    49,     0,     0,     0,    50,     2,
       3,   106,     5,     0,     6,     7,    85,    86,    10,    11,
      87,    88,    89,    90,    91,    17,    92,    19,    93,    94,
      22,    23,    24,    25,    26,    27,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      95,    96,    44,    45,     0,   109,     0,     0,    47,    48,
      49,     0,   -86,     2,     3,   106,     5,     0,     6,     7,
      85,    86,    10,    11,    87,    88,    89,    90,    91,    17,
      92,    19,    93,    94,    22,    23,    24,    25,    26,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    95,    96,    44,    45,     0,   109,
       0,     0,    47,    48,    49,   -86,     2,     3,   106,     5,
       0,     6,     7,    85,    86,    10,    11,    87,    88,    89,
      90,    91,    17,    92,    19,    93,    94,    22,    23,    24,
      25,    26,    27,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    95,    96,    44,
      45,   235,   109,     0,     0,    47,    48,    49,     2,     3,
     106,     5,     0,     6,     7,    85,    86,    10,    11,    87,
      88,    89,    90,    91,    17,    92,    19,    93,    94,    22,
      23,    24,    25,    26,    27,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    95,
      96,    44,    45,     0,   109,     0,     0,    47,    48,    49,
       2,     3,     0,     5,     0,     6,     7,    85,    86,    10,
      11,    87,    88,    89,    90,    91,    17,    92,    19,    93,
      94,    22,    23,    24,    25,    26,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    95,    96,    44,    45,     2,     3,     0,     5,     0,
      48,    49,    85,    86,    10,    11,    87,    88,    89,    90,
      91,    17,    92,   189,    93,    94,    22,    23,     0,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    28,    29,    30,    31,    32,   190,    34,    35,
      36,    37,    38,    39,   119,    41,    95,   191,    44,   102,
       0,     0,     0,   165,   166,     0,   192,   167,   168,   169,
     170,   171,     0,   172,     0,   173,   174,     0,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,     0,   130,
     131,   132,   133,   134,     0,   135,   136,   137,   138,   139,
     140,     0,     0,     0,     0,     0,     0,   175,   176,     0,
     141,   142,   177,     2,     3,     0,     5,     0,     0,     0,
       0,    50,    10,    11,    87,     0,    89,     0,    91,    17,
       2,     3,     0,     5,    22,    23,     0,    25,    26,    10,
      11,    87,     0,    89,     0,    91,    17,   120,   121,     0,
     123,    22,    23,   126,    25,    26,   129,     0,     0,     0,
       0,     0,   134,     0,    95,   101,    44,   102,     0,   140,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     142,    95,     0,    44,   102,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,     0,   130,   131,   132,   133,
     134,     0,   135,   136,   137,   138,   139,   140,     0,     0,
       0,     0,     0,     0,   246,     0,     0,   141,   142,     0,
       0,   247,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,     0,   130,   131,   132,   133,   134,     0,   135,
     136,   137,   138,   139,   140,     0,     0,     0,     0,     0,
       0,   223,     0,     0,   141,   142,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,     0,   130,   131,   132,
     133,   134,     0,   135,   136,   137,   138,   139,   140,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   141,   142,
     120,   121,   122,   123,   124,   125,   126,   127,   234,   129,
       0,   130,   131,   132,   133,   134,     0,   135,   136,   137,
     138,   139,   140,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   141,   142,   120,   121,   122,   123,   124,   125,
     126,   127,     0,   129,     0,   130,   131,   132,   133,   134,
       0,   135,   136,   137,   138,   139,   140,   120,   121,   122,
     123,   124,   125,   126,     0,     0,   129,   142,   130,   131,
     132,   133,   134,     0,     0,   136,   137,   138,   139,   140,
     120,   121,   122,   123,   124,   125,   126,     0,     0,   129,
     142,   130,   131,   132,     0,   134,     0,     0,   136,   137,
     138,     0,   140,   120,   121,   122,   123,     0,   125,   126,
       0,     0,   129,   142,     0,   131,   132,     0,   134,     0,
       0,     0,   137,   138,     0,   140,   120,   121,   122,   123,
       0,     0,   126,     0,     0,   129,   142,     0,   131,   132,
       0,   134,     0,   120,   121,   137,   138,     0,   140,   126,
       0,     0,   129,     0,     0,     0,     0,     0,   134,   142,
       0,     0,     0,     0,     0,   140,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   142
};

static const yytype_int16 yycheck[] =
{
       9,    46,     8,    48,    46,    27,    15,    59,    36,    18,
      14,    20,    21,    49,    50,    67,    61,    45,     0,    59,
      40,    62,    68,    57,    52,    71,    66,    65,    74,    75,
      55,    68,    57,    79,    49,    63,    77,    83,    84,    81,
      76,    61,    64,    42,    65,    59,    24,    62,    59,    27,
      69,    59,    57,    58,    68,    33,    57,    68,    38,    65,
      68,    65,    40,    65,   109,    65,   112,   113,   114,     8,
      57,     9,   117,   118,   115,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,    65,
     115,     0,   101,    68,   145,   104,    57,    57,   115,    66,
      59,   152,    59,    66,   155,   161,     4,   116,    55,    30,
      31,    32,    33,    34,    35,    36,     5,    30,    39,    36,
      66,    42,    43,    36,    45,   177,    39,   152,    49,    50,
     155,    52,    45,    65,    67,   152,    66,    66,   155,    52,
      24,   192,    63,    30,    31,    68,    33,   166,    66,    36,
      63,   170,    39,   172,   173,   174,    43,    -1,    45,    -1,
     116,   116,    -1,    50,    -1,    52,   175,   192,   223,    -1,
      -1,    -1,    30,   229,   230,   192,    63,   228,    36,   234,
      30,    31,   233,    33,    -1,    -1,    36,    45,    -1,    39,
      -1,    -1,    42,    43,    52,    45,    -1,   253,   249,    49,
      50,    -1,    52,    -1,    -1,    63,    -1,    -1,   233,    -1,
      -1,    -1,    -1,    63,    -1,    -1,   233,     0,     1,    -1,
     239,     4,     5,     6,     7,    -1,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    -1,    -1,
      63,    64,    65,     0,    -1,    68,    69,     4,     5,     6,
       7,    -1,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    -1,    -1,    63,    64,    65,    -1,
      -1,    -1,    69,     4,     5,     6,     7,    -1,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      -1,    -1,    63,    64,    65,    -1,    -1,    -1,    69,    70,
       4,     5,     6,     7,    -1,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    60,    -1,    -1,    63,
      64,    65,    -1,    -1,    -1,    69,    70,     4,     5,     6,
       7,    -1,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    60,    -1,    -1,    63,    64,    65,    66,
      -1,    -1,    69,     4,     5,     6,     7,    -1,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      -1,    -1,    63,    64,    65,    -1,    -1,    -1,    69,     4,
       5,     6,     7,    -1,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    60,    -1,    -1,    63,    64,
      65,    -1,    67,     4,     5,     6,     7,    -1,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    60,
      -1,    -1,    63,    64,    65,    66,     4,     5,     6,     7,
      -1,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    -1,    63,    64,    65,     4,     5,
       6,     7,    -1,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    60,    -1,    -1,    63,    64,    65,
       4,     5,    -1,     7,    -1,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,     4,     5,    -1,     7,    -1,
      64,    65,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    -1,    28,
      29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,     6,    54,    55,    56,    57,    58,
      -1,    -1,    -1,    11,    12,    -1,    65,    15,    16,    17,
      18,    19,    -1,    21,    -1,    23,    24,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    -1,    41,
      42,    43,    44,    45,    -1,    47,    48,    49,    50,    51,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,
      62,    63,    60,     4,     5,    -1,     7,    -1,    -1,    -1,
      -1,    69,    13,    14,    15,    -1,    17,    -1,    19,    20,
       4,     5,    -1,     7,    25,    26,    -1,    28,    29,    13,
      14,    15,    -1,    17,    -1,    19,    20,    30,    31,    -1,
      33,    25,    26,    36,    28,    29,    39,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    55,    56,    57,    58,    -1,    52,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      63,    55,    -1,    57,    58,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    -1,    41,    42,    43,    44,
      45,    -1,    47,    48,    49,    50,    51,    52,    -1,    -1,
      -1,    -1,    -1,    -1,    59,    -1,    -1,    62,    63,    -1,
      -1,    66,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    -1,    41,    42,    43,    44,    45,    -1,    47,
      48,    49,    50,    51,    52,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    -1,    -1,    62,    63,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    -1,    41,    42,    43,
      44,    45,    -1,    47,    48,    49,    50,    51,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,    63,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,    41,    42,    43,    44,    45,    -1,    47,    48,    49,
      50,    51,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    62,    63,    30,    31,    32,    33,    34,    35,
      36,    37,    -1,    39,    -1,    41,    42,    43,    44,    45,
      -1,    47,    48,    49,    50,    51,    52,    30,    31,    32,
      33,    34,    35,    36,    -1,    -1,    39,    63,    41,    42,
      43,    44,    45,    -1,    -1,    48,    49,    50,    51,    52,
      30,    31,    32,    33,    34,    35,    36,    -1,    -1,    39,
      63,    41,    42,    43,    -1,    45,    -1,    -1,    48,    49,
      50,    -1,    52,    30,    31,    32,    33,    -1,    35,    36,
      -1,    -1,    39,    63,    -1,    42,    43,    -1,    45,    -1,
      -1,    -1,    49,    50,    -1,    52,    30,    31,    32,    33,
      -1,    -1,    36,    -1,    -1,    39,    63,    -1,    42,    43,
      -1,    45,    -1,    30,    31,    49,    50,    -1,    52,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    45,    63,
      -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    63
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     4,     5,     6,     7,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    60,    63,    64,    65,
      69,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    68,    14,    82,    69,    65,    57,    82,
      57,    82,    65,    82,    82,    11,    12,    15,    16,    17,
      18,    19,    21,    23,    24,    55,    56,    76,    76,    76,
      76,    56,    58,    73,    55,    58,     6,    77,    91,    60,
      77,    80,    93,    93,    38,    65,     8,    40,    61,     6,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      41,    42,    43,    44,    45,    47,    48,    49,    50,    51,
      52,    62,    63,    59,    67,    59,    68,    77,    79,    80,
      81,    27,    64,     9,    57,    65,    92,    92,     0,    92,
      92,    93,    79,    68,    92,    11,    12,    15,    16,    17,
      18,    19,    21,    23,    24,    55,    56,    60,    91,    92,
      92,    73,    73,    57,    66,    66,    70,    92,    81,    22,
      46,    56,    65,    73,    74,    75,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    79,    59,    66,    81,    81,    70,    68,     4,
       5,    66,    81,    65,    38,    59,    77,    67,    66,    24,
      79,    92,    92,    66,    81,    77,    59,    66,    82,    68,
      66,    68,    79,    66,    92
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    71,    72,    73,    73,    73,    73,    73,    73,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    76,    76,    76,    76,    76,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    78,    78,    78,
      79,    79,    80,    80,    80,    81,    81,    82,    82,    83,
      84,    84,    85,    85,    85,    85,    85,    85,    85,    85,
      86,    86,    87,    87,    88,    88,    88,    88,    88,    88,
      88,    89,    89,    89,    90,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    92,    92,    93,    93,    94,    94,    94,    94
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     2,     2,     1,     2,     1,
       1,     3,     3,     5,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     1,     2,     1,     3,
       1,     0,     3,     1,     2,     1,     0,     3,     3,     2,
       5,     4,     1,     4,     2,     4,     5,     6,     3,     5,
       1,     2,     1,     1,     2,     3,     1,     1,     1,     1,
       2,     2,     3,     3,     4,     2,     2,     3,     3,     7,
       9,     3,     5,     3,     3,     3,     5,     3,     3,     2,
       3,     1,     2,     1,     2,     1,     0,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (TDITHREADSTATIC_VAR, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, TDITHREADSTATIC_VAR); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, TDITHREADSTATIC_TYPE *TDITHREADSTATIC_VAR)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (TDITHREADSTATIC_VAR);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, TDITHREADSTATIC_TYPE *TDITHREADSTATIC_VAR)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, TDITHREADSTATIC_VAR);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, TDITHREADSTATIC_TYPE *TDITHREADSTATIC_VAR)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , TDITHREADSTATIC_VAR);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, TDITHREADSTATIC_VAR); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, TDITHREADSTATIC_TYPE *TDITHREADSTATIC_VAR)
{
  YYUSE (yyvaluep);
  YYUSE (TDITHREADSTATIC_VAR);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (TDITHREADSTATIC_TYPE *TDITHREADSTATIC_VAR)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, TDITHREADSTATIC_VAR);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 218 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=(yyvsp[0].mark);}
#line 1807 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 43:
#line 246 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((((yyvsp[-1].mark).builtin == OPC_SUBTRACT) ? OPC_UNARY_MINUS : OPC_UNARY_PLUS),(yyvsp[0].mark),(yyval.mark));}
#line 1813 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 44:
#line 247 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 1819 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 45:
#line 248 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 1825 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 46:
#line 249 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 1831 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 48:
#line 252 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark).rptr=(yyvsp[0].mark).rptr; (yyval.mark).builtin= -2;
					__RUN(tdi_yacc_IMMEDIATE(&(yyval.mark).rptr, TDITHREADSTATIC_VAR));}
#line 1838 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 49:
#line 254 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {yyerror(TDITHREADSTATIC_VAR, "lex error"); return YY_ERR;}
#line 1844 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 51:
#line 256 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2(OPC_EQUALS,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1850 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 52:
#line 257 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {struct marker tmp;		/*binary operation and assign*/
						_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),tmp);
						_JUST1(OPC_EQUALS_FIRST,tmp,(yyval.mark));}
#line 1858 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 53:
#line 260 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST3(OPC_CONDITIONAL,(yyvsp[-2].mark),(yyvsp[0].mark),(yyvsp[-4].mark),(yyval.mark));}
#line 1864 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 54:
#line 261 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {if ((yyvsp[0].mark).rptr && (yyvsp[0].mark).rptr->dtype == DTYPE_RANGE)
						if ((yyvsp[0].mark).rptr->ndesc == 2)
							{(yyval.mark)=(yyvsp[0].mark);
							(yyval.mark).rptr->dscptrs[2]=(yyval.mark).rptr->dscptrs[1];
							(yyval.mark).rptr->dscptrs[1]=(yyval.mark).rptr->dscptrs[0];
							(yyval.mark).rptr->dscptrs[0]=(mdsdsc_t *)(yyvsp[-2].mark).rptr;
							++(yyval.mark).rptr->ndesc;}
						else {TDI_REFZONE.l_status=TdiEXTRA_ARG; yyerror(TDITHREADSTATIC_VAR, "extra args"); return YY_ASIS;}
					else	{static unsigned int vmlen = sizeof(struct descriptor_range);
						LibGetVm(&vmlen, (void **)&(yyval.mark).rptr, &TDI_REFZONE.l_zone);
						(yyval.mark).rptr->length = 0;
						(yyval.mark).rptr->dtype = DTYPE_RANGE;
						(yyval.mark).rptr->class = CLASS_R;
						(yyval.mark).rptr->pointer = 0;
						(yyval.mark).rptr->ndesc = 2;
						(yyval.mark).rptr->dscptrs[0]=(mdsdsc_t *)(yyvsp[-2].mark).rptr;
						(yyval.mark).rptr->dscptrs[1]=(mdsdsc_t *)(yyvsp[0].mark).rptr;
						_RESOLVE((yyval.mark));}
					}
#line 1888 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 55:
#line 280 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1894 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 56:
#line 281 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1900 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 57:
#line 282 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1906 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 58:
#line 283 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1912 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 59:
#line 284 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1918 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 60:
#line 285 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1924 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 61:
#line 286 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1930 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 62:
#line 287 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1936 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 63:
#line 288 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1942 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 64:
#line 289 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1948 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 65:
#line 290 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1954 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 66:
#line 291 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1960 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 67:
#line 292 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1966 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 68:
#line 293 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1972 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 69:
#line 294 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {if ((yyval.mark).rptr == 0) (yyval.mark)=(yyvsp[0].mark);
					else if ((yyval.mark).rptr->dtype == DTYPE_FUNCTION
					&&	*(opcode_t *)(yyval.mark).rptr->pointer == OPC_CONCAT
					&&	(yyval.mark).rptr->ndesc < 250)
						{(yyval.mark).rptr->dscptrs[(yyval.mark).rptr->ndesc++]=(mdsdsc_t *)(yyvsp[0].mark).rptr;
						_RESOLVE((yyval.mark));}
					else {_FULL2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
					}
#line 1985 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 70:
#line 302 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1991 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 71:
#line 303 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1997 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 72:
#line 304 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2003 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 73:
#line 305 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2009 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 74:
#line 306 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2(OPC_MULTIPLY,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2015 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 75:
#line 307 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2021 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 76:
#line 308 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=_EMPTY_MARKER;}
#line 2027 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 77:
#line 315 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_FULL1(OPC_VECTOR,(yyvsp[0].mark),(yyval.mark));}
#line 2033 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 78:
#line 316 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST0(OPC_VECTOR,(yyval.mark));}
#line 2039 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 79:
#line 317 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {if ((yyval.mark).rptr->ndesc >= 250) {
					_RESOLVE((yyvsp[-2].mark));
					_FULL1(OPC_VECTOR,(yyvsp[-2].mark),(yyval.mark));
				}
				(yyval.mark).rptr->dscptrs[(yyval.mark).rptr->ndesc++] = (mdsdsc_t *)(yyvsp[0].mark).rptr;
				}
#line 2050 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 81:
#line 325 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=_EMPTY_MARKER;}
#line 2056 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 82:
#line 327 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {if (	(yyval.mark).rptr			/*comma is left-to-right weakest*/
			&&	(yyval.mark).builtin != -2
			&&	(yyval.mark).rptr->dtype == DTYPE_FUNCTION
			&&	*(opcode_t *)(yyval.mark).rptr->pointer == OPC_COMMA
			&&	(yyval.mark).rptr->ndesc < 250)
				(yyval.mark).rptr->dscptrs[(yyval.mark).rptr->ndesc++]=(mdsdsc_t *)(yyvsp[0].mark).rptr;
			else _FULL2(OPC_COMMA,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2068 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 84:
#line 335 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {yyerror(TDITHREADSTATIC_VAR, "lex error"); return YY_ERR;}
#line 2074 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 85:
#line 337 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {if ((yyval.mark).rptr
				&& (yyval.mark).builtin != -2
				&& (yyval.mark).rptr->dtype == DTYPE_FUNCTION
				&& *(opcode_t *)(yyval.mark).rptr->pointer == OPC_COMMA) ;
				else _JUST1(OPC_ABORT,(yyvsp[0].mark),(yyval.mark));}
#line 2084 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 86:
#line 342 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST0(OPC_ABORT,(yyval.mark));}
#line 2090 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 87:
#line 344 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=(yyvsp[-1].mark); (yyval.mark).builtin= -2;}
#line 2096 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 88:
#line 345 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=(yyvsp[-1].mark); (yyval.mark).builtin= -2;}
#line 2102 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 89:
#line 347 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {++TDI_REFZONE.l_rel_path;}
#line 2108 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 90:
#line 349 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_FULL2(OPC_ABORT,(yyvsp[-3].mark),(yyvsp[-1].mark),(yyval.mark)); --TDI_REFZONE.l_rel_path;}
#line 2114 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 91:
#line 350 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_FULL2(OPC_ABORT,(yyvsp[-2].mark),_EMPTY_MARKER,(yyval.mark)); --TDI_REFZONE.l_rel_path;}
#line 2120 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 93:
#line 356 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {int j;
					(yyval.mark)=(yyvsp[-1].mark);
					(yyval.mark).rptr->pointer= (uint8_t *)&OpcSubscript;
					for (j=(yyval.mark).rptr->ndesc; --j>=0; )
						(yyval.mark).rptr->dscptrs[j+1]=(yyval.mark).rptr->dscptrs[j];
					(yyval.mark).rptr->dscptrs[0]=(mdsdsc_t *)(yyvsp[-3].mark).rptr;
					(yyval.mark).rptr->ndesc++;
					_RESOLVE((yyval.mark));
				}
#line 2134 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 94:
#line 365 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {int j=(yyvsp[0].mark).builtin==OPC_PRE_INC ? OPC_POST_INC :  OPC_POST_DEC;
					_JUST1(j,(yyvsp[-1].mark),(yyval.mark));}
#line 2141 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 95:
#line 367 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=(yyvsp[-1].mark);
				if ((yyvsp[-3].mark).builtin < 0) {int j;		/*unknown today*/
					(yyval.mark).rptr->pointer= (uint8_t *)&OpcExtFunction;
					for (j=(yyval.mark).rptr->ndesc; --j>=0;)
						(yyval.mark).rptr->dscptrs[j+2]=(yyval.mark).rptr->dscptrs[j];
					(yyval.mark).rptr->dscptrs[0]=0;
					(yyval.mark).rptr->dscptrs[1]=(mdsdsc_t *)(yyvsp[-3].mark).rptr;
					(yyval.mark).rptr->ndesc += 2;
				}
				else	{				/*intrinsic*/
					*(opcode_t *)(yyval.mark).rptr->pointer=(yyvsp[-3].mark).builtin;
					_RESOLVE((yyval.mark));}
				}
#line 2159 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 96:
#line 380 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {int j;
				(yyval.mark)=(yyvsp[-1].mark);			/*external*/
				(yyval.mark).rptr->dtype=DTYPE_CALL;
				(yyval.mark).rptr->length=0;
				(yyval.mark).rptr->pointer=0;
				for (j=(yyval.mark).rptr->ndesc; --j>=0;)
					(yyval.mark).rptr->dscptrs[j+2]=(yyval.mark).rptr->dscptrs[j];
				(yyval.mark).rptr->dscptrs[0]=(mdsdsc_t *)(yyvsp[-4].mark).rptr;
				(yyval.mark).rptr->dscptrs[1]=(mdsdsc_t *)(yyvsp[-3].mark).rptr;
				(yyval.mark).rptr->ndesc += 2;
				}
#line 2175 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 97:
#line 391 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {int j;
				(yyval.mark)=(yyvsp[-1].mark);			/*typed external*/
				StrUpcase((mdsdsc_t *)(yyvsp[-3].mark).rptr, (mdsdsc_t *)(yyvsp[-3].mark).rptr);
				for (j=TdiCAT_MAX; j-->0;)
					if (strncmp(TdiREF_CAT[j].name, (char *)(yyvsp[-3].mark).rptr->pointer, (yyvsp[-3].mark).rptr->length) == 0
					&& TdiREF_CAT[j].name[(yyvsp[-3].mark).rptr->length] == '\0') // exact match
						break;
				if (j<0) {TDI_REFZONE.l_status=TdiINVDTYDSC; yyerror(TDITHREADSTATIC_VAR, "invalid dtype desc"); return YY_ERR;}
				(yyval.mark).rptr->dtype=DTYPE_CALL;
				(yyval.mark).rptr->length=1;
				*(unsigned char *)(yyval.mark).rptr->pointer=(unsigned char)j;
				for (j=(yyval.mark).rptr->ndesc; --j>=0;)
					(yyval.mark).rptr->dscptrs[j+2]=(yyval.mark).rptr->dscptrs[j];
				(yyval.mark).rptr->dscptrs[0]=(mdsdsc_t *)(yyvsp[-5].mark).rptr;
				(yyval.mark).rptr->dscptrs[1]=(mdsdsc_t *)(yyvsp[-4].mark).rptr;
				(yyval.mark).rptr->ndesc += 2;
				}
#line 2197 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 98:
#line 408 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {int j;	/*USING(expr,[default],[shotid],[expt])*/
					(yyval.mark).rptr->pointer= (uint8_t *)&OpcUsing;
					for (j=0; j < (yyvsp[-1].mark).rptr->ndesc; ++j)
						(yyval.mark).rptr->dscptrs[(yyval.mark).rptr->ndesc++]=(yyvsp[-1].mark).rptr->dscptrs[j];
				}
#line 2207 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 99:
#line 413 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2(OPC_USING,(yyvsp[-3].mark),(yyvsp[-1].mark),(yyval.mark)); --TDI_REFZONE.l_rel_path;}
#line 2213 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 101:
#line 419 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {MAKE_S(DTYPE_T, (yyvsp[-1].mark).rptr->length + (yyvsp[0].mark).rptr->length, (yyval.mark).rptr);
					StrConcat((mdsdsc_t *)(yyval.mark).rptr, (mdsdsc_t *)(yyvsp[-1].mark).rptr, (yyvsp[0].mark).rptr MDS_END_ARG);
				}
#line 2221 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 104:
#line 429 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 2227 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 105:
#line 430 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {struct marker tmp;			/*OPTIONAL IN/INOUT/OUT*/
					_JUST1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),tmp);
					_JUST1((yyvsp[-2].mark).builtin,tmp,(yyval.mark));}
#line 2235 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 106:
#line 433 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {if (*(yyval.mark).rptr->pointer == '$') {
					if((yyval.mark).builtin < 0) (yyval.mark).rptr->dtype=DTYPE_IDENT;
					else if ((TdiRefFunction[(yyval.mark).builtin].token & LEX_M_TOKEN) == LEX_ARG)
					{__RUN(tdi_yacc_ARG(&(yyval.mark), TDITHREADSTATIC_VAR));}
					else if ((TdiRefFunction[(yyval.mark).builtin].token & LEX_M_TOKEN) == LEX_CONST)
						_JUST0((yyvsp[0].mark).builtin,(yyval.mark));
				} else	if (*(yyval.mark).rptr->pointer == '_')
					(yyval.mark).rptr->dtype=DTYPE_IDENT;
				else if (tdi_lex_path((yyvsp[0].mark).rptr->length, (yyvsp[0].mark).rptr->pointer, &(yyval.mark), TDITHREADSTATIC_VAR) == LEX_ERROR)
					{yyerror(TDITHREADSTATIC_VAR, "yacc_path failed"); return YY_ERR;}
				}
#line 2251 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 110:
#line 447 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_RESOLVE((yyval.mark));}
#line 2257 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 111:
#line 454 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=(yyvsp[0].mark);}
#line 2263 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 112:
#line 455 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 2269 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 113:
#line 456 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-2].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 2275 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 114:
#line 458 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {int j;	(yyval.mark)=(yyvsp[-1].mark);
							(yyval.mark).rptr->pointer= (uint8_t *)&OpcFun;
							for (j=(yyval.mark).rptr->ndesc; --j>=0;)
								(yyval.mark).rptr->dscptrs[j+2]=(yyval.mark).rptr->dscptrs[j];
							(yyval.mark).rptr->dscptrs[0]=(mdsdsc_t *)(yyvsp[-3].mark).rptr;
							(yyval.mark).rptr->ndesc += 2;
							++TDI_REFZONE.l_rel_path;
						}
#line 2288 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 115:
#line 467 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark).rptr=(yyvsp[0].mark).rptr; (yyval.mark).builtin= -2;
						__RUN(tdi_yacc_IMMEDIATE(&(yyval.mark).rptr, TDITHREADSTATIC_VAR));}
#line 2295 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 116:
#line 469 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST0((yyvsp[-1].mark).builtin,(yyval.mark));}
#line 2301 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 117:
#line 470 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_FULL2((yyvsp[-2].mark).builtin,(yyvsp[-1].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2307 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 118:
#line 471 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_FULL1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 2313 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 119:
#line 472 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-6].mark).builtin,(yyvsp[-1].mark),(yyvsp[-4].mark),(yyval.mark));}
#line 2319 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 120:
#line 473 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST4((yyvsp[-8].mark).builtin,(yyvsp[-6].mark),(yyvsp[-4].mark),(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2325 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 121:
#line 474 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-2].mark).builtin,(yyvsp[-1].mark),(yyval.mark));}
#line 2331 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 122:
#line 475 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST3((yyvsp[-4].mark).builtin,(yyvsp[-3].mark),(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2337 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 123:
#line 476 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-2].mark).builtin,(yyvsp[-1].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2343 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 124:
#line 477 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_FULL2(OPC_LABEL,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2349 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 125:
#line 478 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-2].mark).builtin,(yyvsp[-1].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2355 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 126:
#line 479 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST3((yyvsp[-4].mark).builtin,(yyvsp[-3].mark),(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2361 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 127:
#line 480 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-2].mark).builtin,(yyvsp[-1].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2367 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 128:
#line 481 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-2].mark).builtin,(yyvsp[-1].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2373 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 129:
#line 482 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {TDI_REFZONE.l_rel_path--;
						(yyval.mark).rptr->dscptrs[1]=(mdsdsc_t *)(yyvsp[0].mark).rptr;}
#line 2380 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 130:
#line 484 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=(yyvsp[-1].mark); _RESOLVE((yyval.mark));}
#line 2386 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 132:
#line 487 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=(yyvsp[-1].mark);}
#line 2392 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 134:
#line 489 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {short opcode;
			if ((yyval.mark).rptr == 0) {(yyval.mark)=(yyvsp[0].mark);}		/* initial null statement	*/
			else if ((yyvsp[0].mark).rptr == 0) {}		/* trailing null statement	*/
			else if ((yyval.mark).rptr->dtype == DTYPE_FUNCTION
			&& (yyval.mark).rptr->ndesc < 250
			&& ((opcode = *(opcode_t *)(yyval.mark).rptr->pointer) == OPC_STATEMENT
				|| opcode == OPC_CASE
				|| opcode == OPC_DEFAULT
				|| opcode == OPC_LABEL
			)) {(yyval.mark).rptr->dscptrs[(yyval.mark).rptr->ndesc++]=(mdsdsc_t *)(yyvsp[0].mark).rptr;}
			else	{_FULL2(OPC_STATEMENT,(yyvsp[-1].mark),(yyvsp[0].mark),(yyval.mark));}
			}
#line 2409 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 135:
#line 502 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {_RESOLVE((yyval.mark));		/*statements*/
			TDI_REFZONE.a_result=(struct descriptor_d *)(yyval.mark).rptr;
			TDI_REFZONE.l_status=SsSUCCESS;}
#line 2417 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 136:
#line 505 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=_EMPTY_MARKER;}
#line 2423 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 137:
#line 506 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {yyerror(TDITHREADSTATIC_VAR, "lex error"); return YY_ERR;}
#line 2429 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 138:
#line 507 "yylex/TdiYacc.y" /* yacc.c:1646  */
    {TDI_REFZONE.l_status=TdiSYNTAX; yyerror(TDITHREADSTATIC_VAR, "syntax error"); return YY_ASIS;}
#line 2435 "TdiYacc.c" /* yacc.c:1646  */
    break;


#line 2439 "TdiYacc.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (TDITHREADSTATIC_VAR, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (TDITHREADSTATIC_VAR, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, TDITHREADSTATIC_VAR);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, TDITHREADSTATIC_VAR);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (TDITHREADSTATIC_VAR, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, TDITHREADSTATIC_VAR);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, TDITHREADSTATIC_VAR);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 509 "yylex/TdiYacc.y" /* yacc.c:1906  */

