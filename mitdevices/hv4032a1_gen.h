#define HV4032A1_K_CONG_NODES 5
#define HV4032A1_N_HEAD 0
#define HV4032A1_N_VOLTAGE_1 1
#define HV4032A1_N_VOLTAGE_2 2
#define HV4032A1_N_VOLTAGE_3 3
#define HV4032A1_N_VOLTAGE_4 4
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  struct descriptor *voltage_1;
} InGet_settingsStruct;
