#define L8212_08_K_CONG_NODES 37
#define L8212_08_N_HEAD 0
#define L8212_08_N_NAME 1
#define L8212_08_N_COMMENT 2
#define L8212_08_N_FREQ 3
#define L8212_08_N_EXT_CLOCK_IN 4
#define L8212_08_N_STOP_TRIG 5
#define L8212_08_N_MEMORIES 6
#define L8212_08_N_HEADER 7
#define L8212_08_N_PTS 8
#define L8212_08_N_NOC 9
#define L8212_08_N_OFFSET 10
#define L8212_08_N_INIT_ACTION 11
#define L8212_08_N_STORE_ACTION 12
#define L8212_08_N_INPUT_01 13
#define L8212_08_N_INPUT_01_STARTIDX 14
#define L8212_08_N_INPUT_01_ENDIDX 15
#define L8212_08_N_INPUT_02 16
#define L8212_08_N_INPUT_02_STARTIDX 17
#define L8212_08_N_INPUT_02_ENDIDX 18
#define L8212_08_N_INPUT_03 19
#define L8212_08_N_INPUT_03_STARTIDX 20
#define L8212_08_N_INPUT_03_ENDIDX 21
#define L8212_08_N_INPUT_04 22
#define L8212_08_N_INPUT_04_STARTIDX 23
#define L8212_08_N_INPUT_04_ENDIDX 24
#define L8212_08_N_INPUT_05 25
#define L8212_08_N_INPUT_05_STARTIDX 26
#define L8212_08_N_INPUT_05_ENDIDX 27
#define L8212_08_N_INPUT_06 28
#define L8212_08_N_INPUT_06_STARTIDX 29
#define L8212_08_N_INPUT_06_ENDIDX 30
#define L8212_08_N_INPUT_07 31
#define L8212_08_N_INPUT_07_STARTIDX 32
#define L8212_08_N_INPUT_07_ENDIDX 33
#define L8212_08_N_INPUT_08 34
#define L8212_08_N_INPUT_08_STARTIDX 35
#define L8212_08_N_INPUT_08_ENDIDX 36
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int noc;
  int pts;
  int offset;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int memories;
  char *header;
  int offset;
} InStoreStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InTriggerStruct;
