#define JOERGER_TR612_K_CONG_NODES 29
#define JOERGER_TR612_N_HEAD 0
#define JOERGER_TR612_N_NAME 1
#define JOERGER_TR612_N_COMMENT 2
#define JOERGER_TR612_N_ACTMEM 3
#define JOERGER_TR612_N_PRETRIG 4
#define JOERGER_TR612_N_MODE 5
#define JOERGER_TR612_N_FREQUENCY 6
#define JOERGER_TR612_N_EXT_CLOCK 7
#define JOERGER_TR612_N_TRIGGER 8
#define JOERGER_TR612_N_INPUT_1 9
#define JOERGER_TR612_N_INPUT_1_STARTIDX 10
#define JOERGER_TR612_N_INPUT_1_ENDIDX 11
#define JOERGER_TR612_N_INPUT_2 12
#define JOERGER_TR612_N_INPUT_2_STARTIDX 13
#define JOERGER_TR612_N_INPUT_2_ENDIDX 14
#define JOERGER_TR612_N_INPUT_3 15
#define JOERGER_TR612_N_INPUT_3_STARTIDX 16
#define JOERGER_TR612_N_INPUT_3_ENDIDX 17
#define JOERGER_TR612_N_INPUT_4 18
#define JOERGER_TR612_N_INPUT_4_STARTIDX 19
#define JOERGER_TR612_N_INPUT_4_ENDIDX 20
#define JOERGER_TR612_N_INPUT_5 21
#define JOERGER_TR612_N_INPUT_5_STARTIDX 22
#define JOERGER_TR612_N_INPUT_5_ENDIDX 23
#define JOERGER_TR612_N_INPUT_6 24
#define JOERGER_TR612_N_INPUT_6_STARTIDX 25
#define JOERGER_TR612_N_INPUT_6_ENDIDX 26
#define JOERGER_TR612_N_INIT_ACTION 27
#define JOERGER_TR612_N_STORE_ACTION 28
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int actmem;
  int pretrig;
  int mode;
  int frequency;
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
} InStopStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStoreStruct;
