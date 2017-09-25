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
// ESR.h  -- error status register
#ifndef __ESR_H
#define __ESR_H

// for Linux on x86
typedef struct {
  __u32 noq:1;
  __u32 nox:1;
  __u32 ste:1;
  __u32 adnr:1;
  __u32 tpe:1;
  __u32 lpe:1;
  __u32 n_gt_23:1;
  __u32 err:1;
  __u32 no_sync:1;
  __u32 tmo:1;
  __u32 derr:1;
  __u32 should_be_zero:5;
  __u32 error_code:4;
  __u32 should_be_0:2;
  __u32 no_halt:1;
  __u32 read:1;
  __u32 ad:1;
  __u32 word_size:2;
  __u32 qmd:2;
  __u32 tm:2;
  __u32 cm:1;

#if 0
  __u8 noq:1;
  __u8 nox:1;
  __u8 ste:1;
  __u8 adnr:1;
  __u8 tpe:1;
  __u8 lpe:1;
  __u8 n_gt_23:1;
  __u8 err:1;
  __u8 no_sync:1;
  __u8 tmo:1;
  __u8 derr:1;
  __u8 should_be_zero:5;
  __u8 error_code:4;
  __u8 should_be_0:2;
  __u8 no_halt:1;
  __u8 read:1;
  __u8 ad:1;
  __u8 word_size:2;
  __u8 qmd:2;
  __u8 tm:2;
  __u8 cm:1;
#endif
} ErrorStatusRegister;

#endif
