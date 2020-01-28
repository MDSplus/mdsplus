/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
#line 7 "TdiYacc.y" /* yacc.c:1909  */

#define YYLTYPE TDITHREADSTATIC_TYPE
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

#line 52 "tdiyacc.h" /* yacc.c:1909  */

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
#line 140 "TdiYacc.y" /* yacc.c:1909  */
struct marker mark;

#line 126 "tdiyacc.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (TDITHREADSTATIC_TYPE *TDITHREADSTATIC_VAR);

#endif /* !YY_YY_TDIYACC_H_INCLUDED  */
