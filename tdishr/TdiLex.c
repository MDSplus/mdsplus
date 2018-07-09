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
//#define LEXDEBUG
#define tdiyyin  stdin
#define tdiyyout stdout

#include <STATICdef.h>
#include "stdio.h"
#include <treeshr_messages.h>
#define U(x) ((x)&0377)
#define NLSTATE tdiyyprevious=YYNEWLINE
#define BEGIN tdiyybgin = tdiyysvec + 1 +
#define INITIAL 0
#define YYLERR tdiyysvec
#define YYSTATE (tdiyyestate-tdiyysvec-1)
#define YYLMAX 200
#define output(c) putc(c,tdiyyout)
#define input() (((tdiyytchar=tdiyysptr>tdiyysbuf?U(*--tdiyysptr):getc(tdiyyin))==10?(tdiyylineno++,tdiyytchar):tdiyytchar)==EOF?0:tdiyytchar)
#define unput(c) {tdiyytchar= (c);if(tdiyytchar=='\n')tdiyylineno--;*tdiyysptr++=tdiyytchar;}
#define tdiyymore() (tdiyymorfg=1)
#define ECHO fprintf(tdiyyout, "%s",tdiyytext)
#define REJECT { nstr = tdiyyreject(); goto tdiyyfussy;}
int tdiyyleng;
extern unsigned char tdiyytext[];
int tdiyymorfg;
extern unsigned char *tdiyysptr, tdiyysbuf[];
int tdiyytchar;

extern int tdiyylineno;
/* forward declaration for tdiyywork necessary for c++*/
struct tdiyywork;
struct tdiyysvf {
  struct tdiyywork *tdiyystoff;
  struct tdiyysvf *tdiyyother;
  int *tdiyystops;
};
struct tdiyysvf *tdiyyestate;
extern struct tdiyysvf tdiyysvec[], *tdiyybgin;
/*define YY_NOPROTO to disable function prototypes */
#ifndef YY_NOPROTO
#if defined (__STDC__) || defined  (__cplusplus)
/* GNU C always defines __STDC__. Must test if zero */
#if defined (__GNUC__) && !__STDC__
#define YYVOID
#else
#define YYVOID void
#endif /*__GNUC__ */
#else				/* __STDC__ */
#define YYVOID
#endif				/* __STDC__ */
#if defined  (__cplusplus)
extern "C" {
#endif /*__cplusplus_ */
  int tdiyylook(YYVOID);
  int tdiyylex(YYVOID);
  int tdiyywrap(YYVOID);
#if defined  (__cplusplus)
}
#endif /*__cplusplus_ */
#endif				/* YY_NOPROTO */
	/*      TdiLex
	   Lexical analysis to parse tokens for TdiYacc.y.
	   Definition section precedes rule section.
	   Lex regular expression operators:
	   \x "x"               as is unless "" or []
	   [xyz] [x-z]  chars x through z
	   x?           0 or 1 x
	   x*           0 or more x
	   x+           1 or more x
	   x{4,7}               4 to 7 x
	   x|y          either x or y
	   (x)          grouping
	   x/y          x if y follows
	   .            any non-newline char
	   ---------------------------------------
	   Adjustable table sizes used/default.
	   %e   /1000   nodes
	   %p   /2500   positions
	   %n   /500    6095 transitions
	   %k   /1000   packed classes
	   %a   /~2000  packed transitions
	   %o   /3000   output slots
	   Ken Klare, LANL P-4  (c)1989,1990,1991
	   NEED to handle 6..8 and 6...8 and 6...8.9, should use spaces.
	   Limitations:
	   Floating requires (1) digit before exponent
	   (2) either decimal point or exponent, (.E3) looks like path.
	   (3) 6..8 is 6 .. 8 and not 6. .8, 6...8 is ambiguous?
	   (4) prefix + or - on numbers handled elsewhere, a-6 would be tokens a -6.
	   Pathname apostrophe required (1) with wildcards (% or *),
	   (2) without leading \ . or : (path:member looks like file),
	   (3) with son or member starting with number (.7E6 is float not son),
	   (4) with up-tree minus requires leading period. .-.over
	   Filename double quote required for non-simple names.
	   "node::node::device:[--.dir.sub.sub]file.extension;version".
	   Input, nlpos, output, pos, unput, and tdiyylex defined by include file.
	   Floating for exponent but no decimal would fall into integer.
	 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include "tdirefzone.h"
#include "tdilexdef.h"
#include "tdireffunction.h"
#include <strroutines.h>
#include <tdishr_messages.h>
#include <treeshr.h>

#ifdef WIN32
//#pragma warning (disable : 4013 4102 4035)	/* LEX produces code with no forward declarations of tdiyylook and tdiyyback. Also has unreferenced label tdiyyfussy. And two int functions: tdiyyoutput and tdiyyunput do not return a value. */
#endif

#ifdef output
#undef output
#endif
#define output(c)               (c)

#define YYLEX                   TdiLex

extern int TdiConvert();
extern struct marker *TdiYylvalPtr;

extern unsigned short Opcdollar, OpcZero,
    OpcAdd, OpcAnd, OpcConcat, OpcDivide, OpcEq,
    OpcGe, OpcGt, OpcIand, OpcInot, OpcIor,
    OpcLe, OpcLt, OpcMultiply, OpcNe, OpcNot, OpcOr,
    OpcPower, OpcPreDec, OpcPreInc, OpcPromote, OpcDtypeRange,
    OpcShiftLeft, OpcShiftRight, OpcSubtract;

extern int
 LEX_ERROR, LEX_IDENT, LEX_VBL, LEX_TEXT, LEX_VALUE,
 LEX_IN, LEX_LAND, LEX_LEQ, LEX_LEQV, LEX_LGE, LEX_LOR, LEX_MUL,
 LEX_ADD, LEX_CONCAT, LEX_IAND, LEX_INC, LEX_IOR,
 LEX_POINT, LEX_POWER, LEX_PROMO, LEX_RANGE, LEX_SHIFT, LEX_BINEQ,
 LEX_LANDS, LEX_LEQS, LEX_LGES, LEX_LORS, LEX_MULS, LEX_UNARYS;

#define _MOVC3(a,b,c) memcpy(c,b,a)
STATIC_ROUTINE int TdiLexBinEq(int token);

extern void TdiYyReset()
{
/*
  tdiyy_reset();
*/
}

extern int TdiHash();

STATIC_ROUTINE void upcase(unsigned char *str, int str_len)
{
  unsigned char *pc;

  for (pc = str; pc < str + str_len; ++pc)
    if (*pc >= 'a' && *pc <= 'z')
      *pc += (unsigned char)('A' - 'a');
}

/*--------------------------------------------------------
        Remove comment from the Lex input stream.
        Nested comments allowed. Len is not used.
        Limitation:     Not ANSI C standard use of delimiters.
*/
STATIC_ROUTINE int TdiLexComment(int len __attribute__ ((unused)),
                                 unsigned char *str __attribute__ ((unused)),
                                 struct marker *mark_ptr __attribute__ ((unused)))
{
  char c, c1;
  int count = 1;
  GET_TDITHREADSTATIC_P;
  while (count) {
    if ((c = input()) == '/') {
      if ((c1 = input()) == '*')
	++count;
      else
	unput(c1);
    } else if (c == '*') {
      if ((c1 = input()) == '/')
	--count;
      else
	unput(c1);
    } else if (c == '\0') {
      TdiRefZone.l_status = TdiUNBALANCE;
      return 1;
    }
  }
  return 0;
}

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
STATIC_CONSTANT DESCRIPTOR(dfghst_dsc, "DFGHSTVdfghstv");
STATIC_CONSTANT DESCRIPTOR(valid_dsc, "+-.0123456789DEFGHSTV \t");

STATIC_ROUTINE int ConvertFloating(struct descriptor_s *str, struct descriptor_r *out_d)
{
  char str_c[64];
  int len = str->length > 63 ? 63 : str->length;
  strncpy(str_c, str->pointer, len);
  str_c[len] = 0;
  if (out_d->length == sizeof(double)) {
    double tmp;
    struct descriptor tmp_d = { sizeof(double), DTYPE_NATIVE_DOUBLE, CLASS_S, 0 };
    tmp_d.pointer = (char *)&tmp;
    tmp = atof(str_c);
    return TdiConvert(&tmp_d, out_d);
  } else {
    float tmp;
    struct descriptor tmp_d = { sizeof(float), DTYPE_NATIVE_FLOAT, CLASS_S, 0 };
    tmp_d.pointer = (char *)&tmp;
    sscanf(str_c, "%g", &tmp);
    return TdiConvert(&tmp_d, out_d);
  }
}

STATIC_ROUTINE int TdiLexFloat(int str_len, unsigned char *str, struct marker *mark_ptr)
{
  struct descriptor_s str_dsc = { 0, DTYPE_T, CLASS_S, 0 };
  int bad, idx, status, tst, type;
  GET_TDITHREADSTATIC_P;
  STATIC_CONSTANT struct {
    unsigned short length;
    unsigned char dtype;
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
  str_dsc.length = str_len;
  str_dsc.pointer = (char *)str;
  upcase(str, str_len);
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
    switch (tst = str[idx - 1]) {
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
    str[idx - 1] = 'E';
  } else
    type = 0;

  MAKE_S(table[type].dtype, table[type].length, mark_ptr->rptr);

  status = ConvertFloating(&str_dsc, mark_ptr->rptr);
  if (STATUS_OK && bad > 0 && str[bad - 1] != '\0')
    status = TdiEXTRANEOUS;

  mark_ptr->builtin = -1;
  if STATUS_OK
    return (LEX_VALUE);
  TdiRefZone.l_status = status;
  return (LEX_ERROR);
}

/*--------------------------------------------------------
        Convert Lex input to identifier name or builtin.
        Clobbers string with upcase. IDENT token returns name.
        Note, Lex strings are NUL terminated.
*/
STATIC_ROUTINE int TdiLexIdent(int len, unsigned char *str, struct marker *mark_ptr)
{
  int j, token;
  unsigned char *str_l;
  GET_TDITHREADSTATIC_P;
/*
        upcase(str,len);
*/
  mark_ptr->builtin = -1;
  MAKE_S(DTYPE_T, len, mark_ptr->rptr);
  _MOVC3(mark_ptr->rptr->length, str, (char *)mark_ptr->rptr->pointer);

	/*****************************
        $ marks next compile argument.
        $nnn marks nnn-th argument.
        *****************************/
  if (str[0] == '$') {
    for (j = len; --j > 0;)
      if (str[j] < '0' || str[j] > '9')
	break;
    if (j == 0) {
      mark_ptr->builtin = Opcdollar;
      return (LEX_IDENT);
    }
  } else if (str[0] == '_')
    return (LEX_VBL);

	/**********************
        Search of initial list.
        **********************/
  str_l = (unsigned char *)strncpy((char *)malloc(len + 1), (char *)str, len);
  str_l[len] = 0;
  j = TdiHash(len, str_l);
  free(str_l);
  if (j < 0) {
    if (str[0] == '$')
      return (LEX_VBL);
    return (LEX_IDENT);
  }

	/**********************************************
        Note difference of pointers is divided by step.
        Standard function gives number. Token if named.
        **********************************************/
  mark_ptr->builtin = (short)j;
  token = TdiRefFunction[j].token;
  if ((token & LEX_K_NAMED) != 0) {
    token = token & LEX_M_TOKEN;
    if (token == LEX_IN
	|| token == LEX_LAND
	|| token == LEX_LEQ
	|| token == LEX_LEQV || token == LEX_LGE || token == LEX_LOR || token == LEX_MUL)
      return TdiLexBinEq(token);
    return token;
  }
  return (LEX_IDENT);
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

STATIC_ROUTINE int TdiLexInteger(int str_len, unsigned char *str, struct marker *mark_ptr)
{
  GET_TDITHREADSTATIC_P;
  const struct {
    unsigned short length;
    unsigned char udtype, sdtype;
  } table[] = {
    {
    1, DTYPE_BU, DTYPE_B}, {
    2, DTYPE_WU, DTYPE_W}, {
    4, DTYPE_LU, DTYPE_L}, {
    8, DTYPE_QU, DTYPE_Q}, {
  16, DTYPE_OU, DTYPE_O},};
  INIT_STATUS;
  unsigned char sign, *now = str, *end = &str[str_len];
  unsigned char *qptr, qq[num1], qtst;
  int carry = 0, radix;
  int length, is_signed, tst, type;

	/******************************
        Remove leading blanks and tabs.
        ******************************/
  while (now < end && (*now == ' ' || *now == '\t'))
    ++now;
  upcase(now, str_len);

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
	 (unsigned short)length, mark_ptr->rptr);
  mark_ptr->builtin = -1;
  _MOVC3(length, (char *)qq, (char *)mark_ptr->rptr->pointer);

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

  if STATUS_OK {
#ifdef WORDS_BIGENDIAN
    int i;
    unsigned char *ptr = mark_ptr->rptr->pointer;
    for (i = 0; i < length / 2; i++) {
      unsigned char sav = ptr[i];
      ptr[i] = ptr[length - i - 1];
      ptr[length - i - 1] = sav;
    }
#endif
    return (LEX_VALUE);
  }
  TdiRefZone.l_status = status;
  return (LEX_ERROR);
}

/*--------------------------------------------------------
        Convert Lex input to NID or absolute PATH.
*/
int TdiLexPath(int len, unsigned char *str, struct marker *mark_ptr)
{
  GET_TDITHREADSTATIC_P;
  int nid, token = LEX_VALUE;
  unsigned char *str_l;
  str_l = (unsigned char *)strncpy((char *)malloc(len + 1), (char *)str, len);
  str_l[len] = 0;
  upcase(str_l, len);
  mark_ptr->builtin = -1;
  if (TdiRefZone.l_rel_path) {
    MAKE_S(DTYPE_PATH, (unsigned short)len, mark_ptr->rptr);
    _MOVC3(len, str, (char *)mark_ptr->rptr->pointer);
  } else if (TreeFindNode((char *)str_l, &nid) & 1) {
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
      _MOVC3(abs_dsc.length, abs_dsc.pointer, (char *)mark_ptr->rptr->pointer);
      StrFree1Dx(&abs_dsc);
    } else {
      TdiRefZone.l_status = TreeNOT_OPEN;
      token = LEX_ERROR;
    }
  }
  free(str_l);
  return token;
}

/*--------------------------------------------------------
        Remove arrow and trailing punctation.
*/
STATIC_ROUTINE int TdiLexPoint(int len, unsigned char *str, struct marker *mark_ptr)
{
  GET_TDITHREADSTATIC_P;
  int lenx = len - 2;
  while (str[lenx + 1] == '.' || str[lenx + 1] == ':')
    --lenx;
  mark_ptr->builtin = -1;
  MAKE_S(DTYPE_T, lenx, mark_ptr->rptr);
  _MOVC3((unsigned short)lenx, &str[2], (char *)mark_ptr->rptr->pointer);
  return LEX_POINT;
}

/*--------------------------------------------------------
        Recognize some graphic punctuation Lex symbols for YACC.
        Note must be acceptable in written form also: a<=b, a LE b, LE(a,b).
        Binary a<=(b,c) is OK, but unary <=(b,c) should not be.
*/
STATIC_ROUTINE int TdiLexBinEq(int token)
{
  GET_TDITHREADSTATIC_P;
  char cx;

  while ((cx = input()) == ' ' || cx == '\t') ;
  if (cx == '=')
    return (LEX_BINEQ);
  unput(cx);
  return token;
}

STATIC_ROUTINE int TdiLexPunct(int len __attribute__ ((unused)),
			       unsigned char *str,
			       struct marker *mark_ptr)
{
  GET_TDITHREADSTATIC_P;
  char c0 = str[0], c1 = input();

  mark_ptr->rptr = 0;

	/********************
        Two graphic operator.
        ********************/
  switch (c0) {
  case '!':
    if (c1 == '=') {
      mark_ptr->builtin = OpcNe;
      return TdiLexBinEq(LEX_LEQS);
    }
    break;
  case '&':
    if (c1 == '&') {
      mark_ptr->builtin = OpcAnd;
      return TdiLexBinEq(LEX_LANDS);
    }
    break;
  case '*':
    if (c1 == '*') {
      mark_ptr->builtin = OpcPower;
      return TdiLexBinEq(LEX_POWER);
    }
    break;
  case '+':
    if (c1 == '+') {
      mark_ptr->builtin = OpcPreInc;
      return (LEX_INC);
    }
    break;
  case '-':
    if (c1 == '-') {
      mark_ptr->builtin = OpcPreDec;
      return (LEX_INC);
    }
/***
    if (c1 == '>')
      return (LEX_POINT);
***/
    break;
  case '.':
    if (c1 == '.') {
      mark_ptr->builtin = OpcDtypeRange;
      return (LEX_RANGE);
    }
    break;
  case '/':
    if (c1 == '/') {
      mark_ptr->builtin = OpcConcat;
      return TdiLexBinEq(LEX_CONCAT);
    }
    break;
/***                     else if (c1 == '*') return (TdiLexComment(len, str, mark_ptr) == 0) ? input() : 0; break; ***/
/***                    if (c1 == '=') {mark_ptr->builtin = OpcNe;              return TdiLexBinEq      (LEX_LEQS);}*/
/***                    if (c1 == ')') {                                        return                  ']';} break;***/
/***    case '(' :      if (c1 == '/') {                                        return                  '[';} break;***/
  case '<':
    if (c1 == '<') {
      mark_ptr->builtin = OpcShiftLeft;
      return TdiLexBinEq(LEX_SHIFT);
    }
    if (c1 == '=') {
      mark_ptr->builtin = OpcLe;
      return TdiLexBinEq(LEX_LGES);
    }
    if (c1 == '>') {
      mark_ptr->builtin = OpcNe;
      return TdiLexBinEq(LEX_LEQS);
    }
    break;
  case '=':
    if (c1 == '=') {
      mark_ptr->builtin = OpcEq;
      return TdiLexBinEq(LEX_LEQS);
    }
    break;
  case '>':
    if (c1 == '=') {
      mark_ptr->builtin = OpcGe;
      return TdiLexBinEq(LEX_LGES);
    }
    if (c1 == '>') {
      mark_ptr->builtin = OpcShiftRight;
      return TdiLexBinEq(LEX_SHIFT);
    }
    break;
  case '|':
    if (c1 == '|') {
      mark_ptr->builtin = OpcOr;
      return TdiLexBinEq(LEX_LORS);
    }
    break;
  }
  unput(c1);

	/********************
        One graphic operator.
        ********************/
  switch (c0) {
  case '!':
    mark_ptr->builtin = OpcNot;
    return (LEX_UNARYS);
/****   case '%' :      mark_ptr->builtin = OpcMod;             return TdiLexBinEq      (LEX_MULS);****/
  case '&':
    mark_ptr->builtin = OpcIand;
    return TdiLexBinEq(LEX_IAND);
  case '*':
    mark_ptr->builtin = OpcMultiply;
    return TdiLexBinEq('*');
  case '+':
    mark_ptr->builtin = OpcAdd;
    return TdiLexBinEq(LEX_ADD);
  case '-':
    mark_ptr->builtin = OpcSubtract;
    return TdiLexBinEq(LEX_ADD);
  case '/':
    mark_ptr->builtin = OpcDivide;
    return TdiLexBinEq(LEX_MULS);
  case ':':
    mark_ptr->builtin = OpcDtypeRange;
    return (LEX_RANGE);
  case '<':
    mark_ptr->builtin = OpcLt;
    return TdiLexBinEq(LEX_LGES);
  case '>':
    mark_ptr->builtin = OpcGt;
    return TdiLexBinEq(LEX_LGES);
  case '@':
    mark_ptr->builtin = OpcPromote;
    return (LEX_PROMO);
  case '^':
    mark_ptr->builtin = OpcPower;
    return TdiLexBinEq(LEX_POWER);
  case '|':
    mark_ptr->builtin = OpcIor;
    return TdiLexBinEq(LEX_IOR);
  case '~':
    mark_ptr->builtin = OpcInot;
    return (LEX_UNARYS);
  }
  mark_ptr->builtin = -1;
  return (c0);
}

/*--------------------------------------------------------
        C-style text in matching quotes. Strict: must end in quote. Continuation: \ before newline.
        Limitation: Text is ASCII dependent in quotes.
        Limitation: No wide characters. L'\xabc'
        Code all ANSI C escapes.
        On \ followed by non-standard we remove \.
        NEED overflow check on octal and hex. Watch sign extend of char.
*/

int TdiLexQuote(int len __attribute__ ((unused)),
		unsigned char *str,
		struct marker *mark_ptr)
{
  GET_TDITHREADSTATIC_P;
  char c, c1, *cptr = TdiRefZone.a_cur;
  int cur = 0, limit;

  while ((c = input()) && c != str[0])
    if (c == '\\')
      input();
  limit = TdiRefZone.a_cur - cptr - 1;
  TdiRefZone.a_cur = cptr;
  MAKE_S(DTYPE_T, limit, mark_ptr->rptr);
  mark_ptr->builtin = -1;
  while (cur <= limit) {
    if ((c = input()) == str[0]) {
      mark_ptr->rptr->length = cur;
      return (LEX_TEXT);
    } else if (c == '\\') {
      c = input();
      switch (c) {
      default:
	break;			/*non-standard */
      case '\n':
	continue;		/*continuation ignores end of line */
      case 'n':
	c = '\n';
	break;			/*NL/LF newline */
      case 't':
	c = '\t';
	break;			/*HT    horizontal tab */
      case 'v':
	c = '\v';
	break;			/*VT    vertical tab */
      case 'b':
	c = '\b';
	break;			/*BS    backspace */
      case 'r':
	c = '\r';
	break;			/*CR    carriage return */
      case 'f':
	c = '\f';
	break;			/*FF    formfeed */
      case 'a':
	c = '\007';
	break;			/*BEL   audible alert */
      case '\\':
	break;			/*backslash */
      case '\?':
	break;			/*question mark */
      case '\'':
	break;			/*single quote */
      case '\"':
	break;			/*double quote */
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
	c = c - '0';		/*octal number, 1-3 digits */
	if ((c1 = input()) >= '0' && c1 <= '7')
	  c = (c << 3) | (c1 - '0');
	else
	  unput(c1);
	if ((c1 = input()) >= '0' && c1 <= '7')
	  c = (c << 3) | (c1 - '0');
	else
	  unput(c1);
	break;
      case 'x':
	c = 0;			/*hex number, any number of digits */
	while ((c1 = input())) {
	  if (c1 >= '0' && c1 <= '9')
	    c = (c << 4) | (c1 - '0');
	  else if (c1 >= 'A' && c1 <= 'F')
	    c = (c << 4) | (c1 - 'A' + 10);
	  else if (c1 >= 'a' && c1 <= 'f')
	    c = (c << 4) | (c1 - 'a' + 10);
	  else
	    break;
	}
	unput(c1);
      }
    } else if (c == '\0')
      break;
    mark_ptr->rptr->pointer[cur++] = c;
  }
  TdiRefZone.l_status = TdiUNBALANCE;
  return (LEX_ERROR);
}

#define YYNEWLINE 10
int TdiLex() // aka tdiyylex
{
  GET_TDITHREADSTATIC_P;
  int nstr;
  while ((nstr = tdiyylook()) >= 0)
  switch (nstr) {
    case 0:
      if (tdiyywrap())
	return (0);
      break;
    case 1:
      {;
      }
      break;
    case 2:
      {
	nlpos();
      }
      break;
    case 3:
      {
	pos();
	return (TdiLexFloat(tdiyyleng, (unsigned char *)tdiyytext, TdiYylvalPtr));
      }
      break;
    case 4:
      {
	pos();
	return (TdiLexFloat(tdiyyleng, (unsigned char *)tdiyytext, TdiYylvalPtr));
      }
      break;
    case 5:
      {
	pos();
	return (TdiLexInteger(tdiyyleng, (unsigned char *)tdiyytext, TdiYylvalPtr));
      }
      break;
    case 6:
      {
	pos();
	return (TdiLexIdent(tdiyyleng, (unsigned char *)tdiyytext, TdiYylvalPtr));
      }
      break;
    case 7:
      {
	pos();
	return (TdiLexPath(tdiyyleng, (unsigned char *)tdiyytext, TdiYylvalPtr));
      }
      break;
    case 8:
      {
	pos();
	return (TdiLexQuote(tdiyyleng, (unsigned char *)tdiyytext, TdiYylvalPtr));
      }
      break;
    case 9:
      {
	pos();
	return (TdiLexPoint(tdiyyleng, (unsigned char *)tdiyytext, TdiYylvalPtr));
      }
      break;
    case 10:
      {
	pos();
	if (TdiLexComment(tdiyyleng, (unsigned char *)tdiyytext, TdiYylvalPtr))
	  return (LEX_ERROR);
      }
      break;
    case 11:
      {
	pos();
	return (TdiLexPunct(tdiyyleng, (unsigned char *)tdiyytext, TdiYylvalPtr));
      }
      break;
    case -1:
      break;
    default:
      fprintf(tdiyyout, "bad switch tdiyylook %d", nstr);
    }
  return (0);
}

/* end of tdiyylex */

int tdiyyvstop[] = {
  0,

  11,
  0,

  1,
  11,
  0,

  2,
  0,

  8,
  11,
  0,

  6,
  7,
  11,
  0,

  11,
  0,

  11,
  0,

  11,
  0,

  5,
  11,
  0,

  11,
  0,

  11,
  0,

  6,
  7,
  0,

  7,
  0,

  7,
  0,

  4,
  0,

  10,
  0,

  -3,
  0,

  5,
  0,

  5,
  0,

  5,
  0,

  7,
  0,

  7,
  0,

  9,
  0,

  3,
  0,

  4,
  0,

  4,
  5,
  0,

  9,
  0,

  4,
  0,

  4,
  0,
  0
};

#define YYTYPE unsigned int
struct tdiyywork {
  YYTYPE verify, advance;
} tdiyycrank[] = {
  {0, 0}, {0, 0}, {1, 3}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {1, 4}, {1, 5},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {1, 6},
  {0, 0}, {1, 7}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {10, 20},
  {1, 3}, {0, 0}, {1, 8}, {1, 9},
  {1, 10}, {1, 11}, {2, 8}, {28, 18},
  {2, 10}, {27, 36}, {18, 18}, {18, 28},
  {36, 37}, {0, 0}, {0, 0}, {1, 12},
  {35, 41}, {0, 0}, {0, 0}, {8, 16},
  {0, 0}, {27, 37}, {1, 7}, {18, 15},
  {0, 0}, {1, 7}, {7, 14}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {7, 15}, {0, 0}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 15}, {1, 13}, {0, 0}, {0, 0},
  {0, 0}, {2, 13}, {0, 0}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {7, 14}, {0, 0}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {7, 14}, {7, 14}, {7, 14},
  {7, 14}, {9, 17}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {9, 18}, {0, 0},
  {0, 0}, {9, 19}, {9, 19}, {9, 19},
  {9, 19}, {9, 19}, {9, 19}, {9, 19},
  {9, 19}, {9, 19}, {9, 19}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {9, 17}, {0, 0}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {9, 17}, {9, 17}, {9, 17}, {9, 17},
  {11, 21}, {0, 0}, {11, 22}, {11, 22},
  {11, 22}, {11, 22}, {11, 22}, {11, 22},
  {11, 22}, {11, 22}, {11, 22}, {11, 22},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {11, 23},
  {11, 23}, {11, 23}, {11, 24}, {11, 24},
  {11, 24}, {11, 24}, {11, 24}, {11, 23},
  {11, 23}, {11, 23}, {11, 23}, {11, 23},
  {11, 23}, {11, 23}, {11, 23}, {11, 23},
  {11, 23}, {11, 24}, {11, 24}, {11, 23},
  {11, 24}, {11, 23}, {11, 23}, {11, 23},
  {11, 23}, {0, 0}, {0, 0}, {0, 0},
  {12, 17}, {0, 0}, {0, 0}, {11, 23},
  {11, 23}, {11, 23}, {11, 24}, {11, 24},
  {11, 24}, {11, 24}, {11, 24}, {11, 23},
  {11, 23}, {11, 23}, {11, 23}, {11, 23},
  {11, 23}, {11, 23}, {11, 23}, {11, 23},
  {11, 23}, {11, 24}, {11, 24}, {11, 23},
  {11, 24}, {11, 23}, {11, 23}, {11, 23},
  {11, 23}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {0, 0},
  {0, 0}, {0, 0}, {13, 25}, {12, 17},
  {0, 0}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {12, 17},
  {12, 17}, {12, 17}, {12, 17}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {0, 0}, {0, 0}, {0, 0},
  {15, 26}, {13, 25}, {0, 0}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {13, 25}, {13, 25}, {13, 25},
  {13, 25}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {15, 26},
  {0, 0}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {15, 26},
  {15, 26}, {15, 26}, {15, 26}, {16, 27},
  {33, 38}, {33, 38}, {33, 38}, {33, 38},
  {33, 38}, {33, 38}, {33, 38}, {33, 38},
  {33, 38}, {33, 38}, {0, 0}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {16, 27}, {0, 0},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {16, 27}, {16, 27},
  {16, 27}, {16, 27}, {17, 15}, {0, 0},
  {17, 17}, {17, 17}, {17, 17}, {17, 17},
  {17, 17}, {17, 17}, {17, 17}, {17, 17},
  {17, 17}, {17, 17}, {17, 15}, {19, 19},
  {19, 19}, {19, 19}, {19, 19}, {19, 19},
  {19, 19}, {19, 19}, {19, 19}, {19, 19},
  {19, 19}, {34, 34}, {34, 34}, {34, 34},
  {34, 34}, {34, 34}, {34, 34}, {34, 34},
  {34, 34}, {34, 34}, {34, 34}, {19, 29},
  {19, 29}, {19, 29}, {19, 29}, {19, 29},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {21, 30}, {0, 0},
  {0, 0}, {0, 0}, {19, 29}, {19, 29},
  {0, 0}, {19, 29}, {21, 30}, {21, 30},
  {39, 40}, {39, 40}, {39, 40}, {39, 40},
  {39, 40}, {39, 40}, {39, 40}, {39, 40},
  {39, 40}, {39, 40}, {0, 0}, {19, 29},
  {19, 29}, {19, 29}, {19, 29}, {19, 29},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {21, 30},
  {0, 0}, {21, 30}, {19, 29}, {19, 29},
  {0, 0}, {19, 29}, {0, 0}, {0, 0},
  {21, 30}, {0, 0}, {0, 0}, {21, 0},
  {0, 0}, {21, 31}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {21, 30},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {21, 30}, {0, 0},
  {0, 0}, {21, 32}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {23, 23}, {23, 23}, {23, 23},
  {23, 23}, {24, 33}, {0, 0}, {24, 33},
  {0, 0}, {0, 0}, {24, 34}, {24, 34},
  {24, 34}, {24, 34}, {24, 34}, {24, 34},
  {24, 34}, {24, 34}, {24, 34}, {24, 34},
  {25, 15}, {0, 0}, {25, 25}, {25, 25},
  {25, 25}, {25, 25}, {25, 25}, {25, 25},
  {25, 25}, {25, 25}, {25, 25}, {25, 25},
  {25, 35}, {26, 15}, {0, 0}, {26, 26},
  {26, 26}, {26, 26}, {26, 26}, {26, 26},
  {26, 26}, {26, 26}, {26, 26}, {26, 26},
  {26, 26}, {26, 15}, {29, 33}, {0, 0},
  {29, 33}, {0, 0}, {0, 0}, {29, 38},
  {29, 38}, {29, 38}, {29, 38}, {29, 38},
  {29, 38}, {29, 38}, {29, 38}, {29, 38},
  {29, 38}, {31, 31}, {31, 31}, {31, 31},
  {31, 31}, {31, 31}, {31, 31}, {31, 31},
  {31, 31}, {31, 31}, {31, 31}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {31, 32}, {31, 32}, {31, 32},
  {31, 32}, {31, 32}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {31, 32}, {31, 32}, {32, 39}, {31, 32},
  {32, 39}, {0, 0}, {0, 0}, {32, 40},
  {32, 40}, {32, 40}, {32, 40}, {32, 40},
  {32, 40}, {32, 40}, {32, 40}, {32, 40},
  {32, 40}, {31, 32}, {31, 32}, {31, 32},
  {31, 32}, {31, 32}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {0, 0}, {0, 0}, {0, 0}, {0, 0},
  {31, 32}, {31, 32}, {0, 0}, {31, 32},
  {0, 0}};
struct tdiyysvf tdiyysvec[] = {
  {0, 0, 0},
  {tdiyycrank + -1, 0, 0},
  {tdiyycrank + -5, tdiyysvec + 1, 0},
  {tdiyycrank + 0, 0, tdiyyvstop + 1},
  {tdiyycrank + 0, 0, tdiyyvstop + 3},
  {tdiyycrank + 0, 0, tdiyyvstop + 6},
  {tdiyycrank + 0, 0, tdiyyvstop + 8},
  {tdiyycrank + 34, 0, tdiyyvstop + 11},
  {tdiyycrank + 1, 0, tdiyyvstop + 15},
  {tdiyycrank + 121, 0, tdiyyvstop + 17},
  {tdiyycrank + 1, 0, tdiyyvstop + 19},
  {tdiyycrank + 198, 0, tdiyyvstop + 21},
  {tdiyycrank + 256, 0, tdiyyvstop + 24},
  {tdiyycrank + 314, 0, tdiyyvstop + 26},
  {tdiyycrank + 0, tdiyysvec + 7, tdiyyvstop + 28},
  {tdiyycrank + 372, 0, 0},
  {tdiyycrank + 459, 0, 0},
  {tdiyycrank + 536, tdiyysvec + 12, tdiyyvstop + 31},
  {tdiyycrank + 9, 0, tdiyyvstop + 33},
  {tdiyycrank + 547, 0, tdiyyvstop + 35},
  {tdiyycrank + 0, 0, tdiyyvstop + 37},
  {tdiyycrank + -625, 0, tdiyyvstop + 39},
  {tdiyycrank + 0, tdiyysvec + 11, tdiyyvstop + 41},
  {tdiyycrank + 646, 0, tdiyyvstop + 43},
  {tdiyycrank + 726, tdiyysvec + 23, tdiyyvstop + 45},
  {tdiyycrank + 738, tdiyysvec + 13, tdiyyvstop + 47},
  {tdiyycrank + 751, tdiyysvec + 15, tdiyyvstop + 49},
  {tdiyycrank + 7, tdiyysvec + 16, tdiyyvstop + 51},
  {tdiyycrank + 6, tdiyysvec + 15, 0},
  {tdiyycrank + 767, 0, 0},
  {tdiyycrank + 0, 0, tdiyyvstop + 53},
  {tdiyycrank + 777, 0, tdiyyvstop + 55},
  {tdiyycrank + 819, 0, 0},
  {tdiyycrank + 448, 0, 0},
  {tdiyycrank + 557, tdiyysvec + 23, tdiyyvstop + 57},
  {tdiyycrank + 2, tdiyysvec + 15, 0},
  {tdiyycrank + 10, 0, 0},
  {tdiyycrank + 0, 0, tdiyyvstop + 60},
  {tdiyycrank + 0, tdiyysvec + 33, tdiyyvstop + 62},
  {tdiyycrank + 588, 0, 0},
  {tdiyycrank + 0, tdiyysvec + 39, tdiyyvstop + 64},
  {tdiyycrank + 0, tdiyysvec + 12, 0},
  {0, 0, 0}
};

struct tdiyywork *tdiyytop = tdiyycrank + 895;
struct tdiyysvf *tdiyybgin = tdiyysvec + 1;
const unsigned int tdiyymatch[] = {
  00, 01, 01, 01, 01, 01, 01, 01,
  01, 011, 012, 01, 01, 011, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  011, 01, '"', 01, '$', 01, 01, '"',
  01, 01, 01, '+', 01, '+', '.', 01,
  '0', '0', '0', '0', '0', '0', '0', '0',
  '0', '0', ':', 01, 01, 01, 01, 01,
  01, 'A', 'A', 'A', 'D', 'D', 'D', 'D',
  'D', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
  'A', 'A', 'A', 'D', 'D', 'A', 'D', 'A',
  'A', 'A', 'A', 01, 01, 01, 01, '$',
  01, 'A', 'A', 'A', 'D', 'D', 'D', 'D',
  'D', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
  'A', 'A', 'A', 'D', 'D', 'A', 'D', 'A',
  'A', 'A', 'A', 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  01, 01, 01, 01, 01, 01, 01, 01,
  0
};

const unsigned char tdiyyextra[] = {
  0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0
};

/*
 * *****************************************************************
 * *                                                               *
 * *    Copyright (c) Digital Equipment Corporation, 1991, 1999    *
 * *                                                               *
 * *   All Rights Reserved.  Unpublished rights  reserved  under   *
 * *   the copyright laws of the United States.                    *
 * *                                                               *
 * *   The software contained on this media  is  proprietary  to   *
 * *   and  embodies  the  confidential  technology  of  Digital   *
 * *   Equipment Corporation.  Possession, use,  duplication  or   *
 * *   dissemination of the software and media is authorized only  *
 * *   pursuant to a valid written license from Digital Equipment  *
 * *   Corporation.                                                *
 * *                                                               *
 * *   RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure  *
 * *   by the U.S. Government is subject to restrictions  as  set  *
 * *   forth in Subparagraph (c)(1)(ii)  of  DFARS  252.227-7013,  *
 * *   or  in  FAR 52.227-19, as applicable.                       *
 * *                                                               *
 * *****************************************************************
 */
/*
 * HISTORY
 */
/*
 * @(#)$RCSfile$ $Revision$ (DEC) $Date$
 */
/*
 ** C Language Skeleton for lex output - tdiyytext array
 */
int tdiyylineno = 1;
#define YYU(x) x
#define NLSTATE tdiyyprevious=YYNEWLINE
unsigned char tdiyytext[YYLMAX];
struct tdiyysvf *tdiyylstate[YYLMAX], **tdiyylsp, **tdiyyolsp;
unsigned char tdiyysbuf[YYLMAX];
unsigned char *tdiyysptr = tdiyysbuf;
int *tdiyyfnd;
extern struct tdiyysvf *tdiyyestate;
int tdiyyprevious = YYNEWLINE;
/*
 * Define YY_NOPROTO to suppress the prototype declarations
 * GNUC and DECC define __STDC__ differently
 */
#ifdef __GNUC__
#if !__STDC__
#define YY_NOPROTO
#endif				/* __STDC__ */
#elif !defined(__STDC__) &&  !defined (__cplusplus)
#define YY_NOPROTO
#endif				/* __STDC__ */

#ifndef YY_NOPROTO
int tdiyyback(int *, int);
#endif				/* YY_NOPROTO */
int tdiyylook()
{
  GET_TDITHREADSTATIC_P;
  struct tdiyysvf *tdiyystate, **lsp;
  struct tdiyywork *tdiyyt;
  struct tdiyysvf *tdiyyz;
  int tdiyych, tdiyyfirst;
  struct tdiyywork *tdiyyr;
  unsigned char *tdiyylastch;
  /* start off machines */
  tdiyyfirst = 1;
  if (!tdiyymorfg)
    tdiyylastch = tdiyytext;
  else {
    tdiyymorfg = 0;
    tdiyylastch = tdiyytext + tdiyyleng;
  }
  for (;;) {
    lsp = tdiyylstate;
    tdiyyestate = tdiyystate = tdiyybgin;
    if (tdiyyprevious == YYNEWLINE)
      tdiyystate++;
    for (;;) {
#ifdef LEXDEBUG
      fprintf(tdiyyout, "state %d ", (int)(tdiyystate - tdiyysvec - 1));
#endif
      tdiyyt = tdiyystate->tdiyystoff;
      if (tdiyyt == tdiyycrank && !tdiyyfirst) {	/* may not be any transitions */
	tdiyyz = tdiyystate->tdiyyother;
	if (tdiyyz == 0)
	  break;
	if (tdiyyz->tdiyystoff == tdiyycrank)
	  break;
      }
      *tdiyylastch++ = tdiyych = input();
      if (tdiyylastch >= tdiyytext + (YYLMAX - 1)) {
	fprintf(tdiyyout, " ! Maximum token length exceeded\n");
	tdiyytext[YYLMAX - 1] = 0;
	return 0;
      }
      tdiyyfirst = 0;
#define YYOPTIM
#ifdef YYOPTIM
 tryagain:
#endif
#ifdef LEXDEBUG
      fprintf(tdiyyout, "char '%c' (%d)\n",(char)tdiyych,tdiyych);
#endif
      tdiyyr = tdiyyt;
      if (tdiyyt > tdiyycrank) {
	tdiyyt = tdiyyr + tdiyych;
	if (tdiyyt <= tdiyytop && tdiyyt->verify + tdiyysvec == tdiyystate) {
	  if (tdiyyt->advance + tdiyysvec == YYLERR) {	/* error transitions */
	    --tdiyylastch;
	    unput(*tdiyylastch);
	    break;
	  }
	  *lsp++ = tdiyystate = tdiyyt->advance + tdiyysvec;
	  goto contin;
	}
      } else if (tdiyyt < tdiyycrank) {	/* r < tdiyycrank */
	tdiyyt = tdiyyr = tdiyycrank + (tdiyycrank - tdiyyt);
#ifdef LEXDEBUG
	fprintf(tdiyyout, "compressed state\n");
#endif
	tdiyyt = tdiyyt + tdiyych;
	if (tdiyyt <= tdiyytop && tdiyyt->verify + tdiyysvec == tdiyystate) {
	  if (tdiyyt->advance + tdiyysvec == YYLERR) {	/* error transitions */
	    --tdiyylastch;
	    unput(*tdiyylastch);
	    break;
	  }
	  *lsp++ = tdiyystate = tdiyyt->advance + tdiyysvec;
	  goto contin;
	}
	tdiyyt = tdiyyr + YYU(tdiyymatch[tdiyych]);
#ifdef LEXDEBUG
	fprintf(tdiyyout, "try fall back character %d\n",YYU(tdiyymatch[tdiyych]));
#endif
	if (tdiyyt <= tdiyytop && tdiyyt->verify + tdiyysvec == tdiyystate) {
	  if (tdiyyt->advance + tdiyysvec == YYLERR) {	/* error transition */
	    --tdiyylastch;
	    unput(*tdiyylastch);
	    break;
	  }
	  *lsp++ = tdiyystate = tdiyyt->advance + tdiyysvec;
	  goto contin;
	}
      }
      if ((tdiyystate = tdiyystate->tdiyyother)
	  && (tdiyyt = tdiyystate->tdiyystoff) != tdiyycrank) {
#ifdef LEXDEBUG
	fprintf(tdiyyout, "fall back to state %d\n", (int)(tdiyystate - tdiyysvec - 1));
#endif
	goto tryagain;
      } else {
	--tdiyylastch;
	unput(*tdiyylastch);
	break;
      }
 contin:;
#ifdef LEXDEBUG
      fprintf(tdiyyout, "state %ld char '%c' (%d)\n", tdiyystate - tdiyysvec - 1, (char)tdiyych,tdiyych);
#endif
    }
#ifdef LEXDEBUG
    { int pos = *(lsp - 1) - tdiyysvec - 1;
     if (pos<0)
       fprintf(tdiyyout, "stopped EOF\n");
     else
       fprintf(tdiyyout, "stopped at %d with '%c' (%d)\n\n", pos, (char)tdiyych,tdiyych);
    }
#endif
    while (lsp-- > tdiyylstate) {
      // abc..xyz removes 'c' so we need to add the one char that is missing
      // TODO: fix original error before
      if (tdiyylastch[0]=='.' && tdiyylastch[-1]=='.') tdiyylastch++;
      *tdiyylastch-- = 0;
      if (*lsp != 0 && (tdiyyfnd = (*lsp)->tdiyystops) && *tdiyyfnd > 0) {
	tdiyyolsp = lsp;
	if (tdiyyextra[*tdiyyfnd]) {	/* must backup */
	  while (tdiyyback((*lsp)->tdiyystops, -*tdiyyfnd) != 1 && lsp > tdiyylstate) {
	    lsp--;
	    --tdiyylastch;
	    unput(*tdiyylastch);
	  }
	}
	tdiyyprevious = YYU(*tdiyylastch);
	tdiyylsp = lsp;
	tdiyyleng = tdiyylastch - tdiyytext + 1;
	if (tdiyyleng >= (YYLMAX - 1)) {
	  fprintf(tdiyyout, "Maximum token length exceeded\n");
	  tdiyytext[YYLMAX - 1] = 0;
	  return 0;
	}
	tdiyytext[tdiyyleng] = 0;
#ifdef LEXDEBUG
	fprintf(tdiyyout, "match \"%s\" action %d\n",tdiyytext,*tdiyyfnd);
#endif
	return (*tdiyyfnd++);
      }
      --tdiyylastch;
      unput(*tdiyylastch);
    }
    if (tdiyytext[0] == 0 /* && feof(tdiyyin) */ ) {
      tdiyysptr = tdiyysbuf;
      return (0);
    }
    tdiyyprevious = tdiyytext[0] = input();
    //    if (tdiyyprevious > 0)
    //  output(tdiyyprevious);
    tdiyylastch = tdiyytext;
#ifdef LEXDEBUG
    fprintf(tdiyyout, "\n");
#endif
  }
}

int tdiyyback(int *p, int m)
{
  if (p == 0)
    return (0);
  while (*p) {
    if (*p++ == m)
      return (1);
  }
  return (0);
}

	/* the following are only used in the lex library */
int tdiyyinput()
{
  GET_TDITHREADSTATIC_P;
  return (input());
}

void tdiyyoutput()
{
  //  output(c);
}

void tdiyyunput(int c __attribute__ ((unused)))
{
  GET_TDITHREADSTATIC_P;
  unput(c);
}
