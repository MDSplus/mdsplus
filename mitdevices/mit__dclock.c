#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <string.h>
#include "mit__dclock_gen.h"
#include "decoder.h"
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

typedef struct descriptor *Dptr;

extern int mit__dclock___get_setup(Dptr, InGet_setupStruct *);
extern int GenDeviceFree();

EXPORT int mit__dclock__get_setup(Dptr niddsc_ptr, Dptr method, DecoderSetup * setup,
			   EventMask * event_mask, Dptr * output)
{
  int status;
  InGet_setupStruct s;
  static int output_nid;
  static DESCRIPTOR_NID(output_dsc, (char *)&output_nid);
  status = mit__dclock___get_setup(niddsc_ptr, &s);
  if (status & 1) {
    int start_low_nid = s.head_nid + MIT__DCLOCK_N_START_LOW;
    int invert = TreeIsOn(start_low_nid);
    float max_period;
    float period;
    int clock_source;
    memset(event_mask, 0, sizeof(EventMask));
    max_period = 1 / min(s.frequency_1, s.frequency_2);
    for (clock_source = EXT_1MHZ, period = 1E-6;
	 period * 65534 < max_period && clock_source <= EXT_100HZ; clock_source++, period *= 10) ;
    setup->output_control = TOGGLE;
    setup->start_high = (invert == TreeOFF) || (invert == TreeBOTH_OFF);
    setup->count_up = 0;
    setup->bcd_count = 0;
    setup->repeat_count = 1;
    setup->double_load = 1;
    setup->special_gate = 1;
    setup->clock_source = clock_source;
    setup->falling_edge = 0;
    setup->gating = GATE_NONE;
    setup->load = 1 / s.frequency_1 / period / 2 + .5;
    setup->hold = 1 / s.frequency_2 / period / 2 + .5;
    if (setup->load == 0)
      setup->load++;
    if (setup->hold == 0)
      setup->hold++;
    output_nid = s.head_nid + MIT__DCLOCK_N_EDGES_R;
    *output = &output_dsc;
    GenDeviceFree(&s);
  }
  return status;
}
