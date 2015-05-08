#define HV1440_K_CONG_NODES 26
#define HV1440_N_HEAD 0
#define HV1440_N_NAME 1
#define HV1440_N_COMMENT 2
#define HV1440_N_RANGE 3
#define HV1440_N_POD_01 4
#define HV1440_N_POD_02 5
#define HV1440_N_POD_03 6
#define HV1440_N_POD_04 7
#define HV1440_N_POD_05 8
#define HV1440_N_POD_06 9
#define HV1440_N_POD_07 10
#define HV1440_N_POD_08 11
#define HV1440_N_POD_09 12
#define HV1440_N_POD_10 13
#define HV1440_N_POD_11 14
#define HV1440_N_POD_12 15
#define HV1440_N_POD_13 16
#define HV1440_N_POD_14 17
#define HV1440_N_POD_15 18
#define HV1440_N_POD_16 19
#define HV1440_N_READOUT 20
#define HV1440_N_FRAME 21
#define HV1440_N_INIT_ACTION 22
#define HV1440_N_STORE_ACTION 23
#define HV1440_N_ON_ACTION 24
#define HV1440_N_OFF_ACTION 25
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int frame;
  float range;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int frame;
  float range;
} InStoreStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InOnStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InOffStruct;
