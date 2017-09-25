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
#define PARAGON_HIST_K_CONG_NODES 36
#define PARAGON_HIST_N_HEAD 0
#define PARAGON_HIST_N_COMMENT 1
#define PARAGON_HIST_N_REPORT_NAME 2
#define PARAGON_HIST_N_NODE 3
#define PARAGON_HIST_N_NAME_0 4
#define PARAGON_HIST_N_NAME_1 5
#define PARAGON_HIST_N_NAME_2 6
#define PARAGON_HIST_N_NAME_3 7
#define PARAGON_HIST_N_NAME_4 8
#define PARAGON_HIST_N_NAME_5 9
#define PARAGON_HIST_N_NAME_6 10
#define PARAGON_HIST_N_NAME_7 11
#define PARAGON_HIST_N_NAME_8 12
#define PARAGON_HIST_N_NAME_9 13
#define PARAGON_HIST_N_VALUE_0 14
#define PARAGON_HIST_N_VALUE_1 15
#define PARAGON_HIST_N_VALUE_2 16
#define PARAGON_HIST_N_VALUE_3 17
#define PARAGON_HIST_N_VALUE_4 18
#define PARAGON_HIST_N_VALUE_5 19
#define PARAGON_HIST_N_VALUE_6 20
#define PARAGON_HIST_N_VALUE_7 21
#define PARAGON_HIST_N_VALUE_8 22
#define PARAGON_HIST_N_VALUE_9 23
#define PARAGON_HIST_N_LIMIT_0 24
#define PARAGON_HIST_N_LIMIT_1 25
#define PARAGON_HIST_N_LIMIT_2 26
#define PARAGON_HIST_N_LIMIT_3 27
#define PARAGON_HIST_N_LIMIT_4 28
#define PARAGON_HIST_N_LIMIT_5 29
#define PARAGON_HIST_N_LIMIT_6 30
#define PARAGON_HIST_N_LIMIT_7 31
#define PARAGON_HIST_N_LIMIT_8 32
#define PARAGON_HIST_N_LIMIT_9 33
#define PARAGON_HIST_N_DELETE_FILE 34
#define PARAGON_HIST_N_STORE_ACTION 35
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *report_name;
} InStoreStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *report_name;
} InInsertStruct;
