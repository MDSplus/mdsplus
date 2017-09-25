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
#define U_OF_M_SPECT_K_CONG_NODES 21
#define U_OF_M_SPECT_N_HEAD 0
#define U_OF_M_SPECT_N_COMMENT 1
#define U_OF_M_SPECT_N_GO_FILE 2
#define U_OF_M_SPECT_N_DATA_FILE 3
#define U_OF_M_SPECT_N_TRIGGER 4
#define U_OF_M_SPECT_N_SETUP 5
#define U_OF_M_SPECT_N_SETUP_WAVE_LENGTH 6
#define U_OF_M_SPECT_N_SETUP_SLIT_WIDTH 7
#define U_OF_M_SPECT_N_SETUP_NUM_SPECTRA 8
#define U_OF_M_SPECT_N_SETUP_GRATING_TYPE 9
#define U_OF_M_SPECT_N_SETUP_EXPOSURE 10
#define U_OF_M_SPECT_N_SETUP_FILTER 11
#define U_OF_M_SPECT_N_WAVE_LENGTH 12
#define U_OF_M_SPECT_N_SLIT_WIDTH 13
#define U_OF_M_SPECT_N_GRATING_TYPE 14
#define U_OF_M_SPECT_N_EXPOSURE 15
#define U_OF_M_SPECT_N_FILTER 16
#define U_OF_M_SPECT_N_SPECTRA 17
#define U_OF_M_SPECT_N_ORDER_SORTER 18
#define U_OF_M_SPECT_N_INIT_ACTION 19
#define U_OF_M_SPECT_N_STORE_ACTION 20
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *go_file;
  char *data_file;
  int setup_wave_length;
  int setup_slit_width;
  int setup_num_spectra;
  int setup_grating_type;
  int setup_exposure;
  int setup_filter;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *data_file;
} InStoreStruct;
