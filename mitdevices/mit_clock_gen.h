#define MIT_CLOCK_K_CONG_NODES 4
#define MIT_CLOCK_N_HEAD 0
#define MIT_CLOCK_N_COMMENT 1
#define MIT_CLOCK_N_FREQUENCY 2
#define MIT_CLOCK_N_OUTPUT 3
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  struct descriptor *frequency;
} InGet_setupStruct;
