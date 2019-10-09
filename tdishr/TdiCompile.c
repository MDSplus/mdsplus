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

#define MAXLINE 120
#define MAXFRAC 40
#define MINMAX(min, test, max) ((min) >= (test) ? (min) : (test) < (max) ? (test) : (max))
#define MAXMESS 1800

#include <stdlib.h>
#include <string.h>
#include <mdsdescrip.h>
#include <libroutines.h>
#include <treeshr_messages.h>
#include <tdishr_messages.h>
#include "tdithreadsafe.h"
#include <mdsshr.h>
#include <strroutines.h>
#define DEBUG
#ifdef DEBUG
 #define DBG(...) fprintf(stderr,__VA_ARGS__)
#else
 #define DBG(...) {/**/}
#endif

extern int Tdi1Evaluate();
extern int TdiYacc();
/*-------------------------------------------------------
	Interface to compiler/parser.
	        expression = COMPILE(string, [arg1,...])
	YACC converts TdiYacc.Y to TdiYacc.C to parse TDI statements. Also YACC subroutines.
	LEX converts TdiLex.X to TdiLex.C for lexical analysis. Also LEX subroutines.

	supports (`) on COMPILE
*/

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static void cleanup_compile(ThreadStatic * TdiThreadStatic_p){
  LibResetVmZone(&TDI_REFZONE.l_zone);
  if (TDI_REFZONE.a_begin) {
    free(TDI_REFZONE.a_begin);
    TDI_REFZONE.a_begin=NULL;
  }
  TDI_COMPILE_REC = FALSE;
  if (TDI_STACK_IDX==0) // makes it reentrant
    pthread_mutex_unlock(&lock);
}

static inline void add_compile_info(int status,ThreadStatic * TdiThreadStatic_p) {
  if(!(status==TdiSYNTAX
    || status==TdiEXTRANEOUS
    || status==TdiUNBALANCE
    || status==TreeNOT_OPEN
    || status==TreeNNF
    || status==TdiBOMB)) return;
  static const DESCRIPTOR(compile_err, "%TDI Error compiling region marked by ^\n");
  if (!TDI_REFZONE.a_begin || TDI_INTRINSIC_MSG.length >= MAXMESS)
    return;
  // b------x----c----e
  // '-l_ok-'-xc-'-ce-'
  char *b = TDI_REFZONE.a_begin;
  char *e = TDI_REFZONE.a_end;
  char *c = MINMAX(b, TDI_REFZONE.a_cur, e);
  char *x = MINMAX(b, b + TDI_REFZONE.l_ok, c);
  int xc = MINMAX(0, c-x, MAXLINE);
  int ce = MINMAX(0, e-c, MAXLINE);
  int bx = MINMAX(0, x-b, MAXLINE);
  if (xc+ce > MAXLINE)
    ce = MINMAX(0, ce, MAXFRAC);
  if (bx+xc+ce > MAXLINE)
    bx = MINMAX(0, bx, MAXFRAC);
  int len = bx+xc+2;
  mdsdsc_t marker = { len, DTYPE_T, CLASS_S, memset(malloc(len+1),' ',len) };
  mdsdsc_t region = { bx+xc+ce, DTYPE_T, CLASS_S, x-bx };
  marker.pointer[bx]='^';
  marker.pointer[0] = marker.pointer[len-1]='\n';
  if (xc>0)
    marker.pointer[bx+xc]='^';
  else if (bx>0)
    marker.pointer[bx]='^';
  StrAppend(&TDI_INTRINSIC_MSG,(mdsdsc_t *)&compile_err);
  StrAppend(&TDI_INTRINSIC_MSG,(mdsdsc_t *)&region);
  StrAppend(&TDI_INTRINSIC_MSG,(mdsdsc_t *)&marker);
  free(marker.pointer);
}

static inline int tdi_compile(ThreadStatic * TdiThreadStatic_p,mdsdsc_t * text_ptr, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr) {
  int status;
  if (TDI_STACK_IDX==0) // makes it reentrant
    pthread_mutex_lock(&lock);
  TDI_COMPILE_REC = TRUE;
  pthread_cleanup_push((void*)cleanup_compile,(void*)TdiThreadStatic_p);
  if (!TDI_REFZONE.l_zone)
    status = LibCreateVmZone(&TDI_REFZONE.l_zone);
  TDI_REFZONE.l_status = TdiBOMB;  // In case we bomb out
  TDI_REFZONE.a_begin  = TDI_REFZONE.a_cur = memcpy(malloc(text_ptr->length), text_ptr->pointer, text_ptr->length);
  TDI_REFZONE.a_end    = TDI_REFZONE.a_cur + text_ptr->length;
  TDI_REFZONE.l_ok     = 0;
  TDI_REFZONE.l_narg   = narg - 1;
  TDI_REFZONE.l_iarg   = 0;
  TDI_REFZONE.a_list   = list;
  if (IS_NOT_OK(TdiYacc()) && IS_OK(TDI_REFZONE.l_status))
    status = TdiSYNTAX;
  else
    status = TDI_REFZONE.l_status;
 /************************
  Move from temporary zone.
  ************************/
  if STATUS_OK {
    if (TDI_REFZONE.a_result == 0)
      MdsFree1Dx(out_ptr, NULL);
    else
      status = MdsCopyDxXd((mdsdsc_t *)TDI_REFZONE.a_result, out_ptr);
  }
  add_compile_info(status,TdiThreadStatic_p);
  pthread_cleanup_pop(1);
  return status;
}

EXPORT int Tdi1Compile(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr){
  int status;
  GET_TDITHREADSTATIC_P;
  if (TDI_COMPILE_REC) {
    fprintf(stderr, "Error: Recursive calls to TDI Compile\n");
    return TdiRECURSIVE;
  }
  INIT_AND_FREEXD_ON_EXIT(tmp);
  status = Tdi1Evaluate(opcode, 1, list, &tmp);// using Tdi1Evaluate over TdiEvaluate saves 3 stack levels
  mdsdsc_t *text_ptr = tmp.pointer;
  if (STATUS_OK && text_ptr->dtype != DTYPE_T)
    status = TdiINVDTYDSC;
  else if (STATUS_OK && text_ptr->length > 0)
    status = tdi_compile(TdiThreadStatic_p,text_ptr,narg,list,out_ptr);
  FREEXD_NOW(&tmp);
  if STATUS_NOT_OK MdsFree1Dx(out_ptr, NULL);
  return status;
}

/*-------------------------------------------------------
  Compile and evaluate an expression.
      result = EXECUTE(string, [arg1,...])
*/
int Tdi1Execute(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr){
  INIT_STATUS;
  FREEXD_ON_EXIT(out_ptr);
  INIT_AND_FREEXD_ON_EXIT(tmp);
  status = Tdi1Compile(opcode, narg, list, &tmp);
  if STATUS_OK
    status = Tdi1Evaluate(opcode, 1, &tmp.pointer, out_ptr);
  FREEXD_NOW(&tmp);
  if STATUS_NOT_OK MdsFree1Dx(out_ptr, NULL);
  FREE_CANCEL(out_ptr);
  return status;
}
