#define MDSDCL_K_CONG_NODES 5
#define MDSDCL_N_HEAD 0
#define MDSDCL_N_COMMENT 1
#define MDSDCL_N_VERBS 2
#define MDSDCL_N_COMMANDS 3
#define MDSDCL_N_EXEC_ACTION 4
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *commands;
  char *verbs;
} InExecuteStruct;
