#define A14_K_CONG_NODES 33
#define A14_N_HEAD 0
#define A14_N_NAME 1
#define A14_N_COMMENT 2
#define A14_N_CLOCK_DIVIDE 3
#define A14_N_EXT_CLOCK_IN 4
#define A14_N_STOP_TRIG 5
#define A14_N_PTS 6
#define A14_N_MODE 7
#define A14_N_DIMENSION 8
#define A14_N_CLK_POLARITY 9
#define A14_N_STR_POLARITY 10
#define A14_N_STP_POLARITY 11
#define A14_N_GATED 12
#define A14_N_INPUT_1 13
#define A14_N_INPUT_1_STARTIDX 14
#define A14_N_INPUT_1_ENDIDX 15
#define A14_N_INPUT_2 16
#define A14_N_INPUT_2_STARTIDX 17
#define A14_N_INPUT_2_ENDIDX 18
#define A14_N_INPUT_3 19
#define A14_N_INPUT_3_STARTIDX 20
#define A14_N_INPUT_3_ENDIDX 21
#define A14_N_INPUT_4 22
#define A14_N_INPUT_4_STARTIDX 23
#define A14_N_INPUT_4_ENDIDX 24
#define A14_N_INPUT_5 25
#define A14_N_INPUT_5_STARTIDX 26
#define A14_N_INPUT_5_ENDIDX 27
#define A14_N_INPUT_6 28
#define A14_N_INPUT_6_STARTIDX 29
#define A14_N_INPUT_6_ENDIDX 30
#define A14_N_INIT_ACTION 31
#define A14_N_STORE_ACTION 32
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  int clock_divide;
  int clock_divide_convert;
  struct descriptor *ext_clock_in;
  int pts;
  int mode;
  int clk_polarity;
  int str_polarity;
  int stp_polarity;
  int gated;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStartStruct;
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
  struct descriptor *ext_clock_in;
  struct descriptor *dimension;
} InStoreStruct;
