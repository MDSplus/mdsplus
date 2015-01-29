#define L2256_K_CONG_NODES 8
#define L2256_N_HEAD 0
#define L2256_N_NAME 1
#define L2256_N_COMMENT 2
#define L2256_N_TRIGGER 3
#define L2256_N_EXT_CLOCK 4
#define L2256_N_INPUT 5
#define L2256_N_INIT_ACTION 6
#define L2256_N_STORE_ACTION 7
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
} InTriggerStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStoreStruct;
