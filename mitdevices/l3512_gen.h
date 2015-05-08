#define L3512_K_CONG_NODES 12
#define L3512_N_HEAD 0
#define L3512_N_NAME 1
#define L3512_N_COMMENT 2
#define L3512_N_MEMORY_NAME 3
#define L3512_N_ROUTER_NAME 4
#define L3512_N_NUM_SPECTRA 5
#define L3512_N_NUM_CHANNELS 6
#define L3512_N_OFFSET 7
#define L3512_N_DURATION 8
#define L3512_N_SPECTRA 9
#define L3512_N_INIT_ACTION 10
#define L3512_N_STORE_ACTION 11
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
} InStoreStruct;
