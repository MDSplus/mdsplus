#define U_OF_M_SPECT_K_CONG_NODES 21
#define U_OF_M_SPECT_N_HEAD 0
#define U_OF_M_SPECT_N_COMMENT 1
#define U_OF_M_SPECT_N_GO_FILE 2
#define U_OF_M_SPECT_N_DATA_FILE 3
#define U_OF_M_SPECT_N_TRIGGER 4
#define U_OF_M_SPECT_N_SETUP 5
#define U_OF_M_SPECT_N_SETUP_WAVE_LENGTH 6
#define U_OF_M_SPECT_N_SETUP_SLIT_WIDTH 7
#define U_OF_M_SPECT_N_SETUP_NUM_SPECTRA 8
#define U_OF_M_SPECT_N_SETUP_GRATING_TYPE 9
#define U_OF_M_SPECT_N_SETUP_EXPOSURE 10
#define U_OF_M_SPECT_N_SETUP_FILTER 11
#define U_OF_M_SPECT_N_WAVE_LENGTH 12
#define U_OF_M_SPECT_N_SLIT_WIDTH 13
#define U_OF_M_SPECT_N_GRATING_TYPE 14
#define U_OF_M_SPECT_N_EXPOSURE 15
#define U_OF_M_SPECT_N_FILTER 16
#define U_OF_M_SPECT_N_SPECTRA 17
#define U_OF_M_SPECT_N_ORDER_SORTER 18
#define U_OF_M_SPECT_N_INIT_ACTION 19
#define U_OF_M_SPECT_N_STORE_ACTION 20
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *go_file;
  char *data_file;
  int setup_wave_length;
  int setup_slit_width;
  int setup_num_spectra;
  int setup_grating_type;
  int setup_exposure;
  int setup_filter;
} InInitStruct;
typedef struct {
  struct descriptor_xd *__xds;
  int __num_xds;
  int head_nid;
  char *data_file;
} InStoreStruct;
