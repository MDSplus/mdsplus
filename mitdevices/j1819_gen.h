#define J1819_K_CONG_NODES 10
#define J1819_N_HEAD 0
#define J1819_N_NAME 1
#define J1819_N_COMMENT 2
#define J1819_N_EXT_CLOCK_IN 3
#define J1819_N_STOP_TRIG 4
#define J1819_N_SAMPLES 5
#define J1819_N_INPUT 6
#define J1819_N_TRUE_PHASE 7
#define J1819_N_INIT_ACTION 8
#define J1819_N_STORE_ACTION 9
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int samples;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int samples;
} InStoreStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InTriggerStruct;
