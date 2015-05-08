#define L4202_K_CONG_NODES 13
#define L4202_N_HEAD 0
#define L4202_N_NAME 1
#define L4202_N_COMMENT 2
#define L4202_N_MEMORY_NAME 3
#define L4202_N_EXT_CLOCK 4
#define L4202_N_NUM_SPECTRA 5
#define L4202_N_ROUTER_INC 6
#define L4202_N_OFFSET 7
#define L4202_N_RESOLUTION 8
#define L4202_N_LOOP_INHIBIT 9
#define L4202_N_SPECTRA 10
#define L4202_N_INIT_ACTION 11
#define L4202_N_STORE_ACTION 12
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  char *memory_name;
  int router_inc;
  int num_spectra;
  int offset;
  float resolution;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *memory_name;
  int router_inc;
  int num_spectra;
} InStoreStruct;
