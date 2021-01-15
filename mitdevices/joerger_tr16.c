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
#include <stdlib.h>
#include "joerger_tr16_gen.h"
#include <stdio.h>
#include "devroutines.h"



static int one = 1;
static int zero = 0;

#define min(a,b) ((a) <= (b)) ? (a) : (b)
#define max(a,b) ((a) >= (b)) ? (a) : (b)
#define return_on_error(f) if (!((status = f) & 1)) return status;

#define pio(f,a,d,q)  return_on_error(DevCamChk(CamPiow(in_struct->name, a, f, d, 16, 0), &one, &q))
#define stop(f,a,n,d)  return_on_error(DevCamChk(CamStopw(in_struct->name, a, f, n, d, 24, 0), &one, &one))

typedef struct _f0a0 {
  unsigned clock:3;
  unsigned chans:3;
  unsigned mode:2;
  unsigned fill:8;
} F0A0;
typedef struct _f0a1 {
  unsigned active_memory:3;
  unsigned post_trig:4;
  unsigned master:1;
  unsigned fill:8;
} F0A1;

static int ReadChannel(char *name, int chan, int *samples_ptr, short *data_ptr, int use_enhanced);

int joerger_tr16___init(struct descriptor *nid_d_ptr __attribute__ ((unused)), InInitStruct * in_struct)
{
  int status;
  int *gains = &in_struct->input_01_gain;
  int i;
  F0A0 setup1;
  F0A1 setup2;

  pio(25, 1, 0, one);		/* Abort whatever we are doing */
  pio(9, 0, 0, zero);		/* clear the memory adress register */
  pio(10, 0, 0, zero);		/* clear the lam status */

  setup1.clock = in_struct->freq_convert;
  setup1.chans = in_struct->active_chans_convert;
  setup1.mode = 2;		/* use pre-post mode */
  pio(16, 0, (short *)&setup1, one);

  setup2.active_memory = in_struct->active_mem_convert;
  setup2.post_trig = in_struct->post_trig_convert;
  setup2.master = in_struct->master;
  pio(16, 1, (short *)&setup2, one)

      for (i = 0; i < 16; i++)
    pio(20, i, &gains[i * 2 + 1], one);

  pio(11, 0, 0, one);		/* arm the module */

  return status;
}

int joerger_tr16___trigger(struct descriptor *nid_d_ptr __attribute__ ((unused)), InTriggerStruct * in_struct)
{
  int status;
  pio(25, 0, 0, one);		/* Trigger the module */
  return 1;
}

#define JOERGER_TR16_K_NODES_PER_CHANNEL 4
#define JOERGER_TR16_N_CHANNELS JOERGER_TR16_N_INPUT_01
#define JOERGER_TR16_N_CHAN_HEAD 0
#define JOERGER_TR16_N_CHAN_STARTIDX 1
#define JOERGER_TR16_N_CHAN_ENDIDX 2
#define JOERGER_TR16_N_CHAN_GAINIDX 3

//extern unsigned short OpcAdd;
//extern unsigned short OpcMultiply;
//extern unsigned short OpcValue;

int joerger_tr16___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{
#undef return_on_error
#define return_on_error(f) if (!((status = f) & 1)) return status;
#undef pio
#define pio(f,a,d,q)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16, 0), &one, &q))
#define CHAN_NID(chan, field) c_nids[JOERGER_TR16_N_CHANNELS+chan*JOERGER_TR16_K_NODES_PER_CHANNEL+field]

  static int c_nids[JOERGER_TR16_K_CONG_NODES];

  static DESCRIPTOR_A_BOUNDS(raw, sizeof(short), DTYPE_W, 0, 1, 0);
  static DESCRIPTOR(counts_str, "counts");
  static DESCRIPTOR_WITH_UNITS(counts, &raw, &counts_str);
  static DESCRIPTOR_LONG(start_d, &raw.bounds[0].l);
  static DESCRIPTOR_LONG(end_d, &raw.bounds[0].u);
  static DESCRIPTOR_NID(trigger_d, &c_nids[JOERGER_TR16_N_STOP_TRIG]);
  static float frequency = 4E-6;
  static DESCRIPTOR_FLOAT(frequency_d, &frequency);
  static DESCRIPTOR_RANGE(int_clock_d, 0, 0, &frequency_d);
  static DESCRIPTOR_NID(ext_clock_d, &c_nids[JOERGER_TR16_N_EXT_CLOCK_IN]);
  static float coefficient;
  static DESCRIPTOR_FLOAT(coef_d, &coefficient);
  //static int key;
  //static DESCRIPTOR_LONG(key_d, &key);
  static DESCRIPTOR_FUNCTION_1(value, (unsigned char *)&OpcValue, 0);
  static float frequencies[] =
      { 1. / 160E3, 1. / 80E3, 1. / 40E3, 1. / 20E3, 1. / 10E3, 1. / 5E3, 1. / 2.5E3, 0.0 };
  static DESCRIPTOR_FUNCTION_2(mult_exp, (unsigned char *)&OpcMultiply, &coef_d, &value);
  static DESCRIPTOR(volts_str, "volts");
  static DESCRIPTOR_WITH_UNITS(volts, &mult_exp, &volts_str);
  static DESCRIPTOR_WINDOW(window, &start_d, &end_d, &trigger_d);
  static DESCRIPTOR_DIMENSION(dimension, &window, 0);
  static DESCRIPTOR(time_str, "seconds");
  static DESCRIPTOR_WITH_UNITS(time, &dimension, &time_str);
  static DESCRIPTOR_SIGNAL_1(signal, &volts, &counts, &time);

  short *channel_data;
  int status;
  int put_status = 1;
  int chan;
  int samples_to_read;
  //int i;
  int ret_status = 1;

  static F0A0 f0a0 = { 0 };
  static F0A1 f0a1 = { 0 };
  int min_idx;
  int max_idx;
  int active_mem;
  int post_trig;
  int samples_per_channel;

  return_on_error(DevNids(niddsc_ptr, sizeof(c_nids), c_nids));
  value.ndesc = 0;
  return_on_error(DevCamChk(CamPiow(setup->name, 0, 27, 0, 16, 0), &one, 0));
  if ((CamXandQ(0) & 1) == 0)
    return DEV$_NOT_TRIGGERED;

  pio(0, 0, (short *)&f0a0, one);
  dimension.axis = (f0a0.clock == 7) ? (struct descriptor *)(&ext_clock_d) :
      (struct descriptor *)(&int_clock_d);
  frequency = frequencies[f0a0.clock];
  pio(0, 1, (short *)&f0a1, one);
  active_mem = 8192 * (1 << f0a1.active_memory);
  post_trig = 1 << (f0a1.post_trig + 5);
  samples_per_channel = active_mem / (1 << f0a0.chans);
  min_idx = post_trig - samples_per_channel + 1;
  max_idx = post_trig;
  channel_data = (short *)malloc(samples_per_channel * sizeof(short));

#undef return_on_error
#define return_on_error(f) if (!((status = f) & 1)) {free(channel_data); return status;}

  for (chan = 0; ((chan < 16) && (status & 1) && (put_status & 1)); chan++) {
    if (TreeIsOn(CHAN_NID(chan, JOERGER_TR16_N_CHAN_HEAD)) & 1) {
      status = DevLong(&CHAN_NID(chan, JOERGER_TR16_N_CHAN_STARTIDX), (int *)&raw.bounds[0].l);
      if (status & 1)
	raw.bounds[0].l = min(max_idx, max(min_idx, raw.bounds[0].l));
      else
	raw.bounds[0].l = min_idx;

      status = DevLong(&CHAN_NID(chan, JOERGER_TR16_N_CHAN_ENDIDX), (int *)&raw.bounds[0].u);
      if (status & 1)
	raw.bounds[0].u = min(max_idx, max(raw.bounds[0].l, raw.bounds[0].u));
      else
	raw.bounds[0].u = max_idx;

      raw.m[0] = raw.bounds[0].u - raw.bounds[0].l + 1;
      if (raw.m[0] > 0) {
	int tries;
	status = 0;
	for (tries = 0; (!(status & 1) && (tries < 5)); tries++) {
	  samples_to_read = raw.bounds[0].u - min_idx + 1;
	  status =
	      ReadChannel(setup->name, chan, &samples_to_read, channel_data,
			  TreeIsOn(c_nids[1]) & 1);
	  if (status & 1) {
	    coefficient = .610E-3;
	    raw.pointer = (char *)(channel_data + (raw.bounds[0].l - min_idx));
	    raw.a0 = raw.pointer - (raw.bounds[0].l * sizeof(channel_data[0]));
	    raw.arsize = raw.m[0] * 2;
	    put_status =
		TreePutRecord(CHAN_NID(chan, JOERGER_TR16_N_CHAN_HEAD),
			      (struct descriptor *)&signal, 0);
	    if (tries > 0) {
	      printf("TR16 read channel %d in %d tries\n", chan + 1, tries);
	    }
	    if (!(put_status & 1) && (ret_status & 1))
	      ret_status = put_status;
	  }
	}
	if (!(status & 1) && (ret_status & 1))
	  ret_status = status;
	status = 1;
      }
    }
  }
  free(channel_data);
  return ret_status;
}

#undef return_on_error
#define return_on_error(f) if (!((status = f) & 1)) return status;
#undef pio
#define pio(f,a,d,q)  return_on_error(DevCamChk(CamPiow(name, a, f, d, 16, 0), &one, &q))
static int ReadChannel(char *name, int chan, int *samples_ptr, short *data_ptr, int use_enhanced)
{
  int status;
  //int samples_read = 0;
  int samples_to_read = *samples_ptr;
  //int read_size;
  pio(17, 0, &chan, one);
  pio(17, 0, &chan, one);
  /*
     if (bufsize < (samples_to_read * 2))
     {
     for(samples_to_read = *samples_ptr; samples_to_read; samples_to_read -= read_size)
     {
     read_size = min(32767, samples_to_read);
     return_on_error(DevCamChk(CamStopw(name,0,2,read_size,data_ptr+samples_read,16,0),&one,0));
     samples_read += read_size;
     }
     }
     else
     {
   */
  if (use_enhanced) {
    return_on_error(DevCamChk(CamFStopw(name, 0, 2, samples_to_read, data_ptr, 16, 0), &one, 0));
  } else {
    return_on_error(DevCamChk(CamStopw(name, 0, 2, samples_to_read, data_ptr, 16, 0), &one, 0));
  }
  /*
     }
   */
  return 1;
}
