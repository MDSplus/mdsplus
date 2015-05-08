#define MIT_ENCODER_K_CONG_NODES 18
#define MIT_ENCODER_N_HEAD 0
#define MIT_ENCODER_N_NAME 1
#define MIT_ENCODER_N_COMMENT 2
#define MIT_ENCODER_N_INIT_ACTION 3
#define MIT_ENCODER_N_CHANNEL_1 4
#define MIT_ENCODER_N_CHANNEL_1_EVENT 5
#define MIT_ENCODER_N_CHANNEL_2 6
#define MIT_ENCODER_N_CHANNEL_2_EVENT 7
#define MIT_ENCODER_N_CHANNEL_3 8
#define MIT_ENCODER_N_CHANNEL_3_EVENT 9
#define MIT_ENCODER_N_CHANNEL_4 10
#define MIT_ENCODER_N_CHANNEL_4_EVENT 11
#define MIT_ENCODER_N_CHANNEL_5 12
#define MIT_ENCODER_N_CHANNEL_5_EVENT 13
#define MIT_ENCODER_N_CHANNEL_6 14
#define MIT_ENCODER_N_CHANNEL_6_EVENT 15
#define MIT_ENCODER_N_SOFT_CHANNEL 16
#define MIT_ENCODER_N_SOFT_CHANNEL_EVENT 17
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
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
} InSet_eventStruct;
