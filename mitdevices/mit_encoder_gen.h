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
#define MIT_ENCODER_K_CONG_NODES 18
#define MIT_ENCODER_N_HEAD 0
#define MIT_ENCODER_N_NAME 1
#define MIT_ENCODER_N_COMMENT 2
#define MIT_ENCODER_N_INIT_ACTION 3
#define MIT_ENCODER_N_CHANNEL_1 4
#define MIT_ENCODER_N_CHANNEL_1_EVENT 5
#define MIT_ENCODER_N_CHANNEL_2 6
#define MIT_ENCODER_N_CHANNEL_2_EVENT 7
#define MIT_ENCODER_N_CHANNEL_3 8
#define MIT_ENCODER_N_CHANNEL_3_EVENT 9
#define MIT_ENCODER_N_CHANNEL_4 10
#define MIT_ENCODER_N_CHANNEL_4_EVENT 11
#define MIT_ENCODER_N_CHANNEL_5 12
#define MIT_ENCODER_N_CHANNEL_5_EVENT 13
#define MIT_ENCODER_N_CHANNEL_6 14
#define MIT_ENCODER_N_CHANNEL_6_EVENT 15
#define MIT_ENCODER_N_SOFT_CHANNEL 16
#define MIT_ENCODER_N_SOFT_CHANNEL_EVENT 17
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InTriggerStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InSet_eventStruct;
