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
#include "mit_gclock_gen.h"
#include "decoder.h"
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

typedef struct descriptor *Dptr;

extern int mit_gclock___get_setup(Dptr, InGet_setupStruct *);
extern int GenDeviceFree();

EXPORT int mit_gclock__get_setup(struct descriptor *niddsc_ptr __attribute__ ((unused)), struct descriptor *method __attribute__ ((unused)), DecoderSetup * setup,
			  EventMask * event_mask, Dptr * output)
{
  int status;
  InGet_setupStruct s;
  status = mit_gclock___get_setup(niddsc_ptr, &s);
  if (status & 1) {
    float duty_cycle = 0.;
    int invert = 0;
    static float frequency[2];
    float max_period;
    float period;
    static float dt;
    DESCRIPTOR_FLOAT(dt_dsc, &dt);
    int pulses;
    int clock_source;
    static int gate_nid;
    DESCRIPTOR_NID(gate_dsc, (char *)&gate_nid);
    static DESCRIPTOR(output_exp, "PACK($1+$2*.5,REPLICATE([1,0],0,SIZE($1)/2)) : \
                                       PACK($1,REPLICATE([0,1],0,SIZE($1)/2)) : \
                                       $2");
    static EMPTYXD(out);
    DESCRIPTOR_A(frequency_a, sizeof(float), DTYPE_NATIVE_FLOAT, (char *)frequency,
		 sizeof(frequency));
    gate_nid = s.head_nid + MIT_GCLOCK_N_GATE;
    memset(event_mask, 0, sizeof(EventMask));
    status = TdiData((struct descriptor *)s.frequency, (struct descriptor *)&frequency_a MDS_END_ARG);
    if (!(status & 1)) {
      status = TIMING$_INVCLKFRQ;
      goto error;
    }
    if (frequency[1] == frequency[0])
      duty_cycle = .5;
    else if (frequency[1] < 0.0) {
      invert = 1;
      duty_cycle = -frequency[1];
    } else if (frequency[1] > 0.0)
      duty_cycle = frequency[1];
    if ((frequency[0] <= 0.0) || (duty_cycle <= 0.0) || (duty_cycle >= 1.0)) {
      status = TIMING$_INVCLKFRQ;
      goto error;
    }
    max_period = 1 / frequency[0];
    for (clock_source = EXT_1MHZ, period = 1E-6;
	 period * 65534 < max_period && clock_source <= EXT_100HZ; clock_source++, period *= 10) ;
    if (clock_source > EXT_100HZ) {
      status = TIMING$_INVCLKFRQ;
      goto error;
    }
    setup->output_control = TOGGLE;
    setup->start_high = 0;
    setup->count_up = 0;
    setup->bcd_count = 0;
    setup->repeat_count = 1;
    setup->double_load = 1;
    setup->special_gate = 0;
    setup->clock_source = clock_source;
    setup->falling_edge = 0;
    setup->gating = GATE_N_HIGH;
    pulses = max_period / period + .4999;
    setup->load = pulses * duty_cycle + .4999;
    setup->hold = pulses - setup->load;
    if (setup->load == 0) {
      setup->load++;
      setup->hold--;
    }
    if (setup->hold == 0) {
      setup->load--;
      setup->hold++;
    }
    dt = period * (setup->load + setup->hold);
    if (invert) {
      int tmp = setup->load;
      setup->load = setup->hold;
      setup->hold = tmp;
      setup->start_high = 1;
    }
    status = TdiCompile((struct descriptor *)&output_exp, &gate_dsc, &dt_dsc, &out MDS_END_ARG);
    if (status & 1) {
      static int output_nid;
      static DESCRIPTOR_NID(output_dsc, (char *)&output_nid);
      output_nid = s.head_nid + MIT_GCLOCK_N_OUTPUT;
      status = TreePutRecord(output_nid, (struct descriptor *)&out, 0);
      *output = (status & 1) ? &output_dsc : 0;
    } else
      *output = 0;
    GenDeviceFree(&s);
  }
  return status;
 error:
  GenDeviceFree(&s);
  return status;
}
