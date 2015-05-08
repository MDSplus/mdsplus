#define JOERGER_TR812_K_CONG_NODES 42
#define JOERGER_TR812_N_HEAD 0
#define JOERGER_TR812_N_NAME 1
#define JOERGER_TR812_N_COMMENT 2
#define JOERGER_TR812_N_TRIGGER 3
#define JOERGER_TR812_N_BURST_MODE 4
#define JOERGER_TR812_N_EXT_CLOCK 5
#define JOERGER_TR812_N_PRE_TRIGGER 6
#define JOERGER_TR812_N_ACT_MEMORY 7
#define JOERGER_TR812_N_CHANNEL_1 8
#define JOERGER_TR812_N_CHANNEL_1_STARTIDX 9
#define JOERGER_TR812_N_CHANNEL_1_ENDIDX 10
#define JOERGER_TR812_N_CHANNEL_1_GAIN 11
#define JOERGER_TR812_N_CHANNEL_2 12
#define JOERGER_TR812_N_CHANNEL_2_STARTIDX 13
#define JOERGER_TR812_N_CHANNEL_2_ENDIDX 14
#define JOERGER_TR812_N_CHANNEL_2_GAIN 15
#define JOERGER_TR812_N_CHANNEL_3 16
#define JOERGER_TR812_N_CHANNEL_3_STARTIDX 17
#define JOERGER_TR812_N_CHANNEL_3_ENDIDX 18
#define JOERGER_TR812_N_CHANNEL_3_GAIN 19
#define JOERGER_TR812_N_CHANNEL_4 20
#define JOERGER_TR812_N_CHANNEL_4_STARTIDX 21
#define JOERGER_TR812_N_CHANNEL_4_ENDIDX 22
#define JOERGER_TR812_N_CHANNEL_4_GAIN 23
#define JOERGER_TR812_N_CHANNEL_5 24
#define JOERGER_TR812_N_CHANNEL_5_STARTIDX 25
#define JOERGER_TR812_N_CHANNEL_5_ENDIDX 26
#define JOERGER_TR812_N_CHANNEL_5_GAIN 27
#define JOERGER_TR812_N_CHANNEL_6 28
#define JOERGER_TR812_N_CHANNEL_6_STARTIDX 29
#define JOERGER_TR812_N_CHANNEL_6_ENDIDX 30
#define JOERGER_TR812_N_CHANNEL_6_GAIN 31
#define JOERGER_TR812_N_CHANNEL_7 32
#define JOERGER_TR812_N_CHANNEL_7_STARTIDX 33
#define JOERGER_TR812_N_CHANNEL_7_ENDIDX 34
#define JOERGER_TR812_N_CHANNEL_7_GAIN 35
#define JOERGER_TR812_N_CHANNEL_8 36
#define JOERGER_TR812_N_CHANNEL_8_STARTIDX 37
#define JOERGER_TR812_N_CHANNEL_8_ENDIDX 38
#define JOERGER_TR812_N_CHANNEL_8_GAIN 39
#define JOERGER_TR812_N_INIT_ACTION 40
#define JOERGER_TR812_N_STORE_ACTION 41
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int pre_trigger;
  int act_memory;
  int channel_1_gain;
  int channel_1_gain_convert;
  int channel_2_gain;
  int channel_2_gain_convert;
  int channel_3_gain;
  int channel_3_gain_convert;
  int channel_4_gain;
  int channel_4_gain_convert;
  int channel_5_gain;
  int channel_5_gain_convert;
  int channel_6_gain;
  int channel_6_gain_convert;
  int channel_7_gain;
  int channel_7_gain_convert;
  int channel_8_gain;
  int channel_8_gain_convert;
  struct descriptor *ext_clock;
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
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InPartial_storeStruct;
