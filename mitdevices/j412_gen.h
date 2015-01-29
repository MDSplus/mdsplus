#define J412_K_CONG_NODES 9
#define J412_N_HEAD 0
#define J412_N_NAME 1
#define J412_N_COMMENT 2
#define J412_N_DT 3
#define J412_N_START_TRIG 4
#define J412_N_SET_POINTS 5
#define J412_N_OUTPUT 6
#define J412_N_NUM_CYCLES 7
#define J412_N_INIT_ACTION 8
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int num_cycles;
  struct descriptor *set_points;
} InInitStruct;
