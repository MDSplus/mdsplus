%option prefix="tdi" outfile="tdishr/TdiLex.c" header="tdishr/tdilex.h"
%option stack noyymore nodefault noyywrap noyylineno nodefault
%option nounput noinput noyy_push_state noyy_pop_state noyy_top_state
%option noyy_scan_buffer noyy_scan_bytes noyy_scan_string
%option never-interactive reentrant bison-bridge bison-locations
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

TdiLex

Lexical analysis to parse tokens for TdiYacc.y.
Definition section precedes rule section.
Lex regular expression operators:
\x "x"		as is unless "" or []
[xyz] [x-z]	chars x through z
x?		0 or 1 x
x*		0 or more x
x+		1 or more x
x{4,7}		4 to 7 x
x|y		either x or y
(x)		grouping
x/y		x if y follows
.		any non-newline char

Adjustable table sizes used/default.
%e	/1000	nodes
%p	/2500	positions
%n	/500	6095 transitions
%k	/1000	packed classes
%a	/~2000	packed transitions
%o	/3000	output slots
Limitations:
  Floating requires
    (1) digit before exponent
    (2) either decimal point or exponent, (.E3) looks like path.
    (3) NEED to handle 6..8 and 6...8 and 6...8.9, should use spaces.
    (4) 6..8 is 6 .. 8 and not 6. .8, 6...8 is ambiguous?
    (5) prefix + or - on numbers handled elsewhere, a-6 would be tokens a -6.
  Pathname apostrophe required
    (1) with wildcards (% or *),
    (2) without leading \ . or : (path:member looks like file),
    (3) with son or member starting with number (.7E6 is float not son),
    (4) with up-tree minus requires leading period. .-.over
  Filename double quote required for non-simple names.
    "node::node::device:[--.dir.sub.sub]file.extension;version".
  Floating for exponent but no decimal would fall into integer.

	Author:	Ken Klare, LANL P-4	(c)1989,1990,1991
*/

#include <mdsplus/mdsplus.h>

#include <ctype.h>

#include <mdsdescrip.h>
#include <mdsshr.h>
#include <strroutines.h>
#include <tdishr_messages.h>
#include <treeshr.h>

#include "tdithreadstatic.h"
#include "tdirefzone.h"
#include "tdireffunction.h"
#include "tdiyacc.h"

extern int tdi_hash();
extern int TdiConvert();

#define YYLTYPE		TDITHREADSTATIC_TYPE
#define yylloc_param	TDITHREADSTATIC_VAR
#define YY_NO_UNPUT

#define NEWLINE 10
#define MAX_TOKEN_LEN  200
#define LEX_ARGS YYSTYPE * yylval_param, TDITHREADSTATIC_ARG, yyscan_t yyscanner
#define LEX_VARS yylval_param, TDITHREADSTATIC_VAR, yyscanner
#define LEX_INIT struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;(void)(yyg);(void)TDITHREADSTATIC_VAR;

#define LEX_OK()	{\
 yylval_param->mark.w_ok = (TDI_REFZONE.a_cur = yy_cp) - TDI_REFZONE.a_begin;\
 if (yy_flex_debug) fprintf(stderr,"LEX: %s\n",TDI_REFZONE.a_begin);\
 *yy_cp = yyg->yy_hold_char;\
}
//"

#define LEX_UNBALANCE(token) {\
 TDI_REFZONE.a_cur = yy_cp;\
 yylval_param->mark.w_ok = TDI_REFZONE.l_ok = yy_bp - TDI_REFZONE.a_begin;\
 TDI_REFZONE.l_status = TdiUNBALANCE;\
 if (yy_flex_debug) fprintf(stderr,"UNBALANCED %s\n",token);\
 return LEX_ERROR;\
}
//"

static void upcase(char *const str, int len){
  char *pc, *const ec = str + len;
  for (pc = str; *pc && pc < ec; ++pc)
    *pc = (char)toupper(*pc);
}
static inline int lex_float(LEX_ARGS);
static inline int lex_int(LEX_ARGS);
static inline int lex_ident(LEX_ARGS);
static inline int lex_path(LEX_ARGS);
static inline int lex_text(LEX_ARGS);
static inline int lex_libcall(LEX_ARGS);

#define LEX_FLOAT()		{LEX_OK(); return lex_float(LEX_VARS);}
#define LEX_INT()		{LEX_OK(); return lex_int(LEX_VARS);}
#define LEX_IDENT()		{LEX_OK(); return lex_ident(LEX_VARS);}
#define LEX_PATH()		{LEX_OK(); return lex_path(LEX_VARS);}
#define LEX_TEXT()		{LEX_OK(); return lex_text(LEX_VARS);}
#define LEX_LIBCALL()		{LEX_OK(); return lex_libcall(LEX_VARS);}
#define LEX_OP(token,opc)	{LEX_OK(); yylval_param->mark.rptr = NULL; yylval_param->mark.builtin = opc; return LEX_##token;}
#define LEX_CHAR(c)		{LEX_OK(); yylval_param->mark.rptr = NULL; yylval_param->mark.builtin = -1; return c;}
#define LEX_BALANCE(token,cnt,mod)	{cnt mod;\
 if (yy_flex_debug) fprintf(stderr,"BALANCE %c %d\n",token,cnt);\
 if (cnt<0) LEX_UNBALANCE("([{") else LEX_CHAR(token)}
#define yyterminate()		{\
 if (TDI_REFZONE.a_cur == TDI_REFZONE.a_end) {\
  TDI_REFZONE.a_cur++;\
  if (TDI_BALANCE) LEX_UNBALANCE("([{")\
  if (TDI_REFZONE.a_cur[-1] != ';') {if (yy_flex_debug) fprintf(stderr, "FIXUP ';'\n"); LEX_CHAR(';')}\
 } return YY_NULL;\
}
//"
%}

white	[[:blank:]]
anum	[[:alnum:]_$]
exp	([DdEeFfGgHhSsTtVv][\-+]?[[:digit:]]+)
f3	("."?[[:digit:]]+{exp})
f2	([[:digit:]]+"."[[:digit:]]*{exp})
f1	([[:digit:]]*"."[[:digit:]]+)
f0	([[:digit:]]+".")

int	([[:digit:]][[:alnum:]]*)
float	({f0}|{f1}|{f2}|{f3})
ident	([[:alpha:]$_]{anum}*)
path	(((("\\"({ident}"::")?|[.:])?{ident})|(".-"("."?"-")*))([.:]{ident})*)
libcall	("->"{anum}+(":"|"..")?)
comment ("/*"(\n|[^\*]+|\*+[^/\*])*\*+"/")
sqtext  ("\'"(\n|\\.|[^'\\])*"\'")
dqtext	("\""(\n|\\.|[^"\\])*"\"")
%%

{comment}	LEX_OK();
"/*"		LEX_UNBALANCE("/*")	// */
{sqtext}	LEX_TEXT()
"\'"		LEX_UNBALANCE("\'")
{dqtext}	LEX_TEXT()
"\""		LEX_UNBALANCE("\"")

[ \t\r\n\f]+	LEX_OK()	// white and line break and page break

"<="{white}*"="	LEX_OP(BINEQ,	OPC_LE		)
">="{white}*"="	LEX_OP(BINEQ,	OPC_GE		)
"=="{white}*"="	LEX_OP(BINEQ,	OPC_EQ		)
"!="{white}*"="	LEX_OP(BINEQ,	OPC_NE		)
"<>"{white}*"="	LEX_OP(BINEQ,	OPC_NE		)

"<="{white}*	LEX_OP(LGES,	OPC_LE		)
">="{white}*	LEX_OP(LGES,	OPC_GE		)
"=="{white}*	LEX_OP(LGES,	OPC_EQ		)
"!="{white}*	LEX_OP(LEQS,	OPC_NE		)
"<>"{white}*	LEX_OP(LEQS,	OPC_NE		)

"<<"{white}*"="	LEX_OP(BINEQ,	OPC_LE		)
">>"{white}*"="	LEX_OP(BINEQ,	OPC_GE		)
"||"{white}*"="	LEX_OP(BINEQ,	OPC_OR		)
"&&"{white}*"="	LEX_OP(BINEQ,	OPC_AND		)
"**"{white}*"="	LEX_OP(BINEQ,	OPC_POWER	)
"//"{white}*"="	LEX_OP(BINEQ,	OPC_CONCAT	)

"<<"{white}*	LEX_OP(SHIFT,	OPC_SHIFT_LEFT	)
">>"{white}*	LEX_OP(SHIFT,	OPC_SHIFT_RIGHT	)
"||"{white}*	LEX_OP(LORS,	OPC_OR		)
"&&"{white}*	LEX_OP(LANDS,	OPC_AND		)
"**"{white}*	LEX_OP(POWER,	OPC_POWER	)
"//"{white}*	LEX_OP(CONCAT,	OPC_CONCAT	)

"++"		LEX_OP(INC,	OPC_PRE_INC	)
"--"		LEX_OP(INC,	OPC_PRE_DEC	)
".."		LEX_OP(RANGE,   OPC_DTYPE_RANGE )
":"		LEX_OP(RANGE,	OPC_DTYPE_RANGE	)
"!"		LEX_OP(UNARYS,	OPC_NOT		)
"~"		LEX_OP(UNARYS,	OPC_INOT	)
"@"		LEX_OP(PROMO,	OPC_PROMOTE	)

"&"{white}*"="	LEX_OP(BINEQ,	OPC_IAND	)
"+"{white}*"="	LEX_OP(BINEQ,	OPC_ADD		)
"-"{white}*"="	LEX_OP(BINEQ,	OPC_SUBTRACT	)
"<"{white}+"="	LEX_OP(BINEQ,	OPC_LT		)
">"{white}+"="	LEX_OP(BINEQ,	OPC_GT		)
"^"{white}*"="	LEX_OP(BINEQ,	OPC_POWER	)
"|"{white}*"="	LEX_OP(BINEQ,	OPC_IOR		)

"/"{white}*"="	LEX_OP(BINEQ,	OPC_DIVIDE	)
"%"{white}*"="	LEX_OP(BINEQ,	OPC_MOD		)
"*"{white}*"="	LEX_OP(BINEQ,	OPC_MULTIPLY	)

"&"{white}*	LEX_OP(IAND,	OPC_IAND	)
"+"{white}*	LEX_OP(ADD,	OPC_ADD		)
"-"{white}*	LEX_OP(ADD,	OPC_SUBTRACT	)
"<"{white}*	LEX_OP(LGES,	OPC_LT		)
">"{white}*	LEX_OP(LGES,	OPC_GT		)
"^"{white}*	LEX_OP(POWER,	OPC_POWER	)
"|"{white}*	LEX_OP(IOR,	OPC_IOR		)

"/"{white}*	LEX_OP(MUL,	OPC_DIVIDE	)
"%"{white}*	LEX_OP(MUL,	OPC_MOD		)
"*"{white}*	LEX_CHAR('*')

"("		LEX_BALANCE('(',TDI_BALANCE_R,++)
")"		LEX_BALANCE(')',TDI_BALANCE_R,--)
"["		LEX_BALANCE('[',TDI_BALANCE_S,++)
"]"		LEX_BALANCE(']',TDI_BALANCE_S,--)
"{"		LEX_BALANCE('{',TDI_BALANCE_C,++)
"}"		LEX_BALANCE('}',TDI_BALANCE_C,--)

{float}		LEX_FLOAT()
{int}		LEX_INT()
{ident}		LEX_IDENT()
{path}		LEX_PATH()
{libcall}	LEX_LIBCALL()
.		LEX_CHAR(yytext[0])
%%

/*--------------------------------------------------------
	Convert floating point values with the follow
ing
	syntax to internal representation via descriptors:
	        [sign] decimal [fraction] exponent
	        or [sign] [decimal] fraction [exponent]
	where:  decimal         [+|-] 0-9...
	        fraction        . 0-9...
	        exponent        [E|F|D|G|H|S|T] [+|-] 0-9...
	NEED to size based on exponent range and number of digits.
*/
static const DESCRIPTOR(dfghst_dsc, "DFGHSTVdfghstv");
static const DESCRIPTOR(valid_dsc, "+-.0123456789DEFGHSTV \t");

static int lex_convert_floating(struct descriptor_s *str, struct descriptor_r *out_d) {
  char str_c[64];
  int len = str->length > 63 ? 63 : str->length;
  strncpy(str_c, str->pointer, len);
  str_c[len] = 0;
  if (out_d->length == sizeof(double)) {
    double tmp;
    struct descriptor tmp_d = { sizeof(double), DTYPE_NATIVE_DOUBLE, CLASS_S, 0 };
    tmp_d.pointer = (char *)&tmp;
    tmp = strtod(str_c,NULL);
    return TdiConvert(&tmp_d, out_d);
  } else {
    float tmp;
    struct descriptor tmp_d = { sizeof(float), DTYPE_NATIVE_FLOAT, CLASS_S, 0 };
    tmp_d.pointer = (char *)&tmp;
    sscanf(str_c, "%g", &tmp);
    return TdiConvert(&tmp_d, out_d);
  }
}

static int lex_float(LEX_ARGS) {
  LEX_INIT;
  struct descriptor_s str_dsc = { yyleng, DTYPE_T, CLASS_S, yytext };
  int bad, idx, status, tst, type;
  static const struct {
    length_t length;
    dtype_t  dtype;
  } table[] = {
    {
    4, DTYPE_NATIVE_FLOAT}, {
    8, DTYPE_NATIVE_DOUBLE}, {
    8, DTYPE_D}, {
    8, DTYPE_G}, {
    16, DTYPE_H}, {
    4, DTYPE_F}, {
    4, DTYPE_FS}, {
    8, DTYPE_FT}
  };
  upcase(yytext, yyleng);
	/*******************
	Find final location.
	*******************/
  bad = StrFindFirstNotInSet((struct descriptor *)&str_dsc, (struct descriptor *)&valid_dsc);
  if (bad > 0)
    str_dsc.length = bad - 1;

	/**********************
	Find special exponents.
	**********************/
  idx = StrFindFirstInSet((struct descriptor *)&str_dsc, (struct descriptor *)&dfghst_dsc);
  if (idx) {
    switch (tst = yytext[idx - 1]) {
      case 'D':
      type = 1;
      break;
      case 'V':
      type = 2;
      break;
      case 'G':
      type = 3;
      break;
      case 'H':
      type = 4;
      break;
      case 'F':
      type = 5;
      break;
      case 'S':
      type = 6;
      break;
      case 'T':
      type = 7;
      break;
    default:
      type = 0;
      break;
    }
    yytext[idx - 1] = 'E';
  } else
    type = 0;

  MAKE_S(table[type].dtype, table[type].length, yylval_param->mark.rptr);

  status = lex_convert_floating(&str_dsc, yylval_param->mark.rptr);
  if (STATUS_OK && bad > 0 && yytext[bad - 1] != '\0')
    status = TdiEXTRANEOUS;

  yylval_param->mark.builtin = -1;
  if (STATUS_OK)
    return LEX_VALUE;
  TDI_REFZONE.l_status = status;
  return LEX_ERROR;
}

/*--------------------------------------------------------
	Convert Lex input to identifier name or builtin.
	Clobbers string with upcase. IDENT token returns name.
	Note, Lex strings are NUL terminated.
*/
static inline int lex_ident(LEX_ARGS) {
  LEX_INIT;
  int j, token;
  yylval_param->mark.builtin = -1;
  MAKE_S(DTYPE_T, yyleng, yylval_param->mark.rptr);
  memcpy(yylval_param->mark.rptr->pointer, yytext, yylval_param->mark.rptr->length);

	/*****************************
	$ marks next compile argument.
	$nnn marks nnn-th argument.
	*****************************/
  if (yytext[0] == '$') {
    for (j = yyleng; --j > 0;)
      if (yytext[j] < '0' || yytext[j] > '9')
	break;
    if (j == 0) {
      yylval_param->mark.builtin = OPC_$;
      return LEX_IDENT;
    }
  } else if (yytext[0] == '_')
    return LEX_VBL;

	/**********************
	Search of initial list.
	**********************/
  j = tdi_hash(yyleng, yytext);
  if (j < 0) {
    if (yytext[0] == '$')
      return LEX_VBL;
    return LEX_IDENT;
  }

	/**********************************************
	Note difference of pointers is divided by step.
	Standard function gives number. Token if named.
	**********************************************/
  yylval_param->mark.builtin = (short)j;
  token = TdiRefFunction[j].token;
  if ((token & LEX_K_NAMED) != 0) {
    token = token & LEX_M_TOKEN;
    /*
    if (token == IN
	|| token == LAND
	|| token == LEQ
	|| token == LEQV || token == LGE || token == LOR || token == MUL)
      check white*"=";
    */
    return token;
  }
  return LEX_IDENT;
}

/*--------------------------------------------------------
	Convert integer values with the following syntax
	to internal representation via descriptors:
	        [space]...[sign][radix][digit]...[type]
	where:  sign    +       ignored
	                -       negative
	        radix   0B b    binary          digit   0-1
	(digit          0O o    octal                   0-7
	required)       0D d    decimal(float)          0-9
	                0X x    hexadecimal             0-9A-Fa-f
	        type    SB b    signed byte
	                SW w    signed word
	                SL l    signed long
	                SQ q    signed quadword
	                SO o    signed octaword
	                UB bu   unsigned byte
	                UW wu   unsigned word
	                UL lu   unsigned long
	                UQ qu   unsigned quadword
	                UO ou   unsigned octaword
	CAUTION must use unsigned char to avoid sign extend in hex.
	WARNING without following digit B and O radix become byte and octaword.
	WARNING hexadecimal strings must use SB or UB for bytes.

	Limitations:
	Depends on contiguous character set, 0-9 A-F a-f.
	Depends on right-to-left byte assignment.
	Depends on ones- or twos-complement.
	NEED size based on range, thus no overflow.
*/
#define len1 8			/*length of a word in bits */
#define num1 16			/*number of words to accumulate, octaword */

static int lex_int(LEX_ARGS) {
  LEX_INIT;
  const struct {
   length_t length;
   dtype_t  udtype, sdtype;
  } table[] = {
   { 1, DTYPE_BU, DTYPE_B},
   { 2, DTYPE_WU, DTYPE_W},
   { 4, DTYPE_LU, DTYPE_L},
   { 8, DTYPE_QU, DTYPE_Q},
   {16, DTYPE_OU, DTYPE_O},
  };
  INIT_STATUS;
  unsigned char sign;
  char *now = yytext, *end = &yytext[yyleng];
  unsigned char *qptr, qq[num1], qtst;
  int carry = 0, radix;
  int length, is_signed, tst, type;

	/******************************
	Remove leading blanks and tabs.
	******************************/
  while (now < end && (*now == ' ' || *now == '\t'))
    ++now;
  upcase(now, yyleng);

	/*********
	Save sign.
	*********/
  if (now >= end)
    sign = '+';
  else if ((sign = *now) == '-' || sign == '+')
    ++now;

	/***************************************************
	Select radix. Must be followed by appropriate digit.
	Leading zero is required in our LEX.
	Watch, 0bu a proper unsigned byte is good and
	0bub a binary with no digits is bad. Reject 0b33.
	***************************************************/
  if (now < end && *now == '0')
    ++now;
  if (now + 1 < end) {
    switch (tst = *now++) {
      case 'B':
      radix = 2;
      break;
      case 'O':
      radix = 8;
      break;
      /*      case 'D' :      radix = 10;     break; */
      case 'X':
      radix = 16;
      break;
    default:
      --now;
      radix = 0;
      break;
    }
    if ((carry = radix) == 0) {
      radix = 10;
    } else {
      if ((tst = *now) >= '0' && tst <= '9')
	carry = tst - '0';
      else if (tst >= 'A' && tst <= 'F')
	carry = tst - 'A' + 10;
      if (carry >= radix)
	--now;
    }
  } else
    radix = 10;

	/**************
	Convert number.
	**************/
  for (qptr = &qq[0]; qptr < &qq[num1]; ++qptr)
    *qptr = 0;
  for (; now < end; ++now) {
    if ((tst = *now) >= '0' && tst <= '9')
      carry = tst - '0';
    else if (tst >= 'A' && tst <= 'F')
      carry = tst - 'A' + 10;
    else
      break;
    if (carry >= radix) {
      carry = 0;
      break;
    }
    for (qptr = &qq[0]; qptr < &qq[num1]; ++qptr) {
      *qptr = (char)(carry += (int)*qptr * radix);
      carry >>= len1;
    }
  }

	/***********************************************************
	Negative numbers do negation until nonzero, then complement.
	Works for 1 or 2's complement, not sign and magnitude.
	Unsigned overflow: carry | sign, signed: carry | wrong sign.
	***********************************************************/
  if (sign == '-') {
    for (qptr = &qq[0]; qptr < &qq[num1]; ++qptr)
      if ((*qptr = (char)(-*qptr)) != 0)
	break;
    for (; ++qptr < &qq[num1];)
      *qptr = (char)(~*qptr);
  }

	/*******************************
	Find output size and signedness.
	*******************************/
  is_signed = -1;
  type = 2;
  if (now < end)
    switch (tst = *now++) {
      case 'U':
      is_signed = 0;
      break;
      case 'S':
      is_signed = 1;
      break;
      case 'B':
      type = 0;
      break;
      case 'W':
      type = 1;
      break;
      case 'L':
      type = 2;
      break;
      case 'Q':
      type = 3;
      break;
      case 'O':
      type = 4;
      break;
    default:
      --now;
      break;
    }

  if (now >= end) {
  } else if (is_signed < 0)
    switch (tst = *now++) {
      case 'U':
      is_signed = 0;
      break;
      case 'S':
      is_signed = 1;
      break;
    default:
      --now;
      break;
  } else
    switch (tst = *now++) {
      case 'B':
      type = 0;
      break;
      case 'W':
      type = 1;
      break;
      case 'L':
      type = 2;
      break;
      case 'Q':
      type = 3;
      break;
      case 'O':
      type = 4;
      break;
    default:
      --now;
      break;
    }

	/*******************************
	Remove trailing blanks and tabs.
	*******************************/
  while (now < end && (*now == ' ' || *now == '\t'))
    ++now;

  length = table[type].length;
  MAKE_S((unsigned char)(is_signed ? table[type].sdtype : table[type].udtype),
	 (unsigned short)length, yylval_param->mark.rptr);
  yylval_param->mark.builtin = -1;
  memcpy(yylval_param->mark.rptr->pointer, (char *)qq, length);

	/*************************
	Check the high order bits.
	*************************/
  if (now < end && *now != '\0')
    status = TdiEXTRANEOUS;

  qtst = (unsigned char)((is_signed && (char)qq[length - 1] < 0) ? -1 : 0);
  if (carry != 0 || (is_signed && ((qtst != 0) ^ (sign == '-'))))
    status = TdiTOO_BIG;
  else
    for (qptr = &qq[length]; qptr < &qq[num1]; ++qptr)
      if (*qptr != qtst)
	status = TdiTOO_BIG;

  if (STATUS_OK) {
#ifdef WORDS_BIGENDIAN
    int i;
    char *ptr = yylval_param->mark.rptr->pointer;
    for (i = 0; i < length / 2; i++) {
      char sav = ptr[i];
      ptr[i] = ptr[length - i - 1];
      ptr[length - i - 1] = sav;
    }
#endif
    return LEX_VALUE;
  }
  TDI_REFZONE.l_status = status;
  return LEX_ERROR;
}

/*--------------------------------------------------------
	Convert Lex input to NID or absolute PATH.
*/ // used in TdiYacc.c
int tdi_lex_path(int len, char* str, struct marker *mark_ptr, TDITHREADSTATIC_ARG) {
  int nid, token = LEX_VALUE;
  char *str_l;
  str_l = strncpy((char *)malloc(len + 1), str, len);
  str_l[len] = 0;
  upcase(str_l, len);
  mark_ptr->builtin = -1;
  if (TDI_REFZONE.l_rel_path) {
    MAKE_S(DTYPE_PATH, (unsigned short)len, mark_ptr->rptr);
    memcpy((char *)mark_ptr->rptr->pointer, str, len);
  } else if (IS_OK(TreeFindNode((char *)str_l, &nid))) {
    MAKE_S(DTYPE_NID, (unsigned short)sizeof(nid), mark_ptr->rptr);
    *(int *)mark_ptr->rptr->pointer = nid;
  } else {
    struct descriptor_d abs_dsc = { 0, DTYPE_T, CLASS_D, 0 };
    char *apath = TreeAbsPath((char *)str_l);
    if (apath != NULL) {
      unsigned short alen = (unsigned short)strlen(apath);
      StrCopyR((struct descriptor *)&abs_dsc, &alen, apath);
      TreeFree(apath);
      MAKE_S(DTYPE_PATH, abs_dsc.length, mark_ptr->rptr);
      memcpy((char *)mark_ptr->rptr->pointer, abs_dsc.pointer, abs_dsc.length);
      StrFree1Dx(&abs_dsc);
    } else {
      TDI_REFZONE.l_status = TreeNOT_OPEN;
      token = LEX_ERROR;
    }
  }
  free(str_l);
  return token;
}
static inline int lex_path(LEX_ARGS) {
  LEX_INIT;
  return tdi_lex_path(yyleng,yytext,&yylval_param->mark, TDITHREADSTATIC_VAR);
}

/*--------------------------------------------------------
	C-style text in matching quotes. Strict: must end in quote. Continuation: \ before newline.
	Limitation: Text is ASCII dependent in quotes.
	Limitation: No wide characters. L'\xabc'
	Code all ANSI C escapes.
	On \ followed by non-standard we remove \.
	NEED overflow check on octal and hex. Watch sign extend of char.
*/

static inline int lex_text(LEX_ARGS) {
  LEX_INIT;
  if (yytext[yyleng-1] != yytext[0]) {
    TDI_REFZONE.l_status = TdiUNBALANCE;
    return LEX_ERROR;
  }
  MAKE_S(DTYPE_T, yyleng-2, yylval_param->mark.rptr);
  int x;
  char c, *sp = yytext+1, *end = yytext+yyleng-1;
  char *dp = (char*)yylval_param->mark.rptr->pointer;
  memcpy(dp, yytext+1, yyleng-2);
  while (sp < end) {
    if (*sp == '\\') {
     if (++sp < end)
     switch (*sp) {
      default:  break;         /*non-standard */
      case '\n':continue;      /*continuation ignores end of line */
      case 'n': sp++;*dp++ = '\n';break;//NL/LF	newline
      case 't': sp++;*dp++ = '\t';break;//HT	horizontal tab
      case 'v': sp++;*dp++ = '\v';break;//VT	vertical tab
      case 'b': sp++;*dp++ = '\b';break;//BS	backspace
      case 'r': sp++;*dp++ = '\r';break;//CR	carriage return
      case 'f': sp++;*dp++ = '\f';break;//FF	formfeed
      case 'a': sp++;*dp++ = '\007';break;//BEL	audible alert
      case '\\':*dp++ = *sp++;break;//		backslash
      case '\?':*dp++ = *sp++;break;//		question mark
      case '\'':*dp++ = *sp++;break;//		single quote
      case '\"':*dp++ = *sp++;break;//		double quote
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
       c = *sp++ - '0';            //octal number, 1-3 digits
       if ((sp < end) && (*sp >= '0') && (*sp <= '7')) {
         c = (c << 3) | (*sp++ - '0');
         if ((sp < end) && *sp >= '0' && *sp <= '7')
	   c = (c << 3) | (*sp++ - '0');
       }
       *dp++ = c;
       break;
      case 'x':
       x = c = 0;                  //hex number, any number of digits
       while (++sp < end && x++<2) {
              if (*sp >= '0' && *sp <= '9')
           c = (c << 4) | (*sp - '0');
         else if (*sp >= 'A' && *sp <= 'F')
           c = (c << 4) | (*sp - 'A' + 10);
         else if (*sp >= 'a' && *sp <= 'f')
           c = (c << 4) | (*sp - 'a' + 10);
         else
           break;
       }
       *dp++ = c;
      }
    } else if (*sp == '\0')
      break;
    else
      *dp++ = *sp++;
  }
  yylval_param->mark.rptr->length = dp - (char*)yylval_param->mark.rptr->pointer;
  return LEX_TEXT;
}

/*--------------------------------------------------------
 Remove arrow and trailing punctation.
 We cannot match just "->" and token for label POINT label RANGE label
 because RANGE will be consumed by {path}
*/
static inline int lex_libcall(LEX_ARGS) {
  LEX_INIT;
  int lenx = yyleng - 2;
  while (yytext[lenx + 1] == '.' || yytext[lenx + 1] == ':')
    --lenx;
  yylval_param->mark.builtin = -1;
  MAKE_S(DTYPE_T, lenx, yylval_param->mark.rptr);
  memcpy((char *)yylval_param->mark.rptr->pointer, &yytext[2], lenx);
  return LEX_POINT;
}

void tdi_push_new_buffer  (mdsdsc_t * text_ptr, TDITHREADSTATIC_ARG) {
	const yyscan_t yyscanner = TDI_SCANNER;
	TDI_BALANCE = 0;
	TDI_REFZONE.l_status = TdiBOMB;
	TDI_REFZONE.l_ok = 0;
        YY_BUFFER_STATE b = TDI_BUFFER = (YY_BUFFER_STATE) tdialloc(sizeof( struct yy_buffer_state ), yyscanner);
        if ( ! b )	YY_FATAL_ERROR( "out of dynamic memory in tdi_push_new_buffer(): buffer" );
        char *base = memcpy(tdialloc(text_ptr->length + 2, yyscanner), text_ptr->pointer, text_ptr->length);
        if ( ! base )	YY_FATAL_ERROR( "out of dynamic memory in tdi_push_new_buffer(): base" );
	TDI_REFZONE.a_end = (TDI_REFZONE.a_begin = TDI_REFZONE.a_cur = base) + text_ptr->length;
        TDI_REFZONE.a_end[0] = TDI_REFZONE.a_end[1] = YY_END_OF_BUFFER_CHAR;
        b->yy_buf_size = text_ptr->length;
        b->yy_buf_pos = b->yy_ch_buf = base;
        b->yy_is_our_buffer = 1; // makes tdipop_buffer_state free base
        b->yy_input_file = NULL;
        b->yy_n_chars = b->yy_buf_size;
        b->yy_is_interactive = 0;
        b->yy_at_bol = 1;
        b->yy_fill_buffer = 0;
        b->yy_buffer_status = YY_BUFFER_NEW;
	tdipush_buffer_state(b, yyscanner);
}
