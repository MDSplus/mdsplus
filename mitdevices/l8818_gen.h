#define L8818_K_CONG_NODES 14
#define L8818_N_HEAD 0
#define L8818_N_NAME 1
#define L8818_N_COMMENT 2
#define L8818_N_TRIGGER 3
#define L8818_N_CLOCK 4
#define L8818_N_EXT_CLOCK 5
#define L8818_N_PRE_TRIG 6
#define L8818_N_ACTIVE_MEM 7
#define L8818_N_INPUT 8
#define L8818_N_INPUT_STARTIDX 9
#define L8818_N_INPUT_ENDIDX 10
#define L8818_N_INPUT_OFFSET 11
#define L8818_N_INIT_ACTION 12
#define L8818_N_STORE_ACTION 13
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int input_offset;
  int pre_trig;
  int active_mem;
  float clock;
  int clock_convert;
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
} InStoreStruct;
