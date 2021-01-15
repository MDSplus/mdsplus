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
#include "mit_pulse_gen.h"
#include "decoder.h"
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

typedef struct descriptor *Dptr;

extern int mit_pulse___get_setup(Dptr, InGet_setupStruct *);
extern int mit_decoder__get_event(int *, EventMask *);
extern int GenDeviceFree();

static int GetSetup(struct descriptor *niddsc_ptr __attribute__ ((unused)), struct descriptor *method __attribute__ ((unused)), DecoderSetup * setup, EventMask * event_mask,
		    Dptr * output, int gate);

EXPORT int mit_pulse__get_setup(struct descriptor *niddsc_ptr __attribute__ ((unused)), struct descriptor *method __attribute__ ((unused)), DecoderSetup * setup, EventMask * event_mask,
			 Dptr * output)
{
  return GetSetup(niddsc_ptr, method, setup, event_mask, output, 0);
}

EXPORT int mit_gate__get_setup(struct descriptor *niddsc_ptr __attribute__ ((unused)), struct descriptor *method __attribute__ ((unused)), DecoderSetup * setup, EventMask * event_mask,
			Dptr * output)
{
  return GetSetup(niddsc_ptr, method, setup, event_mask, output, 1);
}

static int GetSetup(struct descriptor *niddsc_ptr __attribute__ ((unused)), struct descriptor *method __attribute__ ((unused)), DecoderSetup * setup, EventMask * event_mask,
		    Dptr * output, int gate)
{
  int status;
  InGet_setupStruct s;
  status = mit_pulse___get_setup(niddsc_ptr, &s);
  if (status & 1) {
    float delay = s.pulse_time - s.trigger;
    float max_period = max(delay, s.duration);
    float period;
    static float gate_start;
    static float gate_end;
    static int trigger_nid;
    static int output_nid;
    int clock_source;
    static EMPTYXD(out);
    DESCRIPTOR_NID(trigger_nid_dsc, (char *)&trigger_nid);
    static DESCRIPTOR_NID(output_nid_dsc, (char *)0);
    DESCRIPTOR_FLOAT(gate_start_dsc, &gate_start);
    DESCRIPTOR_FLOAT(gate_end_dsc, &gate_end);
    trigger_nid = s.head_nid + MIT_PULSE_N_TRIGGER;
    output_nid = s.head_nid + MIT_PULSE_N_OUTPUT;
    output_nid_dsc.pointer = (char *)&output_nid;
    for (clock_source = EXT_1MHZ, period = 1E-6;
	 period * 65534 < max_period && clock_source <= EXT_100HZ; clock_source++, period *= 10) ;
    if (clock_source > EXT_100HZ)
      return TIMING$_INVDELDUR;
    switch (s.output_mode) {
    case PO_HIGH_PULSES:
      setup->output_control = HIGH_PULSES;
      setup->start_high = 0;
      break;
    case PO_LOW_PULSES:
      setup->output_control = LOW_PULSES;
      setup->start_high = 1;
      break;
    case PO_TOGGLE_INITIAL_HIGH:
      setup->output_control = TOGGLE;
      setup->start_high = 1;
      break;
    case PO_TOGGLE_INITIAL_LOW:
      setup->output_control = TOGGLE;
      setup->start_high = 0;
      break;
    default:
      return TIMING$_INVOUTCTR;
    }
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
    default:
      status = TIMING$_INVTRGMOD;
      goto error;
    }
    setup->load = max(delay / period + .499999, 3);
    gate_start = setup->load * period;
    setup->hold = max(s.duration / period + .499999, 3);
    gate_end = setup->hold * period + gate_start;
    if (s.trigger_mode == TM_EVENT_TRIGGER) {
      int event_nid = s.head_nid + MIT_PULSE_N_EVENT;
      status = mit_decoder__get_event(&event_nid, event_mask);
      if (!(status & 1))
	goto error;
    } else
      memset(event_mask, 0, sizeof(EventMask));
    if (gate) {
      static DESCRIPTOR(out_expression, "$1 + [$2,$3]");
      TdiCompile((struct descriptor *)&out_expression, &trigger_nid_dsc, &gate_start_dsc, &gate_end_dsc,
		 &out MDS_END_ARG);
    } else {
      static DESCRIPTOR(out_expression, "$1 + $2");
      TdiCompile((struct descriptor *)&out_expression, &trigger_nid_dsc, &gate_start_dsc, &out MDS_END_ARG);
    }
    status = TreePutRecord(output_nid, (struct descriptor *)&out, 0);
    *output = &output_nid_dsc;
    GenDeviceFree(&s);
  }
  return status;
 error:
  GenDeviceFree(&s);
  return status;
}
