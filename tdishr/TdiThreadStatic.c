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
#define _GNU_SOURCE
#include <mdsplus/mdsconfig.h>

#include <stdlib.h>
#include <string.h>

#include <libroutines.h>
#include <mdsshr.h>
#include <strroutines.h>

#include "tdithreadstatic.h"

#define YYLTYPE TDITHREADSTATIC_TYPE
#include "tdiyacc.h"
#include "tdilex.h"
#include <_mdsversion.h>

static inline TDITHREADSTATIC_TYPE *buffer_alloc()
{
  TDITHREADSTATIC_ARG = (TDITHREADSTATIC_TYPE *)calloc(1, sizeof(TDITHREADSTATIC_TYPE));
  LibCreateVmZone(&TDI_VAR_PRIVATE.head_zone);
  LibCreateVmZone(&TDI_VAR_PRIVATE.data_zone);
  TDI_INTRINSIC_STAT = SsSUCCESS;
  TDI_INTRINSIC_MSG.dtype = DTYPE_T;
  TDI_INTRINSIC_MSG.class = CLASS_D;
  TDI_VAR_NEW_NARG_D.length = sizeof(int);
  TDI_VAR_NEW_NARG_D.dtype = DTYPE_L;
  TDI_VAR_NEW_NARG_D.class = CLASS_S;
  TDI_VAR_NEW_NARG_D.pointer = (char *)&TDI_VAR_NEW_NARG;
  for (; TDI_STACK_IDX < TDI_STACK_SIZE; TDI_STACK_IDX++)
  {
    TDI_INDENT = 1;
    TDI_DECOMPILE_MAX = 0xffff;
  }
  tdilex_init(&TDI_SCANNER);
  TDI_STACK_IDX = 0;
  TDI_VAR_REC = 0;
  return TDITHREADSTATIC_VAR;
}
static void buffer_free(TDITHREADSTATIC_ARG)
{
  LibResetVmZone(&TDI_VAR_PRIVATE.head_zone);
  LibResetVmZone(&TDI_VAR_PRIVATE.data_zone);
  StrFree1Dx(&TDI_INTRINSIC_MSG);
  tdilex_destroy(TDI_SCANNER);
  free(TDITHREADSTATIC_VAR);
}

EXPORT IMPLEMENT_GETTHREADSTATIC(TDITHREADSTATIC_TYPE, TdiGetThreadStatic, THREADSTATIC_TDISHR, buffer_alloc, buffer_free)
