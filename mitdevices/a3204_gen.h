#define A3204_K_CONG_NODES 29
#define A3204_N_HEAD 0
#define A3204_N_NAME 1
#define A3204_N_COMMENT 2
#define A3204_N_INPUT_1 3
#define A3204_N_INPUT_1_GAIN 4
#define A3204_N_INPUT_1_OFFSET 5
#define A3204_N_INPUT_1_FILTER 6
#define A3204_N_INPUT_1_OUTPUT 7
#define A3204_N_INPUT_1_FILTER_ON 8
#define A3204_N_INPUT_2 9
#define A3204_N_INPUT_2_GAIN 10
#define A3204_N_INPUT_2_OFFSET 11
#define A3204_N_INPUT_2_FILTER 12
#define A3204_N_INPUT_2_OUTPUT 13
#define A3204_N_INPUT_2_FILTER_ON 14
#define A3204_N_INPUT_3 15
#define A3204_N_INPUT_3_GAIN 16
#define A3204_N_INPUT_3_OFFSET 17
#define A3204_N_INPUT_3_FILTER 18
#define A3204_N_INPUT_3_OUTPUT 19
#define A3204_N_INPUT_3_FILTER_ON 20
#define A3204_N_INPUT_4 21
#define A3204_N_INPUT_4_GAIN 22
#define A3204_N_INPUT_4_OFFSET 23
#define A3204_N_INPUT_4_FILTER 24
#define A3204_N_INPUT_4_OUTPUT 25
#define A3204_N_INPUT_4_FILTER_ON 26
#define A3204_N_INIT_ACTION 27
#define A3204_N_STORE_ACTION 28
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  float input_1_gain;
  int input_1_gain_convert;
  float input_1_offset;
  float input_2_gain;
  int input_2_gain_convert;
  float input_2_offset;
  float input_3_gain;
  int input_3_gain_convert;
  float input_3_offset;
  float input_4_gain;
  int input_4_gain_convert;
  float input_4_offset;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStoreStruct;
