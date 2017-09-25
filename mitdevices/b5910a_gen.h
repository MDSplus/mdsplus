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
#define B5910A_K_CONG_NODES 19
#define B5910A_N_HEAD 0
#define B5910A_N_NAME 1
#define B5910A_N_COMMENT 2
#define B5910A_N_INT_CLOCK 3
#define B5910A_N_EXT_CLOCK 4
#define B5910A_N_START_TRIG 5
#define B5910A_N_SAMPLES 6
#define B5910A_N_ITERATIONS 7
#define B5910A_N_NOC 8
#define B5910A_N_CHANNEL_1 9
#define B5910A_N_CHANNEL_1_PROGRAMMING 10
#define B5910A_N_CHANNEL_2 11
#define B5910A_N_CHANNEL_2_PROGRAMMING 12
#define B5910A_N_CHANNEL_3 13
#define B5910A_N_CHANNEL_3_PROGRAMMING 14
#define B5910A_N_CHANNEL_4 15
#define B5910A_N_CHANNEL_4_PROGRAMMING 16
#define B5910A_N_CLOCK_OUT 17
#define B5910A_N_INIT_ACTION 18
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int noc;
  struct descriptor *int_clock;
  int iterations;
  int samples;
  struct descriptor *channel_1;
  struct descriptor *channel_2;
  struct descriptor *channel_3;
  struct descriptor *channel_4;
  struct descriptor *start_trig;
} InInitStruct;
