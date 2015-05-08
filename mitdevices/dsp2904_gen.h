#define DSP2904_K_CONG_NODES 24
#define DSP2904_N_HEAD 0
#define DSP2904_N_NAME 1
#define DSP2904_N_COMMENT 2
#define DSP2904_N_TRAQ_NAME 3
#define DSP2904_N_TIMER 4
#define DSP2904_N_TIMER_GATE 5
#define DSP2904_N_TIMER_WRAP 6
#define DSP2904_N_TIMER_BITS_32 7
#define DSP2904_N_TIMER_PRESET 8
#define DSP2904_N_TIMER_CHANNEL 9
#define DSP2904_N_COUNTER 10
#define DSP2904_N_COUNTER_GATE 11
#define DSP2904_N_COUNTER_WRAP 12
#define DSP2904_N_COUNTER_BITS_32 13
#define DSP2904_N_COUNTER_PRESET 14
#define DSP2904_N_COUNTER_CHANNEL 15
#define DSP2904_N_IDX_DIVIDER 16
#define DSP2904_N_TIME_OF_CNT 17
#define DSP2904_N_RATE 18
#define DSP2904_N_CLOCK_OUT 19
#define DSP2904_N_EXT_CLK_IN 20
#define DSP2904_N_START 21
#define DSP2904_N_INIT_ACTION 22
#define DSP2904_N_STORE_ACTION 23
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *traq_name;
} InStoreStruct;
