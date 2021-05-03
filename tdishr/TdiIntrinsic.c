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
        If output is class D, then try to grab S or D class. Type is set by
   data. If output is class S or A, then try to convert into it. Only VMS result
   types. Can use this to get scalar text of a long by: DESCRIPTOR(output_dsc,
   "12345678"); status = TdiLong(&input_dsc, &output_dsc); If input_dsc
   describes -123.4, then output_dsc will describe "bbbb-123". WARNING, this
   depends on string writable. WARNING, STR$ routines signal rather that return
   the error.

        Ken Klare, LANL P-4     (c)1989,1990,1991,1992
*/
#define PREC_COMMA 92
#define MAXLINE 120
#define MAXFRAC 40
#define MINMAX(min, test, max) \
  ((min) >= (test) ? (min) : (test) < (max) ? (test) : (max))
#define OPC_ENUM

#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefstandard.h"
#include "tdirefzone.h"
#include "tdithreadstatic.h"
#include <libroutines.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strroutines.h>
#include <tdishr_messages.h>
#include <treeshr.h>
typedef struct _bounds
{
  int l;
  int u;
} BOUNDS;

extern int TdiFaultHandlerNoFixup();
extern int Tdi0Decompile();
extern int TdiConvert();
extern int TdiGetLong();
extern int SysGetMsg();

static mdsdsc_t *fixed_array(mdsdsc_t *in)
{
  array_coeff *a = (array_coeff *)in;
  int dsize = sizeof(mdsdsc_a_t) + sizeof(int) + 3 * sizeof(int) * a->dimct;
  int i;
  BOUNDS *bounds = (BOUNDS *)&a->m[a->dimct];
  array_coeff *answer = (array_coeff *)memcpy(malloc(dsize), a, dsize);
  answer->class = CLASS_A;
  answer->aflags.column = 1;
  answer->aflags.coeff = 1;
  answer->aflags.bounds = 1;
  for (i = 0; i < a->dimct; i++)
    answer->m[i] = bounds[i].u - bounds[i].l + 1;
  return (mdsdsc_t *)answer;
}

static const mdsdsc_t miss_dsc = {0, DTYPE_MISSING, CLASS_S, 0};

/****************************
Explain in 300 words or less.
****************************/
#define MAXMESS 1800
#define ADD(text) add(text, TDITHREADSTATIC_VAR)
static void add(char *const text, TDITHREADSTATIC_ARG)
{
  mdsdsc_d_t new = {0, DTYPE_T, CLASS_D, 0};
  new.length = (length_t)strlen(text);
  new.pointer = text;
  if (TDI_INTRINSIC_MSG.length + new.length < MAXMESS)
    StrAppend(&TDI_INTRINSIC_MSG, (mdsdsc_t *)&new);
}
#define NUMB(count) numb(count, TDITHREADSTATIC_VAR)
static void numb(int count, TDITHREADSTATIC_ARG)
{
  char val[16];
  sprintf(val, "%d", count);
  ADD(val);
}

/***************************************************
Danger: this routine is used by DECOMPILE to report.
***************************************************/
int tdi_trace(mdsdsc_xd_t *out_ptr)
{
  TDITHREADSTATIC_INIT;
  if (TDI_INTRINSIC_MSG.length > MAXMESS)
    return MDSplusERROR;
  ADD("%TDI Decompile text_length");
  NUMB(out_ptr->length);
  ADD(" partial text: ");
  if (out_ptr->length < MAXLINE - 70)
  {
    StrAppend(&TDI_INTRINSIC_MSG, (mdsdsc_t *)out_ptr);
  }
  else
  {
    *((char *)out_ptr->pointer + MAXLINE - 70) = '\0';
    ADD((char *)out_ptr->pointer);
  }
  return MDSplusSUCCESS;
}

#define TRACE(opcode, narg, list) trace(opcode, narg, list, TDITHREADSTATIC_VAR)
static inline void trace(opcode_t opcode, int narg, mdsdsc_t *list[],
                         TDITHREADSTATIC_ARG)
{
  if (TDI_INTRINSIC_MSG.length >= MAXMESS)
    return;
  unsigned short now = TDI_INTRINSIC_MSG.length;
  int j;
  mdsdsc_d_t text = {0, DTYPE_T, CLASS_D, 0};
  if (opcode < TdiFUNCTION_MAX)
  {
    struct TdiFunctionStruct *pfun =
        (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
    if (narg < pfun->m1 || narg > pfun->m2)
    {
      if (pfun->m1 != pfun->m2)
        ADD("%TDI Requires ");
      NUMB(pfun->m1);
      if (pfun->m1 != pfun->m2)
      {
        ADD(" to ");
        NUMB(pfun->m2);
      }
      ADD(" input arguments for ");
    }
    else
      ADD("%TDI Error in ");
    ADD(pfun->name);
  }
  else
    ADD("%TDI Unknown opcode ");
  ADD("(");
  for (j = 0; j < narg;)
  {
    if (IS_OK(Tdi0Decompile(list[j], PREC_COMMA, &text, 5)))
    {
      if (TDI_INTRINSIC_MSG.length - now + text.length < MAXLINE - 2)
        StrAppend(&TDI_INTRINSIC_MSG, (mdsdsc_t *)&text);
      else
      {
        *(text.pointer + MAXFRAC) = '\0';
        ADD(text.pointer);
        ADD(" ...");
      }
    }
    else
      ADD("BAD_INPUT");
    StrFree1Dx(&text);
    if (++j < narg)
    {
      if (TDI_INTRINSIC_MSG.length - now < MAXLINE - MAXFRAC - 7)
        ADD(", ");
      else
      {
        ADD(",\n");
        now = TDI_INTRINSIC_MSG.length;
        ADD("%- ");
      }
    }
  }
  ADD(")\n");
}

typedef struct
{
  int n;
  char f[256];
  mdsdsc_t *a[256];
  int *rec;
} fixed_t;
static void cleanup_list(void *fixed_in)
{
  fixed_t *const fixed = (fixed_t *)fixed_in;
  while (--fixed->n >= 0)
    if (fixed->f[fixed->n])
      free(fixed->a[fixed->n]);
  (*fixed->rec)--;
}

EXPORT int TdiIntrinsic(opcode_t opcode, int narg, mdsdsc_t *list[],
                        mdsdsc_xd_t *out_ptr)
{
  int status;
  struct TdiFunctionStruct *fun_ptr =
      (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
  TDITHREADSTATIC_INIT;
  EMPTYXD(tmp);
  FREEXD_ON_EXIT(&tmp);
  FREEXD_ON_EXIT(out_ptr);
  status = MDSplusSUCCESS;
  mdsdsc_t *dsc_ptr;
  if (narg < fun_ptr->m1)
    status = TdiMISS_ARG;
  else if (narg > fun_ptr->m2)
    status = TdiEXTRA_ARG;
  else if (TDI_INTRINSIC_REC > 1800)
    status = TdiRECURSIVE;
  else
  {
    fixed_t fixed;
    TDI_INTRINSIC_REC++;
    pthread_cleanup_push(cleanup_list, &fixed);
    fixed.rec = &TDI_INTRINSIC_REC;
    for (fixed.n = 0; fixed.n < narg; fixed.n++)
      if (list[fixed.n] != NULL && list[fixed.n]->class == CLASS_NCA)
      {
        fixed.f[fixed.n] = 1;
        fixed.a[fixed.n] = fixed_array(list[fixed.n]);
      }
      else
      {
        fixed.f[fixed.n] = 0;
        fixed.a[fixed.n] = list[fixed.n];
      }
    status = fun_ptr->f1(opcode, narg, fixed.a, &tmp);
    pthread_cleanup_pop(1);
  }
  if (STATUS_OK || status == TdiBREAK || status == TdiCONTINUE ||
      status == TdiGOTO || status == TdiRETURN)
  {
    if (!out_ptr)
      goto notmp;
    int stat1 = MDSplusSUCCESS;
    switch (out_ptr->class)
    {
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
      if ((char *)out_ptr->pointer + out_ptr->l_length <= (char *)tmp.pointer ||
          (char *)out_ptr->pointer >= (char *)tmp.pointer + tmp.l_length)
        MdsFree1Dx(out_ptr, NULL);
      else if (out_ptr->l_length)
      {
        ADD("%TDI DANGER, part of old output descriptor was input to below.\n");
        trace(opcode, narg, list, TDITHREADSTATIC_VAR);
      }
      if (tmp.class == CLASS_XD)
        *out_ptr = tmp;
      else
      {
        stat1 = MdsCopyDxXd((mdsdsc_t *)&tmp, out_ptr);
        MdsFree1Dx(&tmp, NULL);
      }
      break;
    case CLASS_D:
      /************************************
      To D, allocate and copy it (free XD).
      ************************************/
      if (tmp.class == CLASS_XD)
        dsc_ptr = (mdsdsc_t *)tmp.pointer;
      else
        dsc_ptr = (mdsdsc_t *)&tmp;
      if (dsc_ptr == 0)
        stat1 = StrFree1Dx((mdsdsc_d_t *)out_ptr);
      else
        switch (dsc_ptr->class)
        {
        case CLASS_S:
        case CLASS_D:
          if (out_ptr->length != dsc_ptr->length)
          {
            stat1 = StrGet1Dx(&dsc_ptr->length, (mdsdsc_d_t *)out_ptr);
          }
          if (IS_OK(stat1))
          {
            out_ptr->dtype = dsc_ptr->dtype;
            if ((out_ptr->length > 0) && (dsc_ptr != NULL))
              memcpy((char *)out_ptr->pointer, dsc_ptr->pointer,
                     out_ptr->length);
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
        dsc_ptr = (mdsdsc_t *)&tmp;
      if (dsc_ptr)
        stat1 = TdiConvert(dsc_ptr, out_ptr);
      else
        stat1 = TdiConvert(&miss_dsc, out_ptr);
      MdsFree1Dx(&tmp, NULL);
      break;
    case CLASS_NCA:
    {
      mdsdsc_t *fixed_out_ptr = fixed_array((mdsdsc_t *)out_ptr);
      if (tmp.class == CLASS_XD)
        dsc_ptr = tmp.pointer;
      else
        dsc_ptr = (mdsdsc_t *)&tmp;
      if (dsc_ptr)
        stat1 = TdiConvert(dsc_ptr, out_ptr);
      else
        stat1 = TdiConvert(&miss_dsc, out_ptr);
      MdsFree1Dx(&tmp, NULL);
      free(fixed_out_ptr);
    }
    break;
    }
    if (IS_OK(stat1))
      goto done;
    status = stat1;
  }
  if (TDI_INTRINSIC_REC >= 0)
    trace(opcode, narg, list, TDITHREADSTATIC_VAR);
  if (out_ptr)
    MdsFree1Dx(out_ptr, NULL);
notmp:
  MdsFree1Dx(&tmp, NULL);
done:;
  if (TDI_INTRINSIC_REC == 0)
    TDI_INTRINSIC_STAT = status;
  FREE_CANCEL(&tmp);
  FREE_CANCEL(out_ptr);
  return status;
}
EXPORT int _TdiIntrinsic(void **ctx, opcode_t opcode, int narg,
                         mdsdsc_t *list[], mdsdsc_xd_t *out_ptr)
{
  int status;
  CTX_PUSH(ctx);
  status = TdiIntrinsic(opcode, narg, list, out_ptr);
  CTX_POP(ctx);
  return status;
}

/*--------------------------------------------------------------
        Set debugging printout.
                message = DEBUG([option])
        Where option is bitwise combination of:
                1 to prepend first error message
                2 to print the current message
                4 to clear the message buffer
                8 return message before clear
*/
int Tdi1Debug(opcode_t opcode __attribute__((unused)), int narg,
              mdsdsc_t *list[], mdsdsc_xd_t *out_ptr)
{
  INIT_STATUS;
  TDITHREADSTATIC_INIT;
  int option = -1;
  if (narg > 0 && list[0])
    status = TdiGetLong(list[0], &option);
  if (option & 1 && TDI_INTRINSIC_STAT != SsSUCCESS)
  {
    mdsdsc_t dmsg = {0, DTYPE_T, CLASS_S, 0};
    dmsg.pointer = MdsGetMsg(TDI_INTRINSIC_STAT);
    dmsg.length = strlen(dmsg.pointer);
    // in order to prepend we need to move the original message into temp desc
    mdsdsc_d_t oldmsg = TDI_INTRINSIC_MSG;
    TDI_INTRINSIC_MSG.length = 0;
    TDI_INTRINSIC_MSG.pointer = NULL;
    const DESCRIPTOR(newline, "\n");
    StrConcat((mdsdsc_t *)&TDI_INTRINSIC_MSG, &dmsg, &newline,
              &oldmsg MDS_END_ARG);
    StrFree1Dx(&oldmsg);
  }
  if (TDI_INTRINSIC_MSG.length)
  {
    if (option & 2)
      printf("%.*s", TDI_INTRINSIC_MSG.length, TDI_INTRINSIC_MSG.pointer);
    if (option & 4)
    {
      if (option & 8)
        status = MdsCopyDxXd((mdsdsc_t *)&TDI_INTRINSIC_MSG, out_ptr);
      StrFree1Dx(&TDI_INTRINSIC_MSG);
      return status;
    }
  }
  return MdsCopyDxXd((mdsdsc_t *)&TDI_INTRINSIC_MSG, out_ptr);
}
