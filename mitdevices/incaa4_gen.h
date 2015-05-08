#define INCAA4_K_CONG_NODES 26
#define INCAA4_N_HEAD 0
#define INCAA4_N_NAME 1
#define INCAA4_N_COMMENT 2
#define INCAA4_N_INT_CLK_FRQ 3
#define INCAA4_N_CLOCK_IN 4
#define INCAA4_N_CLOCK_OUT 5
#define INCAA4_N_TRIG_IN 6
#define INCAA4_N_TRIG_OUT 7
#define INCAA4_N_MASTER 8
#define INCAA4_N_EXT_1MHZ 9
#define INCAA4_N_PTSC 10
#define INCAA4_N_ACTIVE_CHANS 11
#define INCAA4_N_INPUT_1 12
#define INCAA4_N_INPUT_1_STARTIDX 13
#define INCAA4_N_INPUT_1_ENDIDX 14
#define INCAA4_N_INPUT_2 15
#define INCAA4_N_INPUT_2_STARTIDX 16
#define INCAA4_N_INPUT_2_ENDIDX 17
#define INCAA4_N_INPUT_3 18
#define INCAA4_N_INPUT_3_STARTIDX 19
#define INCAA4_N_INPUT_3_ENDIDX 20
#define INCAA4_N_INPUT_4 21
#define INCAA4_N_INPUT_4_STARTIDX 22
#define INCAA4_N_INPUT_4_ENDIDX 23
#define INCAA4_N_INIT_ACTION 24
#define INCAA4_N_STORE_ACTION 25
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  struct descriptor *int_clk_frq;
  int active_chans;
  int active_chans_convert;
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
  int active_chans_convert;
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
