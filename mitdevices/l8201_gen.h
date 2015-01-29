#define L8201_K_CONG_NODES 7
#define L8201_N_HEAD 0
#define L8201_N_NAME 1
#define L8201_N_COMMENT 2
#define L8201_N_DOWNLOAD 3
#define L8201_N_UPLOAD 4
#define L8201_N_INIT_ACTION 5
#define L8201_N_STORE_ACTION 6
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  struct descriptor *download;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStoreStruct;
