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
#define RETICON120_K_CONG_NODES 29
#define RETICON120_N_HEAD 0
#define RETICON120_N_NAME 1
#define RETICON120_N_COMMENT 2
#define RETICON120_N_MEMORY_NAME 3
#define RETICON120_N_START_TRIG 4
#define RETICON120_N_PIXEL_SELECT 5
#define RETICON120_N_PIXEL_AXIS 6
#define RETICON120_N_DATA 7
#define RETICON120_N_EVENT_LATCH 8
#define RETICON120_N_NUM_STATES 9
#define RETICON120_N_FRAME_INDEX 10
#define RETICON120_N_STATE_0 11
#define RETICON120_N_STATE_0_PERIOD 12
#define RETICON120_N_STATE_0_FRAMES 13
#define RETICON120_N_STATE_0_FRAME_SELECT 14
#define RETICON120_N_STATE_1 15
#define RETICON120_N_STATE_1_PERIOD 16
#define RETICON120_N_STATE_1_FRAMES 17
#define RETICON120_N_STATE_1_FRAME_SELECT 18
#define RETICON120_N_STATE_2 19
#define RETICON120_N_STATE_2_PERIOD 20
#define RETICON120_N_STATE_2_FRAMES 21
#define RETICON120_N_STATE_2_FRAME_SELECT 22
#define RETICON120_N_STATE_3 23
#define RETICON120_N_STATE_3_PERIOD 24
#define RETICON120_N_STATE_3_FRAMES 25
#define RETICON120_N_STATE_3_FRAME_SELECT 26
#define RETICON120_N_INIT_ACTION 27
#define RETICON120_N_STORE_ACTION 28
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  char *memory_name;
  int num_states;
  struct descriptor *pixel_select;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  char *memory_name;
} InStoreStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  char *memory_name;
} InTriggerStruct;
