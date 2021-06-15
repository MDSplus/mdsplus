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
/*      TdiYaccSubs.c
        Subroutines for the expression parser.

        Ken Klare, LANL CTR-7   (c)1989,1990
*/

#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefstandard.h"
#include "tdirefzone.h"
#include <libroutines.h>
#include <mdsdescrip.h>
#include <mdsplus/mdsplus.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <tdishr_messages.h>

//#define DEBUG
#include <mdsmsg.h>

extern int Tdi1Build();
extern int TdiEvaluate();

int tdi_yacc_IMMEDIATE();
int tdi_yacc_RESOLVE();

/*--------------------------------------------------------------
        Allow precomputed expressions to be included in the compile string.
        Limitation: 253 expressions plus compile string and output descriptor.
        Use an input argument for BUILD_FUNCTION as element of expression.
        Do not evaluate the argument. It's up to them to do that.
        $ is next argument, 0 increments to 1.
        $nnn is nnn-th argument.
        $0 is then the compile string itself.
*/
int tdi_yacc_ARG(struct marker *mark_ptr, TDITHREADSTATIC_ARG)
{
  INIT_STATUS;
  mdsdsc_t *ptr;
  mdsdsc_xd_t junk = EMPTY_XD;
  unsigned int len = mark_ptr->rptr->length;
  unsigned char *c_ptr;

  if (len == 1)
    ++TDI_REFZONE.l_iarg;
  else
  {
    TDI_REFZONE.l_iarg = 0;
    c_ptr = mark_ptr->rptr->pointer;
    for (; --len > 0;)
      TDI_REFZONE.l_iarg = TDI_REFZONE.l_iarg * 10 + (*++c_ptr - '0');
  }
  if (TDI_REFZONE.l_iarg > TDI_REFZONE.l_narg)
    return TdiMISS_ARG;
  ptr = TDI_REFZONE.a_list[TDI_REFZONE.l_iarg];
  mark_ptr->builtin = -1;

  /*******************************
  Size it and copy it to our zone.
  *******************************/
  status =
      MdsCopyDxXdZ(ptr, &junk, &TDI_REFZONE.l_zone, NULL, NULL, NULL, NULL);
  if (STATUS_OK)
    mark_ptr->rptr = (mdsdsc_r_t *)junk.pointer;
  return status;
}

/*--------------------------------------------------------------
        Build a descriptor, return its pointer for intrinsic functions of TDI.
        Get memory from compiler-defined virtual memory.
*/
static const DESCRIPTOR_FUNCTION_0(EMPTY_FUN, 0);

int tdi_yacc_BUILD(int ndesc, int nused, opcode_t opcode, struct marker *out,
                   struct marker *arg1, struct marker *arg2,
                   struct marker *arg3, struct marker *arg4,
                   TDITHREADSTATIC_ARG)
{
  mds_function_t *fun;
  int dsc_size = sizeof(mds_function_t) + sizeof(mdsdsc_t *) * (ndesc - 1);
  unsigned int vm_size = dsc_size + sizeof(unsigned short);
  struct TdiFunctionStruct *this_ptr =
      (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
  int status;
  RETURN_IF_NOT_OK(LibGetVm(&vm_size, (void **)&fun, &TDI_REFZONE.l_zone));
  out->builtin = -1;
  out->rptr = (mdsdsc_r_t *)fun;

  *fun = EMPTY_FUN; /* clears ndesc upper bits */
  fun->pointer = (opcode_t *)((char *)fun + dsc_size);
  *fun->pointer = opcode;
  fun->ndesc = (unsigned char)nused;
  switch (nused)
  {
  default:
    return TdiEXTRA_ARG;
  case 4:
    fun->arguments[3] = (mdsdsc_t *)arg4->rptr;
    MDS_ATTR_FALLTHROUGH
  case 3:
    fun->arguments[2] = (mdsdsc_t *)arg3->rptr;
    MDS_ATTR_FALLTHROUGH
  case 2:
    fun->arguments[1] = (mdsdsc_t *)arg2->rptr;
    MDS_ATTR_FALLTHROUGH
  case 1:
    fun->arguments[0] = (mdsdsc_t *)arg1->rptr;
    MDS_ATTR_FALLTHROUGH
  case 0:
    break;
  }

  /*******************************************
  Cannot resolve variable length when created.
  Cannot check external functions.
  If resolved, can change record pointer.
  *******************************************/
  if (nused > this_ptr->m2)
    return tdi_yacc_IMMEDIATE(&out->rptr, TDITHREADSTATIC_VAR);
  /*Force an error */
  if (ndesc >= 254)
    return MDSplusSUCCESS;
  if (nused < this_ptr->m1)
    return tdi_yacc_IMMEDIATE(&out->rptr, TDITHREADSTATIC_VAR);
  /*Force an error */
  return tdi_yacc_RESOLVE(&out->rptr, TDITHREADSTATIC_VAR);
}

/*--------------------------------------------------------------
        Grind out the answer now.
        WARNING the pointer is changed.
        We do not free small stuff because we will throw it all away later.
*/
int tdi_yacc_IMMEDIATE(mdsdsc_xd_t **dsc_ptr_ptr, TDITHREADSTATIC_ARG)
{
  if (TDI_STACK_IDX >= TDI_STACK_SIZE - 1)
  {
    fprintf(stderr,
            "Error: Too many recursive calls using '`': only %d supported\n",
            TDI_STACK_SIZE);
    return TdiRECURSIVE;
  }
  mdsdsc_xd_t xd = EMPTY_XD, junk = EMPTY_XD, *ptr = *dsc_ptr_ptr;
  /*********************************************************
  Must not send compile XD-DSC to MDS, it may give it back.
  And if we release it below, we have trouble.
  *********************************************************/
  while (ptr && ptr->class == CLASS_XD && ptr->dtype == DTYPE_DSC)
    ptr = (mdsdsc_xd_t *)ptr->pointer;

  ++TDI_STACK_IDX;
  MDSDBG("TDI_STACK_IDX = %d\n", TDI_STACK_IDX);
  int status = TdiEvaluate(ptr, &xd MDS_END_ARG);
  --TDI_STACK_IDX;
  MDSDBG("TDI_STACK_IDX = %d\n", TDI_STACK_IDX);

  /*******************
  Copy it to our zone.
  *******************/
  if (STATUS_OK)
    status = MdsCopyDxXdZ(xd.pointer, &junk, &TDI_REFZONE.l_zone, NULL, NULL,
                          NULL, NULL);
  if (STATUS_OK)
    *dsc_ptr_ptr = (mdsdsc_xd_t *)junk.pointer;
  MdsFree1Dx(&xd, NULL);
  return status;
}

/*--------------------------------------------------------------
        Automatic evaluation of constants does what is safe to do.
        Examples: F_COMPLEX(1,5) becomes DTYPE_FC: CMPLX(.1E1,.5E1).

        Basic rule: make decompiled code look like original except type
                conversions, but as efficient as possible.

        We go for it only if all arguments are standard types and
                the function has known properties.
        We do it one step at a time so we don't see function types.

        Don't do DATE_TIME, F$GETSYI, GETNCI, and WAIT, change with time.
        Don't do ARRAY opcodes like RAMP, could require large storage.
        Do type conversions like BYTE(7) to 7B.
        Do AINT group: FLOOR NINT etc. NEED/should we?
        Do BUILD_PATH and BUILD_EVENT only for VMS.
        Do BUILD_classR always because they may have classR arguments and
                are do no evaluation and we want descriptor not function.
        Do xxx_OF which must not have VMS type classR as argument.
                Exempt: DIM_OF, CLASS_OF, DTYPE_OF, NDESC_OF.
        Do SET_RANGE for VMS or DTYPE_RANGE ranges.
        Do UNARY_PLUS and UNARY_MINUS that was not done in Lex.

        Inefficient storage for CMPLX(`3b..22b) etc. But so is `3..22.
        NEED to worry about CLASS_OF, DTYPE_OF, SCALAR, C-statements.
        What about DTYPE_MISSING? It is generated only by evaluation.
*/

int tdi_yacc_RESOLVE(mds_function_t **out_ptr_ptr, TDITHREADSTATIC_ARG)
{
  mds_function_t *out_ptr = *out_ptr_ptr;
  struct TdiFunctionStruct *this_ptr;
  if (out_ptr == 0 || out_ptr->dtype != DTYPE_FUNCTION)
    return MDSplusSUCCESS;
  int j, ndesc = out_ptr->ndesc;
  opcode_t opcode = *(opcode_t *)out_ptr->pointer;
  this_ptr = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
  if (ndesc < this_ptr->m1 || ndesc > this_ptr->m2)
    goto doit; /*force an error */
  if ((this_ptr->token & LEX_K_IMMED) == 0)
    return MDSplusSUCCESS;
  /******************
  Test for VMS types.
  BUILD_xxx always.
  SET_RANGE not last.
  Don't expand scalars.
  ******************/
  if (this_ptr->f1 != Tdi1Build)
    for (j = ndesc; --j >= 0;)
    {
      mdsdsc_t *tst = out_ptr->arguments[j];
      while (tst != 0 && tst->dtype == DTYPE_DSC)
        tst = (mdsdsc_t *)tst->pointer;
      if (opcode == OPC_SET_RANGE)
      { /*Set_Range(value,range,...,array) */
        if (j != ndesc - 1)
        {
          if (tst == 0 || tst->dtype == DTYPE_RANGE)
            continue;
        }
        else if (tst && tst->class != CLASS_A)
          return MDSplusSUCCESS; /*decide at runtime */
      }
      if (tst && tst->dtype >= TdiCAT_MAX)
        return MDSplusSUCCESS;
    }
doit:
  return tdi_yacc_IMMEDIATE((mdsdsc_xd_t **)out_ptr_ptr, TDITHREADSTATIC_VAR);
}
