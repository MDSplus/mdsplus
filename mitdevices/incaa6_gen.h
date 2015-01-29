#define INCAA6_K_CONG_NODES 32
#define INCAA6_N_HEAD 0
#define INCAA6_N_NAME 1
#define INCAA6_N_COMMENT 2
#define INCAA6_N_INT_CLK_FRQ 3
#define INCAA6_N_CLOCK_IN 4
#define INCAA6_N_CLOCK_OUT 5
#define INCAA6_N_TRIG_IN 6
#define INCAA6_N_TRIG_OUT 7
#define INCAA6_N_MASTER 8
#define INCAA6_N_EXT_1MHZ 9
#define INCAA6_N_PTSC 10
#define INCAA6_N_ACTIVE_CHANS 11
#define INCAA6_N_INPUT_1 12
#define INCAA6_N_INPUT_1_STARTIDX 13
#define INCAA6_N_INPUT_1_ENDIDX 14
#define INCAA6_N_INPUT_2 15
#define INCAA6_N_INPUT_2_STARTIDX 16
#define INCAA6_N_INPUT_2_ENDIDX 17
#define INCAA6_N_INPUT_3 18
#define INCAA6_N_INPUT_3_STARTIDX 19
#define INCAA6_N_INPUT_3_ENDIDX 20
#define INCAA6_N_INPUT_4 21
#define INCAA6_N_INPUT_4_STARTIDX 22
#define INCAA6_N_INPUT_4_ENDIDX 23
#define INCAA6_N_INPUT_5 24
#define INCAA6_N_INPUT_5_STARTIDX 25
#define INCAA6_N_INPUT_5_ENDIDX 26
#define INCAA6_N_INPUT_6 27
#define INCAA6_N_INPUT_6_STARTIDX 28
#define INCAA6_N_INPUT_6_ENDIDX 29
#define INCAA6_N_INIT_ACTION 30
#define INCAA6_N_STORE_ACTION 31
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  struct descriptor *int_clk_frq;
  int active_chans;
  int master;
  int ext_1mhz;
  int ptsc;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  struct descriptor *int_clk_frq;
  int active_chans;
  int master;
  int ext_1mhz;
  int ptsc;
} InArmStruct;
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
  int ptsc;
} InStoreStruct;
