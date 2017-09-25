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
#define L8212_04_K_CONG_NODES 25
#define L8212_04_N_HEAD 0
#define L8212_04_N_NAME 1
#define L8212_04_N_COMMENT 2
#define L8212_04_N_FREQ 3
#define L8212_04_N_EXT_CLOCK_IN 4
#define L8212_04_N_STOP_TRIG 5
#define L8212_04_N_MEMORIES 6
#define L8212_04_N_HEADER 7
#define L8212_04_N_PTS 8
#define L8212_04_N_NOC 9
#define L8212_04_N_OFFSET 10
#define L8212_04_N_INIT_ACTION 11
#define L8212_04_N_STORE_ACTION 12
#define L8212_04_N_INPUT_01 13
#define L8212_04_N_INPUT_01_STARTIDX 14
#define L8212_04_N_INPUT_01_ENDIDX 15
#define L8212_04_N_INPUT_02 16
#define L8212_04_N_INPUT_02_STARTIDX 17
#define L8212_04_N_INPUT_02_ENDIDX 18
#define L8212_04_N_INPUT_03 19
#define L8212_04_N_INPUT_03_STARTIDX 20
#define L8212_04_N_INPUT_03_ENDIDX 21
#define L8212_04_N_INPUT_04 22
#define L8212_04_N_INPUT_04_STARTIDX 23
#define L8212_04_N_INPUT_04_ENDIDX 24
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int noc;
  int pts;
  int offset;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int memories;
  char *header;
  int offset;
} InStoreStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InTriggerStruct;
