/*	Tdi1Decompile.C
	Convert expression into text.
		status = TdiDecompile(&compiled_dsc, [&maximum_elements_dsc], &out_xd);

	Limitation: 65535 bytes of text.
	based on work by Josh Stillerman 28-DEC-1988
	Ken Klare, LANL P-4	(c)1989,1990,1991,1993,1994
*/

#include <tdimessages.h>

unsigned int TdiDECOMPILE_MAX = 0xffff;

extern unsigned short OpcDecompile;
extern unsigned int TdiIndent;

#include "tdirefcat.h"
#include "tdirefstandard.h"
#include <strroutines.h>
#include <string.h>
#include <tdimessages.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <mds_stdarg.h>

extern int TdiGetLong();
extern int TdiDecompileDeindent();
extern int Tdi0Decompile_R();
extern int TdiConvert();
extern int TdiEvaluate();
extern int TdiTrace();

int Tdi0Decompile(struct descriptor *in_ptr,int prec, struct descriptor_d *out_ptr);

TdiRefStandard(Tdi1Decompile)
struct descriptor_d		answer = {0,DTYPE_T,CLASS_D,0};

	TdiIndent = 1;
	if (narg > 1 && list[1]) status = TdiGetLong(list[1], &TdiDECOMPILE_MAX);
	else TdiDECOMPILE_MAX = 0xffff;
	if (status & 1) status = Tdi0Decompile(list[0], 9999, &answer);
	while (status == TdiSTRTOOLON && TdiDECOMPILE_MAX > 10) {
		TdiDECOMPILE_MAX /= 10;
		if (TdiDECOMPILE_MAX > 100) TdiDECOMPILE_MAX = 100;
		StrFree1Dx(&answer);
		status = Tdi0Decompile(list[0], 9999, &answer);
	}
	if (status & 1) TdiDecompileDeindent(&answer);
	if (status & 1) status = MdsCopyDxXd((struct descriptor *)&answer, out_ptr);
	StrFree1Dx(&answer);
	return status;
}
/*-------------------------------------------------------
	Decompile an expression into text, recursively.
	Uses dynamic string initialized and freed outside.
	Precedence is used by function evaluation.
*/
#define P_ARG	88
static unsigned char htab[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
static DESCRIPTOR(BUILD_EVENT,	"BuildEvent(\"");
static DESCRIPTOR(CLASS,		"??Class_");
static DESCRIPTOR(CLOSE_EVENT,	"\")");
static DESCRIPTOR(COLON,		":");
static DESCRIPTOR(COMMA,		",");
static DESCRIPTOR(COMMA_SPACE,	", ");
static DESCRIPTOR(CMPLX,		"Cmplx(");
static DESCRIPTOR(DTYPE,		"??Dtype_");
static DESCRIPTOR(HEX,		"0X");
static DESCRIPTOR(LEFT_BRACKET,	"[");
static DESCRIPTOR(MISSING,	"$Missing");
static DESCRIPTOR(MORE,		" /*** etc. ***/");
static DESCRIPTOR(RIGHT_BRACKET,	"]");
static DESCRIPTOR(RIGHT_PAREN,	")");
static DESCRIPTOR(SET_RANGE,	"Set_Range(");
static DESCRIPTOR(STAR,		"*");

int			TdiSingle(
int			val,
struct descriptor_d	*out_ptr)
{
        struct descriptor	val_dsc = {sizeof(int),DTYPE_L,CLASS_S,0};
	val_dsc.pointer = (char *)&val;
	return Tdi0Decompile(&val_dsc, P_ARG, out_ptr);
}
/*-------------------------------------------------------
	Handle arrays and arrays of pointers to descriptors.
*/
int			vector(
struct descriptor	*in_ptr,
int			level,
char			**item_ptr_ptr,
struct descriptor_d	*out_ptr)
{
array_bounds_desc *a_ptr = (array_bounds_desc *)in_ptr;
int	n = a_ptr->aflags.coeff ? a_ptr->m[level] : (int)a_ptr->arsize / (int)a_ptr->length;
int	j, status;

	status = StrAppend(out_ptr, &LEFT_BRACKET);
	if (level > 0)
		for (j = n; --j >= 0 && status & 1;) {
			status = vector(in_ptr, level-1, item_ptr_ptr, out_ptr);
			if (j > 0 && status & 1) status = StrAppend(out_ptr, &COMMA_SPACE);
		}
	else {
	struct descriptor one = *in_ptr;
	int	length = a_ptr->length;
	char	*pitem = *item_ptr_ptr;

		one.class = CLASS_S;
		for (j = 0; status & 1 && j < n; pitem += length) {
			if (a_ptr->class == CLASS_APD)
				status = Tdi0Decompile(a_ptr->pointer[j], P_ARG, out_ptr);
			else switch (one.dtype) {
			case DTYPE_BU :	status = TdiSingle(*(unsigned char *)pitem, out_ptr); break;
			case DTYPE_WU :	status = TdiSingle(*(unsigned short *)pitem, out_ptr); break;
			case DTYPE_B :	status = TdiSingle(*(char *)pitem, out_ptr); break;
			case DTYPE_W :	status = TdiSingle(*(short *)pitem, out_ptr); break;
			default :
				one.pointer = pitem;
				status = Tdi0Decompile(&one, P_ARG, out_ptr);
				break;
			}
			if (++j < n && status & 1) status = StrAppend(out_ptr, &COMMA);
		}
		*item_ptr_ptr = pitem;
	}
	if (status & 1) status = StrAppend(out_ptr, &RIGHT_BRACKET);
	return status;
}
	/******************
	Squeeze out spaces.
	******************/
static int		noblanks(
struct descriptor	*cdsc_ptr)
{
int	n = cdsc_ptr->length;
char	*pwas = cdsc_ptr->pointer, *pnow = pwas;

	while (--n >= 0) if (*pwas != ' ') *pnow++ = *pwas++; else pwas++;
	cdsc_ptr->length = (unsigned short)(pnow - cdsc_ptr->pointer);
	return 1;
}

	/*****************************
	Neater floating point numbers.
	*****************************/
static int		closeup(
char			repl,
struct descriptor	*pfloat,
struct descriptor	*pdc)
{
int	status, sum, j, shift;
char	*pwas = pdc->pointer;
char	*plim = pwas + pdc->length;
char	*pdec, *plast, *pexp, *ppass;

	status = TdiConvert(pfloat, pdc MDS_END_ARG);
	if (!(status & 1)) return status;

	*plim = '\0';
	while (pwas < plim) {
		ppass = pwas;
		/****************************
		Skip spaces and leading zero.
		****************************/
		while (*pwas == ' ') pwas++;
		if (*pwas == '-') pwas++;
	/*Only for VAX TdiConvert.MAR 0.123E+12, careful about 0.*/
		if (pwas+2 < plim && *pwas == '0' && *(pwas+1) == '.'
			&& *(pwas+2) >= '0' && *(pwas+2) <= '9') *pwas++ = ' ';
		pdec = 0;
		plast = pwas;
		while (pwas) {
			if (*pwas == '0' || *pwas == ' ') ;
			else if (*pwas > '0' && *pwas <= '9') plast = pwas;
			else if (*pwas == '.') pdec = pwas;
			else break;
			pwas++;
		}
		if (pwas >= plim) 
                {
                  char *pwas_save = pwas;
                  if (pdec && (pdec < (pwas - 1)))
                    for (pwas-- ;*pwas == '0';pwas--) *pwas = ' ';
                  pwas = pwas_save;
                  continue;
                }
		if (!pdec) pdec = pwas;
		/*********************************
		Zero and $ROPRAND should be quick.
		*********************************/
		if (plast == pdec) {
			if (*pdec == '$') for (; pwas < plim && *pwas != ',';) pwas++;
			else {
				pwas = ppass;
				*pwas++ = '0';
				if (repl == 'E') {
					*pwas++ = '.';
				}
				else {
					*pwas++ = repl;
					*pwas++ = '0';
				}
				for (; pwas < plim && *pwas != ',';) *pwas++ = ' ';
			}
			if (*pwas == ',') pwas++;
			continue;
		}
		/*******************
		We hit the exponent.
		*******************/
		pexp = pwas;
		switch (*pwas) {
		case 'E' :
		case 'F' :
		case 'D' :
		case 'G' :
		case 'H' :
		case 'S' :
		case 'T' :
			*pwas++ = repl;
			if (*pwas == '+') *pwas++ = ' ';
			else if (*pwas == '-') pwas++;
			break;
		case '+' :
			*pwas++ = repl;
			break;
		case '-' :
			if (plast == pwas - 1) {
				for (pwas = ppass; pwas < pexp; pwas++) *pwas = *(pwas+1);
				pdec--; plast--;
			}
			pwas++;
			*--pexp = repl;
			break;
		default : return TdiNOT_NUMBER;
		}
		/*************************
		Make a number of exponent.
		Adjust by thousands.
		.1e6	100.e3		.1e3	100.e0		.1e0	.1e0 special	.1e-3	100.e-6
		.1e5	10.e3		.1e2	10.e0		.1e-1	10.e-3		.1e-4	10.e-6
		.1e4	1000.e0 special	.1e1	1.e0		.1e-2	1.e-3		.1e-5	1.e-6
		*************************/
		while (pwas < plim && *pwas == '0') *pwas++ = ' ';
		sum = 0;
		while (pwas < plim && *pwas != ',') sum = sum*10 + (*pwas++ - '0');
		if (*(pexp+1) == '-') {
			shift = (30002 - sum) % 3 + 1;
			sum = sum + shift;
		}
		else if (sum <= 4) {
			shift = sum;
			sum = 0;
		}
		else {
			shift = (sum - 1) % 3 + 1;
			sum = sum - shift;
		}
		/************************
		Shift the decimal point.
		Remove trailing zeroes.
		Kill decimal at exponent.
		Remove E0 (F_Floating).
		************************/
		for (j = shift; --j >= 0; pdec++) *pdec = *(pdec+1);
		if (plast > pdec) *pdec = '.';
		else {
			plast = pdec;
			if (pdec < pexp) *pdec = (char)((sum == 0 && repl == 'E') ? '.' : ' ');
		}
		for (; ++plast < pexp;) *plast = ' ';

		if (sum == 0 && repl == 'E') {
			while (pexp < plim && *pexp != ',') *pexp++ = ' ';
		} else {
			pdec = pwas;
			if (sum == 0) *--pdec = '0';
			else for (;sum > 0; sum /= 10) *--pdec = (char)((sum % 10) + '0');
			while (*--pdec >= '0' && *pdec <= '9') *pdec = ' ';		
		}
		if (*pwas == ',') pwas++;
	}
	if (pwas != plim) status = TdiNOT_NUMBER;
	noblanks(pdc);
        if ((pdc->length == 1) && (pdc->pointer[0] == '.'))
        {
          pdc->length++;
          pdc->pointer[0] = '0';
          pdc->pointer[1] = '.';
        }
	return status;
}

int			Tdi0Decompile(
struct descriptor	*in_ptr,
int			prec,
struct descriptor_d	*out_ptr)
{
char		c0[85], *cptr, *bptr;
struct descriptor	cdsc = {11,DTYPE_T,CLASS_S,0};
struct descriptor	t2 = {0,DTYPE_T,CLASS_S,0};
int	status = 1, j, dtype, n1, n2;
char n1c;
        cdsc.pointer = c0;
	/******************
	Watch null pointer.
	******************/
	if (!in_ptr) return StrAppend(out_ptr, &STAR);

	dtype = in_ptr->dtype;
	switch(in_ptr->class) {
	default:
		status = StrAppend(out_ptr, &CLASS);
		if (status & 1) status = TdiSingle(in_ptr->class, out_ptr);
		break;

	case CLASS_XD :
	case CLASS_XS :
	case CLASS_S :
	case CLASS_D :
		switch(dtype) {
		default :
			status = StrAppend(out_ptr, &DTYPE);
			if (status & 1) status = TdiSingle(dtype, out_ptr);
			break;
		/****************************************
		Printing characters are added as a block.
		****************************************/
		case DTYPE_T :
		{DESCRIPTOR(QUOTE, "\"");
			cdsc.length = 2;
			t2.pointer = cptr = in_ptr->pointer;
			for (n1 = n2 = 0, j = in_ptr->length; --j >= 0; cptr++) {
				if (*cptr == '\'') n1++;
				else if (*cptr == '\"') n2++;
			}
			n1c = n1 < n2 ? '\'' : '\"';
			QUOTE.pointer = &n1c;
			status = StrAppend(out_ptr, &QUOTE);
			cptr = in_ptr->pointer;
			for (j = in_ptr->length; --j >= 0; cptr++) {
				if (*cptr == n1c) c0[1] = n1c;
				else switch (*cptr) {
			/******************
			Special characters.
			******************/
				case '\\' : c0[1] = '\\'; break;/*backslash*/
				case '\n' : c0[1] = 'n'; break;/*newline*/
				case '\t' : c0[1] = 't'; break;/*horizontal tab*/
				case '\v' : c0[1] = 'v'; break;/*vertical tab*/
				case '\b' : c0[1] = 'b'; break;/*backspace*/
				case '\r' : c0[1] = 'r'; break;/*return*/
				case '\f' : c0[1] = 'f'; break;/*formfeed*/
				/*case '\a' : c0[1] = 'a'; break;*//*audible alert*/
				/*case '\"' : c0[1] = '\"'; break;*//*quote*/
				/*case '\'' : c0[1] = '\''; break;*//*apostrophe*/
				/*case '\?' : c0[1] = '\?'; break;*//*trigraphs*/
				default :
					if (*cptr >= ' ' && *cptr <= '~') continue;/*most printing codes*/
			/********************************
			Octal form of control characters.
			********************************/
					cdsc.length = 4;
					c0[1] = (char)(((*cptr >> 6) & 7) | '0');
					c0[2] = (char)(((*cptr >> 3) & 7) | '0');
					c0[3] = (char)((*cptr & 7) | '0');
					break;
				}
				if (status & 1 && (t2.length = (unsigned short)(cptr - t2.pointer)) > 0)
				  status = StrAppend(out_ptr, &t2);
				t2.pointer = cptr + 1;
				c0[0] = '\\';
				if (status & 1) status = StrAppend(out_ptr, &cdsc);
				cdsc.length = 2;
			}
			if (status & 1 && (t2.length = (unsigned short)(cptr - t2.pointer)) > 0) 
                          status = StrAppend(out_ptr, &t2);
			if (status & 1) status = StrAppend(out_ptr, &QUOTE);
			break;
		}

		case DTYPE_L :
			/*cdsc.length = 11;*/
			status = TdiConvert(in_ptr, &cdsc MDS_END_ARG);
			if (status & 1) status = noblanks(&cdsc);
			if (status & 1) status = StrAppend(out_ptr, &cdsc);
			break;
		case DTYPE_B :
		case DTYPE_W :
		case DTYPE_BU :
		case DTYPE_WU :
		case DTYPE_LU :
			cdsc.length = (unsigned short)(in_ptr->length * 8 * .30103 + 2);
			status = TdiConvert(in_ptr, &cdsc MDS_END_ARG);
			if (status & 1) status = noblanks(&cdsc);
			if (status & 1) {
			struct descriptor	sdsc = {0,DTYPE_T,CLASS_S,0};
                                sdsc.length = (unsigned short)strlen(TdiREF_CAT[dtype].name);
                                sdsc.pointer = TdiREF_CAT[dtype].name;
				status = StrConcat(out_ptr, out_ptr, &cdsc, &sdsc MDS_END_ARG);
			}
			break;

		/***********************************************
		Assumes: low-order byte is first. right-to-left.
		***********************************************/
		case DTYPE_Q :
		case DTYPE_QU :
		case DTYPE_O :
		case DTYPE_OU :
		  {     static int endiantest = 1;
                        char littleendian = *(char *)&endiantest;
			cptr = c0;
			j = in_ptr->length;
			bptr = littleendian ? (in_ptr->pointer+j) : (in_ptr->pointer - 1);
			while (--j >= 0) {
				*cptr++ = htab[( *(littleendian ? --bptr : ++bptr) >> 4) & 15];
				*cptr++ = htab[*bptr & 15];
			}
			while (cdsc.pointer < cptr-1 && *cdsc.pointer == '0') {cdsc.pointer++;}
			cdsc.length = (unsigned short)(cptr - cdsc.pointer);
			{struct descriptor	sdsc = {0,DTYPE_T,CLASS_S,0};
                                sdsc.length = (unsigned short)strlen(TdiREF_CAT[dtype].name);
                                sdsc.pointer = TdiREF_CAT[dtype].name;
				status = StrConcat(out_ptr, out_ptr, &HEX, &cdsc, &sdsc MDS_END_ARG);
			}
		  }
			break;

		case DTYPE_D :
		case DTYPE_F :
		case DTYPE_G :
		case DTYPE_H :
		case DTYPE_FS :
		case DTYPE_FT :
			cdsc.length = (unsigned short)((in_ptr->length - 1) * 8 * .30103 + 6.8);
			status = closeup((char)TdiREF_CAT[dtype].fname[0], in_ptr, &cdsc);
			if (status & 1) status = StrAppend(out_ptr, &cdsc);
			break;

		case DTYPE_DC :
		case DTYPE_FC :
		case DTYPE_GC :
		case DTYPE_HC :
		case DTYPE_FSC :
		case DTYPE_FTC :
                        { struct descriptor temp = *in_ptr;
                          StrAppend(out_ptr,&CMPLX);
                          switch (temp.dtype)
                          {
                             case DTYPE_DC: temp.dtype = DTYPE_D; break;
                             case DTYPE_FC: temp.dtype = DTYPE_F; break;
                             case DTYPE_GC: temp.dtype = DTYPE_G; break;
                             case DTYPE_HC: temp.dtype = DTYPE_H; break;
                             case DTYPE_FSC: temp.dtype = DTYPE_FS; break;
			     case DTYPE_FTC: temp.dtype = DTYPE_FT; break;
                          }
                          temp.length /= 2;
                          Tdi0Decompile(&temp,P_ARG,out_ptr);
                          StrAppend(out_ptr,&COMMA);
                          temp.pointer += temp.length;
                          Tdi0Decompile(&temp,P_ARG,out_ptr);
                          StrAppend(out_ptr,&RIGHT_PAREN);
                          status = 1;
                          break;
                        }
		case DTYPE_DSC :
			status = Tdi0Decompile((struct descriptor *)in_ptr->pointer, prec, out_ptr);
			break;

		/*****************
		TDI special types.
		*****************/
		case DTYPE_MISSING :
			if (in_ptr->class != CLASS_XD) status = StrAppend(out_ptr, &MISSING);
			break;
		case DTYPE_IDENT :
			t2 = *in_ptr;
			t2.dtype = DTYPE_T;
			status = StrAppend(out_ptr, &t2);
			break;

		/*********
		MDS types.
		*********/
		case DTYPE_NID :
			{ char *path = TreeGetMinimumPath(0, *(int *)in_ptr->pointer);
			  if (path != NULL)
			  {
				DESCRIPTOR_FROM_CSTRING(path_d,path);
				status = StrAppend(out_ptr, &path_d);
				TreeFree(path);
			  }
			  else
				  status = TreeFAILURE;
			}
			break;
		case DTYPE_PATH :
			/*****************************************************
			Convert forward refs to nid to reduce to minimum path.
			Then back to string. If it fails, use path as is.
			*****************************************************/
			{
			    int	nid;
				char *path = MdsDescrToCstring((struct descriptor *)in_ptr);
				status = TreeFindNode(path, &nid);
				MdsFree(path);
				if (status & 1) path = TreeGetMinimumPath(0, nid);
				if (path == NULL) status = TreeFAILURE;
				if (status & 1)
				{
					DESCRIPTOR_FROM_CSTRING(path_d,path);
					status = StrAppend(out_ptr, &path_d);
					TreeFree(path);
				}
				else status = StrAppend(out_ptr, in_ptr);
			}
			break;
		case DTYPE_EVENT :
			t2 = *in_ptr;
			t2.dtype = DTYPE_T;
			status = StrConcat(out_ptr, out_ptr, &BUILD_EVENT, &t2, &CLOSE_EVENT MDS_END_ARG);
			break;
		}
		break;

	/***********
	MDS records.
	***********/
	case CLASS_R :
		status = Tdi0Decompile_R(in_ptr, prec, out_ptr);
		break;

	/**********************************
	Compressed data should be expanded.
	**********************************/
	case CLASS_CA :
		{struct descriptor_xd tmp = EMPTY_XD;
			status = TdiEvaluate(in_ptr, &tmp MDS_END_ARG);
			if (status & 1) status = Tdi0Decompile(tmp.pointer, prec, out_ptr);
			MdsFree1Dx(&tmp, NULL);
		}
		break;

	/******************
	Arrays in brackets.
	******************/
	case CLASS_APD :
	case CLASS_A :
		{array_bounds_desc *a_ptr = (array_bounds_desc *)in_ptr;
		int	length = a_ptr->length;
		int	coeff = a_ptr->aflags.coeff;
		int	dimct = coeff ? a_ptr->dimct : 1;
		unsigned int	count = (int)a_ptr->arsize / length;
		int	more = count > TdiDECOMPILE_MAX || a_ptr->arsize >= 32768;

		/**************************************
		Special data types made easier to read.
		**************************************/
			switch (dtype) {
			case DTYPE_BU :	bptr = "Byte_Unsigned("; break;
			case DTYPE_WU :	bptr = "Word_Unsigned("; break;
			case DTYPE_LU :	bptr = "Long_Unsigned("; break;
			case DTYPE_B :	bptr = "Byte("; break;
			case DTYPE_W :	bptr = "Word("; break;
			default : 		bptr = 0; break;
			}
			if (bptr) {
			struct descriptor text = {0,DTYPE_T,CLASS_S,0};
				text.length = (unsigned short)strlen(bptr);
				text.pointer = bptr;
				status = StrAppend(out_ptr, &text);
			}

		/*****************************************
		Specify bounds of array. SET_RANGE(l:u,...
		*****************************************/
			if (a_ptr->aflags.bounds) {
				more = 1;
				status = StrAppend(out_ptr, &SET_RANGE);
				for (j = 0; j < dimct; ++j) {
					if (status & 1) status = TdiSingle(a_ptr->m[dimct+2*j], out_ptr);
					if (status & 1) status = StrAppend(out_ptr, &COLON);
					if (status & 1) status = TdiSingle(a_ptr->m[dimct+2*j+1], out_ptr);
					if (status & 1) status = StrAppend(out_ptr, &COMMA);
				}
			}

		/******************************************
		Specify shape of array. SET_RANGE(size, ...
		******************************************/
			else if (more) {
				status = StrAppend(out_ptr, &SET_RANGE);
				for (j = 0; j < dimct; ++j) {
					if (status & 1) status = TdiSingle(coeff ? a_ptr->m[j] : count, out_ptr);
					if (status & 1) status = StrAppend(out_ptr, &COMMA);
				}
			}

		/*********************************
		Specify data of array. [value,...]
		*********************************/
			if (count > TdiDECOMPILE_MAX) {
			struct descriptor one = *in_ptr;
				one.class = CLASS_S;
				if (status & 1) status = Tdi0Decompile(&one, P_ARG, out_ptr);
				if (status & 1) status = StrAppend(out_ptr, &MORE);
			}
			else {
			char *pitem = (char *)a_ptr->pointer;
				status = vector((struct descriptor *)a_ptr, dimct-1, &pitem, out_ptr);
			}
			if (more && status & 1) status = StrAppend(out_ptr, &RIGHT_PAREN);
			if (bptr && status & 1) status = StrAppend(out_ptr, &RIGHT_PAREN);
		}
		break;
	}/*switch class*/
	if (!(status & 1)) TdiTrace(OpcDecompile, 1, in_ptr, out_ptr);
	return status;
}
