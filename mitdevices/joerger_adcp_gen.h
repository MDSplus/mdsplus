#define JOERGER_ADCP_K_CONG_NODES 7
#define JOERGER_ADCP_N_HEAD 0
#define JOERGER_ADCP_N_NAME 1
#define JOERGER_ADCP_N_COMMENT 2
#define JOERGER_ADCP_N_NO_TRIG 3
#define JOERGER_ADCP_N_INPUTS 4
#define JOERGER_ADCP_N_INIT_ACTION 5
#define JOERGER_ADCP_N_STORE_ACTION 6
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
  int no_trig;
} InStoreStruct;
