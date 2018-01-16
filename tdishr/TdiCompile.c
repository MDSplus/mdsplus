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
/*      Tdi1Compile.C
        The interface to compiler.

        Ken Klare, LANL CTR-7   (c)1989,1990
*/

extern unsigned short OpcCompile;
#define DEF_FREEXD

#include <stdlib.h>
#include <string.h>
#include <mdsdescrip.h>
#include "tdirefzone.h"
#include "tdirefstandard.h"
#include <libroutines.h>
#include <tdishr_messages.h>
#include "tdithreadsafe.h"
#include <mdsshr.h>
#include <STATICdef.h>

extern void LockMdsShrMutex(pthread_mutex_t *, int *);
extern void UnlockMdsShrMutex(pthread_mutex_t *);

extern int TdiEvaluate();
extern int TdiYacc();
extern int TdiIntrinsic();
extern void TdiYyReset();
/*-------------------------------------------------------
        Interface to compiler/parser.
                expression = COMPILE(string, [arg1,...])
        YACC converts TdiYacc.Y to TdiYacc.C to parse TDI statements. Also YACC subroutines.
        LEX converts TdiLex.X to TdiLex.C for lexical analysis. Also LEX subroutines.

        Limitations:
        For recursion must pass LEX:
                zone status bol cur end
                tdiyylval tdiyyval
                tdiyytext[YYLMAX] tdiyyleng tdiyymorfg tdiyytchar tdiyyin? tdiyyout?
        For recursion must pass YACC also:
                tdiyydebug? tdiyyv[YYMAXDEPTH] tdiyychar tdiyynerrs tdiyyerrflag
        Thus no recursion because the tdiyy's are built into LEX and YACC.
        IMMEDIATE (`) must never call COMPILE. NEED to prevent this.
*/

int Tdi1Compile(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[],
		struct descriptor_xd *out_ptr)
{
  int status;
  struct descriptor *text_ptr;
  GET_TDITHREADSTATIC_P;
  if (TdiThreadStatic_p->compiler_recursing == 1) {
    fprintf(stderr, "Error: Recursive calls to TDI Compile is not supported");
    return TdiRECURSIVE;
  }
  static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // TODO: try to avoid
  pthread_mutex_lock(&lock);
  pthread_cleanup_push((void*)pthread_mutex_unlock,&lock);

  EMPTYXD(tmp);
  FREEXD_ON_EXIT(&tmp);
  status = TdiEvaluate(list[0], &tmp MDS_END_ARG);
  text_ptr = tmp.pointer;
  if (STATUS_OK && text_ptr->dtype != DTYPE_T)
    status = TdiINVDTYDSC;
  if STATUS_OK {
    if (text_ptr->length > 0) {
      TdiThreadStatic_p->compiler_recursing = 1;
      if (!TdiRefZone.l_zone)
	status = LibCreateVmZone(&TdiRefZone.l_zone);
      /****************************************
      In case we bomb out, probably not needed.
      ****************************************/
      TdiRefZone.l_status = TdiBOMB;
      if (TdiRefZone.a_begin) free(TdiRefZone.a_begin);
      TdiRefZone.a_begin = TdiRefZone.a_cur = memcpy(malloc(text_ptr->length), text_ptr->pointer, text_ptr->length);
      TdiRefZone.a_end = TdiRefZone.a_cur + text_ptr->length;
      TdiRefZone.l_ok = 0;
      TdiRefZone.l_narg = narg - 1;
      TdiRefZone.l_iarg = 0;
      TdiRefZone.a_list = &list[0];
      if STATUS_OK {
	TdiYyReset();
	if (IS_NOT_OK(TdiYacc()) && IS_OK(TdiRefZone.l_status))
	  status = TdiSYNTAX;
	else
	  status = TdiRefZone.l_status;
      }
      /************************
      Move from temporary zone.
      ************************/
      if STATUS_OK {
	if (TdiRefZone.a_result == 0)
	  MdsFree1Dx(out_ptr, NULL);
	else
	  status = MdsCopyDxXd((struct descriptor *)TdiRefZone.a_result, out_ptr);
      }
      LibResetVmZone(&TdiRefZone.l_zone);
      TdiThreadStatic_p->compiler_recursing = 0;
    }
  }
  FREEXD_NOW(&tmp);
  if STATUS_NOT_OK MdsFree1Dx(out_ptr, NULL);

  pthread_cleanup_pop(1);
  return status;
}

/*-------------------------------------------------------
  Compile and evaluate an expression.
      result = EXECUTE(string, [arg1,...])
*/
int Tdi1Execute(int opcode __attribute__ ((unused)), int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  FREEXD_ON_EXIT(out_ptr);
  EMPTYXD(tmp);
  FREEXD_ON_EXIT(&tmp);
  status = TdiIntrinsic(OpcCompile, narg, list, &tmp);
  if STATUS_OK
    status = TdiEvaluate(tmp.pointer, out_ptr MDS_END_ARG);
  FREEXD_NOW(&tmp);
  if STATUS_NOT_OK MdsFree1Dx(out_ptr, NULL);
  FREE_CANCEL(out_ptr);
  return status;
}
