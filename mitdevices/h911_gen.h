#define H911_K_CONG_NODES 38
#define H911_N_HEAD 0
#define H911_N_NAME 1
#define H911_N_COMMENT 2
#define H911_N_EXT_CLOCK 3
#define H911_N_CHANNEL_01 4
#define H911_N_CHANNEL_02 5
#define H911_N_CHANNEL_03 6
#define H911_N_CHANNEL_04 7
#define H911_N_CHANNEL_05 8
#define H911_N_CHANNEL_06 9
#define H911_N_CHANNEL_07 10
#define H911_N_CHANNEL_08 11
#define H911_N_CHANNEL_09 12
#define H911_N_CHANNEL_10 13
#define H911_N_CHANNEL_11 14
#define H911_N_CHANNEL_12 15
#define H911_N_CHANNEL_13 16
#define H911_N_CHANNEL_14 17
#define H911_N_CHANNEL_15 18
#define H911_N_CHANNEL_16 19
#define H911_N_CHANNEL_17 20
#define H911_N_CHANNEL_18 21
#define H911_N_CHANNEL_19 22
#define H911_N_CHANNEL_20 23
#define H911_N_CHANNEL_21 24
#define H911_N_CHANNEL_22 25
#define H911_N_CHANNEL_23 26
#define H911_N_CHANNEL_24 27
#define H911_N_CHANNEL_25 28
#define H911_N_CHANNEL_26 29
#define H911_N_CHANNEL_27 30
#define H911_N_CHANNEL_28 31
#define H911_N_CHANNEL_29 32
#define H911_N_CHANNEL_30 33
#define H911_N_CHANNEL_31 34
#define H911_N_CHANNEL_32 35
#define H911_N_INIT_ACTION 36
#define H911_N_STORE_ACTION 37
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
  char *name;
} InStoreStruct;
