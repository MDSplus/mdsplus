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
#include "tdirefzone.h"
#else
#ifndef TDITHREADSAFE_H
#define TDITHREADSAFE_H
#include <mdsdescrip.h>
#include <pthread_port.h>

typedef struct _thread_static {
  int TdiGetData_recursion_count;
  int TdiIntrinsic_mess_stat;
  int TdiIntrinsic_recursion_count;
  struct descriptor_d TdiIntrinsic_message;
  struct {
    void *head;
    void *head_zone;
    void *data_zone;
    int public;
  } TdiVar_private;
  int TdiVar_new_narg;
  struct descriptor TdiVar_new_narg_d;
  int compiler_recursing;
  struct descriptor *TdiRANGE_PTRS[3];
  struct descriptor_xd *TdiSELF_PTR;
  struct TdiZoneStruct TdiRefZone;
  unsigned int TdiIndent;
  unsigned short TdiDecompile_max;
} ThreadStatic;

extern ThreadStatic *TdiGetThreadStatic();
extern void LockTdiMutex(pthread_mutex_t *, int *);
extern void UnlockTdiMutex(pthread_mutex_t *);

#define GET_TDITHREADSTATIC_P ThreadStatic *TdiThreadStatic_p = TdiGetThreadStatic()

#endif
#endif
