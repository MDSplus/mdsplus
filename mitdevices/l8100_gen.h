#define L8100_K_CONG_NODES 21
#define L8100_N_HEAD 0
#define L8100_N_NAME 1
#define L8100_N_COMMENT 2
#define L8100_N_INPUT_1 3
#define L8100_N_INPUT_1_GAIN 4
#define L8100_N_INPUT_1_MULT 5
#define L8100_N_INPUT_1_GROUND 6
#define L8100_N_INPUT_1_OFFSET 7
#define L8100_N_INPUT_1_FILTER 8
#define L8100_N_INPUT_1_OUTPUT 9
#define L8100_N_INPUT_1_FILTER_ON 10
#define L8100_N_INPUT_2 11
#define L8100_N_INPUT_2_GAIN 12
#define L8100_N_INPUT_2_MULT 13
#define L8100_N_INPUT_2_GROUND 14
#define L8100_N_INPUT_2_OFFSET 15
#define L8100_N_INPUT_2_FILTER 16
#define L8100_N_INPUT_2_OUTPUT 17
#define L8100_N_INPUT_2_FILTER_ON 18
#define L8100_N_INIT_ACTION 19
#define L8100_N_STORE_ACTION 20
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  float input_1_gain;
  int input_1_gain_convert;
  float input_1_mult;
  int input_1_mult_convert;
  float input_1_offset;
  float input_2_gain;
  int input_2_gain_convert;
  float input_2_mult;
  int input_2_mult_convert;
  float input_2_offset;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStoreStruct;
