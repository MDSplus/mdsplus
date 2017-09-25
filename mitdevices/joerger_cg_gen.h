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
#define JOERGER_CG_K_CONG_NODES 12
#define JOERGER_CG_N_HEAD 0
#define JOERGER_CG_N_NAME 1
#define JOERGER_CG_N_COMMENT 2
#define JOERGER_CG_N_CHANNEL_1 3
#define JOERGER_CG_N_CHANNEL_1_INVERTED 4
#define JOERGER_CG_N_CHANNEL_2 5
#define JOERGER_CG_N_CHANNEL_2_INVERTED 6
#define JOERGER_CG_N_CHANNEL_3 7
#define JOERGER_CG_N_CHANNEL_3_INVERTED 8
#define JOERGER_CG_N_CHANNEL_4 9
#define JOERGER_CG_N_CHANNEL_4_INVERTED 10
#define JOERGER_CG_N_INIT_ACTION 11
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  struct descriptor *channel_1;
  struct descriptor *channel_2;
  struct descriptor *channel_3;
  struct descriptor *channel_4;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStopStruct;
