#define H912_K_CONG_NODES 56
#define H912_N_HEAD 0
#define H912_N_NAME 1
#define H912_N_COMMENT 2
#define H912_N_EXT_CLOCK 3
#define H912_N_TRIGGER 4
#define H912_N_BLOCKS 5
#define H912_N_PTS 6
#define H912_N_INT_CLOCK 7
#define H912_N_TIMESTAMPS 8
#define H912_N_INPUT_1 9
#define H912_N_INPUT_1_STARTIDX 10
#define H912_N_INPUT_1_ENDIDX 11
#define H912_N_INPUT_2 12
#define H912_N_INPUT_2_STARTIDX 13
#define H912_N_INPUT_2_ENDIDX 14
#define H912_N_INPUT_3 15
#define H912_N_INPUT_3_STARTIDX 16
#define H912_N_INPUT_3_ENDIDX 17
#define H912_N_INPUT_4 18
#define H912_N_INPUT_4_STARTIDX 19
#define H912_N_INPUT_4_ENDIDX 20
#define H912_N_INPUT_5 21
#define H912_N_INPUT_5_STARTIDX 22
#define H912_N_INPUT_5_ENDIDX 23
#define H912_N_INPUT_6 24
#define H912_N_INPUT_6_STARTIDX 25
#define H912_N_INPUT_6_ENDIDX 26
#define H912_N_INPUT_7 27
#define H912_N_INPUT_7_STARTIDX 28
#define H912_N_INPUT_7_ENDIDX 29
#define H912_N_INPUT_8 30
#define H912_N_INPUT_8_STARTIDX 31
#define H912_N_INPUT_8_ENDIDX 32
#define H912_N_INPUT_9 33
#define H912_N_INPUT_9_STARTIDX 34
#define H912_N_INPUT_9_ENDIDX 35
#define H912_N_INPUT_10 36
#define H912_N_INPUT_10_STARTIDX 37
#define H912_N_INPUT_10_ENDIDX 38
#define H912_N_INPUT_11 39
#define H912_N_INPUT_11_STARTIDX 40
#define H912_N_INPUT_11_ENDIDX 41
#define H912_N_INPUT_12 42
#define H912_N_INPUT_12_STARTIDX 43
#define H912_N_INPUT_12_ENDIDX 44
#define H912_N_INPUT_13 45
#define H912_N_INPUT_13_STARTIDX 46
#define H912_N_INPUT_13_ENDIDX 47
#define H912_N_INPUT_14 48
#define H912_N_INPUT_14_STARTIDX 49
#define H912_N_INPUT_14_ENDIDX 50
#define H912_N_INPUT_15 51
#define H912_N_INPUT_15_STARTIDX 52
#define H912_N_INPUT_15_ENDIDX 53
#define H912_N_INIT_ACTION 54
#define H912_N_STORE_ACTION 55
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int int_clock;
  int int_clock_convert;
  int blocks;
  int blocks_convert;
  struct descriptor *pts;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InTriggerStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStopStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStoreStruct;
