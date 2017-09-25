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
#define MIT__GATE_K_CONG_NODES 11
#define MIT__GATE_N_HEAD 0
#define MIT__GATE_N_COMMENT 1
#define MIT__GATE_N_TRIGGER_MODE 2
#define MIT__GATE_N_START_LOW 3
#define MIT__GATE_N_TRIGGER 4
#define MIT__GATE_N_EVENT 5
#define MIT__GATE_N_DURATION 6
#define MIT__GATE_N_PULSE_TIME 7
#define MIT__GATE_N_GATE_OUT 8
#define MIT__GATE_N_EDGES_R 9
#define MIT__GATE_N_EDGES_F 10
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  float trigger;
  float pulse_time;
  float duration;
  int trigger_mode;
} InGet_setupStruct;
