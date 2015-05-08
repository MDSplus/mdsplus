#define L8590_K_CONG_NODES 32
#define L8590_N_HEAD 0
#define L8590_N_NAME 1
#define L8590_N_COMMENT 2
#define L8590_N_MEM_CTS_NAME 3
#define L8590_N_LATCH 4
#define L8590_N_NUM_ACTIVE 5
#define L8590_N_INPUT_1 6
#define L8590_N_INPUT_1_STARTIDX 7
#define L8590_N_INPUT_1_ENDIDX 8
#define L8590_N_INPUT_2 9
#define L8590_N_INPUT_2_STARTIDX 10
#define L8590_N_INPUT_2_ENDIDX 11
#define L8590_N_INPUT_3 12
#define L8590_N_INPUT_3_STARTIDX 13
#define L8590_N_INPUT_3_ENDIDX 14
#define L8590_N_INPUT_4 15
#define L8590_N_INPUT_4_STARTIDX 16
#define L8590_N_INPUT_4_ENDIDX 17
#define L8590_N_INPUT_5 18
#define L8590_N_INPUT_5_STARTIDX 19
#define L8590_N_INPUT_5_ENDIDX 20
#define L8590_N_INPUT_6 21
#define L8590_N_INPUT_6_STARTIDX 22
#define L8590_N_INPUT_6_ENDIDX 23
#define L8590_N_INPUT_7 24
#define L8590_N_INPUT_7_STARTIDX 25
#define L8590_N_INPUT_7_ENDIDX 26
#define L8590_N_INPUT_8 27
#define L8590_N_INPUT_8_STARTIDX 28
#define L8590_N_INPUT_8_ENDIDX 29
#define L8590_N_INIT_ACTION 30
#define L8590_N_STORE_ACTION 31
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  char *mem_cts_name;
  int num_active;
  int num_active_convert;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  char *mem_cts_name;
  int num_active;
  int num_active_convert;
} InStoreStruct;
