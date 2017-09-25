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
// RSD.h -- request sense data
#ifndef __RSD_H
#define __RSD_H
#pragma pack(1)
// for Linux on x86
// NB! order of members is different from original
//      to get data to pack into same size memory, ie
//      40 bytes
typedef struct {
  __u8 error_code:7;
  __u8 valid:1;

  __u8 segment_number;

  __u8 sense_key:4;
  __u8 rsvd:1;
  __u8 ili:1;
  __u8 eom:1;
  __u8 fm:1;

  __u32 information_long;

  __u8 sense_length;

  __u32 command_specific_information;

  __u8 sense_code;

  __u8 sense_qual;

  __u32 reserved;

  __u32 stat;

  union {
    __u16 stacsr;
    struct {
      __u8 noq:1;
      __u8 nox:1;
      __u8 done:1;
      __u8 lam_pending:1;
       __u8:1;
      __u8 qrpt_timeout:1;
       __u8:1;
      __u8 abort:1;
      __u8 xmt_fifo_empty:1;
      __u8 xmt_fifo_full:1;
      __u8 rcv_fifo_empty:1;
      __u8 rcv_fifo_full:1;
      __u8 high_byte_first:1;
      __u8 scsi_id:3;
    } csr;
  } u1;

  union {
    __u32 staesr;
    ErrorStatusRegister esr;
  } u2;

  __u32 staccs;

  __u32 stasum;

  __u32 stacnt;
} RequestSenseData;
#pragma pack(4)
#endif
