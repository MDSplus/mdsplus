	/*	TdiLex
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
	---------------------------------------
	Adjustable table sizes used/default.
	%e	/1000	nodes
	%p	/2500	positions
	%n	/500	6095 transitions
	%k	/1000	packed classes
	%a	/~2000	packed transitions
	%o	/3000	output slots

	Ken Klare, LANL P-4	(c)1989,1990,1991
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

	Input, nlpos, output, pos, unput, and yylex defined by include file.
	Floating for exponent but no decimal would fall into integer.
	*/
%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include "tdirefzone.h"
#include "tdilexdef.h"
#include "tdireffunction.h"
#include <strroutines.h>
#include <tdimessages.h>
#include <treeshr.h>
static char *cvsrev = "@(#)$RCSfile$ $Revision$ $Date$";
#ifdef _WIN32
#pragma warning (disable : 4013 4102 4035) /* LEX produces code with no forward declarations of yylook and yyback. Also has unreferenced label yyfussy. And two int functions: yyoutput and yyunput do not return a value.*/
#endif

#ifdef output
#undef output
#endif
#define output(c)		(c)

#define YYLEX			TdiLex

extern int TdiConvert();
extern struct marker *TdiYylvalPtr;

extern unsigned short Opcdollar, OpcZero,
	OpcAdd,	OpcAnd,	OpcConcat,	OpcDivide,	OpcEq,	
	OpcGe,		OpcGt,		OpcIand,	OpcInot,	OpcIor,
	OpcLe,		OpcLt,		OpcMultiply,	OpcNe,		OpcNot,	OpcOr,	
	OpcPower,	OpcPreDec,	OpcPreInc,	OpcPromote,	OpcDtypeRange,
	OpcShiftLeft,	OpcShiftRight,	OpcSubtract;

extern
	LEX_ERROR,	LEX_IDENT,	LEX_VBL,	LEX_TEXT,	LEX_VALUE,
	LEX_IN,		LEX_LAND,	LEX_LEQ,	LEX_LEQV,	LEX_LGE,	LEX_LOR,	LEX_MUL,
	LEX_ADD,	LEX_CONCAT,	LEX_IAND,	LEX_INC,	LEX_IOR,
	LEX_POINT,	LEX_POWER,	LEX_PROMO,	LEX_RANGE,	LEX_SHIFT,	LEX_BINEQ,
	LEX_LANDS,	LEX_LEQS,	LEX_LGES,	LEX_LORS,	LEX_MULS,	LEX_UNARYS;

#define _MOVC3(a,b,c) memcpy(c,b,a)
static int TdiLexBinEq(int token);


extern void TdiYyReset()
{
/*
  yy_reset();
*/
}

extern int TdiHash(  );

static void	upcase(unsigned char *str, int str_len) {
	unsigned char	*pc;

	for (pc = str; pc < str+str_len; ++pc) if (*pc >= 'a' && *pc <= 'z') *pc += (unsigned char)('A' - 'a');
}
/*--------------------------------------------------------
	Remove comment from the Lex input stream.
	Nested comments allowed. Len is not used.
	Limitation:	Not ANSI C standard use of delimiters.
*/
static int			TdiLexComment(
int			len,
unsigned char		*str,
struct marker		*mark_ptr)
{
char			c, c1;
int			count = 1;

	while (count) {
		if((c = input()) == '/') {
			if ((c1 = input()) == '*') ++count;
			else	unput(c1);
		}
		else if (c == '*') {
			if ((c1 = input()) == '/') --count;
			else	unput(c1);
		}
		else if (c == '\0') {
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
	where:	decimal		[+|-] 0-9...
		fraction	. 0-9...
		exponent	[E|F|D|G|H|S|T] [+|-] 0-9...
	NEED to size based on exponent range and number of digits.
*/
static DESCRIPTOR(dfghst_dsc, "DFGHSTVdfghstv");
static DESCRIPTOR(e_dsc, "E");
static DESCRIPTOR(valid_dsc, "+-.0123456789DEFGHSTV \t");

static int ConvertFloating(struct descriptor_s *str, struct descriptor_r *out_d)
{
  char str_c[64];
  int len = str->length > 63 ? 63 : str->length;
  strncpy(str_c,str->pointer,len);
  str_c[len]=0;
  if (out_d->length == sizeof(double))
  {
    double tmp;
    struct descriptor tmp_d = {sizeof(double),DTYPE_NATIVE_DOUBLE,CLASS_S,0};
    tmp_d.pointer = (char *)&tmp;
    tmp = atof(str_c);
    return TdiConvert(&tmp_d,out_d);
  }
  else
  {
    float tmp;
    struct descriptor tmp_d = {sizeof(float),DTYPE_NATIVE_FLOAT,CLASS_S,0};
    tmp_d.pointer = (char *)&tmp;
    sscanf(str_c,"%g",&tmp);
    return TdiConvert(&tmp_d,out_d);
  }
}

static int			TdiLexFloat(
int		str_len,
unsigned char		*str,
struct marker		*mark_ptr)
{
struct descriptor_s str_dsc = {0,DTYPE_T,CLASS_S,0};
int			bad, idx, status, tst, type;
static struct {
	unsigned short	length;
	unsigned char	dtype;
} table[] = {
		{4,	DTYPE_NATIVE_FLOAT},
                {8,	DTYPE_NATIVE_DOUBLE},
		{8,	DTYPE_D},
		{8,	DTYPE_G},
		{16,	DTYPE_H},
		{4,	DTYPE_F},
		{4,	DTYPE_FS},
		{8,	DTYPE_FT}
	};
        str_dsc.length = str_len;
        str_dsc.pointer = (char *)str;
	upcase(str,str_len);
	/*******************
	Find final location.
	*******************/
	bad = StrFindFirstNotInSet(&str_dsc, &valid_dsc);
	if (bad > 0) str_dsc.length = bad - 1;

	/**********************
	Find special exponents.
	**********************/
	idx = StrFindFirstInSet(&str_dsc, &dfghst_dsc);
	if (idx) { switch (tst = str[idx-1]) {
          case 'D' :	type = 1;	break;
          case 'V' :    type = 2;       break;
	  case 'G' :	type = 3;	break;
	  case 'H' :	type = 4;	break;
	  case 'F' :	type = 5;	break;
	  case 'S' :	type = 6;	break;
	  case 'T' :    type = 7;	break;
	  }
          str[idx-1] = 'E';
        }
	else type = 0;

	MAKE_S(table[type].dtype, table[type].length, mark_ptr->rptr);

	status = ConvertFloating(&str_dsc, mark_ptr->rptr);
	if (status & 1 && bad > 0 && str[bad-1] != '\0') status = TdiEXTRANEOUS;

	mark_ptr->builtin = -1;
	if (status & 1) return(LEX_VALUE);
	TdiRefZone.l_status = status;
	return(LEX_ERROR);
}
/*--------------------------------------------------------
	Convert Lex input to identifier name or builtin.
	Clobbers string with upcase. IDENT token returns name.
	Note, Lex strings are NUL terminated.
*/
static int compare (
char				*s1,
struct TdiFunctionStruct	*s2)
{
	return strcmp(s1, s2->name);
}

static int			TdiLexIdent(
int                     len,
unsigned char		*str,
struct marker		*mark_ptr)
{
struct descriptor_s		sd = {0,DTYPE_T,CLASS_S,0};
int				j, token;
	unsigned char *str_l;

        sd.length = len;
        sd.pointer = (char *)str;
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
		for (j = len; --j > 0;) if (str[j] < '0' || str[j] > '9') break;
		if (j == 0) {
			mark_ptr->builtin = Opcdollar;
			return(LEX_IDENT);
		}
	}
	else if (str[0] == '_') return (LEX_VBL);

	/**********************
	Search of initial list.
	**********************/
	str_l = (unsigned char *)strncpy((char *)malloc(len+1),(char *)str,len);
        str_l[len] = 0;
	j = TdiHash(len, str_l);
	free(str_l);
	if (j < 0) {
		if (str[0] == '$') return(LEX_VBL);
		return(LEX_IDENT);
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
		|| token == LEX_LEQV
		|| token == LEX_LGE
		|| token == LEX_LOR
		|| token == LEX_MUL) return TdiLexBinEq(token);
		return token;
	}
	return(LEX_IDENT);
}
/*--------------------------------------------------------
	Convert integer values with the following syntax
	to internal representation via descriptors:
		[space]...[sign][radix][digit]...[type]
	where:	sign	+	ignored
			-	negative
		radix	0B b	binary		digit	0-1
	(digit		0O o	octal			0-7
	required)	0D d	decimal(float)		0-9
			0X x	hexadecimal		0-9A-Fa-f
		type	SB b	signed byte
			SW w	signed word
			SL l	signed long
			SQ q	signed quadword
			SO o	signed octaword
			UB bu	unsigned byte
			UW wu	unsigned word
			UL lu	unsigned long
			UQ qu	unsigned quadword
			UO ou	unsigned octaword
	CAUTION must use unsigned char to avoid sign extend in hex.
	WARNING without following digit B and O radix become byte and octaword.
	WARNING hexadecimal strings must use SB or UB for bytes.

	Limitations:
	Depends on contiguous character set, 0-9 A-F a-f.
	Depends on right-to-left byte assignment.
	Depends on ones- or twos-complement.
	NEED size based on range, thus no overflow.
*/
#define len1 8		/*length of a word in bits*/
#define num1 16		/*number of words to accumulate, octaword*/

static int			TdiLexInteger(
int		str_len,
unsigned char	*str,
struct marker		*mark_ptr)
{
static struct {
	unsigned short	length;
	unsigned char	udtype, sdtype;
} table[] = {
	{1,	DTYPE_BU,DTYPE_B},
	{2,	DTYPE_WU,DTYPE_W},
	{4,	DTYPE_LU,DTYPE_L},
	{8,	DTYPE_QU,DTYPE_Q},
	{16,	DTYPE_OU,DTYPE_O},
};
unsigned char			sign, *now = str, *end = &str[str_len];
unsigned char		*qptr, qq[num1], qtst;
int			carry = 0, radix;
int			length, is_signed, status = 1, tst, type;

	/******************************
	Remove leading blanks and tabs.
	******************************/
	while (now < end && (*now == ' ' || *now == '\t')) ++now;
	upcase(now,str_len);

	/*********
	Save sign.
	*********/
	if (now >= end) sign = '+';
	else if ((sign = *now) == '-' || sign == '+') ++now;

	/***************************************************
	Select radix. Must be followed by appropriate digit.
	Leading zero is required in our LEX.
	Watch, 0bu a proper unsigned byte is good and
	0bub a binary with no digits is bad. Reject 0b33.
	***************************************************/
	if (now < end && *now == '0') ++now;
	if (now+1 < end) {
		switch (tst = *now++) {
		case 'B' :	radix = 2;	break;
		case 'O' :	radix = 8;	break;
	/*	case 'D' :	radix = 10;	break;*/
		case 'X' :	radix = 16;	break;
		default :	--now;	radix = 0;	break;
		}
		if ((carry = radix) == 0) {radix = 10;}
		else {
			if	((tst = *now)		>= '0' && tst <= '9') carry = tst - '0';
			else if (tst >= 'A' && tst <= 'F') carry = tst - 'A' + 10;
			if (carry >= radix) --now;
		}
	}
	else radix = 10;

	/**************
	Convert number.
	**************/
	for (qptr = &qq[0]; qptr < &qq[num1]; ++qptr) *qptr = 0;
	for ( ; now < end; ++now) {
		if	((tst = *now) >= '0' && tst <= '9') carry = tst - '0';
		else if (tst >= 'A' && tst <= 'F') carry = tst - 'A' + 10;
		else break;
		if (carry >= radix) {carry = 0; break;}
		for (qptr = &qq[0]; qptr < &qq[num1]; ++qptr) {*qptr = (char)(carry += (int)*qptr * radix); carry >>= len1;}
	}

	/***********************************************************
	Negative numbers do negation until nonzero, then complement.
	Works for 1 or 2's complement, not sign and magnitude.
	Unsigned overflow: carry | sign, signed: carry | wrong sign.
	***********************************************************/
	if (sign == '-') {
		for (qptr = &qq[0]; qptr < &qq[num1]; ++qptr) if ((*qptr = (char)(- *qptr)) != 0) break;
		for ( ; ++qptr < &qq[num1]; ) *qptr = (char)(~ *qptr);
	}

	/*******************************
	Find output size and signedness.
	*******************************/
	is_signed = -1;
	type = 2;
	if (now < end) switch (tst = *now++) {
	case 'U' :	is_signed = 0;	break;
	case 'S' :	is_signed = 1;	break;
	case 'B' :	type = 0;	break;
	case 'W' :	type = 1;	break;
	case 'L' :	type = 2;	break;
	case 'Q' :	type = 3;	break;
	case 'O' :	type = 4;	break;
	default :	--now;		break;
	}

	if (now >= end) {}
	else if (is_signed < 0) switch (tst = *now++) {
	case 'U' :	is_signed = 0;	break;
	case 'S' :	is_signed = 1;	break;
	default :	--now;		break;
	}
	else switch (tst = *now++) {
	case 'B' :	type = 0;	break;
	case 'W' :	type = 1;	break;
	case 'L' :	type = 2;	break;
	case 'Q' :	type = 3;	break;
	case 'O' :	type = 4;	break;
	default :	--now;		break;
	}

	/*******************************
	Remove trailing blanks and tabs.
	*******************************/
	while (now < end && (*now == ' ' || *now == '\t')) ++now;

	length = table[type].length;
	MAKE_S((unsigned char)(is_signed ? table[type].sdtype : table[type].udtype), (unsigned short)length, mark_ptr->rptr);
	mark_ptr->builtin = -1;
	_MOVC3(length, (char *)qq, (char *)mark_ptr->rptr->pointer);

	/*************************
	Check the high order bits.
	*************************/
	if (now < end && *now != '\0') status = TdiEXTRANEOUS;

	qtst = (unsigned char)((is_signed && (char)qq[length-1] < 0) ? -1 : 0);
	if (carry != 0 || (is_signed && ((qtst != 0) ^ (sign == '-')))) status = TdiTOO_BIG;
	else for (qptr = &qq[length]; qptr < &qq[num1]; ++qptr) if (*qptr != qtst) status = TdiTOO_BIG;

	if (status & 1)
        {
#ifdef WORDS_BIGENDIAN
          int i;
          unsigned char *ptr = mark_ptr->rptr->pointer;
          for (i=0;i<length/2;i++)
          {
            unsigned char sav = ptr[i];
            ptr[i] = ptr[length - i - 1];
            ptr[length - i - 1] = sav;
          }
#endif
          return(LEX_VALUE);
        }
	TdiRefZone.l_status = status;
	return(LEX_ERROR);
}
/*--------------------------------------------------------
	Convert Lex input to NID or absolute PATH.
*/
int			TdiLexPath(
int			len,
unsigned char		*str,
struct marker		*mark_ptr)
{
	int nid, token = LEX_VALUE;
	unsigned char *str_l;
	str_l = (unsigned char *)strncpy((char *)malloc(len+1),(char *)str,len);
        str_l[len] = 0;
	upcase(str_l,len);
	mark_ptr->builtin = -1;
	if (TdiRefZone.l_rel_path)
	{
		MAKE_S(DTYPE_PATH, (unsigned short)len, mark_ptr->rptr);
		_MOVC3(len, str, (char *)mark_ptr->rptr->pointer);
	}
	else if (TreeFindNode((char *)str_l, &nid) & 1)
	{
		MAKE_S(DTYPE_NID, (unsigned short)sizeof(nid), mark_ptr->rptr);
		*(int *)mark_ptr->rptr->pointer = nid;
	}
	else
	{
		struct descriptor	abs_dsc = {0,DTYPE_T,CLASS_D,0};
		char *apath = TreeAbsPath((char *)str_l);
		if (apath != NULL)
		{
			unsigned short alen = (unsigned short)strlen(apath);
			StrCopyR(&abs_dsc,&alen,apath);
			TreeFree(apath);
			MAKE_S(DTYPE_PATH, abs_dsc.length, mark_ptr->rptr);
			_MOVC3(abs_dsc.length, abs_dsc.pointer, (char *)mark_ptr->rptr->pointer);
                        StrFree1Dx(&abs_dsc);
                }
		else
		{
		  TdiRefZone.l_status = 0;
	          token = LEX_ERROR;
		}
	}
        free(str_l);
	return token;
}
/*--------------------------------------------------------
	Remove arrow and trailing punctation.
*/
static int			TdiLexPoint(
int			len,
unsigned char		*str,
struct marker		*mark_ptr)
{
int		lenx = len - 2;
	while (str[lenx+1] == '.' || str[lenx+1] == ':') --lenx;
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
static int			TdiLexBinEq(
int			token)
{
char		cx;

	while ((cx = input()) == ' ' || cx == '\t') ;
	if (cx == '=') return (LEX_BINEQ);
	unput(cx);
	return token;
}

static int			TdiLexPunct(
int			len,
unsigned char		*str,
struct marker		*mark_ptr)
{
char		c0 = str[0], c1 = input();

	mark_ptr->rptr = 0;

	/********************
	Two graphic operator.
	********************/
	switch (c0) {
	case '!' :	if (c1 == '=') {mark_ptr->builtin = OpcNe;		return TdiLexBinEq	(LEX_LEQS);} break;
	case '&' :	if (c1 == '&') {mark_ptr->builtin = OpcAnd;		return TdiLexBinEq	(LEX_LANDS);} break;
	case '*' :	if (c1 == '*') {mark_ptr->builtin = OpcPower;		return TdiLexBinEq	(LEX_POWER);} break;
	case '+' :	if (c1 == '+') {mark_ptr->builtin = OpcPreInc;	return			(LEX_INC);} break;
	case '-' :	if (c1 == '-') {mark_ptr->builtin = OpcPreDec;	return			(LEX_INC);} break;
/***			if (c1 == '>') {					return			(LEX_POINT);} break;***/
	case '.' :	if (c1 == '.') {mark_ptr->builtin = OpcDtypeRange;	return			(LEX_RANGE);} break;
	case '/' :	if (c1 == '/') {mark_ptr->builtin = OpcConcat;		return TdiLexBinEq	(LEX_CONCAT);} break;
/***                     else if (c1 == '*') return (TdiLexComment(len, str, mark_ptr) == 0) ? input() : 0; break; ***/
/***			if (c1 == '=') {mark_ptr->builtin = OpcNe;		return TdiLexBinEq	(LEX_LEQS);}*/
/***			if (c1 == ')') {					return			']';} break;***/
/***	case '(' :	if (c1 == '/') {					return			'[';} break;***/
	case '<' :	if (c1 == '<') {mark_ptr->builtin = OpcShiftLeft;	return TdiLexBinEq	(LEX_SHIFT);}
			if (c1 == '=') {mark_ptr->builtin = OpcLe;		return TdiLexBinEq	(LEX_LGES);}
			if (c1 == '>') {mark_ptr->builtin = OpcNe;		return TdiLexBinEq	(LEX_LEQS);} break;
	case '=' :	if (c1 == '=') {mark_ptr->builtin = OpcEq;		return TdiLexBinEq	(LEX_LEQS);} break;
	case '>' :	if (c1 == '=') {mark_ptr->builtin = OpcGe;		return TdiLexBinEq	(LEX_LGES);}
			if (c1 == '>') {mark_ptr->builtin = OpcShiftRight;	return TdiLexBinEq	(LEX_SHIFT);} break;
	case '|' :	if (c1 == '|') {mark_ptr->builtin = OpcOr;		return TdiLexBinEq	(LEX_LORS);} break;
	}
	unput(c1);

	/********************
	One graphic operator.
	********************/
	switch (c0) {
	case '!' :	mark_ptr->builtin = OpcNot;		return			(LEX_UNARYS);
/****	case '%' :	mark_ptr->builtin = OpcMod;		return TdiLexBinEq	(LEX_MULS);****/
	case '&' :	mark_ptr->builtin = OpcIand;		return TdiLexBinEq	(LEX_IAND);
	case '*' :	mark_ptr->builtin = OpcMultiply;	return TdiLexBinEq	('*');
	case '+' :	mark_ptr->builtin = OpcAdd;		return TdiLexBinEq	(LEX_ADD);
	case '-' :	mark_ptr->builtin = OpcSubtract;	return TdiLexBinEq	(LEX_ADD);
	case '/' :	mark_ptr->builtin = OpcDivide;		return TdiLexBinEq	(LEX_MULS);
	case ':' :	mark_ptr->builtin = OpcDtypeRange;	return			(LEX_RANGE);
	case '<' :	mark_ptr->builtin = OpcLt;		return TdiLexBinEq	(LEX_LGES);
	case '>' :	mark_ptr->builtin = OpcGt;		return TdiLexBinEq	(LEX_LGES);
	case '@' :	mark_ptr->builtin = OpcPromote;	return			(LEX_PROMO);
	case '^' :	mark_ptr->builtin = OpcPower;		return TdiLexBinEq	(LEX_POWER);
	case '|' :	mark_ptr->builtin = OpcIor;		return TdiLexBinEq	(LEX_IOR);
	case '~' :	mark_ptr->builtin = OpcInot;		return			(LEX_UNARYS);
	}
	mark_ptr->builtin = -1;
	return(c0);
}
/*--------------------------------------------------------
	C-style text in matching quotes. Strict: must end in quote. Continuation: \ before newline.
	Limitation: Text is ASCII dependent in quotes.
	Limitation: No wide characters. L'\xabc'
	Code all ANSI C escapes.
	On \ followed by non-standard we remove \.
	NEED overflow check on octal and hex. Watch sign extend of char.
*/

int			TdiLexQuote(
int			len,
unsigned char		*str,
struct marker		*mark_ptr)
{
char		c, c1, *cptr = TdiRefZone.a_cur;
int		cur = 0, limit;

	while ((c = input()) && c != str[0]) if (c == '\\') input();
	limit = TdiRefZone.a_cur - cptr - 1;
	TdiRefZone.a_cur = cptr;
	MAKE_S(DTYPE_T, limit, mark_ptr->rptr);
	mark_ptr->builtin = -1;
	while (cur <= limit) {
		if ((c = input()) == str[0]) {
			mark_ptr->rptr->length = cur;
			return(LEX_TEXT);
		}
		else if (c == '\\') {
			c = input();
			switch (c) {
			default : break;		/*non-standard*/
			case '\n' : continue;		/*continuation ignores end of line*/
			case 'n' : c = '\n'; break;	/*NL/LF	newline*/
			case 't' : c = '\t'; break;	/*HT	horizontal tab*/
			case 'v' : c = '\v'; break;	/*VT	vertical tab*/
			case 'b' : c = '\b'; break;	/*BS	backspace*/
			case 'r' : c = '\r'; break;	/*CR	carriage return*/
			case 'f' : c = '\f'; break;	/*FF	formfeed*/
			case 'a' : c = '\007'; break;	/*BEL	audible alert*/
			case '\\' : break;		/*backslash*/
			case '\?' : break;		/*question mark*/
			case '\'' : break;		/*single quote*/
			case '\"' : break;		/*double quote*/
			case '0' :
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
				c = c - '0';		/*octal number, 1-3 digits*/
				if ((c1 = input()) >= '0' && c1 <= '7') c = (c << 3) | (c1 - '0'); else unput(c1);
				if ((c1 = input()) >= '0' && c1 <= '7') c = (c << 3) | (c1 - '0'); else unput(c1);
				break;
			case 'x' :
				c = 0;			/*hex number, any number of digits*/
				while (c1 = input()) {
					if	(c1 >= '0' && c1 <= '9') c = (c << 4) | (c1 - '0');
					else if (c1 >= 'A' && c1 <= 'F') c = (c << 4) | (c1 - 'A' + 10);
					else if (c1 >= 'a' && c1 <= 'f') c = (c << 4) | (c1 - 'a' + 10);
					else break;
				}
				unput(c1);
			}
		}
		else if (c == '\0') break;
		mark_ptr->rptr->pointer[cur++] = c;
	}
	TdiRefZone.l_status = TdiUNBALANCE;
	return(LEX_ERROR);
}
%}

%a 2888
%o 4000
anum		[A-Za-z$_0-9]
name		[A-Za-z$_]{anum}*
exponent	[DdEeFfGgHhSsTtVv][\-+]?[0-9]+
integer		[0-9][0-9A-Za-z]*
flt0		[0-9]+"."/[^0-9DdEeFfGgHhSsTtVv.]
flt1		[0-9]*"."[0-9]+
flt2		[0-9]+"."[0-9]*{exponent}
flt3		"."?[0-9]+{exponent}
node		{anum}+"::"
device		[A-Za-z$_]{anum}*":"
dir		"["("-"*|{anum}*)("."{anum}+)*"]"
ext		"."{anum}+
image		{node}*{device}?{dir}?{anum}+{ext}?

path		((("\\"({name}"::")?|[.:])?{name})|(".-"("."?"-")*))([.:]{name})*
point		"->"{anum}+(":"|"..")?

%%

[ \t\r]		{;}

\n	{nlpos();}

{flt0}		{pos();  return	(TdiLexFloat(	yyleng, (unsigned char *)yytext, TdiYylvalPtr));}

({flt1})|({flt2})|({flt3}) {pos(); return	(TdiLexFloat(	yyleng, (unsigned char *)yytext, TdiYylvalPtr));}

{integer}	{pos();  return	(TdiLexInteger(	yyleng, (unsigned char *)yytext, TdiYylvalPtr));}

{name}		{pos();  return	(TdiLexIdent(	yyleng, (unsigned char *)yytext, TdiYylvalPtr));}

{path}		{pos();  return	(TdiLexPath(	yyleng, (unsigned char *)yytext, TdiYylvalPtr));}

[\"\']		{pos();  return	(TdiLexQuote(	yyleng, (unsigned char *)yytext, TdiYylvalPtr));}

{point}		{pos();  return	(TdiLexPoint(	yyleng, (unsigned char *)yytext, TdiYylvalPtr));}

"/*"            {pos(); if      (TdiLexComment(      yyleng, (unsigned char *)yytext, TdiYylvalPtr)) return(LEX_ERROR);}

.		{pos(); return	(TdiLexPunct(	yyleng, (unsigned char *)yytext, TdiYylvalPtr));}

%%

