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
/*      TdiIntrinsic.C
        Dispatch internal functions to their routines.
        These are extensions of the basic types.
        All operation must be defined in our table.
        Each standard FUNCTION is called:
                status = Tdi1name(opcode, narg, &list, &out)
        NOTE first and second arguments are passed by value.
        The returned expression is not re-evaluated.
        This is used by Tdi1Evaluate for expression evaluation
        and by TDISHR$SHARE.MAR for external calls.
        If output is class XD, free it when we can and make it the result.
        If output is class D, then try to grab S or D class. Type is set by data.
        If output is class S or A, then try to convert into it. Only VMS result types.
        Can use this to get scalar text of a long by:
                DESCRIPTOR(output_dsc, "12345678");
                status = TdiLong(&input_dsc, &output_dsc);
        If input_dsc describes -123.4, then output_dsc will describe "bbbb-123".
        WARNING, this depends on string writable.
        WARNING, STR$ routines signal rather that return the error.

        Ken Klare, LANL P-4     (c)1989,1990,1991,1992
*/
#define PREC_COMMA 92
#define MAXLINE 120
#define MAXFRAC 40
#define MINMAX(min, test, max) ((min) >= (test) ? (min) : (test) < (max) ? (test) : (max))
#define DEF_FREEXD
#define DEF_FREEBEGIN

#include <STATICdef.h>
#include "tdithreadsafe.h"
#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefstandard.h"
#include "tdirefzone.h"
#include <string.h>
#include <strroutines.h>
#include <libroutines.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tdishr_messages.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
typedef struct _bounds {
  int l;
  int u;
} BOUNDS;

#define _MOVC3(a,b,c) memcpy(c,b,a)
extern unsigned short OpcCompile;
extern int TdiFaultHandlerNoFixup();
extern int Tdi0Decompile();
extern int TdiConvert();
extern int TdiGetLong();
extern int SysGetMsg();
STATIC_ROUTINE struct descriptor *FixedArray();

STATIC_CONSTANT DESCRIPTOR(compile_err, "%TDI Syntax error near # marked region\n");
STATIC_CONSTANT DESCRIPTOR(hilite, "##");
STATIC_CONSTANT DESCRIPTOR(newline, "\n");
STATIC_CONSTANT struct descriptor miss_dsc = { 0, DTYPE_MISSING, CLASS_S, 0 };

/****************************
Explain in 300 words or less.
****************************/
#define MAXMESS 1800
STATIC_ROUTINE void add(char *text)
{
  struct descriptor_d new = { 0, DTYPE_T, CLASS_D, 0 };
  struct descriptor_d *message = &((TdiGetThreadStatic())->TdiIntrinsic_message);
  new.length = (unsigned short)strlen(text);
  new.pointer = text;
  if (message->length + new.length < MAXMESS)
    StrAppend(message, (struct descriptor *)&new);
}

STATIC_ROUTINE void numb(int count)
{
  STATIC_CONSTANT char blanks[] = "            ";
  char val[sizeof(blanks)], *pval;
  strcpy(val, blanks);
  pval = &val[sizeof(val) - 1];
  if (count < 0)
    *--pval = '-';
  else if (count == 0)
    *--pval = '0';
  else
    for (; count > 0 && pval > val; count /= 10)
      *--pval = (char)(count % 10 + '0');
  if (pval > val)
    *--pval = ' ';
  add(pval);
}

/***************************************************
Danger: this routine is used by DECOMPILE to report.
***************************************************/
int TdiTrace(int opcode __attribute__ ((unused)),
	     int narg __attribute__ ((unused)),
	     struct descriptor *list[] __attribute__ ((unused)),
	     struct descriptor_xd *out_ptr)
{
  struct descriptor_d *message = &(TdiGetThreadStatic()->TdiIntrinsic_message);
  if (message->length > MAXMESS)
    return MDSplusERROR;
  add("%TDI Decompile text_length");
  numb(out_ptr->length);
  add(" partial text: ");
  if (out_ptr->length < MAXLINE - 70)
    StrAppend(message, (struct descriptor *)out_ptr);
  else {
    *((char *)out_ptr->pointer + MAXLINE - 70) = '\0';
    add((char *)out_ptr->pointer);
  }
  return MDSplusSUCCESS;
}

int TRACE(int opcode, int narg,
	  struct descriptor *list[],
	  struct descriptor_xd *out_ptr __attribute__ ((unused)))
{
  int j;
  struct descriptor_d text = { 0, DTYPE_T, CLASS_D, 0 };
  struct descriptor_d *message = &((TdiGetThreadStatic())->TdiIntrinsic_message);
  unsigned short now = message->length;

  if (now > MAXMESS)
    return C_ERROR;
  if (opcode >= 0 && opcode <= TdiFUNCTION_MAX) {
    struct TdiFunctionStruct *pfun = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
    if (narg < pfun->m1 || narg > pfun->m2) {
      add("%TDI Requires");
      numb(pfun->m1);
      if (pfun->m1 != pfun->m2) {
	add(" to");
	numb(pfun->m2);
      }
      add(" input arguments for ");
    } else
      add("%TDI Error in ");
    add(pfun->name);
  } else
    add("%TDI Unknown opcode ");
  add("(");
  for (j = 0; j < narg;) {
    if IS_OK(Tdi0Decompile(list[j], PREC_COMMA, &text, 5)) {
      if (message->length - now + text.length < MAXLINE - 2)
	StrAppend(message, (struct descriptor *)&text);
      else {
	*(text.pointer + MAXFRAC) = '\0';
	add(text.pointer);
	add(" ...");
      }
    } else
      add("BAD_INPUT");
    StrFree1Dx(&text);
    if (++j < narg) {
      if (message->length - now < MAXLINE - MAXFRAC - 7)
	add(", ");
      else {
	add(",\n");
	now = message->length;
	add("%- ");
      }
    }
  }
  add(")\n");
  return C_OK;
}

/**********************************
Useful for access violation errors.
**********************************/
STATIC_ROUTINE int interlude(int (*f1) (), int opcode, int narg,
                             struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  return (*f1) (opcode, narg, list, out_ptr);
}

struct _fixed {
  int n;
  char f[256];
  struct descriptor *a[256];
};
void cleanup_list(void* fixed_in) {
  struct _fixed* fixed = (struct _fixed*)fixed_in;
    for (; --fixed->n >= 0 ;)
      if (fixed->f[fixed->n])
	free(fixed->a[fixed->n]);
}

EXPORT int TdiIntrinsic(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS, stat1 = MDSplusSUCCESS;
  struct TdiFunctionStruct *fun_ptr = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
  GET_TDITHREADSTATIC_P;
  EMPTYXD(tmp);
  FREEXD_ON_EXIT(&tmp);
  FREEXD_ON_EXIT(out_ptr);
  FREEBEGIN_ON_EXIT();
  struct descriptor *dsc_ptr;
  struct descriptor_d *message = &(TdiThreadStatic_p->TdiIntrinsic_message);
  TdiThreadStatic_p->TdiIntrinsic_recursion_count++;
  if (narg < fun_ptr->m1)
    status = TdiMISS_ARG;
  else if (narg > fun_ptr->m2)
    status = TdiEXTRA_ARG;
  else if (TdiThreadStatic_p->TdiIntrinsic_recursion_count > 1800)
    status = TdiRECURSIVE;
  else {
    struct _fixed fixed = {0};
    pthread_cleanup_push(cleanup_list,&fixed);
    for (fixed.n = 0; fixed.n < narg; fixed.n++)
      if (list[fixed.n] != NULL && list[fixed.n]->class == CLASS_NCA) {
	fixed.f[fixed.n] = 1;
	fixed.a[fixed.n] = FixedArray(list[fixed.n]);
      } else {
	fixed.f[fixed.n] = 0;
	fixed.a[fixed.n] = list[fixed.n];
      }
    status = interlude(fun_ptr->f1, opcode, narg, fixed.a, &tmp);
    pthread_cleanup_pop(1);
  }
  if (STATUS_OK || status == TdiBREAK || status == TdiCONTINUE || status == TdiGOTO || status == TdiRETURN) {
    if (!out_ptr)
      goto notmp;
    switch (out_ptr->class) {
    default:
      status = TdiINVCLADSC;
      break;
    case CLASS_XD:
      /*****************************************
      To XD, rename it if from XD, else make it.
      *****************************************/
      /*************************************************************************
      WARNING could be tricky if an input is same as output and propagated thru.
      If out overlaps tmp, then some input is the same as the output
      and the input was freed, so don't free output.
      *************************************************************************/
      if ((char *)out_ptr->pointer + out_ptr->l_length <=
	  (char *)tmp.pointer || (char *)out_ptr->pointer >= (char *)tmp.pointer + tmp.l_length)
	MdsFree1Dx(out_ptr, NULL);
      else if (out_ptr->l_length) {
	add("%TDI DANGER, part of old output descriptor was input to below.\n");
	TRACE(opcode, narg, list, out_ptr);
      }
      if (tmp.class == CLASS_XD)
	*out_ptr = tmp;
      else {
	stat1 = MdsCopyDxXd((struct descriptor *)&tmp, out_ptr);
	MdsFree1Dx(&tmp, NULL);
      }
      break;
    case CLASS_D:
      /************************************
      To D, allocate and copy it (free XD).
      ************************************/
      if (tmp.class == CLASS_XD)
	dsc_ptr = (struct descriptor *)tmp.pointer;
      else
	dsc_ptr = (struct descriptor *)&tmp;
      if (dsc_ptr == 0)
	stat1 = StrFree1Dx((struct descriptor_d *)out_ptr);
      else
	switch (dsc_ptr->class) {
	case CLASS_S:
	case CLASS_D:
	  if (out_ptr->length != dsc_ptr->length) {
	    stat1 = StrGet1Dx(&dsc_ptr->length, (struct descriptor_d *)out_ptr);
	  }
	  if IS_OK(stat1) {
	    out_ptr->dtype = dsc_ptr->dtype;
	    if ((out_ptr->length > 0) && (dsc_ptr != NULL))
	      _MOVC3(out_ptr->length, dsc_ptr->pointer, (char *)out_ptr->pointer);
	  }
	  break;
	default:
	  stat1 = TdiINVCLADSC;
	  break;
	}
      MdsFree1Dx(&tmp, NULL);
      break;
    case CLASS_S:
    case CLASS_A:
      /*******************************
      To S and A, convert (free if from XD).
      *******************************/
      if (tmp.class == CLASS_XD)
	dsc_ptr = tmp.pointer;
      else
	dsc_ptr = (struct descriptor *)&tmp;
      if (dsc_ptr)
	stat1 = TdiConvert(dsc_ptr, out_ptr);
      else
	stat1 = TdiConvert(&miss_dsc, out_ptr);
      MdsFree1Dx(&tmp, NULL);
      break;
    case CLASS_NCA:
      {
	struct descriptor *fixed_out_ptr = FixedArray(out_ptr);
	if (tmp.class == CLASS_XD)
	  dsc_ptr = tmp.pointer;
	else
	  dsc_ptr = (struct descriptor *)&tmp;
	if (dsc_ptr)
	  stat1 = TdiConvert(dsc_ptr, out_ptr);
	else
	  stat1 = TdiConvert(&miss_dsc, out_ptr);
	MdsFree1Dx(&tmp, NULL);
	free(fixed_out_ptr);
      }
      break;
    }
    if (stat1 & 1)
      goto done;
    status = stat1;
  }
  TRACE(opcode, narg, list, out_ptr);
  /********************************
  Compiler errors get special help.
  ********************************/
  if (opcode == OpcCompile && message->length < MAXMESS && TdiRefZone.a_begin) {
    struct descriptor pre = { 0, DTYPE_T, CLASS_S, 0 };
    struct descriptor body = { 0, DTYPE_T, CLASS_S, 0 };
    struct descriptor post = { 0, DTYPE_T, CLASS_S, 0 };
    // b------x----c----e
    // '-l_ok-'-xc-'-ce-'
    char *b = TdiRefZone.a_begin;
    char *e = TdiRefZone.a_end;
    char *c = MINMAX(b, TdiRefZone.a_cur, e);
    char *x = MINMAX(b, b + TdiRefZone.l_ok, c);
    body.length = (unsigned short)MINMAX(0, c-x, MAXLINE);
    body.pointer = body.length>0 ? x : NULL;
    post.length = (unsigned short)MINMAX(0, e-c, MAXLINE);
    if (body.length + post.length > MAXLINE)
        post.length = MINMAX(0, post.length, MAXFRAC);
    post.pointer = post.length>0 ? c : NULL;
    pre.length = (unsigned short)MINMAX(0, x-b, MAXLINE);
    if (pre.length + body.length + post.length > MAXLINE)
        pre.length = 0;
    pre.pointer = pre.length>0 ? x - pre.length : NULL;
    StrConcat((struct descriptor *)message,
              (struct descriptor *)message, &compile_err, &pre, &hilite,
              &body, &hilite, &post, &newline MDS_END_ARG);
    freebegin(&TdiRefZone);
  }
  if (out_ptr)
    MdsFree1Dx(out_ptr, NULL);
 notmp:MdsFree1Dx(&tmp, NULL);
 done:TdiThreadStatic_p->TdiIntrinsic_recursion_count--;
  TdiThreadStatic_p->TdiIntrinsic_mess_stat = status;
  if (!TdiThreadStatic_p->TdiIntrinsic_recursion_count)
    freebegin(&TdiRefZone);
  FREE_CANCEL(&tmp);
  FREE_CANCEL(out_ptr);
  FREE_CANCEL(a_begin);
  return status;
}

/*--------------------------------------------------------------
        Set debugging printout.
                message = DEBUG([option])
        Where option is bitwise combination of:
                1 to prepend first error message
                2 to print the current message
                4 to clear the message buffer
*/
int Tdi1Debug(int opcode __attribute__ ((unused)),
	      int narg,
	      struct descriptor *list[],
	      struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  GET_TDITHREADSTATIC_P;
  int option = -1;
  int mess_stat = TdiThreadStatic_p->TdiIntrinsic_mess_stat;
  struct descriptor_d *message = &TdiThreadStatic_p->TdiIntrinsic_message;
  if (narg > 0 && list[0])
    status = TdiGetLong(list[0], &option);
  if (option & 1 && mess_stat != 1) {
    struct descriptor dmsg = { 0, DTYPE_T, CLASS_S, 0 };
    dmsg.pointer = MdsGetMsg(mess_stat);
    dmsg.length = strlen(dmsg.pointer);
    StrConcat((struct descriptor *)message, &dmsg, &newline, message MDS_END_ARG);
  }
  if (message->length) {
    if (option & 2)
      printf("%.*s", message->length, message->pointer);
    if (option & 4)
      TdiThreadStatic_p->TdiIntrinsic_mess_stat = StrFree1Dx(message);
  }
  status = MdsCopyDxXd((struct descriptor *)message, out_ptr);
  return status;
}

STATIC_ROUTINE struct descriptor *FixedArray(struct descriptor *in)
{
  array_coeff *a = (array_coeff *) in;
  int dsize = sizeof(struct descriptor_a) + sizeof(int) + 3 * sizeof(int) * a->dimct;
  int i;
  BOUNDS *bounds = (BOUNDS *) & a->m[a->dimct];
  array_coeff *answer = (array_coeff *) memcpy(malloc(dsize), a, dsize);
  answer->class = CLASS_A;
  answer->aflags.column = 1;
  answer->aflags.coeff = 1;
  answer->aflags.bounds = 1;
  for (i = 0; i < a->dimct; i++)
    answer->m[i] = bounds[i].u - bounds[i].l + 1;
  return (struct descriptor *)answer;
}
