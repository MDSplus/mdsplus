#define MIT_DCLOCK_K_CONG_NODES 6
#define MIT_DCLOCK_N_HEAD 0
#define MIT_DCLOCK_N_COMMENT 1
#define MIT_DCLOCK_N_FREQUENCY_1 2
#define MIT_DCLOCK_N_FREQUENCY_2 3
#define MIT_DCLOCK_N_GATE 4
#define MIT_DCLOCK_N_OUTPUT 5
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  float frequency_1;
  float frequency_2;
} InGet_setupStruct;
