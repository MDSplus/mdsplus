#define L2415_K_CONG_NODES 11
#define L2415_N_HEAD 0
#define L2415_N_NAME 1
#define L2415_N_COMMENT 2
#define L2415_N_POLARITY 3
#define L2415_N_RANGE 4
#define L2415_N_CURRENT 5
#define L2415_N_VOLTAGE 6
#define L2415_N_CURR_OUT 7
#define L2415_N_VOLT_OUT 8
#define L2415_N_INIT_ACTION 9
#define L2415_N_STORE_ACTION 10
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
  float polarity;
  float range;
  float current;
  float voltage;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStoreStruct;
