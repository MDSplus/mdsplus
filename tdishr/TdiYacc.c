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
#line 13 "TdiYacc.y" /* yacc.c:339  */

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

#define YYDEBUG 1
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

#line 185 "TdiYacc.c" /* yacc.c:339  */

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
#line 6 "TdiYacc.y" /* yacc.c:355  */

#define YYLTYPE TDITHREADSTATIC_TYPE
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

#line 223 "TdiYacc.c" /* yacc.c:355  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    END = 258,
    ELSE = 259,
    ELSEW = 260,
    ERROR = 261,
    IDENT = 262,
    POINT = 263,
    TEXT = 264,
    VALUE = 265,
    BREAK = 266,
    CASE = 267,
    COND = 268,
    DEFAULT = 269,
    DO = 270,
    FOR = 271,
    GOTO = 272,
    IF = 273,
    LABEL = 274,
    RETURN = 275,
    SIZEOF = 276,
    SWITCH = 277,
    USING = 278,
    WHERE = 279,
    WHILE = 280,
    CAST = 281,
    CONST = 282,
    INC = 283,
    ARG = 284,
    ADD = 285,
    CONCAT = 286,
    IAND = 287,
    IN = 288,
    IOR = 289,
    IXOR = 290,
    LEQV = 291,
    POWER = 292,
    PROMO = 293,
    RANGE = 294,
    SHIFT = 295,
    BINEQ = 296,
    LAND = 297,
    LEQ = 298,
    LGE = 299,
    LOR = 300,
    MUL = 301,
    UNARY = 302,
    LANDS = 303,
    LEQS = 304,
    LGES = 305,
    LORS = 306,
    MULS = 307,
    UNARYS = 308,
    FUN = 309,
    MODIF = 310,
    VBL = 311,
    AMODIF = 312
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 135 "TdiYacc.y" /* yacc.c:355  */
struct marker mark;

#line 296 "TdiYacc.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (TDITHREADSTATIC_TYPE *TDITHREADSTATIC_VAR);

#endif /* !YY_YY_TDIYACC_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 312 "TdiYacc.c" /* yacc.c:358  */

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
#define YYFINAL  146
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1402

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  70
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  24
/* YYNRULES -- Number of rules.  */
#define YYNRULES  126
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  243

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   312

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
      64,    65,    62,     2,    58,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    67,
       2,    60,     2,    61,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    63,     2,    66,     2,     2,    59,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    68,     2,    69,     2,     2,     2,     2,
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
      55,    56,    57
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   213,   213,   220,   220,   220,   220,   220,   221,   221,
     221,   221,   222,   222,   223,   223,   223,   223,   223,   225,
     225,   226,   227,   227,   227,   227,   227,   228,   228,   228,
     228,   236,   237,   238,   239,   240,   242,   244,   245,   246,
     247,   250,   251,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   292,   293,
     294,   295,   296,   297,   298,   305,   306,   307,   314,   315,
     317,   324,   325,   327,   332,   334,   335,   337,   339,   340,
     345,   346,   355,   357,   370,   381,   398,   403,   408,   409,
     418,   418,   419,   420,   423,   434,   435,   436,   437,   444,
     445,   446,   448,   457,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   474,   476,
     477,   478,   479,   492,   495,   496,   497
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "END", "ELSE", "ELSEW", "ERROR", "IDENT",
  "POINT", "TEXT", "VALUE", "BREAK", "CASE", "COND", "DEFAULT", "DO",
  "FOR", "GOTO", "IF", "LABEL", "RETURN", "SIZEOF", "SWITCH", "USING",
  "WHERE", "WHILE", "CAST", "CONST", "INC", "ARG", "ADD", "CONCAT", "IAND",
  "IN", "IOR", "IXOR", "LEQV", "POWER", "PROMO", "RANGE", "SHIFT", "BINEQ",
  "LAND", "LEQ", "LGE", "LOR", "MUL", "UNARY", "LANDS", "LEQS", "LGES",
  "LORS", "MULS", "UNARYS", "FUN", "MODIF", "VBL", "AMODIF", "','", "'`'",
  "'='", "'?'", "'*'", "'['", "'('", "')'", "']'", "';'", "'{'", "'}'",
  "$accept", "slabel", "ulabel", "flabel", "label", "unaryX", "ass",
  "bracket", "opt", "exp", "sub", "paren", "using0", "using", "postX",
  "textX", "modif", "primaX", "funvbl", "fun", "stmt0", "stmt", "stmt_lst",
  "program", YY_NULLPTR
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
     305,   306,   307,   308,   309,   310,   311,   312,    44,    96,
      61,    63,    42,    91,    40,    41,    93,    59,   123,   125
};
# endif

#define YYPACT_NINF -76

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-76)))

#define YYTABLE_NINF -126

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     225,   -76,   -76,   -76,     9,   -76,   -76,   -76,   -35,    -7,
     -76,   -76,   -32,   -13,    -1,     2,     7,   -76,   -76,     2,
      10,     2,     2,   -76,   -76,   864,   -76,   864,   864,   864,
    1003,    13,   -76,   -31,   552,   -76,   803,   552,   552,    21,
      -6,    36,    63,   -19,   953,     4,   -37,   -76,   -76,   803,
     680,   -23,    93,    48,   -76,    42,   552,   -76,   -76,   290,
     108,   -76,   552,   552,   552,   803,    47,   552,   -76,   109,
     -76,   552,   552,   -76,   -76,   -76,   -76,   -76,   -76,   -76,
     -76,   -76,   -76,   -76,    59,   -76,   -76,   -76,   -76,  1021,
     -76,   -76,  1021,    60,   -76,  1118,   -76,   803,  1118,    52,
     487,   355,   552,   680,   918,   803,   803,   -76,   803,   803,
     803,   803,   803,   803,   803,   803,   803,   803,   803,   803,
     803,   803,   803,   803,   803,   803,   803,   803,   803,   803,
     803,   803,   -76,   803,   -76,  1085,    61,    65,    53,   -76,
     617,   -76,   -76,   680,   -76,   -76,   -76,   -76,   -76,   421,
     -25,   -76,   126,   -35,    -7,   -32,   -13,    -1,     2,     7,
       2,     2,     2,  1003,    78,   109,   -76,   132,   -76,   -76,
     -76,   -76,   -76,   -76,   -76,   -76,    73,   -76,   -76,   -76,
     680,   -76,   -76,    77,  1118,  1118,   -17,   916,  1299,  1340,
    1276,   142,  1207,   105,  1184,  1118,    -3,  1253,  1322,  1327,
    1230,   105,  1253,  1322,  1327,  1230,   105,  1151,   105,  1118,
     -76,   742,   -76,    81,    79,   123,   803,   552,   552,   -76,
      85,   680,   803,   -76,  1049,   -76,   -76,     2,   -11,   -76,
     -76,   -76,    89,  1118,   -76,   -76,    88,   803,   -76,   -76,
     -27,   552,   -76
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,   126,     8,     9,    37,     3,    88,    95,    20,    24,
      16,    18,    12,    27,     6,    28,    10,    11,     7,    30,
      26,    22,    23,     5,     4,     0,    17,     0,    29,     0,
      14,    25,    15,    13,     0,    64,    66,    69,    69,     0,
      94,    21,     0,    38,    71,     0,     0,    68,    97,     0,
      69,    35,    96,     0,    80,     0,    69,   119,   121,    69,
       0,   104,    69,    69,    69,    69,     0,    69,     2,     0,
      77,    69,    69,    20,    24,    12,    27,     6,    28,    10,
      30,    22,    23,    14,    25,    34,    31,    32,    33,     0,
      13,    99,     0,     0,    37,    36,   103,     0,    65,    68,
      69,    69,    69,    69,     0,     0,     0,    72,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    98,    69,   120,     0,     0,    73,     0,    82,
      69,    89,    92,    69,   117,   122,     1,   106,   105,    69,
       0,   109,   111,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   113,   115,   116,   100,
     101,    93,    75,    76,   118,   112,     0,    26,    29,    25,
      69,    19,    21,     0,    40,    39,    59,    57,    51,    56,
      49,    50,    44,    63,    43,    42,    58,    47,    52,    54,
      45,    60,    48,    53,    55,    46,    61,     0,    62,    67,
      70,     0,    86,     0,     0,     0,    69,    69,    69,    83,
       0,    69,     0,    79,     0,    81,   102,     0,     0,   110,
     114,    84,     0,    41,    78,    87,     0,    69,    85,   107,
       0,    69,   108
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -76,   -76,    18,    54,    55,    84,   -33,   -76,    -4,     0,
     -75,    -9,   -76,   -76,   -76,   -76,   -76,   -76,   -76,   -76,
     -30,   -48,   -20,   -76
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    39,    40,    41,    42,    43,    44,    45,    46,    47,
     138,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      63,    95,   -19,    98,    96,   139,    67,    62,   144,  -125,
      69,   145,    71,    72,   147,   148,   135,   100,   101,   152,
     115,   133,   105,   167,   168,   -91,    93,   108,   176,   123,
     134,   133,    61,   133,   115,   128,    64,    99,   241,   166,
     140,   106,   216,   123,   149,   130,   136,   133,    91,   128,
     137,    65,   145,   145,   175,    66,   237,    37,   -19,   130,
     102,   150,   131,    68,    95,   213,    37,    92,   214,   -90,
     132,   104,   184,   185,    70,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   136,
     103,   145,   141,   137,   142,   220,   143,   169,   146,    85,
     170,    86,    87,    88,   151,   -90,   171,   172,   212,   133,
     153,   154,   181,   -68,   155,   156,   157,   158,   159,   210,
     217,   160,    92,   161,   162,    96,   136,   218,   219,   136,
     137,   221,   115,   137,   226,    63,   232,   225,   227,    67,
     231,    69,    71,    72,   238,   239,     0,     0,   182,   183,
       0,     0,     0,   163,   164,     0,     0,     0,   165,   229,
     230,     0,   108,   109,   110,   111,   136,    38,   224,   115,
     137,    91,   118,     0,     0,   120,   121,     0,   123,   233,
       0,   125,   126,   242,   128,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   130,     0,     0,     0,     0,     0,
       0,     0,   228,     0,     0,     0,     0,   136,   236,     0,
       0,   137,     0,     0,     0,  -124,     1,     0,     0,     2,
       3,     4,     5,   240,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    28,     0,     0,     0,     0,     0,    29,    30,
      31,    32,    33,   -69,    34,     0,     0,    35,    36,    37,
    -123,     0,   -69,    38,     2,     3,    94,     5,     0,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    28,     0,     0,
       0,     0,     0,    29,    30,    31,    32,    33,     0,    34,
       0,     0,    35,    36,    37,     0,     0,     0,    38,     2,
       3,    94,     5,     0,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    28,     0,     0,     0,     0,     0,    29,    30,
      31,    32,    33,     0,    34,     0,     0,    35,    36,    37,
       0,     0,     0,    38,   174,     2,     3,    94,     5,     0,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    28,     0,
       0,     0,     0,     0,    29,    30,    31,    32,    33,     0,
      34,     0,     0,    35,    36,    37,     0,     0,     0,    38,
     215,     2,     3,    94,     5,     0,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    28,     0,     0,     0,     0,     0,
      29,    30,    31,    32,    33,     0,    34,     0,     0,    35,
      36,    37,   173,     0,     0,    38,     2,     3,    94,     5,
       0,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    28,
       0,     0,     0,     0,     0,    29,    30,    31,    32,    33,
       0,    34,     0,     0,    35,    36,    37,     0,     0,     0,
      38,     2,     3,    94,     5,     0,     6,     7,    73,    74,
      10,    11,    75,    76,    77,    78,    79,    17,    18,    80,
      20,    81,    82,    23,    24,    25,    26,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    28,     0,     0,     0,     0,     0,
      29,    83,    84,    32,    33,     0,    97,     0,     0,    35,
      36,    37,     0,   -74,     2,     3,    94,     5,     0,     6,
       7,    73,    74,    10,    11,    75,    76,    77,    78,    79,
      17,    18,    80,    20,    81,    82,    23,    24,    25,    26,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    28,     0,     0,
       0,     0,     0,    29,    83,    84,    32,    33,     0,    97,
       0,     0,    35,    36,    37,   -74,     2,     3,    94,     5,
       0,     6,     7,    73,    74,    10,    11,    75,    76,    77,
      78,    79,    17,    18,    80,    20,    81,    82,    23,    24,
      25,    26,    27,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    28,
       0,     0,     0,     0,     0,    29,    83,    84,    32,    33,
     223,    97,     0,     0,    35,    36,    37,     2,     3,    94,
       5,     0,     6,     7,    73,    74,    10,    11,    75,    76,
      77,    78,    79,    17,    18,    80,    20,    81,    82,    23,
      24,    25,    26,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      28,     0,     0,     0,     0,     0,    29,    83,    84,    32,
      33,     0,    97,     0,     0,    35,    36,    37,     2,     3,
       0,     5,     0,     6,     7,    73,    74,    10,    11,    75,
      76,    77,    78,    79,    17,    18,    80,    20,    81,    82,
      23,    24,    25,    26,    27,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    28,     0,     0,     0,     0,     0,    29,    83,    84,
      32,    33,     2,     3,     0,     5,     0,    36,    37,    73,
      74,    10,    11,    75,    76,    77,    78,    79,    17,    18,
      80,   177,    81,    82,    23,    24,   108,    26,     0,     0,
       0,     0,     0,   115,     0,     0,   118,     0,     0,   107,
       0,     0,   123,     0,     0,   178,     0,     0,   128,     0,
       0,     0,    83,   179,    32,    90,     0,     0,   130,     0,
       0,     0,   180,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,     0,   119,   120,   121,   122,   123,
       0,   124,   125,   126,   127,   128,     0,     2,     3,     0,
       5,     0,     0,     0,   129,   130,    10,    11,    75,     0,
      77,     0,    79,    17,    18,     2,     3,     0,     5,    23,
      24,     0,    26,     0,    10,    11,    75,     0,    77,     0,
      79,    17,    18,     0,     0,     0,     0,    23,    24,     0,
      26,     0,     0,     0,     0,     0,     0,    83,    89,    32,
      90,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    83,     0,    32,    90,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
       0,   119,   120,   121,   122,   123,     0,   124,   125,   126,
     127,   128,     0,     0,     0,     0,     0,   234,     0,     0,
     129,   130,     0,     0,   235,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,     0,   119,   120,   121,
     122,   123,     0,   124,   125,   126,   127,   128,     0,     0,
       0,     0,     0,   211,     0,     0,   129,   130,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,     0,
     119,   120,   121,   122,   123,     0,   124,   125,   126,   127,
     128,     0,     0,     0,     0,     0,     0,     0,     0,   129,
     130,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     222,   118,     0,   119,   120,   121,   122,   123,     0,   124,
     125,   126,   127,   128,     0,     0,     0,     0,     0,     0,
       0,     0,   129,   130,   108,   109,   110,   111,   112,   113,
     114,   115,   116,     0,   118,     0,   119,   120,   121,   122,
     123,     0,   124,   125,   126,   127,   128,   108,   109,   110,
     111,   112,   113,     0,   115,     0,   130,   118,     0,   119,
     120,   121,   122,   123,     0,   124,   125,   126,   127,   128,
     108,   109,   110,   111,   112,   113,     0,   115,     0,   130,
     118,     0,   119,   120,   121,     0,   123,     0,   124,   125,
     126,     0,   128,   108,   109,   110,   111,   112,   113,     0,
     115,     0,   130,   118,     0,     0,   120,   121,     0,   123,
       0,     0,   125,   126,     0,   128,   108,   109,   110,   111,
       0,   113,     0,   115,     0,   130,   118,     0,     0,   120,
     121,     0,   123,     0,     0,   125,   126,     0,   128,   108,
     109,     0,   111,     0,     0,     0,   115,     0,   130,   118,
       0,     0,   120,   121,     0,   123,     0,     0,   125,   126,
       0,   128,   108,   109,     0,   111,     0,   108,   109,   115,
     111,   130,   118,     0,   115,     0,   121,   118,   123,     0,
     108,   109,   126,   123,   128,     0,     0,   115,     0,   128,
     118,     0,     0,     0,   130,     0,   123,     0,     0,   130,
       0,     0,   128,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   130
};

static const yytype_int16 yycheck[] =
{
       9,    34,     8,    36,    34,    28,    15,    14,    56,     0,
      19,    59,    21,    22,    62,    63,    49,    37,    38,    67,
      37,    58,    41,    71,    72,    56,    57,    30,   103,    46,
      67,    58,    67,    58,    37,    52,    68,    37,    65,    69,
      63,    60,    67,    46,    64,    62,    50,    58,    30,    52,
      50,    64,   100,   101,   102,    56,    67,    64,    64,    62,
      39,    65,    58,    56,    97,   140,    64,    54,   143,    56,
      66,     8,   105,   106,    64,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   103,
      64,   149,     9,   103,    56,   180,    64,    89,     0,    25,
      92,    27,    28,    29,    67,    56,    56,    65,    65,    58,
      11,    12,   104,    58,    15,    16,    17,    18,    19,   133,
       4,    22,    54,    24,    25,   165,   140,     5,    65,   143,
     140,    64,    37,   143,    65,   154,   221,    66,    25,   158,
      65,   160,   161,   162,    65,    67,    -1,    -1,   104,   104,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    59,   217,
     218,    -1,    30,    31,    32,    33,   180,    68,   211,    37,
     180,   163,    40,    -1,    -1,    43,    44,    -1,    46,   222,
      -1,    49,    50,   241,    52,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    62,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   216,    -1,    -1,    -1,    -1,   221,   227,    -1,
      -1,   221,    -1,    -1,    -1,     0,     1,    -1,    -1,     4,
       5,     6,     7,   237,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    64,
       0,    -1,    67,    68,     4,     5,     6,     7,    -1,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    -1,    -1,
      -1,    -1,    -1,    53,    54,    55,    56,    57,    -1,    59,
      -1,    -1,    62,    63,    64,    -1,    -1,    -1,    68,     4,
       5,     6,     7,    -1,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    53,    54,
      55,    56,    57,    -1,    59,    -1,    -1,    62,    63,    64,
      -1,    -1,    -1,    68,    69,     4,     5,     6,     7,    -1,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    -1,    -1,    -1,    53,    54,    55,    56,    57,    -1,
      59,    -1,    -1,    62,    63,    64,    -1,    -1,    -1,    68,
      69,     4,     5,     6,     7,    -1,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,
      53,    54,    55,    56,    57,    -1,    59,    -1,    -1,    62,
      63,    64,    65,    -1,    -1,    68,     4,     5,     6,     7,
      -1,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,    54,    55,    56,    57,
      -1,    59,    -1,    -1,    62,    63,    64,    -1,    -1,    -1,
      68,     4,     5,     6,     7,    -1,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,
      53,    54,    55,    56,    57,    -1,    59,    -1,    -1,    62,
      63,    64,    -1,    66,     4,     5,     6,     7,    -1,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    -1,    -1,
      -1,    -1,    -1,    53,    54,    55,    56,    57,    -1,    59,
      -1,    -1,    62,    63,    64,    65,     4,     5,     6,     7,
      -1,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    62,    63,    64,     4,     5,     6,
       7,    -1,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    -1,    -1,    -1,    -1,    -1,    53,    54,    55,    56,
      57,    -1,    59,    -1,    -1,    62,    63,    64,     4,     5,
      -1,     7,    -1,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    47,    -1,    -1,    -1,    -1,    -1,    53,    54,    55,
      56,    57,     4,     5,    -1,     7,    -1,    63,    64,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    30,    29,    -1,    -1,
      -1,    -1,    -1,    37,    -1,    -1,    40,    -1,    -1,     6,
      -1,    -1,    46,    -1,    -1,    47,    -1,    -1,    52,    -1,
      -1,    -1,    54,    55,    56,    57,    -1,    -1,    62,    -1,
      -1,    -1,    64,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    -1,    42,    43,    44,    45,    46,
      -1,    48,    49,    50,    51,    52,    -1,     4,     5,    -1,
       7,    -1,    -1,    -1,    61,    62,    13,    14,    15,    -1,
      17,    -1,    19,    20,    21,     4,     5,    -1,     7,    26,
      27,    -1,    29,    -1,    13,    14,    15,    -1,    17,    -1,
      19,    20,    21,    -1,    -1,    -1,    -1,    26,    27,    -1,
      29,    -1,    -1,    -1,    -1,    -1,    -1,    54,    55,    56,
      57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    54,    -1,    56,    57,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    42,    43,    44,    45,    46,    -1,    48,    49,    50,
      51,    52,    -1,    -1,    -1,    -1,    -1,    58,    -1,    -1,
      61,    62,    -1,    -1,    65,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    42,    43,    44,
      45,    46,    -1,    48,    49,    50,    51,    52,    -1,    -1,
      -1,    -1,    -1,    58,    -1,    -1,    61,    62,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    -1,
      42,    43,    44,    45,    46,    -1,    48,    49,    50,    51,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      62,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    -1,    42,    43,    44,    45,    46,    -1,    48,
      49,    50,    51,    52,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    61,    62,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    -1,    40,    -1,    42,    43,    44,    45,
      46,    -1,    48,    49,    50,    51,    52,    30,    31,    32,
      33,    34,    35,    -1,    37,    -1,    62,    40,    -1,    42,
      43,    44,    45,    46,    -1,    48,    49,    50,    51,    52,
      30,    31,    32,    33,    34,    35,    -1,    37,    -1,    62,
      40,    -1,    42,    43,    44,    -1,    46,    -1,    48,    49,
      50,    -1,    52,    30,    31,    32,    33,    34,    35,    -1,
      37,    -1,    62,    40,    -1,    -1,    43,    44,    -1,    46,
      -1,    -1,    49,    50,    -1,    52,    30,    31,    32,    33,
      -1,    35,    -1,    37,    -1,    62,    40,    -1,    -1,    43,
      44,    -1,    46,    -1,    -1,    49,    50,    -1,    52,    30,
      31,    -1,    33,    -1,    -1,    -1,    37,    -1,    62,    40,
      -1,    -1,    43,    44,    -1,    46,    -1,    -1,    49,    50,
      -1,    52,    30,    31,    -1,    33,    -1,    30,    31,    37,
      33,    62,    40,    -1,    37,    -1,    44,    40,    46,    -1,
      30,    31,    50,    46,    52,    -1,    -1,    37,    -1,    52,
      40,    -1,    -1,    -1,    62,    -1,    46,    -1,    -1,    62,
      -1,    -1,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    62
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     4,     5,     6,     7,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    47,    53,
      54,    55,    56,    57,    59,    62,    63,    64,    68,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    67,    14,    81,    68,    64,    56,    81,    56,    81,
      64,    81,    81,    11,    12,    15,    16,    17,    18,    19,
      22,    24,    25,    54,    55,    75,    75,    75,    75,    55,
      57,    72,    54,    57,     6,    76,    90,    59,    76,    79,
      92,    92,    39,    64,     8,    41,    60,     6,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    42,
      43,    44,    45,    46,    48,    49,    50,    51,    52,    61,
      62,    58,    66,    58,    67,    76,    78,    79,    80,    28,
      63,     9,    56,    64,    91,    91,     0,    91,    91,    92,
      78,    67,    91,    11,    12,    15,    16,    17,    18,    19,
      22,    24,    25,    54,    55,    59,    90,    91,    91,    72,
      72,    56,    65,    65,    69,    91,    80,    23,    47,    55,
      64,    72,    73,    74,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      78,    58,    65,    80,    80,    69,    67,     4,     5,    65,
      80,    64,    39,    58,    76,    66,    65,    25,    78,    91,
      91,    65,    80,    76,    58,    65,    81,    67,    65,    67,
      78,    65,    91
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    70,    71,    72,    72,    72,    72,    72,    72,    72,
      72,    72,    72,    72,    72,    72,    72,    72,    72,    73,
      73,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    75,    75,    75,    75,    75,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    77,    77,    77,    78,    78,
      79,    79,    79,    80,    80,    81,    81,    82,    83,    83,
      84,    84,    84,    84,    84,    84,    84,    84,    85,    85,
      86,    86,    87,    87,    87,    87,    87,    87,    87,    88,
      88,    88,    89,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    91,
      91,    92,    92,    93,    93,    93,    93
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     2,     2,     1,     2,     1,     1,     3,
       3,     5,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     2,     1,     3,     1,     0,
       3,     1,     2,     1,     0,     3,     3,     2,     5,     4,
       1,     4,     2,     4,     5,     6,     3,     5,     1,     2,
       1,     1,     2,     3,     1,     1,     1,     1,     2,     2,
       3,     3,     4,     2,     2,     3,     3,     7,     9,     3,
       5,     3,     3,     3,     5,     3,     3,     2,     3,     1,
       2,     1,     2,     1,     0,     1,     1
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
#line 213 "TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=(yyvsp[0].mark);}
#line 1797 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 31:
#line 236 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((((yyvsp[-1].mark).builtin == OPC_SUBTRACT) ? OPC_UNARY_MINUS : OPC_UNARY_PLUS),(yyvsp[0].mark),(yyval.mark));}
#line 1803 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 32:
#line 237 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 1809 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 33:
#line 238 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 1815 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 34:
#line 239 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 1821 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 36:
#line 242 "TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark).rptr=(yyvsp[0].mark).rptr; (yyval.mark).builtin= -2;
					__RUN(tdi_yacc_IMMEDIATE(&(yyval.mark).rptr, TDITHREADSTATIC_VAR));}
#line 1828 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 37:
#line 244 "TdiYacc.y" /* yacc.c:1646  */
    {yyerror(TDITHREADSTATIC_VAR, "lex error"); return YY_ERR;}
#line 1834 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 39:
#line 246 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2(OPC_EQUALS,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1840 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 40:
#line 247 "TdiYacc.y" /* yacc.c:1646  */
    {struct marker tmp;		/*binary operation and assign*/
						_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),tmp);
						_JUST1(OPC_EQUALS_FIRST,tmp,(yyval.mark));}
#line 1848 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 41:
#line 250 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST3(OPC_CONDITIONAL,(yyvsp[-2].mark),(yyvsp[0].mark),(yyvsp[-4].mark),(yyval.mark));}
#line 1854 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 42:
#line 251 "TdiYacc.y" /* yacc.c:1646  */
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
#line 1878 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 43:
#line 270 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1884 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 44:
#line 271 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1890 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 45:
#line 272 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1896 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 46:
#line 273 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1902 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 47:
#line 274 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1908 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 48:
#line 275 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1914 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 49:
#line 276 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1920 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 50:
#line 277 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1926 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 51:
#line 278 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1932 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 52:
#line 279 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1938 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 53:
#line 280 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1944 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 54:
#line 281 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1950 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 55:
#line 282 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1956 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 56:
#line 283 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1962 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 57:
#line 284 "TdiYacc.y" /* yacc.c:1646  */
    {if ((yyval.mark).rptr == 0) (yyval.mark)=(yyvsp[0].mark);
					else if ((yyval.mark).rptr->dtype == DTYPE_FUNCTION
					&&	*(opcode_t *)(yyval.mark).rptr->pointer == OPC_CONCAT
					&&	(yyval.mark).rptr->ndesc < 250)
						{(yyval.mark).rptr->dscptrs[(yyval.mark).rptr->ndesc++]=(mdsdsc_t *)(yyvsp[0].mark).rptr;
						_RESOLVE((yyval.mark));}
					else {_FULL2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
					}
#line 1975 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 58:
#line 292 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1981 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 59:
#line 293 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1987 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 60:
#line 294 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1993 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 61:
#line 295 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 1999 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 62:
#line 296 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2(OPC_MULTIPLY,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2005 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 63:
#line 297 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-1].mark).builtin,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2011 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 64:
#line 298 "TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=_EMPTY_MARKER;}
#line 2017 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 65:
#line 305 "TdiYacc.y" /* yacc.c:1646  */
    {_FULL1(OPC_VECTOR,(yyvsp[0].mark),(yyval.mark));}
#line 2023 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 66:
#line 306 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST0(OPC_VECTOR,(yyval.mark));}
#line 2029 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 67:
#line 307 "TdiYacc.y" /* yacc.c:1646  */
    {if ((yyval.mark).rptr->ndesc >= 250) {
					_RESOLVE((yyvsp[-2].mark));
					_FULL1(OPC_VECTOR,(yyvsp[-2].mark),(yyval.mark));
				}
				(yyval.mark).rptr->dscptrs[(yyval.mark).rptr->ndesc++] = (mdsdsc_t *)(yyvsp[0].mark).rptr;
				}
#line 2040 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 69:
#line 315 "TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=_EMPTY_MARKER;}
#line 2046 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 70:
#line 317 "TdiYacc.y" /* yacc.c:1646  */
    {if (	(yyval.mark).rptr			/*comma is left-to-right weakest*/
			&&	(yyval.mark).builtin != -2
			&&	(yyval.mark).rptr->dtype == DTYPE_FUNCTION
			&&	*(opcode_t *)(yyval.mark).rptr->pointer == OPC_COMMA
			&&	(yyval.mark).rptr->ndesc < 250)
				(yyval.mark).rptr->dscptrs[(yyval.mark).rptr->ndesc++]=(mdsdsc_t *)(yyvsp[0].mark).rptr;
			else _FULL2(OPC_COMMA,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2058 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 72:
#line 325 "TdiYacc.y" /* yacc.c:1646  */
    {yyerror(TDITHREADSTATIC_VAR, "lex error"); return YY_ERR;}
#line 2064 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 73:
#line 327 "TdiYacc.y" /* yacc.c:1646  */
    {if ((yyval.mark).rptr
				&& (yyval.mark).builtin != -2
				&& (yyval.mark).rptr->dtype == DTYPE_FUNCTION
				&& *(opcode_t *)(yyval.mark).rptr->pointer == OPC_COMMA) ;
				else _JUST1(OPC_ABORT,(yyvsp[0].mark),(yyval.mark));}
#line 2074 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 74:
#line 332 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST0(OPC_ABORT,(yyval.mark));}
#line 2080 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 75:
#line 334 "TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=(yyvsp[-1].mark); (yyval.mark).builtin= -2;}
#line 2086 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 76:
#line 335 "TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=(yyvsp[-1].mark); (yyval.mark).builtin= -2;}
#line 2092 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 77:
#line 337 "TdiYacc.y" /* yacc.c:1646  */
    {++TDI_REFZONE.l_rel_path;}
#line 2098 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 78:
#line 339 "TdiYacc.y" /* yacc.c:1646  */
    {_FULL2(OPC_ABORT,(yyvsp[-3].mark),(yyvsp[-1].mark),(yyval.mark)); --TDI_REFZONE.l_rel_path;}
#line 2104 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 79:
#line 340 "TdiYacc.y" /* yacc.c:1646  */
    {_FULL2(OPC_ABORT,(yyvsp[-2].mark),_EMPTY_MARKER,(yyval.mark)); --TDI_REFZONE.l_rel_path;}
#line 2110 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 81:
#line 346 "TdiYacc.y" /* yacc.c:1646  */
    {int j;
					(yyval.mark)=(yyvsp[-1].mark);
					(yyval.mark).rptr->pointer= (uint8_t *)&OpcSubscript;
					for (j=(yyval.mark).rptr->ndesc; --j>=0; )
						(yyval.mark).rptr->dscptrs[j+1]=(yyval.mark).rptr->dscptrs[j];
					(yyval.mark).rptr->dscptrs[0]=(mdsdsc_t *)(yyvsp[-3].mark).rptr;
					(yyval.mark).rptr->ndesc++;
					_RESOLVE((yyval.mark));
				}
#line 2124 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 82:
#line 355 "TdiYacc.y" /* yacc.c:1646  */
    {int j=(yyvsp[0].mark).builtin==OPC_PRE_INC ? OPC_POST_INC :  OPC_POST_DEC;
					_JUST1(j,(yyvsp[-1].mark),(yyval.mark));}
#line 2131 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 83:
#line 357 "TdiYacc.y" /* yacc.c:1646  */
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
#line 2149 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 84:
#line 370 "TdiYacc.y" /* yacc.c:1646  */
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
#line 2165 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 85:
#line 381 "TdiYacc.y" /* yacc.c:1646  */
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
#line 2187 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 86:
#line 398 "TdiYacc.y" /* yacc.c:1646  */
    {int j;	/*USING(expr,[default],[shotid],[expt])*/
					(yyval.mark).rptr->pointer= (uint8_t *)&OpcUsing;
					for (j=0; j < (yyvsp[-1].mark).rptr->ndesc; ++j)
						(yyval.mark).rptr->dscptrs[(yyval.mark).rptr->ndesc++]=(yyvsp[-1].mark).rptr->dscptrs[j];
				}
#line 2197 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 87:
#line 403 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2(OPC_USING,(yyvsp[-3].mark),(yyvsp[-1].mark),(yyval.mark)); --TDI_REFZONE.l_rel_path;}
#line 2203 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 89:
#line 409 "TdiYacc.y" /* yacc.c:1646  */
    {MAKE_S(DTYPE_T, (yyvsp[-1].mark).rptr->length + (yyvsp[0].mark).rptr->length, (yyval.mark).rptr);
					StrConcat((mdsdsc_t *)(yyval.mark).rptr, (mdsdsc_t *)(yyvsp[-1].mark).rptr, (yyvsp[0].mark).rptr MDS_END_ARG);
				}
#line 2211 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 92:
#line 419 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 2217 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 93:
#line 420 "TdiYacc.y" /* yacc.c:1646  */
    {struct marker tmp;			/*OPTIONAL IN/INOUT/OUT*/
					_JUST1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),tmp);
					_JUST1((yyvsp[-2].mark).builtin,tmp,(yyval.mark));}
#line 2225 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 94:
#line 423 "TdiYacc.y" /* yacc.c:1646  */
    {if (*(yyval.mark).rptr->pointer == '$') {
					if((yyval.mark).builtin < 0) (yyval.mark).rptr->dtype=DTYPE_IDENT;
					else if ((TdiRefFunction[(yyval.mark).builtin].token & LEX_M_TOKEN) == ARG)
					{__RUN(tdi_yacc_ARG(&(yyval.mark), TDITHREADSTATIC_VAR));}
					else if ((TdiRefFunction[(yyval.mark).builtin].token & LEX_M_TOKEN) == CONST)
						_JUST0((yyvsp[0].mark).builtin,(yyval.mark));
				} else	if (*(yyval.mark).rptr->pointer == '_')
					(yyval.mark).rptr->dtype=DTYPE_IDENT;
				else if (tdi_lex_path((yyvsp[0].mark).rptr->length, (yyvsp[0].mark).rptr->pointer, &(yyval.mark), TDITHREADSTATIC_VAR) == ERROR)
					{yyerror(TDITHREADSTATIC_VAR, "yacc_path failed"); return YY_ERR;}
				}
#line 2241 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 98:
#line 437 "TdiYacc.y" /* yacc.c:1646  */
    {_RESOLVE((yyval.mark));}
#line 2247 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 99:
#line 444 "TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=(yyvsp[0].mark);}
#line 2253 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 100:
#line 445 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 2259 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 101:
#line 446 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-2].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 2265 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 102:
#line 448 "TdiYacc.y" /* yacc.c:1646  */
    {int j;	(yyval.mark)=(yyvsp[-1].mark);
							(yyval.mark).rptr->pointer= (uint8_t *)&OpcFun;
							for (j=(yyval.mark).rptr->ndesc; --j>=0;)
								(yyval.mark).rptr->dscptrs[j+2]=(yyval.mark).rptr->dscptrs[j];
							(yyval.mark).rptr->dscptrs[0]=(mdsdsc_t *)(yyvsp[-3].mark).rptr;
							(yyval.mark).rptr->ndesc += 2;
							++TDI_REFZONE.l_rel_path;
						}
#line 2278 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 103:
#line 457 "TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark).rptr=(yyvsp[0].mark).rptr; (yyval.mark).builtin= -2;
						__RUN(tdi_yacc_IMMEDIATE(&(yyval.mark).rptr, TDITHREADSTATIC_VAR));}
#line 2285 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 104:
#line 459 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST0((yyvsp[-1].mark).builtin,(yyval.mark));}
#line 2291 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 105:
#line 460 "TdiYacc.y" /* yacc.c:1646  */
    {_FULL2((yyvsp[-2].mark).builtin,(yyvsp[-1].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2297 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 106:
#line 461 "TdiYacc.y" /* yacc.c:1646  */
    {_FULL1((yyvsp[-1].mark).builtin,(yyvsp[0].mark),(yyval.mark));}
#line 2303 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 107:
#line 462 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-6].mark).builtin,(yyvsp[-1].mark),(yyvsp[-4].mark),(yyval.mark));}
#line 2309 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 108:
#line 463 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST4((yyvsp[-8].mark).builtin,(yyvsp[-6].mark),(yyvsp[-4].mark),(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2315 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 109:
#line 464 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST1((yyvsp[-2].mark).builtin,(yyvsp[-1].mark),(yyval.mark));}
#line 2321 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 110:
#line 465 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST3((yyvsp[-4].mark).builtin,(yyvsp[-3].mark),(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2327 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 111:
#line 466 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-2].mark).builtin,(yyvsp[-1].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2333 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 112:
#line 467 "TdiYacc.y" /* yacc.c:1646  */
    {_FULL2(OPC_LABEL,(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2339 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 113:
#line 468 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-2].mark).builtin,(yyvsp[-1].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2345 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 114:
#line 469 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST3((yyvsp[-4].mark).builtin,(yyvsp[-3].mark),(yyvsp[-2].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2351 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 115:
#line 470 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-2].mark).builtin,(yyvsp[-1].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2357 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 116:
#line 471 "TdiYacc.y" /* yacc.c:1646  */
    {_JUST2((yyvsp[-2].mark).builtin,(yyvsp[-1].mark),(yyvsp[0].mark),(yyval.mark));}
#line 2363 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 117:
#line 472 "TdiYacc.y" /* yacc.c:1646  */
    {TDI_REFZONE.l_rel_path--;
						(yyval.mark).rptr->dscptrs[1]=(mdsdsc_t *)(yyvsp[0].mark).rptr;}
#line 2370 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 118:
#line 474 "TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=(yyvsp[-1].mark); _RESOLVE((yyval.mark));}
#line 2376 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 120:
#line 477 "TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=(yyvsp[-1].mark);}
#line 2382 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 122:
#line 479 "TdiYacc.y" /* yacc.c:1646  */
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
#line 2399 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 123:
#line 492 "TdiYacc.y" /* yacc.c:1646  */
    {_RESOLVE((yyval.mark));		/*statements*/
			TDI_REFZONE.a_result=(struct descriptor_d *)(yyval.mark).rptr;
			TDI_REFZONE.l_status=SsSUCCESS;}
#line 2407 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 124:
#line 495 "TdiYacc.y" /* yacc.c:1646  */
    {(yyval.mark)=_EMPTY_MARKER;}
#line 2413 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 125:
#line 496 "TdiYacc.y" /* yacc.c:1646  */
    {yyerror(TDITHREADSTATIC_VAR, "lex error"); return YY_ERR;}
#line 2419 "TdiYacc.c" /* yacc.c:1646  */
    break;

  case 126:
#line 497 "TdiYacc.y" /* yacc.c:1646  */
    {TDI_REFZONE.l_status=TdiSYNTAX; yyerror(TDITHREADSTATIC_VAR, "syntax error"); return YY_ASIS;}
#line 2425 "TdiYacc.c" /* yacc.c:1646  */
    break;


#line 2429 "TdiYacc.c" /* yacc.c:1646  */
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
#line 499 "TdiYacc.y" /* yacc.c:1906  */


#define DEFINE(NAME) const int LEX_##NAME=NAME;
#include "lexdef.h"
#undef DEFINE
