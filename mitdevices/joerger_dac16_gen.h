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
#define JOERGER_DAC16_K_CONG_NODES 20
#define JOERGER_DAC16_N_HEAD 0
#define JOERGER_DAC16_N_NAME 1
#define JOERGER_DAC16_N_COMMENT 2
#define JOERGER_DAC16_N_OUTPUT_01 3
#define JOERGER_DAC16_N_OUTPUT_02 4
#define JOERGER_DAC16_N_OUTPUT_03 5
#define JOERGER_DAC16_N_OUTPUT_04 6
#define JOERGER_DAC16_N_OUTPUT_05 7
#define JOERGER_DAC16_N_OUTPUT_06 8
#define JOERGER_DAC16_N_OUTPUT_07 9
#define JOERGER_DAC16_N_OUTPUT_08 10
#define JOERGER_DAC16_N_OUTPUT_09 11
#define JOERGER_DAC16_N_OUTPUT_10 12
#define JOERGER_DAC16_N_OUTPUT_11 13
#define JOERGER_DAC16_N_OUTPUT_12 14
#define JOERGER_DAC16_N_OUTPUT_13 15
#define JOERGER_DAC16_N_OUTPUT_14 16
#define JOERGER_DAC16_N_OUTPUT_15 17
#define JOERGER_DAC16_N_OUTPUT_16 18
#define JOERGER_DAC16_N_INIT_ACTION 19
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  float output_01;
  float output_02;
  float output_03;
  float output_04;
  float output_05;
  float output_06;
  float output_07;
  float output_08;
  float output_09;
  float output_10;
  float output_11;
  float output_12;
  float output_13;
  float output_14;
  float output_15;
  float output_16;
} InInitStruct;
