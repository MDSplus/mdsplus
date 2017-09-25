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
#define L6810_K_CONG_NODES 31
#define L6810_N_HEAD 0
#define L6810_N_NAME 1
#define L6810_N_COMMENT 2
#define L6810_N_FREQ 3
#define L6810_N_EXT_CLOCK_IN 4
#define L6810_N_STOP_TRIG 5
#define L6810_N_MEMORIES 6
#define L6810_N_ACTIVE_MEM 7
#define L6810_N_ACTIVE_CHANS 8
#define L6810_N_INPUT_01 9
#define L6810_N_INPUT_01_STARTIDX 10
#define L6810_N_INPUT_01_ENDIDX 11
#define L6810_N_INPUT_01_FULL_SCALE 12
#define L6810_N_INPUT_01_SRC_CPLING 13
#define L6810_N_INPUT_02 14
#define L6810_N_INPUT_02_STARTIDX 15
#define L6810_N_INPUT_02_ENDIDX 16
#define L6810_N_INPUT_02_FULL_SCALE 17
#define L6810_N_INPUT_02_SRC_CPLING 18
#define L6810_N_INPUT_03 19
#define L6810_N_INPUT_03_STARTIDX 20
#define L6810_N_INPUT_03_ENDIDX 21
#define L6810_N_INPUT_03_FULL_SCALE 22
#define L6810_N_INPUT_03_SRC_CPLING 23
#define L6810_N_INPUT_04 24
#define L6810_N_INPUT_04_STARTIDX 25
#define L6810_N_INPUT_04_ENDIDX 26
#define L6810_N_INPUT_04_FULL_SCALE 27
#define L6810_N_INPUT_04_SRC_CPLING 28
#define L6810_N_INIT_ACTION 29
#define L6810_N_STORE_ACTION 30
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int memories;
  int active_mem;
  int active_mem_convert;
  int active_chans;
  int freq;
  int freq_convert;
  float input_01_full_scale;
  int input_01_full_scale_convert;
  float input_02_full_scale;
  int input_02_full_scale_convert;
  float input_03_full_scale;
  int input_03_full_scale_convert;
  float input_04_full_scale;
  int input_04_full_scale_convert;
  int input_01_src_cpling;
  int input_02_src_cpling;
  int input_03_src_cpling;
  int input_04_src_cpling;
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
} InStoreStruct;
