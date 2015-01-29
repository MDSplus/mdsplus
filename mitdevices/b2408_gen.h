#define B2408_K_CONG_NODES 8
#define B2408_N_HEAD 0
#define B2408_N_NAME 1
#define B2408_N_COMMENT 2
#define B2408_N_TRIGGER 3
#define B2408_N_EXT_CLOCK 4
#define B2408_N_TIMEBASE 5
#define B2408_N_INIT_ACTION 6
#define B2408_N_STORE_ACTION 7
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
