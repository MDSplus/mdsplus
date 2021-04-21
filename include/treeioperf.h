#ifndef TREE_IO_PERF_H

#define TREE_IO_PERF_H

#include <mdstypes.h>

typedef struct _TREE_IO_PERF
{

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
