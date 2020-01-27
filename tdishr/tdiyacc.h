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
#line 6 "TdiYacc.y" /* yacc.c:1909  */

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
#line 135 "TdiYacc.y" /* yacc.c:1909  */
struct marker mark;

#line 125 "tdiyacc.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (TDITHREADSTATIC_TYPE *TDITHREADSTATIC_VAR);

#endif /* !YY_YY_TDIYACC_H_INCLUDED  */
