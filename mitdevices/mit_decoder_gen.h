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
#define MIT_DECODER_K_CONG_NODES 15
#define MIT_DECODER_N_HEAD 0
#define MIT_DECODER_N_NAME 1
#define MIT_DECODER_N_COMMENT 2
#define MIT_DECODER_N_MASTER_REG 3
#define MIT_DECODER_N_CHANNEL_0 4
#define MIT_DECODER_N_CHANNEL_0_PSEUDO_DEV 5
#define MIT_DECODER_N_CHANNEL_1 6
#define MIT_DECODER_N_CHANNEL_1_PSEUDO_DEV 7
#define MIT_DECODER_N_CHANNEL_2 8
#define MIT_DECODER_N_CHANNEL_2_PSEUDO_DEV 9
#define MIT_DECODER_N_CHANNEL_3 10
#define MIT_DECODER_N_CHANNEL_3_PSEUDO_DEV 11
#define MIT_DECODER_N_CHANNEL_4 12
#define MIT_DECODER_N_CHANNEL_4_PSEUDO_DEV 13
#define MIT_DECODER_N_INIT_ACTION 14
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InInitStruct;
