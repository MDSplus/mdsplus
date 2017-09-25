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
#define HV1440_K_CONG_NODES 26
#define HV1440_N_HEAD 0
#define HV1440_N_NAME 1
#define HV1440_N_COMMENT 2
#define HV1440_N_RANGE 3
#define HV1440_N_POD_01 4
#define HV1440_N_POD_02 5
#define HV1440_N_POD_03 6
#define HV1440_N_POD_04 7
#define HV1440_N_POD_05 8
#define HV1440_N_POD_06 9
#define HV1440_N_POD_07 10
#define HV1440_N_POD_08 11
#define HV1440_N_POD_09 12
#define HV1440_N_POD_10 13
#define HV1440_N_POD_11 14
#define HV1440_N_POD_12 15
#define HV1440_N_POD_13 16
#define HV1440_N_POD_14 17
#define HV1440_N_POD_15 18
#define HV1440_N_POD_16 19
#define HV1440_N_READOUT 20
#define HV1440_N_FRAME 21
#define HV1440_N_INIT_ACTION 22
#define HV1440_N_STORE_ACTION 23
#define HV1440_N_ON_ACTION 24
#define HV1440_N_OFF_ACTION 25
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int frame;
  float range;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int frame;
  float range;
} InStoreStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InOnStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InOffStruct;
