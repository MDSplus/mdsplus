#define T4012_K_CONG_NODES 16
#define T4012_N_HEAD 0
#define T4012_N_NAME 1
#define T4012_N_COMMENT 2
#define T4012_N_CHANNELS 3
#define T4012_N_POST_SAMPLES 4
#define T4012_N_SMP_PER_CHAN 5
#define T4012_N_DISPLAY_CHAN 6
#define T4012_N_TRIGGER 7
#define T4012_N_FREQUENCY_1 8
#define T4012_N_FREQUENCY_2 9
#define T4012_N_SWITCH_MODE 10
#define T4012_N_SWITCH_TRIG 11
#define T4012_N_EXTERN_CLOCK 12
#define T4012_N_CLOCK_OUT 13
#define T4012_N_INIT_ACTION 14
#define T4012_N_STORE_ACTION 15
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int channels;
  int post_samples;
  int smp_per_chan;
  int switch_mode;
  int display_chan;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InTriggerStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStoreStruct;
