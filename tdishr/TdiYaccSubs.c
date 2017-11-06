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
/*      TdiYacc_SUBS.C
        Subroutines for the expression parser.

        Ken Klare, LANL CTR-7   (c)1989,1990
*/

extern unsigned short OpcSetRange;

#include <mdsplus/mdsplus.h>
#include "STATICdef.h"
#include <stdlib.h>
#include <mdsdescrip.h>
#include "tdirefstandard.h"
#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefzone.h"
#include <libroutines.h>
#include <tdishr_messages.h>
#include <mdsshr.h>



extern int Tdi1Build();
extern int TdiEvaluate();

int TdiYacc_IMMEDIATE();
int TdiYacc_RESOLVE();

/*--------------------------------------------------------------
        Allow precomputed expressions to be included in the compile string.
        Limitation: 253 expressions plus compile string and output descriptor.
        Use an input argument for BUILD_FUNCTION as element of expression.
        Do not evaluate the argument. It's up to them to do that.
        $ is next argument, 0 increments to 1.
        $nnn is nnn-th argument.
        $0 is then the compile string itself.
*/
int TdiYacc_ARG(struct marker *mark_ptr)
{
  INIT_STATUS;
  GET_TDITHREADSTATIC_P;
  struct descriptor *ptr;
  struct descriptor_xd junk = EMPTY_XD;
  unsigned int len = mark_ptr->rptr->length;
  unsigned char *c_ptr;

  if (len == 1)
    ++TdiRefZone.l_iarg;
  else {
    TdiRefZone.l_iarg = 0;
    c_ptr = mark_ptr->rptr->pointer;
    for (; --len > 0;)
      TdiRefZone.l_iarg = TdiRefZone.l_iarg * 10 + (*++c_ptr - '0');
  }
  if (TdiRefZone.l_iarg > TdiRefZone.l_narg)
    return TdiMISS_ARG;
  ptr = TdiRefZone.a_list[TdiRefZone.l_iarg];
  mark_ptr->builtin = -1;

	/*******************************
        Size it and copy it to our zone.
        *******************************/
  status = MdsCopyDxXdZ(ptr, &junk, &TdiRefZone.l_zone, NULL, NULL, NULL, NULL);
  if (status & 1)
    mark_ptr->rptr = (struct descriptor_r *)junk.pointer;
  return status;
}

/*--------------------------------------------------------------
        Build a descriptor, return its pointer for intrinsic functions of TDI.
        Get memory from compiler-defined virtual memory.
*/
DESCRIPTOR_FUNCTION_0(EMPTY_FUN, 0);

int TdiYacc_BUILD(int ndesc,
		  int nused,
		  int opcode,
		  struct marker *out,
		  struct marker *arg1,
		  struct marker *arg2, struct marker *arg3, struct marker *arg4)
{
  GET_TDITHREADSTATIC_P;
  struct descriptor_function *tmp;
  int dsc_size = sizeof(struct descriptor_function) + sizeof(struct descriptor *) * (ndesc - 1);
  unsigned int vm_size = dsc_size + sizeof(unsigned short);
  struct TdiFunctionStruct *this_ptr = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];

  TdiRefZone.l_status = LibGetVm(&vm_size, (void **)&tmp, &TdiRefZone.l_zone);
  if IS_NOT_OK(TdiRefZone.l_status)
    return MDSplusERROR;
  out->builtin = -1;
  out->rptr = (struct descriptor_r *)tmp;

  *tmp = EMPTY_FUN;		/* clears ndesc upper bits */
  tmp->pointer = (unsigned char *)((char *)tmp + dsc_size);
  *(unsigned short *)tmp->pointer = (unsigned short)opcode;
  tmp->ndesc = (unsigned char)nused;
  switch (nused) {
  default:
    TdiRefZone.l_status = TdiEXTRA_ARG;
    return MDSplusERROR;
  case 4:
    tmp->arguments[3] = (struct descriptor *)arg4->rptr;
    MDS_ATTR_FALLTHROUGH
  case 3:
    tmp->arguments[2] = (struct descriptor *)arg3->rptr;
    MDS_ATTR_FALLTHROUGH
  case 2:
    tmp->arguments[1] = (struct descriptor *)arg2->rptr;
    MDS_ATTR_FALLTHROUGH
  case 1:
    tmp->arguments[0] = (struct descriptor *)arg1->rptr;
    MDS_ATTR_FALLTHROUGH
  case 0:
    break;
  }

	/*******************************************
        Cannot resolve variable length when created.
        Cannot check external functions.
        If resolved, can change record pointer.
        *******************************************/
  if (nused > this_ptr->m2) {
    TdiRefZone.l_status = TdiYacc_IMMEDIATE(&out->rptr);
    return MDSplusERROR;
  }				/*Force an error */
  if (ndesc >= 254)
    return MDSplusSUCCESS;
  if (nused < this_ptr->m1) {
    TdiRefZone.l_status = TdiYacc_IMMEDIATE(&out->rptr);
    return MDSplusERROR;
  }				/*Force an error */
  return TdiYacc_RESOLVE(&out->rptr);
}

/*--------------------------------------------------------------
        Grind out the answer now.
        WARNING the pointer is changed.
        We do not free small stuff because we will throw it all away later.
*/
int TdiYacc_IMMEDIATE(struct descriptor_xd **dsc_ptr_ptr)
{
  GET_TDITHREADSTATIC_P;
  struct descriptor_xd xd = EMPTY_XD, junk = EMPTY_XD, *ptr = *dsc_ptr_ptr;
  int status;

	/*********************************************************
        Must not send compile XD-DSC to MDS, it may give it back.
        And if we release it below, we have trouble.
        *********************************************************/
  while (ptr && ptr->class == CLASS_XD && ptr->dtype == DTYPE_DSC)
    ptr = (struct descriptor_xd *)ptr->pointer;
  status = TdiEvaluate(ptr, &xd MDS_END_ARG);

	/*******************
        Copy it to our zone.
        *******************/
  if STATUS_OK
    status = MdsCopyDxXdZ(xd.pointer, &junk, &TdiRefZone.l_zone, NULL, NULL, NULL, NULL);
  if STATUS_OK
    *dsc_ptr_ptr = (struct descriptor_xd *)junk.pointer;
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

int TdiYacc_RESOLVE(struct descriptor_function **out_ptr_ptr)
{
  GET_TDITHREADSTATIC_P;
  struct descriptor_function *out_ptr = *out_ptr_ptr;
  struct TdiFunctionStruct *this_ptr;
  int j, ndesc, opcode;

  if (out_ptr == 0 || out_ptr->dtype != DTYPE_FUNCTION)
    return MDSplusSUCCESS;
  ndesc = out_ptr->ndesc;
  opcode = *(unsigned short *)out_ptr->pointer;
  this_ptr = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
  if (ndesc < this_ptr->m1 || ndesc > this_ptr->m2)
    goto doit;			/*force an error */
  if ((this_ptr->token & LEX_K_IMMED) == 0)
    return MDSplusSUCCESS;
	/******************
        Test for VMS types.
        BUILD_xxx always.
        SET_RANGE not last.
        Don't expand scalars.
        ******************/
  if (this_ptr->f1 != Tdi1Build)
    for (j = ndesc; --j >= 0;) {
      struct descriptor *tst = out_ptr->arguments[j];
      while (tst != 0 && tst->dtype == DTYPE_DSC)
	tst = (struct descriptor *)tst->pointer;
      if (opcode == OpcSetRange) {	/*Set_Range(value,range,...,array) */
	if (j != ndesc - 1) {
	  if (tst == 0 || tst->dtype == DTYPE_RANGE)
	    continue;
	} else if (tst && tst->class != CLASS_A)
	  return MDSplusSUCCESS;		/*decide at runtime */
      }
      if (tst && tst->dtype >= TdiCAT_MAX)
	return MDSplusSUCCESS;
    }
 doit:
  if IS_OK(TdiRefZone.l_status = TdiYacc_IMMEDIATE((struct descriptor_xd **)out_ptr_ptr))
    return MDSplusSUCCESS;
  return MDSplusERROR;
}
