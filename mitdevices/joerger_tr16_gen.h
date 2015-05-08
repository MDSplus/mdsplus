#define JOERGER_TR16_K_CONG_NODES 77
#define JOERGER_TR16_N_HEAD 0
#define JOERGER_TR16_N_NAME 1
#define JOERGER_TR16_N_COMMENT 2
#define JOERGER_TR16_N_FREQ 3
#define JOERGER_TR16_N_EXT_CLOCK_IN 4
#define JOERGER_TR16_N_STOP_TRIG 5
#define JOERGER_TR16_N_MEM_SIZE 6
#define JOERGER_TR16_N_ACTIVE_MEM 7
#define JOERGER_TR16_N_ACTIVE_CHANS 8
#define JOERGER_TR16_N_POST_TRIG 9
#define JOERGER_TR16_N_MASTER 10
#define JOERGER_TR16_N_INPUT_01 11
#define JOERGER_TR16_N_INPUT_01_STARTIDX 12
#define JOERGER_TR16_N_INPUT_01_ENDIDX 13
#define JOERGER_TR16_N_INPUT_01_GAIN 14
#define JOERGER_TR16_N_INPUT_02 15
#define JOERGER_TR16_N_INPUT_02_STARTIDX 16
#define JOERGER_TR16_N_INPUT_02_ENDIDX 17
#define JOERGER_TR16_N_INPUT_02_GAIN 18
#define JOERGER_TR16_N_INPUT_03 19
#define JOERGER_TR16_N_INPUT_03_STARTIDX 20
#define JOERGER_TR16_N_INPUT_03_ENDIDX 21
#define JOERGER_TR16_N_INPUT_03_GAIN 22
#define JOERGER_TR16_N_INPUT_04 23
#define JOERGER_TR16_N_INPUT_04_STARTIDX 24
#define JOERGER_TR16_N_INPUT_04_ENDIDX 25
#define JOERGER_TR16_N_INPUT_04_GAIN 26
#define JOERGER_TR16_N_INPUT_05 27
#define JOERGER_TR16_N_INPUT_05_STARTIDX 28
#define JOERGER_TR16_N_INPUT_05_ENDIDX 29
#define JOERGER_TR16_N_INPUT_05_GAIN 30
#define JOERGER_TR16_N_INPUT_06 31
#define JOERGER_TR16_N_INPUT_06_STARTIDX 32
#define JOERGER_TR16_N_INPUT_06_ENDIDX 33
#define JOERGER_TR16_N_INPUT_06_GAIN 34
#define JOERGER_TR16_N_INPUT_07 35
#define JOERGER_TR16_N_INPUT_07_STARTIDX 36
#define JOERGER_TR16_N_INPUT_07_ENDIDX 37
#define JOERGER_TR16_N_INPUT_07_GAIN 38
#define JOERGER_TR16_N_INPUT_08 39
#define JOERGER_TR16_N_INPUT_08_STARTIDX 40
#define JOERGER_TR16_N_INPUT_08_ENDIDX 41
#define JOERGER_TR16_N_INPUT_08_GAIN 42
#define JOERGER_TR16_N_INPUT_09 43
#define JOERGER_TR16_N_INPUT_09_STARTIDX 44
#define JOERGER_TR16_N_INPUT_09_ENDIDX 45
#define JOERGER_TR16_N_INPUT_09_GAIN 46
#define JOERGER_TR16_N_INPUT_10 47
#define JOERGER_TR16_N_INPUT_10_STARTIDX 48
#define JOERGER_TR16_N_INPUT_10_ENDIDX 49
#define JOERGER_TR16_N_INPUT_10_GAIN 50
#define JOERGER_TR16_N_INPUT_11 51
#define JOERGER_TR16_N_INPUT_11_STARTIDX 52
#define JOERGER_TR16_N_INPUT_11_ENDIDX 53
#define JOERGER_TR16_N_INPUT_11_GAIN 54
#define JOERGER_TR16_N_INPUT_12 55
#define JOERGER_TR16_N_INPUT_12_STARTIDX 56
#define JOERGER_TR16_N_INPUT_12_ENDIDX 57
#define JOERGER_TR16_N_INPUT_12_GAIN 58
#define JOERGER_TR16_N_INPUT_13 59
#define JOERGER_TR16_N_INPUT_13_STARTIDX 60
#define JOERGER_TR16_N_INPUT_13_ENDIDX 61
#define JOERGER_TR16_N_INPUT_13_GAIN 62
#define JOERGER_TR16_N_INPUT_14 63
#define JOERGER_TR16_N_INPUT_14_STARTIDX 64
#define JOERGER_TR16_N_INPUT_14_ENDIDX 65
#define JOERGER_TR16_N_INPUT_14_GAIN 66
#define JOERGER_TR16_N_INPUT_15 67
#define JOERGER_TR16_N_INPUT_15_STARTIDX 68
#define JOERGER_TR16_N_INPUT_15_ENDIDX 69
#define JOERGER_TR16_N_INPUT_15_GAIN 70
#define JOERGER_TR16_N_INPUT_16 71
#define JOERGER_TR16_N_INPUT_16_STARTIDX 72
#define JOERGER_TR16_N_INPUT_16_ENDIDX 73
#define JOERGER_TR16_N_INPUT_16_GAIN 74
#define JOERGER_TR16_N_INIT_ACTION 75
#define JOERGER_TR16_N_STORE_ACTION 76
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int mem_size;
  int active_mem;
  int active_mem_convert;
  int active_chans;
  int active_chans_convert;
  int post_trig;
  int post_trig_convert;
  float freq;
  int freq_convert;
  int master;
  int input_01_gain;
  int input_01_gain_convert;
  int input_02_gain;
  int input_02_gain_convert;
  int input_03_gain;
  int input_03_gain_convert;
  int input_04_gain;
  int input_04_gain_convert;
  int input_05_gain;
  int input_05_gain_convert;
  int input_06_gain;
  int input_06_gain_convert;
  int input_07_gain;
  int input_07_gain_convert;
  int input_08_gain;
  int input_08_gain_convert;
  int input_09_gain;
  int input_09_gain_convert;
  int input_10_gain;
  int input_10_gain_convert;
  int input_11_gain;
  int input_11_gain_convert;
  int input_12_gain;
  int input_12_gain_convert;
  int input_13_gain;
  int input_13_gain_convert;
  int input_14_gain;
  int input_14_gain_convert;
  int input_15_gain;
  int input_15_gain_convert;
  int input_16_gain;
  int input_16_gain_convert;
  struct descriptor *ext_clock_in;
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
  int mem_size;
  int active_mem;
  int active_chans;
  float freq;
  struct descriptor *ext_clock_in;
} InStoreStruct;
