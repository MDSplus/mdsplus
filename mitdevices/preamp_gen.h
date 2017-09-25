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
#define PREAMP_K_CONG_NODES 84
#define PREAMP_N_HEAD 0
#define PREAMP_N_NAME 1
#define PREAMP_N_COMMENT 2
#define PREAMP_N_INPUT_01 3
#define PREAMP_N_INPUT_01_GAIN1 4
#define PREAMP_N_INPUT_01_GAIN2 5
#define PREAMP_N_INPUT_01_OFFSET 6
#define PREAMP_N_INPUT_01_OUTPUT 7
#define PREAMP_N_INPUT_02 8
#define PREAMP_N_INPUT_02_GAIN1 9
#define PREAMP_N_INPUT_02_GAIN2 10
#define PREAMP_N_INPUT_02_OFFSET 11
#define PREAMP_N_INPUT_02_OUTPUT 12
#define PREAMP_N_INPUT_03 13
#define PREAMP_N_INPUT_03_GAIN1 14
#define PREAMP_N_INPUT_03_GAIN2 15
#define PREAMP_N_INPUT_03_OFFSET 16
#define PREAMP_N_INPUT_03_OUTPUT 17
#define PREAMP_N_INPUT_04 18
#define PREAMP_N_INPUT_04_GAIN1 19
#define PREAMP_N_INPUT_04_GAIN2 20
#define PREAMP_N_INPUT_04_OFFSET 21
#define PREAMP_N_INPUT_04_OUTPUT 22
#define PREAMP_N_INPUT_05 23
#define PREAMP_N_INPUT_05_GAIN1 24
#define PREAMP_N_INPUT_05_GAIN2 25
#define PREAMP_N_INPUT_05_OFFSET 26
#define PREAMP_N_INPUT_05_OUTPUT 27
#define PREAMP_N_INPUT_06 28
#define PREAMP_N_INPUT_06_GAIN1 29
#define PREAMP_N_INPUT_06_GAIN2 30
#define PREAMP_N_INPUT_06_OFFSET 31
#define PREAMP_N_INPUT_06_OUTPUT 32
#define PREAMP_N_INPUT_07 33
#define PREAMP_N_INPUT_07_GAIN1 34
#define PREAMP_N_INPUT_07_GAIN2 35
#define PREAMP_N_INPUT_07_OFFSET 36
#define PREAMP_N_INPUT_07_OUTPUT 37
#define PREAMP_N_INPUT_08 38
#define PREAMP_N_INPUT_08_GAIN1 39
#define PREAMP_N_INPUT_08_GAIN2 40
#define PREAMP_N_INPUT_08_OFFSET 41
#define PREAMP_N_INPUT_08_OUTPUT 42
#define PREAMP_N_INPUT_09 43
#define PREAMP_N_INPUT_09_GAIN1 44
#define PREAMP_N_INPUT_09_GAIN2 45
#define PREAMP_N_INPUT_09_OFFSET 46
#define PREAMP_N_INPUT_09_OUTPUT 47
#define PREAMP_N_INPUT_10 48
#define PREAMP_N_INPUT_10_GAIN1 49
#define PREAMP_N_INPUT_10_GAIN2 50
#define PREAMP_N_INPUT_10_OFFSET 51
#define PREAMP_N_INPUT_10_OUTPUT 52
#define PREAMP_N_INPUT_11 53
#define PREAMP_N_INPUT_11_GAIN1 54
#define PREAMP_N_INPUT_11_GAIN2 55
#define PREAMP_N_INPUT_11_OFFSET 56
#define PREAMP_N_INPUT_11_OUTPUT 57
#define PREAMP_N_INPUT_12 58
#define PREAMP_N_INPUT_12_GAIN1 59
#define PREAMP_N_INPUT_12_GAIN2 60
#define PREAMP_N_INPUT_12_OFFSET 61
#define PREAMP_N_INPUT_12_OUTPUT 62
#define PREAMP_N_INPUT_13 63
#define PREAMP_N_INPUT_13_GAIN1 64
#define PREAMP_N_INPUT_13_GAIN2 65
#define PREAMP_N_INPUT_13_OFFSET 66
#define PREAMP_N_INPUT_13_OUTPUT 67
#define PREAMP_N_INPUT_14 68
#define PREAMP_N_INPUT_14_GAIN1 69
#define PREAMP_N_INPUT_14_GAIN2 70
#define PREAMP_N_INPUT_14_OFFSET 71
#define PREAMP_N_INPUT_14_OUTPUT 72
#define PREAMP_N_INPUT_15 73
#define PREAMP_N_INPUT_15_GAIN1 74
#define PREAMP_N_INPUT_15_GAIN2 75
#define PREAMP_N_INPUT_15_OFFSET 76
#define PREAMP_N_INPUT_15_OUTPUT 77
#define PREAMP_N_INPUT_16 78
#define PREAMP_N_INPUT_16_GAIN1 79
#define PREAMP_N_INPUT_16_GAIN2 80
#define PREAMP_N_INPUT_16_OFFSET 81
#define PREAMP_N_INPUT_16_OUTPUT 82
#define PREAMP_N_INIT_ACTION 83
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InInitStruct;
