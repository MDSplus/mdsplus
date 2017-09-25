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
#ifndef TDIREFZONE_H
#define TDIREFZONE_H
/*	tdirefzone.h
	References to zone used by Tdi1BUILD_FUNCTION, TdiYacc, and TdiLex_... .

	Ken Klare, LANL CTR-7	(c)1989,1990
*/
struct TdiZoneStruct {
  char *a_begin;		/*beginning of text     */
  char *a_cur;			/*current character     */
  char *a_end;			/*end of buffer + 1     */
  int l_ok;			/*parsed correctly      */
  int l_status;			/*error code            */
  void *l_zone;			/*virtual memory zone   */
  struct descriptor_d *a_result;	/*the answer             */
  int l_narg;			/*number of arguments   */
  int l_iarg;			/*current arguments     */
  struct descriptor **a_list;	/*first argument        */
  int l_rel_path;		/*keep relative paths   */
};

#include <libroutines.h>
#include "tdithreadsafe.h"
#define TdiRefZone (TdiThreadStatic_p->TdiRefZone)

struct marker {
  struct descriptor_r *rptr;
  short builtin;
  unsigned short w_ok;
};
	/*--------------------------------------------------
	Definitions needed by Lex and friends.
	--------------------------------------------------*/
#define LEX_M_TOKEN	1023
#define LEX_K_IMMED	2048
#define LEX_K_NAMED	4096
#define LEX_K_SYMBOL	8192
#define LEX_K_UNUSUAL	16384

	/*--------------------------------------------------
	Definitions needed by Lex and Yacc.
	--------------------------------------------------*/
#define tdiyylex()		TdiLex()
#define tdiyyerror(s)	TdiRefZone.l_ok = tdiyyval.mark.w_ok; return MDSplusERROR

#define MAKE_S(dtype_in,bytes,out)					\
	{unsigned int dsc_size = sizeof(struct descriptor_s);			\
	unsigned int vm_size = (dsc_size + (bytes));			\
	LibGetVm(&vm_size,(void *)&(out),(void *)&TdiRefZone.l_zone);	\
	((struct descriptor *)(out))->length = bytes;			\
	((struct descriptor *)(out))->dtype = dtype_in;			\
	((struct descriptor *)(out))->class = CLASS_S;			\
	((struct descriptor *)(out))->pointer = (char *)(out) + dsc_size;}

#define MAKE_XD(dtype_in,bytes,out)					\
	{unsigned int dsc_size = sizeof(struct descriptor_xd);			\
	unsigned int vm_size = dsc_size + (bytes);			\
	LibGetVm(&vm_size,(void *)&(out),(void *)&TdiRefZone.l_zone);	\
	((struct descriptor_xd *)(out))->l_length = bytes;		\
	((struct descriptor_xd *)(out))->length = 0;			\
	((struct descriptor_xd *)(out))->dtype = dtype_in;		\
	((struct descriptor_xd *)(out))->class = CLASS_XD;		\
	((struct descriptor_xd *)(out))->pointer = (struct descriptor *)((char *)(out) + dsc_size);}

#define MAKE_R(ndesc,dtype_in,bytes,out)				\
	{unsigned int dsc_size = sizeof($RECORD(ndesc));				\
	unsigned int vm_size = dsc_size + (bytes);			\
	LibGetVm(&vm_size,(void *)&(out),(void *)&TdiRefZone.l_zone);	\
	((struct descriptor *)(out))->length = bytes;			\
	((struct descriptor *)(out))->dtype = dtype_in;			\
	((struct descriptor *)(out))->class = CLASS_R;			\
	((struct descriptor *)(out))->pointer = (char *)(out) + dsc_size;}

	/**********************************************
	Give an extra semicolon. Must be able to unput.
	Caution: side effect--unput changes c pointer.
	**********************************************/
#endif
