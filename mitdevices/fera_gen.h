#define FERA_K_CONG_NODES 9
#define FERA_N_HEAD 0
#define FERA_N_COMMENT 1
#define FERA_N_DIG_NAME 2
#define FERA_N_CNTRL_NAME 3
#define FERA_N_MEM_NAME 4
#define FERA_N_EXT_CLOCK 5
#define FERA_N_OUTPUT 6
#define FERA_N_INIT_ACTION 7
#define FERA_N_STORE_ACTION 8
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *cntrl_name;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *cntrl_name;
} InStoreStruct;
