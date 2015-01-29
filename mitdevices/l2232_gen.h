#define L2232_K_CONG_NODES 7
#define L2232_N_HEAD 0
#define L2232_N_CTS_NAME 1
#define L2232_N_COMMENT 2
#define L2232_N_TRIGGER 3
#define L2232_N_INPUTS 4
#define L2232_N_INIT_ACTION 5
#define L2232_N_STORE_ACTION 6
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *cts_name;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *cts_name;
} InStoreStruct;
