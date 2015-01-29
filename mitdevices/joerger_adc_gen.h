#define JOERGER_ADC_K_CONG_NODES 6
#define JOERGER_ADC_N_HEAD 0
#define JOERGER_ADC_N_NAME 1
#define JOERGER_ADC_N_COMMENT 2
#define JOERGER_ADC_N_DATA 3
#define JOERGER_ADC_N_VSTRAP 4
#define JOERGER_ADC_N_STORE_ACTION 5
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *name;
} InStoreStruct;
