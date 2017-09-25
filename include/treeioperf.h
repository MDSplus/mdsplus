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
#ifndef TREE_IO_PERF_H

#define TREE_IO_PERF_H

#include <mdstypes.h>

typedef struct _TREE_IO_PERF {

  int num_writes_10_0_to_10_1;
  int num_writes_10_1_to_10_2;
  int num_writes_10_2_to_10_3;
  int num_writes_10_3_to_10_5;
  int num_writes_10_5_to_10_7;
  int num_writes_10_7_to_inf;
  int num_reads_10_0_to_10_1;
  int num_reads_10_1_to_10_2;
  int num_reads_10_2_to_10_3;
  int num_reads_10_3_to_10_5;
  int num_reads_10_5_to_10_7;
  int num_reads_10_7_to_inf;

  int64_t write_bytes_10_0_to_10_1;
  int64_t write_bytes_10_1_to_10_2;
  int64_t write_bytes_10_2_to_10_3;
  int64_t write_bytes_10_3_to_10_5;
  int64_t write_bytes_10_5_to_10_7;
  int64_t write_bytes_10_7_to_inf;
  int64_t read_bytes_10_0_to_10_1;
  int64_t read_bytes_10_1_to_10_2;
  int64_t read_bytes_10_2_to_10_3;
  int64_t read_bytes_10_3_to_10_5;
  int64_t read_bytes_10_5_to_10_7;
  int64_t read_bytes_10_7_to_inf;

} TREE_IO_PERF;

#endif
