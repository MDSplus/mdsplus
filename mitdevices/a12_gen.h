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
#define A12_K_CONG_NODES 25
#define A12_N_HEAD 0
#define A12_N_NAME 1
#define A12_N_COMMENT 2
#define A12_N_EXT_CLOCK_IN 3
#define A12_N_STOP_TRIG 4
#define A12_N_INPUT_1 5
#define A12_N_INPUT_1_STARTIDX 6
#define A12_N_INPUT_1_ENDIDX 7
#define A12_N_INPUT_2 8
#define A12_N_INPUT_2_STARTIDX 9
#define A12_N_INPUT_2_ENDIDX 10
#define A12_N_INPUT_3 11
#define A12_N_INPUT_3_STARTIDX 12
#define A12_N_INPUT_3_ENDIDX 13
#define A12_N_INPUT_4 14
#define A12_N_INPUT_4_STARTIDX 15
#define A12_N_INPUT_4_ENDIDX 16
#define A12_N_INPUT_5 17
#define A12_N_INPUT_5_STARTIDX 18
#define A12_N_INPUT_5_ENDIDX 19
#define A12_N_INPUT_6 20
#define A12_N_INPUT_6_STARTIDX 21
#define A12_N_INPUT_6_ENDIDX 22
#define A12_N_INIT_ACTION 23
#define A12_N_STORE_ACTION 24
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  struct descriptor *ext_clock_in;
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
} InStopStruct;
