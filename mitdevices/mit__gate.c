#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>

#include <treeshr.h>
#include <string.h>
#include "mit__gate_gen.h"
#include "decoder.h"
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

typedef struct descriptor *Dptr;

extern int mit__gate___get_setup(Dptr, InGet_setupStruct *);
extern int mit_decoder__get_event(int *, EventMask *);
extern int GenDeviceFree();

EXPORT int mit__gate__get_setup(struct descriptor *niddsc_ptr __attribute__ ((unused)), struct descriptor *method __attribute__ ((unused)), DecoderSetup * setup, EventMask * event_mask,
			 Dptr * output)
{
  int status;
  InGet_setupStruct s;
  status = mit__gate___get_setup(niddsc_ptr, &s);
  if (status & 1) {
    float delay = s.pulse_time - s.trigger;
    float max_period = max(delay, s.duration);
    float period;
    int clock_source;
    static int output_nid;
    static DESCRIPTOR_NID(out, &output_nid);
    int start_low_nid = s.head_nid + MIT__GATE_N_START_LOW;
    int invert = TreeIsOn(start_low_nid);
    for (clock_source = EXT_1MHZ, period = 1E-6;
	 period * 65534 < max_period && clock_source <= EXT_100HZ; clock_source++, period *= 10) ;
    if (clock_source > EXT_100HZ) {
      status = TIMING$_INVDELDUR;
      goto error;
    }
    setup->output_control = TOGGLE;
    setup->start_high = (invert == TreeOFF) || (invert == TreeBOTH_OFF);
    setup->count_up = 0;
    setup->bcd_count = 0;
    setup->repeat_count = 0;
    setup->double_load = s.duration > 0;
    setup->special_gate = 0;
    setup->clock_source = clock_source;
    setup->falling_edge = 0;
    switch (s.trigger_mode) {
    case TM_EVENT_TRIGGER:
    case TM_RISING_TRIGGER:
      setup->gating = GATE_RISING;
      break;
    case TM_FALLING_TRIGGER:
      setup->gating = GATE_FALLING;
      break;
    case TM_SOFTWARE_TRIGGER:
      setup->gating = GATE_NONE;
      break;
    }
    setup->load = max(delay / period + .5, 3);
    setup->hold = max(s.duration / period + .5, 3);
    if (s.trigger_mode == TM_EVENT_TRIGGER) {
      int event_nid = s.head_nid + MIT__GATE_N_EVENT;
      status = mit_decoder__get_event(&event_nid, event_mask);
      if (!(status & 1))
	goto error;
    } else
      memset(event_mask, 0, sizeof(EventMask));
    output_nid = s.head_nid + MIT__GATE_N_GATE_OUT;
    *output = &out;
    GenDeviceFree(&s);
  }
  return status;
 error:
  GenDeviceFree(&s);
  return status;
}
