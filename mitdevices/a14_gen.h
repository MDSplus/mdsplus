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
#define A14_K_CONG_NODES 33
#define A14_N_HEAD 0
#define A14_N_NAME 1
#define A14_N_COMMENT 2
#define A14_N_CLOCK_DIVIDE 3
#define A14_N_EXT_CLOCK_IN 4
#define A14_N_STOP_TRIG 5
#define A14_N_PTS 6
#define A14_N_MODE 7
#define A14_N_DIMENSION 8
#define A14_N_CLK_POLARITY 9
#define A14_N_STR_POLARITY 10
#define A14_N_STP_POLARITY 11
#define A14_N_GATED 12
#define A14_N_INPUT_1 13
#define A14_N_INPUT_1_STARTIDX 14
#define A14_N_INPUT_1_ENDIDX 15
#define A14_N_INPUT_2 16
#define A14_N_INPUT_2_STARTIDX 17
#define A14_N_INPUT_2_ENDIDX 18
#define A14_N_INPUT_3 19
#define A14_N_INPUT_3_STARTIDX 20
#define A14_N_INPUT_3_ENDIDX 21
#define A14_N_INPUT_4 22
#define A14_N_INPUT_4_STARTIDX 23
#define A14_N_INPUT_4_ENDIDX 24
#define A14_N_INPUT_5 25
#define A14_N_INPUT_5_STARTIDX 26
#define A14_N_INPUT_5_ENDIDX 27
#define A14_N_INPUT_6 28
#define A14_N_INPUT_6_STARTIDX 29
#define A14_N_INPUT_6_ENDIDX 30
#define A14_N_INIT_ACTION 31
#define A14_N_STORE_ACTION 32
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int clock_divide;
  int clock_divide_convert;
  struct descriptor *ext_clock_in;
  int pts;
  int mode;
  int clk_polarity;
  int str_polarity;
  int stp_polarity;
  int gated;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStartStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStopStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  struct descriptor *ext_clock_in;
  struct descriptor *dimension;
} InStoreStruct;
