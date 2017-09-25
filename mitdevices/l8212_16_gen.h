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
#define L8212_16_K_CONG_NODES 61
#define L8212_16_N_HEAD 0
#define L8212_16_N_NAME 1
#define L8212_16_N_COMMENT 2
#define L8212_16_N_FREQ 3
#define L8212_16_N_EXT_CLOCK_IN 4
#define L8212_16_N_STOP_TRIG 5
#define L8212_16_N_MEMORIES 6
#define L8212_16_N_HEADER 7
#define L8212_16_N_PTS 8
#define L8212_16_N_NOC 9
#define L8212_16_N_OFFSET 10
#define L8212_16_N_INIT_ACTION 11
#define L8212_16_N_STORE_ACTION 12
#define L8212_16_N_INPUT_01 13
#define L8212_16_N_INPUT_01_STARTIDX 14
#define L8212_16_N_INPUT_01_ENDIDX 15
#define L8212_16_N_INPUT_02 16
#define L8212_16_N_INPUT_02_STARTIDX 17
#define L8212_16_N_INPUT_02_ENDIDX 18
#define L8212_16_N_INPUT_03 19
#define L8212_16_N_INPUT_03_STARTIDX 20
#define L8212_16_N_INPUT_03_ENDIDX 21
#define L8212_16_N_INPUT_04 22
#define L8212_16_N_INPUT_04_STARTIDX 23
#define L8212_16_N_INPUT_04_ENDIDX 24
#define L8212_16_N_INPUT_05 25
#define L8212_16_N_INPUT_05_STARTIDX 26
#define L8212_16_N_INPUT_05_ENDIDX 27
#define L8212_16_N_INPUT_06 28
#define L8212_16_N_INPUT_06_STARTIDX 29
#define L8212_16_N_INPUT_06_ENDIDX 30
#define L8212_16_N_INPUT_07 31
#define L8212_16_N_INPUT_07_STARTIDX 32
#define L8212_16_N_INPUT_07_ENDIDX 33
#define L8212_16_N_INPUT_08 34
#define L8212_16_N_INPUT_08_STARTIDX 35
#define L8212_16_N_INPUT_08_ENDIDX 36
#define L8212_16_N_INPUT_09 37
#define L8212_16_N_INPUT_09_STARTIDX 38
#define L8212_16_N_INPUT_09_ENDIDX 39
#define L8212_16_N_INPUT_10 40
#define L8212_16_N_INPUT_10_STARTIDX 41
#define L8212_16_N_INPUT_10_ENDIDX 42
#define L8212_16_N_INPUT_11 43
#define L8212_16_N_INPUT_11_STARTIDX 44
#define L8212_16_N_INPUT_11_ENDIDX 45
#define L8212_16_N_INPUT_12 46
#define L8212_16_N_INPUT_12_STARTIDX 47
#define L8212_16_N_INPUT_12_ENDIDX 48
#define L8212_16_N_INPUT_13 49
#define L8212_16_N_INPUT_13_STARTIDX 50
#define L8212_16_N_INPUT_13_ENDIDX 51
#define L8212_16_N_INPUT_14 52
#define L8212_16_N_INPUT_14_STARTIDX 53
#define L8212_16_N_INPUT_14_ENDIDX 54
#define L8212_16_N_INPUT_15 55
#define L8212_16_N_INPUT_15_STARTIDX 56
#define L8212_16_N_INPUT_15_ENDIDX 57
#define L8212_16_N_INPUT_16 58
#define L8212_16_N_INPUT_16_STARTIDX 59
#define L8212_16_N_INPUT_16_ENDIDX 60
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
