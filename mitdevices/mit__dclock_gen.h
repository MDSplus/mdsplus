#define MIT__DCLOCK_K_CONG_NODES 9
#define MIT__DCLOCK_N_HEAD 0
#define MIT__DCLOCK_N_COMMENT 1
#define MIT__DCLOCK_N_FREQUENCY_1 2
#define MIT__DCLOCK_N_FREQUENCY_2 3
#define MIT__DCLOCK_N_TSTART 4
#define MIT__DCLOCK_N_START_LOW 5
#define MIT__DCLOCK_N_GATE 6
#define MIT__DCLOCK_N_EDGES_R 7
#define MIT__DCLOCK_N_EDGES_F 8
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  float frequency_1;
  float frequency_2;
} InGet_setupStruct;
