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
// CAMAC data command
#ifndef __CDC_H
#define __CDC_H

// for Linux on x86
typedef struct {
  __u8 opcode;

  __u8 zero1;

  __u8 f:5;
  __u8 bs:1;			// 1->24-bit   0->16-bit
  __u8 zero2:2;

  __u8 n:5;
  __u8 m:3;

  __u8 a:4;
  __u8 sncx:1;			// skip non existant == 0
  __u8 scs:1;			// single crate scan == 0
  __u8 dd:1;			// disable SCSI detection & reconnection
  __u8 zero3:1;

  __u8 crate:7;
  __u8 sp:1;			// serial or parallel

  __u8 transfer_len[3];		// reverse order, ie MSB, ..., LSB

  __u8 zero4;
} CamacDataCommand;

#endif
