#define L8590_MEM_K_CONG_NODES 6
#define L8590_MEM_N_HEAD 0
#define L8590_MEM_N_NAME 1
#define L8590_MEM_N_COMMENT 2
#define L8590_MEM_N_LATCH 3
#define L8590_MEM_N_INIT_ACTION 4
#define L8590_MEM_N_STORE_ACTION 5
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
