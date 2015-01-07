#define IDL_K_CONG_NODES 4
#define IDL_N_HEAD 0
#define IDL_N_COMMENT 1
#define IDL_N_COMMANDS 2
#define IDL_N_EXEC_ACTION 3
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *commands;
} InExecuteStruct;
