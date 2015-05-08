#define B3224_K_CONG_NODES 6
#define B3224_N_HEAD 0
#define B3224_N_NAME 1
#define B3224_N_COMMENT 2
#define B3224_N_OUTPUT_1 3
#define B3224_N_OUTPUT_2 4
#define B3224_N_INIT_ACTION 5
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int output_1;
  int output_2;
} InInitStruct;
