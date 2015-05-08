#define MIT_PULSE_K_CONG_NODES 9
#define MIT_PULSE_N_HEAD 0
#define MIT_PULSE_N_COMMENT 1
#define MIT_PULSE_N_TRIGGER_MODE 2
#define MIT_PULSE_N_OUTPUT_MODE 3
#define MIT_PULSE_N_TRIGGER 4
#define MIT_PULSE_N_EVENT 5
#define MIT_PULSE_N_DURATION 6
#define MIT_PULSE_N_PULSE_TIME 7
#define MIT_PULSE_N_OUTPUT 8
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  float trigger;
  float pulse_time;
  float duration;
  int trigger_mode;
  int output_mode;
} InGet_setupStruct;
