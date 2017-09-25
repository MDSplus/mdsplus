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
#define HM650_K_CONG_NODES 29
#define HM650_N_HEAD 0
#define HM650_N_NAME 1
#define HM650_N_COMMENT 2
#define HM650_N_THRESHOLD 3
#define HM650_N_TRIG_IN_0 4
#define HM650_N_TRIG_IN_1 5
#define HM650_N_TRIG_IN_2 6
#define HM650_N_TRIG_IN_3 7
#define HM650_N_TRIG_IN_4 8
#define HM650_N_TRIG_IN_5 9
#define HM650_N_TRIG_IN_6 10
#define HM650_N_TRIG_IN_7 11
#define HM650_N_DELAY_0 12
#define HM650_N_DELAY_1 13
#define HM650_N_DELAY_2 14
#define HM650_N_DELAY_3 15
#define HM650_N_DELAY_4 16
#define HM650_N_DELAY_5 17
#define HM650_N_DELAY_6 18
#define HM650_N_DELAY_7 19
#define HM650_N_TRIG_OUT_0 20
#define HM650_N_TRIG_OUT_1 21
#define HM650_N_TRIG_OUT_2 22
#define HM650_N_TRIG_OUT_3 23
#define HM650_N_TRIG_OUT_4 24
#define HM650_N_TRIG_OUT_5 25
#define HM650_N_TRIG_OUT_6 26
#define HM650_N_TRIG_OUT_7 27
#define HM650_N_INIT_ACTION 28
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  float threshold;
  float delay_0;
  float delay_1;
  float delay_2;
  float delay_3;
  float delay_4;
  float delay_5;
  float delay_6;
  float delay_7;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InTriggerStruct;
