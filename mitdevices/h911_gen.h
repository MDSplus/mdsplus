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
#define H911_K_CONG_NODES 38
#define H911_N_HEAD 0
#define H911_N_NAME 1
#define H911_N_COMMENT 2
#define H911_N_EXT_CLOCK 3
#define H911_N_CHANNEL_01 4
#define H911_N_CHANNEL_02 5
#define H911_N_CHANNEL_03 6
#define H911_N_CHANNEL_04 7
#define H911_N_CHANNEL_05 8
#define H911_N_CHANNEL_06 9
#define H911_N_CHANNEL_07 10
#define H911_N_CHANNEL_08 11
#define H911_N_CHANNEL_09 12
#define H911_N_CHANNEL_10 13
#define H911_N_CHANNEL_11 14
#define H911_N_CHANNEL_12 15
#define H911_N_CHANNEL_13 16
#define H911_N_CHANNEL_14 17
#define H911_N_CHANNEL_15 18
#define H911_N_CHANNEL_16 19
#define H911_N_CHANNEL_17 20
#define H911_N_CHANNEL_18 21
#define H911_N_CHANNEL_19 22
#define H911_N_CHANNEL_20 23
#define H911_N_CHANNEL_21 24
#define H911_N_CHANNEL_22 25
#define H911_N_CHANNEL_23 26
#define H911_N_CHANNEL_24 27
#define H911_N_CHANNEL_25 28
#define H911_N_CHANNEL_26 29
#define H911_N_CHANNEL_27 30
#define H911_N_CHANNEL_28 31
#define H911_N_CHANNEL_29 32
#define H911_N_CHANNEL_30 33
#define H911_N_CHANNEL_31 34
#define H911_N_CHANNEL_32 35
#define H911_N_INIT_ACTION 36
#define H911_N_STORE_ACTION 37
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
} InStoreStruct;
