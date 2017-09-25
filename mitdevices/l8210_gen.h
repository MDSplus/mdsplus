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
#define L8210_K_CONG_NODES 26
#define L8210_N_HEAD 0
#define L8210_N_NAME 1
#define L8210_N_COMMENT 2
#define L8210_N_EXT_CLOCK_IN 3
#define L8210_N_STOP_TRIG 4
#define L8210_N_CLOCK_OUT 5
#define L8210_N_MEMORIES 6
#define L8210_N_HEADER 7
#define L8210_N_INPUT_1 8
#define L8210_N_INPUT_1_STARTIDX 9
#define L8210_N_INPUT_1_ENDIDX 10
#define L8210_N_INPUT_1_USETIMES 11
#define L8210_N_INPUT_2 12
#define L8210_N_INPUT_2_STARTIDX 13
#define L8210_N_INPUT_2_ENDIDX 14
#define L8210_N_INPUT_2_USETIMES 15
#define L8210_N_INPUT_3 16
#define L8210_N_INPUT_3_STARTIDX 17
#define L8210_N_INPUT_3_ENDIDX 18
#define L8210_N_INPUT_3_USETIMES 19
#define L8210_N_INPUT_4 20
#define L8210_N_INPUT_4_STARTIDX 21
#define L8210_N_INPUT_4_ENDIDX 22
#define L8210_N_INPUT_4_USETIMES 23
#define L8210_N_INIT_ACTION 24
#define L8210_N_STORE_ACTION 25
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
  int memories;
  char *header;
} InStoreStruct;
