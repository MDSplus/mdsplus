#define HV4032_K_CONG_NODES 16
#define HV4032_N_HEAD 0
#define HV4032_N_NAME 1
#define HV4032_N_COMMENT 2
#define HV4032_N_POD_1 3
#define HV4032_N_POD_2 4
#define HV4032_N_POD_3 5
#define HV4032_N_POD_4 6
#define HV4032_N_POD_5 7
#define HV4032_N_POD_6 8
#define HV4032_N_POD_7 9
#define HV4032_N_POD_8 10
#define HV4032_N_READOUT 11
#define HV4032_N_INIT_ACTION 12
#define HV4032_N_STORE_ACTION 13
#define HV4032_N_ON_ACTION 14
#define HV4032_N_OFF_ACTION 15
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
