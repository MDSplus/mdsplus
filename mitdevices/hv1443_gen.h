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
#define HV1443_K_CONG_NODES 17
#define HV1443_N_HEAD 0
#define HV1443_N_VOLTAGE_01 1
#define HV1443_N_VOLTAGE_02 2
#define HV1443_N_VOLTAGE_03 3
#define HV1443_N_VOLTAGE_04 4
#define HV1443_N_VOLTAGE_05 5
#define HV1443_N_VOLTAGE_06 6
#define HV1443_N_VOLTAGE_07 7
#define HV1443_N_VOLTAGE_08 8
#define HV1443_N_VOLTAGE_09 9
#define HV1443_N_VOLTAGE_10 10
#define HV1443_N_VOLTAGE_11 11
#define HV1443_N_VOLTAGE_12 12
#define HV1443_N_VOLTAGE_13 13
#define HV1443_N_VOLTAGE_14 14
#define HV1443_N_VOLTAGE_15 15
#define HV1443_N_VOLTAGE_16 16
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  struct descriptor *voltage_01;
} InGet_settingsStruct;
