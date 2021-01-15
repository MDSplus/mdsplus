/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
