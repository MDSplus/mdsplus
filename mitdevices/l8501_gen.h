#define L8501_K_CONG_NODES 16
#define L8501_N_HEAD 0
#define L8501_N_NAME 1
#define L8501_N_COMMENT 2
#define L8501_N_TRIGGER1 3
#define L8501_N_TRIGGER2 4
#define L8501_N_TRIGGER3 5
#define L8501_N_MODE 6
#define L8501_N_FREQ1 7
#define L8501_N_FREQ2 8
#define L8501_N_FREQ3 9
#define L8501_N_F2_COUNT 10
#define L8501_N_F3_COUNT 11
#define L8501_N_CLOCK_OUT 12
#define L8501_N_STOP_OUT 13
#define L8501_N_INIT_ACTION 14
#define L8501_N_STORE_ACTION 15
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int mode;
  int mode_convert;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int mode;
  int mode_convert;
} InStoreStruct;
