#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>

#include <treeshr.h>
#include <string.h>
#include "mit__clock_gen.h"
#include "decoder.h"
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

typedef struct descriptor *Dptr;

extern int mit__clock___get_setup(Dptr, InGet_setupStruct *);
extern int GenDeviceFree();

EXPORT int mit__clock__get_setup(struct descriptor *niddsc_ptr __attribute__ ((unused)), struct descriptor *method __attribute__ ((unused)), DecoderSetup * setup,
			  EventMask * event_mask, Dptr * output)
{
  int status;
  InGet_setupStruct s;
  status = mit__clock___get_setup(niddsc_ptr, &s);
  if (status & 1) {
    static int output_nid;
    static DESCRIPTOR_NID(output_dsc, (char *)&output_nid);
    int invert = 0;
    int start_low_nid = s.head_nid + MIT__CLOCK_N_START_LOW;
    float max_period;
    float period;
    int pulses;
    int clock_source;
    memset(event_mask, 0, sizeof(EventMask));
    invert = TreeIsOn(start_low_nid);
    invert = (invert == TreeOFF) || (invert == TreeBOTH_OFF);
    max_period = 1 / s.frequency;
    for (clock_source = EXT_1MHZ, period = 1E-6;
	 period * 65534 < max_period && clock_source <= EXT_100HZ; clock_source++, period *= 10) ;
    setup->output_control = TOGGLE;
    setup->start_high = invert;
    setup->count_up = 0;
    setup->bcd_count = 0;
    setup->repeat_count = 1;
    setup->double_load = 1;
    setup->special_gate = 0;
    setup->clock_source = clock_source;
    setup->falling_edge = 0;
    setup->gating = GATE_NONE;
    pulses = max_period / period + .4999;
    setup->load = pulses * s.duty_cycle + .4999;
    setup->hold = pulses - setup->load;
    if (setup->load == 0) {
      setup->load++;
      setup->hold--;
    }
    if (setup->hold == 0) {
      setup->load--;
      setup->hold++;
    }
    if (invert) {
      int tmp = setup->load;
      setup->load = setup->hold;
      setup->hold = tmp;
    }
    output_nid = s.head_nid + MIT__CLOCK_N_EDGES_R;
    *output = &output_dsc;
    GenDeviceFree(&s);
  }
  return status;
}
