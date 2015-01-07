#define L3512A_K_CONG_NODES 13
#define L3512A_N_HEAD 0
#define L3512A_N_NAME 1
#define L3512A_N_COMMENT 2
#define L3512A_N_MEMORY_NAME 3
#define L3512A_N_ROUTER_NAME 4
#define L3512A_N_NUM_SPECTRA 5
#define L3512A_N_NUM_CHANNELS 6
#define L3512A_N_TRIGGER 7
#define L3512A_N_OFFSET 8
#define L3512A_N_DURATION 9
#define L3512A_N_SPECTRA 10
#define L3512A_N_INIT_ACTION 11
#define L3512A_N_STORE_ACTION 12
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  char *memory_name;
  char *router_name;
  int num_spectra;
  int num_channels;
  int offset;
  struct descriptor *duration;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  char *memory_name;
  int num_spectra;
  int num_channels;
  struct descriptor *duration;
  struct descriptor *trigger;
} InStoreStruct;
