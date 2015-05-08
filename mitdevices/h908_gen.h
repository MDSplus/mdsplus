#define H908_K_CONG_NODES 106
#define H908_N_HEAD 0
#define H908_N_NAME 1
#define H908_N_COMMENT 2
#define H908_N_EXT_CLOCK 3
#define H908_N_TRIGGER 4
#define H908_N_PTS 5
#define H908_N_INT_CLOCK 6
#define H908_N_ACTIVE_CHANS 7
#define H908_N_INPUT_01 8
#define H908_N_INPUT_01_STARTIDX 9
#define H908_N_INPUT_01_ENDIDX 10
#define H908_N_INPUT_02 11
#define H908_N_INPUT_02_STARTIDX 12
#define H908_N_INPUT_02_ENDIDX 13
#define H908_N_INPUT_03 14
#define H908_N_INPUT_03_STARTIDX 15
#define H908_N_INPUT_03_ENDIDX 16
#define H908_N_INPUT_04 17
#define H908_N_INPUT_04_STARTIDX 18
#define H908_N_INPUT_04_ENDIDX 19
#define H908_N_INPUT_05 20
#define H908_N_INPUT_05_STARTIDX 21
#define H908_N_INPUT_05_ENDIDX 22
#define H908_N_INPUT_06 23
#define H908_N_INPUT_06_STARTIDX 24
#define H908_N_INPUT_06_ENDIDX 25
#define H908_N_INPUT_07 26
#define H908_N_INPUT_07_STARTIDX 27
#define H908_N_INPUT_07_ENDIDX 28
#define H908_N_INPUT_08 29
#define H908_N_INPUT_08_STARTIDX 30
#define H908_N_INPUT_08_ENDIDX 31
#define H908_N_INPUT_09 32
#define H908_N_INPUT_09_STARTIDX 33
#define H908_N_INPUT_09_ENDIDX 34
#define H908_N_INPUT_10 35
#define H908_N_INPUT_10_STARTIDX 36
#define H908_N_INPUT_10_ENDIDX 37
#define H908_N_INPUT_11 38
#define H908_N_INPUT_11_STARTIDX 39
#define H908_N_INPUT_11_ENDIDX 40
#define H908_N_INPUT_12 41
#define H908_N_INPUT_12_STARTIDX 42
#define H908_N_INPUT_12_ENDIDX 43
#define H908_N_INPUT_13 44
#define H908_N_INPUT_13_STARTIDX 45
#define H908_N_INPUT_13_ENDIDX 46
#define H908_N_INPUT_14 47
#define H908_N_INPUT_14_STARTIDX 48
#define H908_N_INPUT_14_ENDIDX 49
#define H908_N_INPUT_15 50
#define H908_N_INPUT_15_STARTIDX 51
#define H908_N_INPUT_15_ENDIDX 52
#define H908_N_INPUT_16 53
#define H908_N_INPUT_16_STARTIDX 54
#define H908_N_INPUT_16_ENDIDX 55
#define H908_N_INPUT_17 56
#define H908_N_INPUT_17_STARTIDX 57
#define H908_N_INPUT_17_ENDIDX 58
#define H908_N_INPUT_18 59
#define H908_N_INPUT_18_STARTIDX 60
#define H908_N_INPUT_18_ENDIDX 61
#define H908_N_INPUT_19 62
#define H908_N_INPUT_19_STARTIDX 63
#define H908_N_INPUT_19_ENDIDX 64
#define H908_N_INPUT_20 65
#define H908_N_INPUT_20_STARTIDX 66
#define H908_N_INPUT_20_ENDIDX 67
#define H908_N_INPUT_21 68
#define H908_N_INPUT_21_STARTIDX 69
#define H908_N_INPUT_21_ENDIDX 70
#define H908_N_INPUT_22 71
#define H908_N_INPUT_22_STARTIDX 72
#define H908_N_INPUT_22_ENDIDX 73
#define H908_N_INPUT_23 74
#define H908_N_INPUT_23_STARTIDX 75
#define H908_N_INPUT_23_ENDIDX 76
#define H908_N_INPUT_24 77
#define H908_N_INPUT_24_STARTIDX 78
#define H908_N_INPUT_24_ENDIDX 79
#define H908_N_INPUT_25 80
#define H908_N_INPUT_25_STARTIDX 81
#define H908_N_INPUT_25_ENDIDX 82
#define H908_N_INPUT_26 83
#define H908_N_INPUT_26_STARTIDX 84
#define H908_N_INPUT_26_ENDIDX 85
#define H908_N_INPUT_27 86
#define H908_N_INPUT_27_STARTIDX 87
#define H908_N_INPUT_27_ENDIDX 88
#define H908_N_INPUT_28 89
#define H908_N_INPUT_28_STARTIDX 90
#define H908_N_INPUT_28_ENDIDX 91
#define H908_N_INPUT_29 92
#define H908_N_INPUT_29_STARTIDX 93
#define H908_N_INPUT_29_ENDIDX 94
#define H908_N_INPUT_30 95
#define H908_N_INPUT_30_STARTIDX 96
#define H908_N_INPUT_30_ENDIDX 97
#define H908_N_INPUT_31 98
#define H908_N_INPUT_31_STARTIDX 99
#define H908_N_INPUT_31_ENDIDX 100
#define H908_N_INPUT_32 101
#define H908_N_INPUT_32_STARTIDX 102
#define H908_N_INPUT_32_ENDIDX 103
#define H908_N_INIT_ACTION 104
#define H908_N_STORE_ACTION 105
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int int_clock;
  int int_clock_convert;
  int active_chans;
  int active_chans_convert;
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
