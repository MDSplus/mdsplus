#define INCAA16_K_CONG_NODES 62
#define INCAA16_N_HEAD 0
#define INCAA16_N_NAME 1
#define INCAA16_N_COMMENT 2
#define INCAA16_N_INT_CLK_FRQ 3
#define INCAA16_N_CLOCK_IN 4
#define INCAA16_N_CLOCK_OUT 5
#define INCAA16_N_TRIG_IN 6
#define INCAA16_N_TRIG_OUT 7
#define INCAA16_N_MASTER 8
#define INCAA16_N_EXT_1MHZ 9
#define INCAA16_N_PTSC 10
#define INCAA16_N_ACTIVE_CHANS 11
#define INCAA16_N_INPUT_01 12
#define INCAA16_N_INPUT_01_STARTIDX 13
#define INCAA16_N_INPUT_01_ENDIDX 14
#define INCAA16_N_INPUT_02 15
#define INCAA16_N_INPUT_02_STARTIDX 16
#define INCAA16_N_INPUT_02_ENDIDX 17
#define INCAA16_N_INPUT_03 18
#define INCAA16_N_INPUT_03_STARTIDX 19
#define INCAA16_N_INPUT_03_ENDIDX 20
#define INCAA16_N_INPUT_04 21
#define INCAA16_N_INPUT_04_STARTIDX 22
#define INCAA16_N_INPUT_04_ENDIDX 23
#define INCAA16_N_INPUT_05 24
#define INCAA16_N_INPUT_05_STARTIDX 25
#define INCAA16_N_INPUT_05_ENDIDX 26
#define INCAA16_N_INPUT_06 27
#define INCAA16_N_INPUT_06_STARTIDX 28
#define INCAA16_N_INPUT_06_ENDIDX 29
#define INCAA16_N_INPUT_07 30
#define INCAA16_N_INPUT_07_STARTIDX 31
#define INCAA16_N_INPUT_07_ENDIDX 32
#define INCAA16_N_INPUT_08 33
#define INCAA16_N_INPUT_08_STARTIDX 34
#define INCAA16_N_INPUT_08_ENDIDX 35
#define INCAA16_N_INPUT_09 36
#define INCAA16_N_INPUT_09_STARTIDX 37
#define INCAA16_N_INPUT_09_ENDIDX 38
#define INCAA16_N_INPUT_10 39
#define INCAA16_N_INPUT_10_STARTIDX 40
#define INCAA16_N_INPUT_10_ENDIDX 41
#define INCAA16_N_INPUT_11 42
#define INCAA16_N_INPUT_11_STARTIDX 43
#define INCAA16_N_INPUT_11_ENDIDX 44
#define INCAA16_N_INPUT_12 45
#define INCAA16_N_INPUT_12_STARTIDX 46
#define INCAA16_N_INPUT_12_ENDIDX 47
#define INCAA16_N_INPUT_13 48
#define INCAA16_N_INPUT_13_STARTIDX 49
#define INCAA16_N_INPUT_13_ENDIDX 50
#define INCAA16_N_INPUT_14 51
#define INCAA16_N_INPUT_14_STARTIDX 52
#define INCAA16_N_INPUT_14_ENDIDX 53
#define INCAA16_N_INPUT_15 54
#define INCAA16_N_INPUT_15_STARTIDX 55
#define INCAA16_N_INPUT_15_ENDIDX 56
#define INCAA16_N_INPUT_16 57
#define INCAA16_N_INPUT_16_STARTIDX 58
#define INCAA16_N_INPUT_16_ENDIDX 59
#define INCAA16_N_INIT_ACTION 60
#define INCAA16_N_STORE_ACTION 61
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
