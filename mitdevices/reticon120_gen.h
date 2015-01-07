#define RETICON120_K_CONG_NODES 29
#define RETICON120_N_HEAD 0
#define RETICON120_N_NAME 1
#define RETICON120_N_COMMENT 2
#define RETICON120_N_MEMORY_NAME 3
#define RETICON120_N_START_TRIG 4
#define RETICON120_N_PIXEL_SELECT 5
#define RETICON120_N_PIXEL_AXIS 6
#define RETICON120_N_DATA 7
#define RETICON120_N_EVENT_LATCH 8
#define RETICON120_N_NUM_STATES 9
#define RETICON120_N_FRAME_INDEX 10
#define RETICON120_N_STATE_0 11
#define RETICON120_N_STATE_0_PERIOD 12
#define RETICON120_N_STATE_0_FRAMES 13
#define RETICON120_N_STATE_0_FRAME_SELECT 14
#define RETICON120_N_STATE_1 15
#define RETICON120_N_STATE_1_PERIOD 16
#define RETICON120_N_STATE_1_FRAMES 17
#define RETICON120_N_STATE_1_FRAME_SELECT 18
#define RETICON120_N_STATE_2 19
#define RETICON120_N_STATE_2_PERIOD 20
#define RETICON120_N_STATE_2_FRAMES 21
#define RETICON120_N_STATE_2_FRAME_SELECT 22
#define RETICON120_N_STATE_3 23
#define RETICON120_N_STATE_3_PERIOD 24
#define RETICON120_N_STATE_3_FRAMES 25
#define RETICON120_N_STATE_3_FRAME_SELECT 26
#define RETICON120_N_INIT_ACTION 27
#define RETICON120_N_STORE_ACTION 28
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  char *memory_name;
  int num_states;
  struct descriptor *pixel_select;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  char *memory_name;
} InStoreStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  char *memory_name;
} InTriggerStruct;
