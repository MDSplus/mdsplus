#define L6810A_K_CONG_NODES 35
#define L6810A_N_HEAD 0
#define L6810A_N_NAME 1
#define L6810A_N_COMMENT 2
#define L6810A_N_FREQ 3
#define L6810A_N_EXT_CLOCK_IN 4
#define L6810A_N_STOP_TRIG 5
#define L6810A_N_MEMORIES 6
#define L6810A_N_ACTIVE_MEM 7
#define L6810A_N_ACTIVE_CHANS 8
#define L6810A_N_INPUT_01 9
#define L6810A_N_INPUT_01_STARTIDX 10
#define L6810A_N_INPUT_01_ENDIDX 11
#define L6810A_N_INPUT_01_FULL_SCALE 12
#define L6810A_N_INPUT_01_SRC_CPLING 13
#define L6810A_N_INPUT_01_OFFSET 14
#define L6810A_N_INPUT_02 15
#define L6810A_N_INPUT_02_STARTIDX 16
#define L6810A_N_INPUT_02_ENDIDX 17
#define L6810A_N_INPUT_02_FULL_SCALE 18
#define L6810A_N_INPUT_02_SRC_CPLING 19
#define L6810A_N_INPUT_02_OFFSET 20
#define L6810A_N_INPUT_03 21
#define L6810A_N_INPUT_03_STARTIDX 22
#define L6810A_N_INPUT_03_ENDIDX 23
#define L6810A_N_INPUT_03_FULL_SCALE 24
#define L6810A_N_INPUT_03_SRC_CPLING 25
#define L6810A_N_INPUT_03_OFFSET 26
#define L6810A_N_INPUT_04 27
#define L6810A_N_INPUT_04_STARTIDX 28
#define L6810A_N_INPUT_04_ENDIDX 29
#define L6810A_N_INPUT_04_FULL_SCALE 30
#define L6810A_N_INPUT_04_SRC_CPLING 31
#define L6810A_N_INPUT_04_OFFSET 32
#define L6810A_N_INIT_ACTION 33
#define L6810A_N_STORE_ACTION 34
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int memories;
  int active_mem;
  int active_mem_convert;
  int active_chans;
  int freq;
  int freq_convert;
  float input_01_full_scale;
  int input_01_full_scale_convert;
  float input_02_full_scale;
  int input_02_full_scale_convert;
  float input_03_full_scale;
  int input_03_full_scale_convert;
  float input_04_full_scale;
  int input_04_full_scale_convert;
  int input_01_src_cpling;
  int input_02_src_cpling;
  int input_03_src_cpling;
  int input_04_src_cpling;
  int input_01_offset;
  int input_02_offset;
  int input_03_offset;
  int input_04_offset;
  struct descriptor *ext_clock_in;
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
