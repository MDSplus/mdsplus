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
/*      Tdi1Decompile.C
        Convert expression into text.
                status = TdiDecompile(&compiled_dsc, [&maximum_elements_dsc], &out_xd);

        Limitation: 65535 bytes of text.
        based on work by Josh Stillerman 28-DEC-1988
        Ken Klare, LANL P-4     (c)1989,1990,1991,1993,1994
*/

#include <tdishr_messages.h>
#include <STATICdef.h>

#define TdiDECOMPILE_MAX TdiThreadStatic_p->TdiDecompile_max

extern unsigned short OpcDecompile;

#include "tdirefcat.h"
#include "tdirefstandard.h"
#include <strroutines.h>
#include <string.h>
#include <tdishr_messages.h>
#include <stdlib.h>
#include <inttypes.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <mds_stdarg.h>
#include <mdsshr_messages.h>
#include "tdithreadsafe.h"
#ifdef max
#undef max
#endif
#define max(a,b) (((a) > (b)) ? (a) : (b))

extern int TdiGetLong();
extern int TdiDecompileDeindent();
extern int Tdi0Decompile_R();
extern int TdiConvert();
extern int TdiEvaluate();
extern int TdiTrace();

int Tdi0Decompile(struct descriptor *in_ptr, int prec, struct descriptor_d *out_ptr);

int Tdi1Decompile(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  GET_TDITHREADSTATIC_P;
  struct descriptor_d answer = { 0, DTYPE_T, CLASS_D, 0 };
  TdiThreadStatic_p->TdiIndent = 1;
  if (narg > 1 && list[1])
    status = TdiGetLong(list[1], &TdiDECOMPILE_MAX);
  else
    TdiDECOMPILE_MAX = 0xffff;
  if STATUS_OK
    status = Tdi0Decompile(list[0], 9999, &answer);
  while (status == StrSTRTOOLON && TdiDECOMPILE_MAX > 10) {
    TdiDECOMPILE_MAX /= 10;
    if (TdiDECOMPILE_MAX > 100)
      TdiDECOMPILE_MAX = 100;
    StrFree1Dx(&answer);
    status = Tdi0Decompile(list[0], 9999, &answer);
  }
  if STATUS_OK
    TdiDecompileDeindent(&answer);
  if STATUS_OK
    status = MdsCopyDxXd((struct descriptor *)&answer, out_ptr);
  StrFree1Dx(&answer);
  return status;
}

/*-------------------------------------------------------
        Decompile an expression into text, recursively.
        Uses dynamic string initialized and freed outside.
        Precedence is used by function evaluation.
*/
#define P_ARG   88
STATIC_CONSTANT unsigned char htab[16] =
    { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e',
  'f'
};

STATIC_CONSTANT DESCRIPTOR(BUILD_EVENT, "BuildEvent(\"");
STATIC_CONSTANT DESCRIPTOR(CLASS, "??Class_");
STATIC_CONSTANT DESCRIPTOR(CLOSE_EVENT, "\")");
STATIC_CONSTANT DESCRIPTOR(COLON, ":");
STATIC_CONSTANT DESCRIPTOR(COMMA, ",");
STATIC_CONSTANT DESCRIPTOR(COMMA_SPACE, ", ");
STATIC_CONSTANT DESCRIPTOR(CMPLX, "Cmplx(");
STATIC_CONSTANT DESCRIPTOR(DTYPE, "??Dtype_");
STATIC_CONSTANT DESCRIPTOR(HEX, "0X");
STATIC_CONSTANT DESCRIPTOR(LEFT_BRACKET, "[");
STATIC_CONSTANT DESCRIPTOR(MISSING, "$Missing");
STATIC_CONSTANT DESCRIPTOR(MORE, " /*** etc. ***/");
STATIC_CONSTANT DESCRIPTOR(RIGHT_BRACKET, "]");
STATIC_CONSTANT DESCRIPTOR(RIGHT_PAREN, ")");
STATIC_CONSTANT DESCRIPTOR(SET_RANGE, "Set_Range(");
STATIC_CONSTANT DESCRIPTOR(STAR, "*");

int TdiSingle(int val, struct descriptor_d *out_ptr)
{
  struct descriptor val_dsc = { sizeof(int), DTYPE_L, CLASS_S, 0 };
  val_dsc.pointer = (char *)&val;
  return Tdi0Decompile(&val_dsc, P_ARG, out_ptr);
}

/*-------------------------------------------------------
        Handle arrays and arrays of pointers to descriptors.
*/
STATIC_ROUTINE int tdi_vector(struct descriptor *in_ptr,
			      int level, char **item_ptr_ptr, struct descriptor_d *out_ptr)
{
  array_bounds_desc *a_ptr = (array_bounds_desc *) in_ptr;
  int n =
      a_ptr->aflags.coeff ? a_ptr->m[level] : (int)a_ptr->arsize /
      max((unsigned int)1, a_ptr->length);
  int j, status;

  status = StrAppend(out_ptr, (struct descriptor *)&LEFT_BRACKET);
  if (level > 0)
    for (j = n; --j >= 0 && STATUS_OK;) {
      status = tdi_vector(in_ptr, level - 1, item_ptr_ptr, out_ptr);
      if (j > 0 && STATUS_OK)
	status = StrAppend(out_ptr, (struct descriptor *)&COMMA_SPACE);
  } else {
    struct descriptor one = *in_ptr;
    int length = a_ptr->length;
    char *pitem = *item_ptr_ptr;

    one.class = CLASS_S;
    for (j = 0; STATUS_OK && j < n; pitem += length) {
      if (a_ptr->class == CLASS_APD)
	status = Tdi0Decompile(a_ptr->pointer[j], P_ARG, out_ptr);
      else
	switch (one.dtype) {
	case DTYPE_BU:
	  status = TdiSingle(*(unsigned char *)pitem, out_ptr);
	  break;
	case DTYPE_WU:
	  status = TdiSingle(*(unsigned short *)pitem, out_ptr);
	  break;
	case DTYPE_B:
	  status = TdiSingle(*(char *)pitem, out_ptr);
	  break;
	case DTYPE_W:
	  status = TdiSingle(*(short *)pitem, out_ptr);
	  break;
	default:
	  one.pointer = pitem;
	  status = Tdi0Decompile(&one, P_ARG, out_ptr);
	  break;
	}
      if (++j < n && STATUS_OK)
	status = StrAppend(out_ptr, (struct descriptor *)&COMMA);
    }
    *item_ptr_ptr = pitem;
  }
  if STATUS_OK
    status = StrAppend(out_ptr, (struct descriptor *)&RIGHT_BRACKET);
  return status;
}

	/******************
        Squeeze out spaces.
        ******************/
STATIC_ROUTINE int noblanks(struct descriptor *cdsc_ptr)
{
  int n = cdsc_ptr->length;
  char *pwas = cdsc_ptr->pointer, *pnow = pwas;

  while (--n >= 0)
    if (*pwas != ' ')
      *pnow++ = *pwas++;
    else
      pwas++;
  cdsc_ptr->length = (unsigned short)(pnow - cdsc_ptr->pointer);
  return 1;
}

	/*****************************
        Neater floating point numbers.
        *****************************/
STATIC_ROUTINE int closeup(char repl, struct descriptor *pfloat, struct descriptor *pdc)
{
  int status, sum, j, shift;
  char *pwas = pdc->pointer;
  char *plim = pwas + pdc->length;
  char *pdec, *plast, *pexp, *ppass;

  status = TdiConvert(pfloat, pdc MDS_END_ARG);
  if STATUS_NOT_OK
    return status;

  *plim = '\0';
  while (pwas < plim) {
    ppass = pwas;
		/****************************
                Skip spaces and leading zero.
                ****************************/
    while (*pwas == ' ')
      pwas++;
    if (*pwas == '-')
      pwas++;
    /*Only for VAX TdiConvert.MAR 0.123E+12, careful about 0. */
    if (pwas + 2 < plim && *pwas == '0' && *(pwas + 1) == '.'
	&& *(pwas + 2) >= '0' && *(pwas + 2) <= '9')
      *pwas++ = ' ';
    pdec = 0;
    plast = pwas;
    while (pwas) {
      if (*pwas == '0' || *pwas == ' ') ;
      else if (*pwas > '0' && *pwas <= '9')
	plast = pwas;
      else if (*pwas == '.')
	pdec = pwas;
      else
	break;
      pwas++;
    }
    if (pwas >= plim) {
      char *pwas_save = pwas;
      if (pdec && (pdec < (pwas - 1)))
	for (pwas--; *pwas == '0'; pwas--)
	  *pwas = ' ';
      pwas = pwas_save;
      continue;
    }
    if (!pdec)
      pdec = pwas;
		/*********************************
                Zero and $ROPRAND should be quick.
                *********************************/
    if (plast == pdec) {
      if (*pdec == '$')
	for (; pwas < plim && *pwas != ',';)
	  pwas++;
      else {
	pwas = ppass;
	*pwas++ = '0';
	if (repl == 'E') {
	  *pwas++ = '.';
	} else {
	  *pwas++ = repl;
	  *pwas++ = '0';
	}
	for (; pwas < plim && *pwas != ',';)
	  *pwas++ = ' ';
      }
      if (*pwas == ',')
	pwas++;
      continue;
    }
		/*******************
                We hit the exponent.
                *******************/
    pexp = pwas;
    switch (*pwas) {
    case 'E':
    case 'F':
    case 'D':
    case 'G':
    case 'H':
    case 'S':
    case 'T':
    case 'V':
      *pwas++ = repl;
      if (*pwas == '+')
	*pwas++ = ' ';
      else if (*pwas == '-')
	pwas++;
      break;
    case '+':
      *pwas++ = repl;
      break;
    case '-':
      if (plast == pwas - 1) {
	for (pwas = ppass; pwas < pexp; pwas++)
	  *pwas = *(pwas + 1);
	pdec--;
	plast--;
      }
      pwas++;
      *--pexp = repl;
      break;
    default:
      return TdiNOT_NUMBER;
    }
		/*************************
                Make a number of exponent.
                Adjust by thousands.
                .1e6    100.e3          .1e3    100.e0          .1e0    .1e0 special    .1e-3   100.e-6
                .1e5    10.e3           .1e2    10.e0           .1e-1   10.e-3          .1e-4   10.e-6
                .1e4    1000.e0 special .1e1    1.e0            .1e-2   1.e-3           .1e-5   1.e-6
                *************************/
    while (pwas < plim && *pwas == '0')
      *pwas++ = ' ';
    sum = 0;
    while (pwas < plim && *pwas != ',')
      sum = sum * 10 + (*pwas++ - '0');
    if (*(pexp + 1) == '-') {
      shift = (30002 - sum) % 3 + 1;
      sum = sum + shift;
    } else if (sum <= 4) {
      shift = sum;
      sum = 0;
    } else {
      shift = (sum - 1) % 3 + 1;
      sum = sum - shift;
    }
		/************************
                Shift the decimal point.
                Remove trailing zeroes.
                Kill decimal at exponent.
                Remove E0 (F_Floating).
                ************************/
    for (j = shift; --j >= 0; pdec++)
      *pdec = *(pdec + 1);
    if (plast > pdec)
      *pdec = '.';
    else {
      plast = pdec;
      if (pdec < pexp)
	*pdec = (char)((sum == 0 && repl == 'E') ? '.' : ' ');
    }
    for (; ++plast < pexp;)
      *plast = ' ';

    if (sum == 0 && repl == 'E') {
      while (pexp < plim && *pexp != ',')
	*pexp++ = ' ';
    } else {
      pdec = pwas;
      if (sum == 0)
	*--pdec = '0';
      else
	for (; sum > 0; sum /= 10)
	  *--pdec = (char)((sum % 10) + '0');
      while (*--pdec >= '0' && *pdec <= '9')
	*pdec = ' ';
    }
    if (*pwas == ',')
      pwas++;
  }
  if (pwas != plim)
    status = TdiNOT_NUMBER;
  noblanks(pdc);
  if ((pdc->length == 1) && (pdc->pointer[0] == '.')) {
    pdc->length++;
    pdc->pointer[0] = '0';
    pdc->pointer[1] = '.';
  }
  return status;
}

int Tdi0Decompile(struct descriptor *in_ptr, int prec, struct descriptor_d *out_ptr)
{
  GET_TDITHREADSTATIC_P;
  char c0[85], *cptr, *bptr;
  struct descriptor cdsc = { 11, DTYPE_T, CLASS_S, 0 };
  struct descriptor t2 = { 0, DTYPE_T, CLASS_S, 0 };
  int status = MDSplusSUCCESS, j, dtype, n1, n2;
  char n1c;
  cdsc.pointer = c0;
	/******************
        Watch null pointer.
        ******************/
  if (!in_ptr)
    return StrAppend(out_ptr, (struct descriptor *)&STAR);

  dtype = in_ptr->dtype;
  switch (in_ptr->class) {
  default:
    status = StrAppend(out_ptr, (struct descriptor *)&CLASS);
    if STATUS_OK
      status = TdiSingle(in_ptr->class, out_ptr);
    break;

  case CLASS_XD:
  case CLASS_XS:
  case CLASS_S:
  case CLASS_D:
    switch (dtype) {
    default:
      status = StrAppend(out_ptr, (struct descriptor *)&DTYPE);
      if STATUS_OK
	status = TdiSingle(dtype, out_ptr);
      break;
		/****************************************
                Printing characters are added as a block.
                ****************************************/
    case DTYPE_T:
      {
	DESCRIPTOR(QUOTE, "\"");
	cdsc.length = 2;
	t2.pointer = cptr = in_ptr->pointer;
	for (n1 = n2 = 0, j = in_ptr->length; --j >= 0; cptr++) {
	  if (*cptr == '\'')
	    n1++;
	  else if (*cptr == '\"')
	    n2++;
	}
	n1c = n1 < n2 ? '\'' : '\"';
	QUOTE.pointer = &n1c;
	status = StrAppend(out_ptr, (struct descriptor *)&QUOTE);
	cptr = in_ptr->pointer;
	for (j = in_ptr->length; --j >= 0; cptr++) {
	  if (*cptr == n1c)
	    c0[1] = n1c;
	  else
	    switch (*cptr) {
			/******************
                        Special characters.
                        ******************/
	    case '\\':
	      c0[1] = '\\';
	      break;		/*backslash */
	    case '\n':
	      c0[1] = 'n';
	      break;		/*newline */
	    case '\t':
	      c0[1] = 't';
	      break;		/*horizontal tab */
	    case '\v':
	      c0[1] = 'v';
	      break;		/*vertical tab */
	    case '\b':
	      c0[1] = 'b';
	      break;		/*backspace */
	    case '\r':
	      c0[1] = 'r';
	      break;		/*return */
	    case '\f':
	      c0[1] = 'f';
	      break;		/*formfeed */
	      /*case '\a' : c0[1] = 'a'; break; *//*audible alert */
	      /*case '\"' : c0[1] = '\"'; break; *//*quote */
	      /*case '\'' : c0[1] = '\''; break; *//*apostrophe */
	      /*case '\?' : c0[1] = '\?'; break; *//*trigraphs */
	    default:
	      if (*cptr >= ' ' && *cptr <= '~')
		continue;	/*most printing codes */
			/********************************
                        Octal form of control characters.
                        ********************************/
	      cdsc.length = 4;
	      c0[1] = (char)(((*cptr >> 6) & 7) | '0');
	      c0[2] = (char)(((*cptr >> 3) & 7) | '0');
	      c0[3] = (char)((*cptr & 7) | '0');
	      break;
	    }
	  if (STATUS_OK && (t2.length = (unsigned short)(cptr - t2.pointer)) > 0)
	    status = StrAppend(out_ptr, (struct descriptor *)&t2);
	  t2.pointer = cptr + 1;
	  c0[0] = '\\';
	  if STATUS_OK
	    status = StrAppend(out_ptr,(struct descriptor *) &cdsc);
	  cdsc.length = 2;
	}
	if (STATUS_OK && (t2.length = (unsigned short)(cptr - t2.pointer)) > 0)
	  status = StrAppend(out_ptr, (struct descriptor *)&t2);
	if STATUS_OK
	  status = StrAppend(out_ptr, (struct descriptor *)&QUOTE);
	break;
      }

    case DTYPE_L:
      /*cdsc.length = 11; */
      status = TdiConvert(in_ptr, &cdsc MDS_END_ARG);
      if STATUS_OK
	status = noblanks(&cdsc);
      if STATUS_OK
	status = StrAppend(out_ptr, (struct descriptor *)&cdsc);
      break;
    case DTYPE_B:
    case DTYPE_W:
    case DTYPE_BU:
    case DTYPE_WU:
    case DTYPE_LU:
    case DTYPE_Q:
    case DTYPE_QU:
      cdsc.length = (unsigned short)(in_ptr->length * 2.4 + 1.6);
      status = TdiConvert(in_ptr, &cdsc MDS_END_ARG);
      if STATUS_OK
	status = noblanks(&cdsc);
      if STATUS_OK {
	struct descriptor sdsc = { 0, DTYPE_T, CLASS_S, 0 };
	sdsc.length = (unsigned short)strlen(TdiREF_CAT[dtype].name);
	sdsc.pointer = TdiREF_CAT[dtype].name;
	status =
	    StrConcat((struct descriptor *)out_ptr,
		      (struct descriptor *)out_ptr, &cdsc, &sdsc MDS_END_ARG);
      }
      break;

		/***********************************************
                Assumes: low-order byte is first. right-to-left.
                ***********************************************/
    case DTYPE_O:
    case DTYPE_OU:
      cptr = c0;
      j = in_ptr->length;
#ifdef WORDS_BIGENDIAN
      bptr = in_ptr->pointer - 1;
      while (--j >= 0) {
	*cptr++ = htab[(*(++bptr) >> 4) & 15];
	*cptr++ = htab[*bptr & 15];
      }
#else
      bptr = in_ptr->pointer + j;
      while (--j >= 0) {
	*cptr++ = htab[(*(--bptr) >> 4) & 15];
	*cptr++ = htab[*bptr & 15];
      }
#endif
      while (cdsc.pointer < cptr - 1 && *cdsc.pointer == '0') {
	cdsc.pointer++;
      }
      cdsc.length = (unsigned short)(cptr - cdsc.pointer);
      {
	struct descriptor sdsc = { 0, DTYPE_T, CLASS_S, 0 };
	sdsc.length = (unsigned short)strlen(TdiREF_CAT[dtype].name);
	sdsc.pointer = TdiREF_CAT[dtype].name;
	status =
	    StrConcat((struct descriptor *)out_ptr,
		      (struct descriptor *)out_ptr, &HEX, &cdsc, &sdsc MDS_END_ARG);
      }
      break;

    case DTYPE_D:
    case DTYPE_F:
    case DTYPE_G:
    case DTYPE_H:
    case DTYPE_FS:
    case DTYPE_FT:
      cdsc.length = (unsigned short)((in_ptr->length - 1) * 8 * .30103 + 6.8);
      status = closeup((char)TdiREF_CAT[dtype].fname[0], in_ptr, &cdsc);
      if STATUS_OK
	status = StrAppend(out_ptr, (struct descriptor *)&cdsc);
      break;

    case DTYPE_DC:
    case DTYPE_FC:
    case DTYPE_GC:
    case DTYPE_HC:
    case DTYPE_FSC:
    case DTYPE_FTC:
      {
	struct descriptor temp = *in_ptr;
	StrAppend(out_ptr, (struct descriptor *)&CMPLX);
	switch (temp.dtype) {
	case DTYPE_DC:
	  temp.dtype = DTYPE_D;
	  break;
	case DTYPE_FC:
	  temp.dtype = DTYPE_F;
	  break;
	case DTYPE_GC:
	  temp.dtype = DTYPE_G;
	  break;
	case DTYPE_HC:
	  temp.dtype = DTYPE_H;
	  break;
	case DTYPE_FSC:
	  temp.dtype = DTYPE_FS;
	  break;
	case DTYPE_FTC:
	  temp.dtype = DTYPE_FT;
	  break;
	}
	temp.length /= 2;
	Tdi0Decompile(&temp, P_ARG, out_ptr);
	StrAppend(out_ptr, (struct descriptor *)&COMMA);
	temp.pointer += temp.length;
	Tdi0Decompile(&temp, P_ARG, out_ptr);
	StrAppend(out_ptr, (struct descriptor *)&RIGHT_PAREN);
	status = MDSplusSUCCESS;
	break;
      }
    case DTYPE_DSC:
      status = Tdi0Decompile((struct descriptor *)in_ptr->pointer, prec, out_ptr);
      break;

		/*****************
                TDI special types.
                *****************/
    case DTYPE_MISSING:
      if (in_ptr->class != CLASS_XD)
	status = StrAppend(out_ptr, (struct descriptor *)&MISSING);
      break;
    case DTYPE_IDENT:
      t2 = *in_ptr;
      t2.dtype = DTYPE_T;
      status = StrAppend(out_ptr, (struct descriptor *)&t2);
      break;

		/*********
                MDS types.
                *********/
    case DTYPE_NID:
      {
	char *path = TreeGetMinimumPath(0, *(int *)in_ptr->pointer);
	if (path != NULL) {
	  DESCRIPTOR_FROM_CSTRING(path_d, path);
	  status = StrAppend(out_ptr, (struct descriptor *)&path_d);
	  TreeFree(path);
	} else
	  status = TreeFAILURE;
      }
      break;
    case DTYPE_PATH:
			/*****************************************************
                        Convert forward refs to nid to reduce to minimum path.
                        Then back to string. If it fails, use path as is.
                        *****************************************************/
      {
	int nid;
	char *path = MdsDescrToCstring((struct descriptor *)in_ptr);
	status = TreeFindNode(path, &nid);
	MdsFree(path);
	if STATUS_OK
	  path = TreeGetMinimumPath(0, nid);
	if (path == NULL)
	  status = TreeFAILURE;
	if STATUS_OK {
	  DESCRIPTOR_FROM_CSTRING(path_d, path);
	  status = StrAppend(out_ptr, (struct descriptor *)&path_d);
	  TreeFree(path);
	} else
	  status = StrAppend(out_ptr, (struct descriptor *)in_ptr);
      }
      break;
    case DTYPE_EVENT:
      t2 = *in_ptr;
      t2.dtype = DTYPE_T;
      status =
	  StrConcat((struct descriptor *)out_ptr,
		    (struct descriptor *)out_ptr, &BUILD_EVENT, &t2, &CLOSE_EVENT MDS_END_ARG);
      break;
    case DTYPE_POINTER:{
	char outstr[11+2*in_ptr->length];
	struct descriptor out = { 0, DTYPE_T, CLASS_S, outstr };
        if (in_ptr->length==4)
          out.length = sprintf(outstr, "Pointer(%#"PRIx32")", *(uint32_t *)in_ptr->pointer);
        else
          out.length = sprintf(outstr, "Pointer(%#"PRIx64")", *(uint64_t *)in_ptr->pointer);
	status = StrAppend(out_ptr, (struct descriptor *)&out);
	break;
      }
    }
    break;

	/***********
        MDS records.
        ***********/
  case CLASS_R:
    status = Tdi0Decompile_R(in_ptr, prec, out_ptr);
    break;

	/**********************************
        Compressed data should be expanded.
        **********************************/
  case CLASS_CA:
    {
      struct descriptor_xd tmp = EMPTY_XD;
      status = TdiEvaluate(in_ptr, &tmp MDS_END_ARG);
      if STATUS_OK
	status = Tdi0Decompile(tmp.pointer, prec, out_ptr);
      MdsFree1Dx(&tmp, NULL);
    }
    break;

	/******************
        Arrays in brackets.
        ******************/
  case CLASS_APD:
  case CLASS_A:
    {
      array_bounds_desc *a_ptr = (array_bounds_desc *) in_ptr;
      int length = a_ptr->length;
      int coeff = a_ptr->aflags.coeff;
      int dimct = coeff ? a_ptr->dimct : 1;
      unsigned int count = (int)a_ptr->arsize / max(1, length);
      int more = count > TdiDECOMPILE_MAX || a_ptr->arsize >= 32768;

		/**************************************
                Special data types made easier to read.
                **************************************/
      switch (dtype) {
      case DTYPE_BU:
	bptr = "Byte_Unsigned(";
	break;
      case DTYPE_WU:
	bptr = "Word_Unsigned(";
	break;
      case DTYPE_LU:
	bptr = "Long_Unsigned(";
	break;
      case DTYPE_B:
	bptr = "Byte(";
	break;
      case DTYPE_W:
	bptr = "Word(";
	break;
      default:
	bptr = 0;
	break;
      }
      if (bptr) {
	struct descriptor text = { 0, DTYPE_T, CLASS_S, 0 };
	text.length = (unsigned short)strlen(bptr);
	text.pointer = bptr;
	status = StrAppend(out_ptr, (struct descriptor *)&text);
      }

		/*****************************************
                Specify bounds of array. SET_RANGE(l:u,...
                *****************************************/
      if (a_ptr->aflags.bounds) {
	more = 1;
	status = StrAppend(out_ptr, (struct descriptor *)&SET_RANGE);
	for (j = 0; j < dimct; ++j) {
	  if STATUS_OK
	    status = TdiSingle(a_ptr->m[dimct + 2 * j], out_ptr);
	  if STATUS_OK
	    status = StrAppend(out_ptr, (struct descriptor *)&COLON);
	  if STATUS_OK
	    status = TdiSingle(a_ptr->m[dimct + 2 * j + 1], out_ptr);
	  if STATUS_OK
	    status = StrAppend(out_ptr, (struct descriptor *)&COMMA);
	}
      }

		/******************************************
                Specify shape of array. SET_RANGE(size, ...
                ******************************************/
      else if (more) {
	status = StrAppend(out_ptr, (struct descriptor *)&SET_RANGE);
	for (j = 0; j < dimct; ++j) {
	  if STATUS_OK
	    status = TdiSingle(coeff ? a_ptr->m[j] : count, out_ptr);
	  if STATUS_OK
	    status = StrAppend(out_ptr, (struct descriptor *)&COMMA);
	}
      }

		/*********************************
                Specify data of array. [value,...]
                *********************************/
      if (count > TdiDECOMPILE_MAX) {
	struct descriptor one = *in_ptr;
	one.class = CLASS_S;
	if STATUS_OK
	  status = Tdi0Decompile(&one, P_ARG, out_ptr);
	if STATUS_OK
	  status = StrAppend(out_ptr, (struct descriptor *)&MORE);
      } else {
	char *pitem = (char *)a_ptr->pointer;
	status = tdi_vector((struct descriptor *)a_ptr, dimct - 1, &pitem, out_ptr);
      }
      if (more && STATUS_OK)
	status = StrAppend(out_ptr, (struct descriptor *)&RIGHT_PAREN);
      if (bptr && STATUS_OK)
	status = StrAppend(out_ptr, (struct descriptor *)&RIGHT_PAREN);
    }
    break;
  }				/*switch class */
  if STATUS_NOT_OK
    TdiTrace(OpcDecompile, 1, in_ptr, out_ptr);
  return status;
}
