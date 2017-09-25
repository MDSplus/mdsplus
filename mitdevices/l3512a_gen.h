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
#define L3512A_K_CONG_NODES 13
#define L3512A_N_HEAD 0
#define L3512A_N_NAME 1
#define L3512A_N_COMMENT 2
#define L3512A_N_MEMORY_NAME 3
#define L3512A_N_ROUTER_NAME 4
#define L3512A_N_NUM_SPECTRA 5
#define L3512A_N_NUM_CHANNELS 6
#define L3512A_N_TRIGGER 7
#define L3512A_N_OFFSET 8
#define L3512A_N_DURATION 9
#define L3512A_N_SPECTRA 10
#define L3512A_N_INIT_ACTION 11
#define L3512A_N_STORE_ACTION 12
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  char *memory_name;
  char *router_name;
  int num_spectra;
  int num_channels;
  int offset;
  struct descriptor *duration;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  char *memory_name;
  int num_spectra;
  int num_channels;
  struct descriptor *duration;
  struct descriptor *trigger;
} InStoreStruct;
