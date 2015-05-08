#define MIT__CLOCK_K_CONG_NODES 8
#define MIT__CLOCK_N_HEAD 0
#define MIT__CLOCK_N_COMMENT 1
#define MIT__CLOCK_N_FREQUENCY 2
#define MIT__CLOCK_N_DUTY_CYCLE 3
#define MIT__CLOCK_N_TSTART 4
#define MIT__CLOCK_N_START_LOW 5
#define MIT__CLOCK_N_EDGES_R 6
#define MIT__CLOCK_N_EDGES_F 7
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  float frequency;
  float duty_cycle;
} InGet_setupStruct;
