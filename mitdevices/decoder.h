typedef struct {
  unsigned output_control : 3;
  unsigned count_up : 1;
  unsigned bcd_count : 1;
  unsigned repeat_count : 1;
  unsigned double_load : 1;
  unsigned special_gate : 1;
  unsigned clock_source : 4;
  unsigned falling_edge : 1;
  unsigned gating : 3;
  unsigned short load;
  unsigned short hold;
  unsigned start_high : 1;
  unsigned : 7;
} DecoderSetup;

typedef struct {
  unsigned : 2;
  unsigned compare_1 : 1;
  unsigned compare_2 : 1;
  unsigned f_out_src : 4;
  unsigned f_out_div : 4;
  unsigned f_out_disabled : 1;
  unsigned data_bus_width : 1;
  unsigned data_ptr_ctrl : 1;
  unsigned int_scaler_ctrl : 1;
} MasterRegister;

typedef struct {
  unsigned int bits[4];
} EventMask;

/** output control settings **/
#define ALWAYS_HIGH 0
#define LOW_PULSES 1
#define TOGGLE 2
#define ALWAYS_LOW 4
#define HIGH_PULSES 5
/*        illegal           6
          illegal                         7
*****************************/

/** clock source settings ****/
#define TCN_1 0
#define EXT_1MHZ 1
#define EXT_100KHZ 2
#define EXT_10KHZ 3
#define EXT_1KHZ 4
#define EXT_100HZ 5
#define EVENT_OR_G0 6
#define EVENT_OR_G1 7
#define EVENT_OR_G2 8
#define EVENT_OR_G3 9
#define EVENT_OR_G4 10
#define INT_1MHZ 11
#define INT_100KHZ 12
#define INT_10KHZ 13
#define INT_1KHZ 14
#define INT_100HZ 15
/*****************************/

/******* gating control ******/
#define GATE_NONE 0
#define GATE_TCN_1 1
#define GATE_N_PLUS_1 2
#define GATE_N_1 3
#define GATE_N_HIGH 4
#define GATE_N_LOW 5
#define GATE_RISING 6
#define GATE_FALLING 7
/*****************************/

/** Pulse output modes **********/
#define PO_HIGH_PULSES 0
#define PO_LOW_PULSES 1
#define PO_TOGGLE_INITIAL_HIGH 2
#define PO_TOGGLE_INITIAL_LOW 3
/********************************/

/**** Pulse trigger modes ****/
#define TM_EVENT_TRIGGER 0
#define TM_RISING_TRIGGER 1
#define TM_FALLING_TRIGGER 2
#define TM_SOFTWARE_TRIGGER 3
/*****************************/
