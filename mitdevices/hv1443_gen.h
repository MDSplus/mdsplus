#define HV1443_K_CONG_NODES 17
#define HV1443_N_HEAD 0
#define HV1443_N_VOLTAGE_01 1
#define HV1443_N_VOLTAGE_02 2
#define HV1443_N_VOLTAGE_03 3
#define HV1443_N_VOLTAGE_04 4
#define HV1443_N_VOLTAGE_05 5
#define HV1443_N_VOLTAGE_06 6
#define HV1443_N_VOLTAGE_07 7
#define HV1443_N_VOLTAGE_08 8
#define HV1443_N_VOLTAGE_09 9
#define HV1443_N_VOLTAGE_10 10
#define HV1443_N_VOLTAGE_11 11
#define HV1443_N_VOLTAGE_12 12
#define HV1443_N_VOLTAGE_13 13
#define HV1443_N_VOLTAGE_14 14
#define HV1443_N_VOLTAGE_15 15
#define HV1443_N_VOLTAGE_16 16
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  struct descriptor *voltage_01;
} InGet_settingsStruct;
