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
/*      Tdi1Trim.C
        Generic transformation with single string output.

        Ken Klare, LANL P-4     (c)1989,1990,1991
*/
#include <STATICdef.h>
#include "tdinelements.h"
#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefstandard.h"
#include <strroutines.h>
#include <string.h>
#include <tdishr_messages.h>
#include <stdlib.h>
#include <mdsshr.h>
#include <mds_stdarg.h>



extern int TdiGetLong();
extern int TdiGetArgs();
extern int TdiCvtArgs();
extern int TdiMasterData();

int Tdi1Trim(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  INIT_STATUS;
  struct descriptor_xd sig[1], uni[1], dat[1];
  struct TdiCatStruct cats[2];
  struct TdiFunctionStruct *fun_ptr = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
  int j, cmode = -1;

  uni[0] = EMPTY_XD;
  status = TdiGetArgs(opcode, narg, list, sig, uni, dat, cats);
  if STATUS_OK
    status = TdiCvtArgs(narg, dat, cats);
  if (STATUS_OK && narg > 0) {
    N_ELEMENTS(dat[0].pointer, j);
    if (STATUS_OK && j != 1)
      status = TdiINVCLADSC;
  }
	/***********************
        Go off and do something.
        ***********************/
  if STATUS_OK
    status = (*fun_ptr->f3) (dat[0].pointer, out_ptr);
  if STATUS_OK
    status = TdiMasterData(narg, sig, uni, &cmode, out_ptr);
  if (narg > 0) {
    if (sig[0].pointer)
      MdsFree1Dx(&sig[0], NULL);
    if (uni[0].pointer)
      MdsFree1Dx(&uni[0], NULL);
    if (dat[0].pointer)
      MdsFree1Dx(&dat[0], NULL);
  }
  return status;
}

/*--------------------------------------------------------------
        F90 transformation, string trailing blanks removed.
*/
int Tdi3Trim(struct descriptor *in_ptr, struct descriptor_xd *out_ptr)
{
  int n = in_ptr->length;
  char *t_ptr = in_ptr->pointer + n;

  for (; n > 0; --n)
    if (*--t_ptr != ' ' && *t_ptr != '\t')
      break;
  in_ptr->length = (unsigned short)n;
  return MdsCopyDxXd(in_ptr, out_ptr);
}

/*--------------------------------------------------------------
        Convert opcode to builtin name.
*/
int Tdi3OpcodeBuiltin(struct descriptor *in_ptr, struct descriptor_xd *out_ptr)
{
  unsigned int ind = 0xffffffff;
  INIT_STATUS;
  STATIC_CONSTANT unsigned char dtype = (unsigned char)DTYPE_T;
  status = TdiGetLong(in_ptr, &ind);
  if (STATUS_OK && ind < (unsigned int)TdiFUNCTION_MAX) {
    char *name_ptr = TdiRefFunction[ind].name;
    struct descriptor str2 = { 0, DTYPE_T, CLASS_S, 0 };
    str2.length = (unsigned short)strlen(name_ptr);
    str2.pointer = name_ptr;
    status = MdsGet1DxS(&str2.length, &dtype, out_ptr);
    if STATUS_OK
      status = StrCopyDx(out_ptr->pointer, &str2);
  } else if STATUS_OK
    status = TdiINV_OPC;
  return status;
}

/*--------------------------------------------------------------
        Convert opcode to string name.
*/
int Tdi3OpcodeString(struct descriptor *in_ptr, struct descriptor_xd *out_ptr)
{
  STATIC_CONSTANT DESCRIPTOR(str1, "OPC" "$");
  unsigned int ind = 0xffffffff;
  INIT_STATUS;
  STATIC_CONSTANT unsigned char dtype = (unsigned char)DTYPE_T;
  status = TdiGetLong(in_ptr, &ind);
  if (STATUS_OK && ind < (unsigned int)TdiFUNCTION_MAX) {
    char *name_ptr = TdiRefFunction[ind].name;
    struct descriptor str2 = { 0, DTYPE_T, CLASS_S, 0 };
    unsigned short total;
    str2.length = (unsigned short)strlen(name_ptr);
    str2.pointer = name_ptr;
    total = (unsigned short)(str1.length + str2.length);
    status = MdsGet1DxS(&total, &dtype, out_ptr);
    if STATUS_OK
      status =
	  StrConcat((struct descriptor *)out_ptr->pointer,
		    (struct descriptor *)&str1, &str2 MDS_END_ARG);
  } else if STATUS_OK
    status = TdiINV_OPC;
  return status;
}
