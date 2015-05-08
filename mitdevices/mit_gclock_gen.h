#define MIT_GCLOCK_K_CONG_NODES 5
#define MIT_GCLOCK_N_HEAD 0
#define MIT_GCLOCK_N_COMMENT 1
#define MIT_GCLOCK_N_FREQUENCY 2
#define MIT_GCLOCK_N_GATE 3
#define MIT_GCLOCK_N_OUTPUT 4
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  struct descriptor *frequency;
} InGet_setupStruct;
