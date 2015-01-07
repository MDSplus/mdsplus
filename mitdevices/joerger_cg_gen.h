#define JOERGER_CG_K_CONG_NODES 12
#define JOERGER_CG_N_HEAD 0
#define JOERGER_CG_N_NAME 1
#define JOERGER_CG_N_COMMENT 2
#define JOERGER_CG_N_CHANNEL_1 3
#define JOERGER_CG_N_CHANNEL_1_INVERTED 4
#define JOERGER_CG_N_CHANNEL_2 5
#define JOERGER_CG_N_CHANNEL_2_INVERTED 6
#define JOERGER_CG_N_CHANNEL_3 7
#define JOERGER_CG_N_CHANNEL_3_INVERTED 8
#define JOERGER_CG_N_CHANNEL_4 9
#define JOERGER_CG_N_CHANNEL_4_INVERTED 10
#define JOERGER_CG_N_INIT_ACTION 11
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  struct descriptor *channel_1;
  struct descriptor *channel_2;
  struct descriptor *channel_3;
  struct descriptor *channel_4;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStopStruct;
