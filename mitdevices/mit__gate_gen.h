#define MIT__GATE_K_CONG_NODES 11
#define MIT__GATE_N_HEAD 0
#define MIT__GATE_N_COMMENT 1
#define MIT__GATE_N_TRIGGER_MODE 2
#define MIT__GATE_N_START_LOW 3
#define MIT__GATE_N_TRIGGER 4
#define MIT__GATE_N_EVENT 5
#define MIT__GATE_N_DURATION 6
#define MIT__GATE_N_PULSE_TIME 7
#define MIT__GATE_N_GATE_OUT 8
#define MIT__GATE_N_EDGES_R 9
#define MIT__GATE_N_EDGES_F 10
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  float trigger;
  float pulse_time;
  float duration;
  int trigger_mode;
} InGet_setupStruct;
