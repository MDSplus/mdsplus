# include "stdio.h"
#if defined(__cplusplus)
   extern "C" {
#endif
#if (defined(__cplusplus) || defined(__STDC__))
     extern int yyreject();
     extern int yywrap();
     extern int yylook();
     extern int yyback(int *, int);
     extern int yyinput();
     extern void yyoutput(int);
     extern void yyunput(int);
     extern int yylex();
     extern int yyless(int);
#ifdef LEXDEBUG
     extern void allprint();
     extern void sprint();
#endif
#if defined(__cplusplus)
   }
#endif
#endif	/* __cplusplus or __STDC__ */
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng;
int yylenguc;
extern unsigned char yytextarr[];
# ifdef YYCHAR_ARRAY
extern char yytext[];
# else
extern unsigned char yytext[];
# endif
int yyposix_point=0;
int yynls16=0;
int yynls_wchar=0;
char *yylocale = "C C C C C C";
int yymorfg;
extern unsigned char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	int yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
	/*	TdiLex.x
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mdsshr.h>
#include <mdsdescrip.h>
#include "tdirefzone.h"
#include "tdilexdef.h"
#include "tdireffunction.h"
#include <strroutines.h>
#include <tdimessages.h>
#include <treeshr.h>

#ifdef WIN32
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
static DESCRIPTOR(dfghst_dsc, "DFGHSTdfghst");
static DESCRIPTOR(e_dsc, "E");
static DESCRIPTOR(valid_dsc, "+-.0123456789DEFGHST \t");

static int ConvertFloating(struct descriptor_s *str, struct descriptor_r *out_d)
{
  char str_c[64];
  int len = str->length > 63 ? 63 : str->length;
  strncpy(str_c,str->pointer,len);
  str_c[len]=0;
  if (out_d->length == sizeof(double))
  {
    double tmp;
    struct descriptor tmp_d = {sizeof(double),DTYPE_DOUBLE,CLASS_S,0};
    tmp_d.pointer = (char *)&tmp;
    tmp = atof(str_c);
    return TdiConvert(&tmp_d,out_d);
  }
  else
  {
    float tmp;
    struct descriptor tmp_d = {sizeof(float),DTYPE_FLOAT,CLASS_S,0};
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
		{4,	DTYPE_FLOAT},
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
	  case 'G' :	type = 2;	break;
	  case 'H' :	type = 3;	break;
	  case 'F' :	type = 4;	break;
	  case 'S' :	type = 5;	break;
	  case 'T' : 	type = 6;	break;
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
int 			len,
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
	str_l = (unsigned char *)strncpy((char *)malloc(len+1),str,len);
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

	if (status & 1) return(LEX_VALUE);
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
  	int nid, status, token = LEX_VALUE;
	unsigned char *str_l;
	str_l = (unsigned char *)strncpy((char *)malloc(len+1),str,len);
        str_l[len] = 0;
	upcase(str_l,len);
	mark_ptr->builtin = -1;
	if (TdiRefZone.l_rel_path)
	{
		MAKE_S(DTYPE_PATH, (unsigned short)len, mark_ptr->rptr);
		_MOVC3(len, str, (char *)mark_ptr->rptr->pointer);
	}
	else if (TreeFindNode(str_l, &nid) & 1)
	{
		MAKE_S(DTYPE_NID, (unsigned short)sizeof(nid), mark_ptr->rptr);
		*(int *)mark_ptr->rptr->pointer = nid;
	}
	else
	{
		struct descriptor	abs_dsc = {0,DTYPE_T,CLASS_D,0};
		char *apath = TreeAbsPath(str_l);
		if (apath != NULL)
		{
			unsigned short alen = (unsigned short)strlen(apath);
			StrCopyR(&abs_dsc,&alen,apath);
			TreeFree(apath);
			if (status & 1)
			{
				MAKE_S(DTYPE_PATH, abs_dsc.length, mark_ptr->rptr);
				_MOVC3(abs_dsc.length, abs_dsc.pointer, (char *)mark_ptr->rptr->pointer);
			}
			else
			{
				TdiRefZone.l_status = status;
				token = LEX_ERROR;
			}
			StrFree1Dx(&abs_dsc);
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
/***			if (c1 == ')') {					return 			']';} break;***/
/***	case '(' :	if (c1 == '/') {					return 			'[';} break;***/
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
# define YYNEWLINE 10
yylex(){
   int nstr; extern int yyprevious;
   while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
   if(yywrap()) return(0); break;
case 1:
	{;}
break;
case 2:
{nlpos();}
break;
case 3:
	{pos();  return	(TdiLexFloat(	yyleng, yytext, TdiYylvalPtr));}
break;
case 4:
{pos(); return	(TdiLexFloat(	yyleng, yytext, TdiYylvalPtr));}
break;
case 5:
{pos();  return	(TdiLexInteger(	yyleng, yytext, TdiYylvalPtr));}
break;
case 6:
	{pos();  return	(TdiLexIdent(	yyleng, yytext, TdiYylvalPtr));}
break;
case 7:
	{pos();  return	(TdiLexPath(	yyleng, yytext, TdiYylvalPtr));}
break;
case 8:
	{pos();  return	(TdiLexQuote(	yyleng, yytext, TdiYylvalPtr));}
break;
case 9:
	{pos();  return	(TdiLexPoint(	yyleng, yytext, TdiYylvalPtr));}
break;
case 10:
           {pos(); if      (TdiLexComment(      yyleng, yytext, TdiYylvalPtr)) return(LEX_ERROR);}
break;
case 11:
	{pos(); return	(TdiLexPunct(	yyleng, yytext, TdiYylvalPtr));}
break;
case -1:
break;
default:
   fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

#ifndef __cplusplus
static void __yy__unused() { main(); }
#endif

int yyvstop[] = {
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
0};
# define YYTYPE unsigned char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
{0,0},	{0,0},	{1,3},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{1,4},	{1,5},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{1,6},	
{0,0},	{1,7},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{10,20},	
{1,3},	{0,0},	{1,8},	{1,9},	
{1,10},	{1,11},	{2,8},	{28,18},	
{2,10},	{27,36},	{18,18},	{18,28},	
{36,37},	{0,0},	{0,0},	{1,12},	
{35,41},	{0,0},	{0,0},	{8,16},	
{0,0},	{27,37},	{1,7},	{18,15},	
{0,0},	{1,7},	{7,14},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{7,15},	{0,0},	{7,14},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,15},	{1,13},	{0,0},	{0,0},	
{0,0},	{2,13},	{0,0},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,14},	{0,0},	{0,0},	{0,0},	
{0,0},	{7,14},	{0,0},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,14},	{7,14},	{7,14},	{7,14},	
{7,14},	{9,17},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{9,18},	{0,0},	
{0,0},	{9,19},	{9,19},	{9,19},	
{9,19},	{9,19},	{9,19},	{9,19},	
{9,19},	{9,19},	{9,19},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{9,17},	{9,17},	
{9,17},	{9,17},	{9,17},	{9,17},	
{9,17},	{9,17},	{9,17},	{9,17},	
{9,17},	{9,17},	{9,17},	{9,17},	
{9,17},	{9,17},	{9,17},	{9,17},	
{9,17},	{9,17},	{9,17},	{9,17},	
{9,17},	{9,17},	{9,17},	{9,17},	
{0,0},	{0,0},	{0,0},	{0,0},	
{9,17},	{0,0},	{9,17},	{9,17},	
{9,17},	{9,17},	{9,17},	{9,17},	
{9,17},	{9,17},	{9,17},	{9,17},	
{9,17},	{9,17},	{9,17},	{9,17},	
{9,17},	{9,17},	{9,17},	{9,17},	
{9,17},	{9,17},	{9,17},	{9,17},	
{9,17},	{9,17},	{9,17},	{9,17},	
{11,21},	{0,0},	{11,22},	{11,22},	
{11,22},	{11,22},	{11,22},	{11,22},	
{11,22},	{11,22},	{11,22},	{11,22},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{11,23},	
{11,23},	{11,23},	{11,24},	{11,24},	
{11,24},	{11,24},	{11,24},	{11,23},	
{11,23},	{11,23},	{11,23},	{11,23},	
{11,23},	{11,23},	{11,23},	{11,23},	
{11,23},	{11,24},	{11,24},	{11,23},	
{11,23},	{11,23},	{11,23},	{11,23},	
{11,23},	{0,0},	{0,0},	{0,0},	
{12,17},	{0,0},	{0,0},	{11,23},	
{11,23},	{11,23},	{11,24},	{11,24},	
{11,24},	{11,24},	{11,24},	{11,23},	
{11,23},	{11,23},	{11,23},	{11,23},	
{11,23},	{11,23},	{11,23},	{11,23},	
{11,23},	{11,24},	{11,24},	{11,23},	
{11,23},	{11,23},	{11,23},	{11,23},	
{11,23},	{12,17},	{12,17},	{12,17},	
{12,17},	{12,17},	{12,17},	{12,17},	
{12,17},	{12,17},	{12,17},	{12,17},	
{12,17},	{12,17},	{12,17},	{12,17},	
{12,17},	{12,17},	{12,17},	{12,17},	
{12,17},	{12,17},	{12,17},	{12,17},	
{12,17},	{12,17},	{12,17},	{0,0},	
{0,0},	{0,0},	{13,25},	{12,17},	
{0,0},	{12,17},	{12,17},	{12,17},	
{12,17},	{12,17},	{12,17},	{12,17},	
{12,17},	{12,17},	{12,17},	{12,17},	
{12,17},	{12,17},	{12,17},	{12,17},	
{12,17},	{12,17},	{12,17},	{12,17},	
{12,17},	{12,17},	{12,17},	{12,17},	
{12,17},	{12,17},	{12,17},	{13,25},	
{13,25},	{13,25},	{13,25},	{13,25},	
{13,25},	{13,25},	{13,25},	{13,25},	
{13,25},	{13,25},	{13,25},	{13,25},	
{13,25},	{13,25},	{13,25},	{13,25},	
{13,25},	{13,25},	{13,25},	{13,25},	
{13,25},	{13,25},	{13,25},	{13,25},	
{13,25},	{0,0},	{0,0},	{0,0},	
{15,26},	{13,25},	{0,0},	{13,25},	
{13,25},	{13,25},	{13,25},	{13,25},	
{13,25},	{13,25},	{13,25},	{13,25},	
{13,25},	{13,25},	{13,25},	{13,25},	
{13,25},	{13,25},	{13,25},	{13,25},	
{13,25},	{13,25},	{13,25},	{13,25},	
{13,25},	{13,25},	{13,25},	{13,25},	
{13,25},	{15,26},	{15,26},	{15,26},	
{15,26},	{15,26},	{15,26},	{15,26},	
{15,26},	{15,26},	{15,26},	{15,26},	
{15,26},	{15,26},	{15,26},	{15,26},	
{15,26},	{15,26},	{15,26},	{15,26},	
{15,26},	{15,26},	{15,26},	{15,26},	
{15,26},	{15,26},	{15,26},	{0,0},	
{0,0},	{0,0},	{0,0},	{15,26},	
{0,0},	{15,26},	{15,26},	{15,26},	
{15,26},	{15,26},	{15,26},	{15,26},	
{15,26},	{15,26},	{15,26},	{15,26},	
{15,26},	{15,26},	{15,26},	{15,26},	
{15,26},	{15,26},	{15,26},	{15,26},	
{15,26},	{15,26},	{15,26},	{15,26},	
{15,26},	{15,26},	{15,26},	{16,27},	
{33,38},	{33,38},	{33,38},	{33,38},	
{33,38},	{33,38},	{33,38},	{33,38},	
{33,38},	{33,38},	{0,0},	{16,27},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{16,27},	{0,0},	{0,0},	
{0,0},	{0,0},	{16,27},	{0,0},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{16,27},	{16,27},	{16,27},	
{16,27},	{16,27},	{17,15},	{0,0},	
{17,17},	{17,17},	{17,17},	{17,17},	
{17,17},	{17,17},	{17,17},	{17,17},	
{17,17},	{17,17},	{17,15},	{19,19},	
{19,19},	{19,19},	{19,19},	{19,19},	
{19,19},	{19,19},	{19,19},	{19,19},	
{19,19},	{34,34},	{34,34},	{34,34},	
{34,34},	{34,34},	{34,34},	{34,34},	
{34,34},	{34,34},	{34,34},	{19,29},	
{19,29},	{19,29},	{19,29},	{19,29},	
{0,0},	{0,0},	{0,0},	{0,0},	
{21,30},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{19,29},	{19,29},	
{21,30},	{21,30},	{25,15},	{0,0},	
{25,25},	{25,25},	{25,25},	{25,25},	
{25,25},	{25,25},	{25,25},	{25,25},	
{25,25},	{25,25},	{25,35},	{19,29},	
{19,29},	{19,29},	{19,29},	{19,29},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{21,30},	{0,0},	{21,30},	
{0,0},	{0,0},	{19,29},	{19,29},	
{0,0},	{0,0},	{21,30},	{0,0},	
{0,0},	{21,0},	{0,0},	{21,31},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{21,30},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{21,30},	{0,0},	{0,0},	{21,32},	
{23,23},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{23,23},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{23,23},	{23,23},	
{23,23},	{23,23},	{23,23},	{24,33},	
{0,0},	{24,33},	{0,0},	{0,0},	
{24,34},	{24,34},	{24,34},	{24,34},	
{24,34},	{24,34},	{24,34},	{24,34},	
{24,34},	{24,34},	{26,15},	{0,0},	
{26,26},	{26,26},	{26,26},	{26,26},	
{26,26},	{26,26},	{26,26},	{26,26},	
{26,26},	{26,26},	{26,15},	{29,33},	
{0,0},	{29,33},	{0,0},	{0,0},	
{29,38},	{29,38},	{29,38},	{29,38},	
{29,38},	{29,38},	{29,38},	{29,38},	
{29,38},	{29,38},	{31,31},	{31,31},	
{31,31},	{31,31},	{31,31},	{31,31},	
{31,31},	{31,31},	{31,31},	{31,31},	
{39,40},	{39,40},	{39,40},	{39,40},	
{39,40},	{39,40},	{39,40},	{39,40},	
{39,40},	{39,40},	{31,32},	{31,32},	
{31,32},	{31,32},	{31,32},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{32,39},	{0,0},	
{32,39},	{31,32},	{31,32},	{32,40},	
{32,40},	{32,40},	{32,40},	{32,40},	
{32,40},	{32,40},	{32,40},	{32,40},	
{32,40},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{31,32},	{31,32},	
{31,32},	{31,32},	{31,32},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{0,0},	{0,0},	{0,0},	
{0,0},	{31,32},	{31,32},	{0,0},	
{0,0}};
struct yysvf yysvec[] = {
{0,	0,	0},
{-1,	0,		0},	
{-5,	yysvec+1,	0},	
{0,	0,		yyvstop+1},
{0,	0,		yyvstop+3},
{0,	0,		yyvstop+6},
{0,	0,		yyvstop+8},
{34,	0,		yyvstop+11},
{1,	0,		yyvstop+15},
{121,	0,		yyvstop+17},
{1,	0,		yyvstop+19},
{198,	0,		yyvstop+21},
{256,	0,		yyvstop+24},
{314,	0,		yyvstop+26},
{0,	yysvec+7,	yyvstop+28},
{372,	0,		0},	
{459,	0,		0},	
{536,	yysvec+12,	yyvstop+31},
{9,	0,		yyvstop+33},
{547,	0,		yyvstop+35},
{0,	0,		yyvstop+37},
{-623,	0,		yyvstop+39},
{0,	yysvec+11,	yyvstop+41},
{644,	0,		yyvstop+43},
{724,	yysvec+23,	yyvstop+45},
{588,	yysvec+13,	yyvstop+47},
{736,	yysvec+15,	yyvstop+49},
{7,	yysvec+16,	yyvstop+51},
{6,	yysvec+15,	0},	
{752,	0,		0},	
{0,	0,		yyvstop+53},
{762,	0,		yyvstop+55},
{799,	0,		0},	
{448,	0,		0},	
{557,	yysvec+23,	yyvstop+57},
{2,	yysvec+15,	0},	
{10,	0,		0},	
{0,	0,		yyvstop+60},
{0,	yysvec+33,	yyvstop+62},
{772,	0,		0},	
{0,	yysvec+39,	yyvstop+64},
{0,	yysvec+12,	0},	
{0,	0,	0}};
struct yywork *yytop = yycrank+878;
struct yysvf *yybgin = yysvec+1;
unsigned char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,011 ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,01  ,'"' ,01  ,'$' ,01  ,01  ,'"' ,
01  ,01  ,01  ,'+' ,01  ,'+' ,'.' ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,':' ,01  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'A' ,'A' ,'D' ,'D' ,'D' ,'D' ,
'D' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'D' ,'D' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,'$' ,
01  ,'A' ,'A' ,'A' ,'D' ,'D' ,'D' ,'D' ,
'D' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'D' ,'D' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
0};
unsigned char yyextra[] = {
0,0,0,1,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/* @(#) A.10.32.03 HP C LANGUAGE TOOL (NCFORM) 960517 $      */
int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
 
#ifdef YYNLS16_WCHAR
unsigned char yytextuc[YYLMAX * sizeof(wchar_t)];
# ifdef YY_PCT_POINT /* for %pointer */
wchar_t yytextarr[YYLMAX];
wchar_t *yytext;
# else               /* %array */
wchar_t yytextarr[1];
wchar_t yytext[YYLMAX];
# endif
#else
unsigned char yytextuc;
# ifdef YY_PCT_POINT /* for %pointer */
unsigned char yytextarr[YYLMAX];
unsigned char *yytext;
# else               /* %array */
unsigned char yytextarr[1];
# ifdef YYCHAR_ARRAY
char yytext[YYLMAX];
# else
unsigned char yytext[YYLMAX];
# endif
# endif
#endif

struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
unsigned char yysbuf[YYLMAX];
unsigned char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
/*	char *yylastch;
 * ***** nls8 ***** */
	unsigned char *yylastch, sec, third, fourth;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
#ifdef YYNLS16_WCHAR
		yylastch = yytextuc;
#else
# ifdef YYCHAR_ARRAY
		yylastch = (unsigned char *)yytext;
# else
		yylastch = yytext;
# endif
#endif
	else {
		yymorfg=0;
#ifdef YYNLS16_WCHAR
		yylastch = yytextuc+yylenguc;
#else
# ifdef YYCHAR_ARRAY
		yylastch = (unsigned char *)yytext+yyleng;
# else
		yylastch = yytext+yyleng;
# endif
#endif
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = &yycrank[yystate->yystoff];
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == 0)break;
				}
			*yylastch++ = yych = input();
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt = &yycrank[yystate->yystoff]) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
#ifdef YYNLS16_WCHAR
				yylenguc = yylastch-yytextuc+1;
				yytextuc[yylenguc] = 0;
#else
# ifdef YYCHAR_ARRAY
				yyleng = yylastch-(unsigned char*)yytext+1;
# else
				yyleng = yylastch-yytext+1;
# endif
				yytext[yyleng] = 0;
#endif
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
#ifdef YYNLS16_WCHAR
					sprint(yytextuc);
#else
					sprint(yytext);
#endif
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
#ifdef YYNLS16_WCHAR
		if (yytextuc[0] == 0  /* && feof(yyin) */)
#else
		if (yytext[0] == 0  /* && feof(yyin) */)
#endif
			{
			yysptr=yysbuf;
			return(0);
			}
#ifdef YYNLS16_WCHAR
		yyprevious = yytextuc[0] = input();
#else
		yyprevious = yytext[0] = input();
#endif
		if (yyprevious>0) {
			output(yyprevious);
#ifdef YYNLS16
                        if (yynls16) {
			int noBytes;
                        sec = input();
                        third = input();
                        fourth = input();
#ifdef YYNLS16_WCHAR
                        noBytes = MultiByte(yytextuc[0],sec,third,fourth);
#else 
                        noBytes = MultiByte(yytext[0],sec,third,fourth);
#endif          
     					switch(noBytes) {
     					case 2:
#ifdef YYNLS16_WCHAR
 						output(yyprevious=yytextuc[0]=sec);
#else
 						output(yyprevious=yytext[0]=sec);
#endif
                                                 unput(fourth);
                                                 unput(third);
                                                 break;
     					case 3:
#ifdef YYNLS16_WCHAR
 						output(yyprevious=yytextuc[0]=sec);
 						output(yyprevious=yytextuc[0]=third);
#else
 						output(yyprevious=yytext[0]=sec);
 						output(yyprevious=yytext[0]=third);
#endif
                                                 unput(fourth);
                                                 break; 
                                         case 4:
#ifdef YYNLS16_WCHAR
 						output(yyprevious=yytextuc[0]=sec);
 						output(yyprevious=yytextuc[0]=third);
 						output(yyprevious=yytextuc[0]=fourth);
#else
 						output(yyprevious=yytext[0]=sec);
 						output(yyprevious=yytext[0]=third);
 						output(yyprevious=yytext[0]=fourth);
#endif
                                                 break;                                                                                            
					default:
					        unput(fourth);
					        unput(third);
						unput(sec);
						break;
						}
					}
#endif
                }
#ifdef YYNLS16_WCHAR
		yylastch=yytextuc;
#else
# ifdef YYCHAR_ARRAY
		yylastch=(unsigned char*)yytext;
# else
		yylastch=yytext;
# endif
#endif
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}

# ifdef __cplusplus
yyback(int *p, int m)
# else
yyback(p, m)
	int *p;
# endif
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	
	}

#if (defined(__cplusplus) || defined(__STDC__))
void yyoutput(int c)
#else
yyoutput(c)
  int c;
# endif
{
	output(c);
}

#if (defined(__cplusplus) || defined(__STDC__))
void yyunput(int c)
#else
yyunput(c)
   int c;
#endif
{
	unput(c);
}
