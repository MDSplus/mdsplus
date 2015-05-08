#define L8210_K_CONG_NODES 26
#define L8210_N_HEAD 0
#define L8210_N_NAME 1
#define L8210_N_COMMENT 2
#define L8210_N_EXT_CLOCK_IN 3
#define L8210_N_STOP_TRIG 4
#define L8210_N_CLOCK_OUT 5
#define L8210_N_MEMORIES 6
#define L8210_N_HEADER 7
#define L8210_N_INPUT_1 8
#define L8210_N_INPUT_1_STARTIDX 9
#define L8210_N_INPUT_1_ENDIDX 10
#define L8210_N_INPUT_1_USETIMES 11
#define L8210_N_INPUT_2 12
#define L8210_N_INPUT_2_STARTIDX 13
#define L8210_N_INPUT_2_ENDIDX 14
#define L8210_N_INPUT_2_USETIMES 15
#define L8210_N_INPUT_3 16
#define L8210_N_INPUT_3_STARTIDX 17
#define L8210_N_INPUT_3_ENDIDX 18
#define L8210_N_INPUT_3_USETIMES 19
#define L8210_N_INPUT_4 20
#define L8210_N_INPUT_4_STARTIDX 21
#define L8210_N_INPUT_4_ENDIDX 22
#define L8210_N_INPUT_4_USETIMES 23
#define L8210_N_INIT_ACTION 24
#define L8210_N_STORE_ACTION 25
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  struct descriptor *ext_clock_in;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int memories;
  char *header;
} InStoreStruct;
