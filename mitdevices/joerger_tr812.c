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
#include <mdsshr.h>
#include <strroutines.h>
#include <libroutines.h>
#include <string.h>
#include "joerger_tr812_gen.h"
#include "devroutines.h"

//extern unsigned short OpcAdd;
//extern unsigned short OpcMultiply;
//extern unsigned short OpcValue;



struct _f0a0 {
  unsigned pretrigger:3;
  unsigned act_memory:3;
  unsigned burst_mode:1;
  unsigned external:1;
  unsigned adc:1;
  unsigned fill:7;
};
static int one = 1;
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define pio_noq(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16, 0), &one, 0),status)
#define pio(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16, 0), &one, &one),status)
#define stop(f,a,n,d) return_on_error(DevCamChk(CamQstopw(setup->name,a,f,n,d,16,0),&one,&one),status);
#define fstop(f,a,n,d) return_on_error(DevCamChk(CamFQstopw(setup->name,a,f,n,d,16,0),&one,&one),status);
#define min(a,b) ((a) <= (b)) ? (a) : (b)
#define max(a,b) ((a) >= (b)) ? (a) : (b)

static int ReadChannel(int partial, struct _f0a0 *f0a0, InStoreStruct * setup, int *chan_ptr,
		       int *samples_ptr, short *data_ptr);

int joerger_tr812___trigger(struct descriptor *niddsc_ptr __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(25, 0, 0);
  return status;
}

int joerger_tr812___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  //static struct descriptor_xd xd = { 0, DTYPE_T, CLASS_XD, 0, 0 };
  static struct _f0a0 f16a0 = { 0, 0, 0, 0, 0, 0 };
  unsigned short gains;
  //int i;
  int burst_mode_nid = setup->head_nid + JOERGER_TR812_N_BURST_MODE;
  f16a0.pretrigger = setup->pre_trigger;
  f16a0.act_memory = setup->act_memory - 1;
  f16a0.burst_mode = ((status = TreeIsOn(burst_mode_nid) == TreeON) || (status == TreePARENT_OFF));
  f16a0.external = setup->ext_clock && setup->ext_clock->pointer;
  pio_noq(25, 0, 0);
  pio(25, 1, 0);
  pio(16, 0, (short *)&f16a0);
  gains = (setup->channel_1_gain_convert << 0) |
      (setup->channel_2_gain_convert << 2) |
      (setup->channel_3_gain_convert << 4) |
      (setup->channel_4_gain_convert << 6) |
      (setup->channel_5_gain_convert << 8) |
      (setup->channel_6_gain_convert << 10) |
      (setup->channel_7_gain_convert << 12) | (setup->channel_8_gain_convert << 14);
  pio(17, 1, &gains);
  pio_noq(9, 0, 0);
  pio_noq(26, 0, 0);
  return status;
}

static int Store(InStoreStruct * setup, int partial);

int joerger_tr812___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{
  return Store(setup, 0);
}

int joerger_tr812___partial_store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InPartial_storeStruct * setup)
{
  return Store((InStoreStruct *) setup, 1);
}

static int Store(InStoreStruct * setup, int partial)
{
  static DESCRIPTOR_A_BOUNDS(raw, sizeof(short), DTYPE_W, 0, 1, 0);
  static DESCRIPTOR(counts_str, "counts");
  static DESCRIPTOR_WITH_UNITS(counts, &raw, &counts_str);
  static DESCRIPTOR_LONG(start_d, &raw.bounds[0].l);
  static DESCRIPTOR_LONG(end_d, &raw.bounds[0].u);
  static int trigger_nid;
  static DESCRIPTOR_NID(trigger_d, &trigger_nid);
  static float frequency = 4E-6;
  static DESCRIPTOR_FLOAT(frequency_d, &frequency);
  static DESCRIPTOR_RANGE(int_clock_d, 0, 0, &frequency_d);
  static int ext_clock_nid;
  static DESCRIPTOR_NID(ext_clock_d, &ext_clock_nid);
  static float coefficient;
  static DESCRIPTOR_FLOAT(coef_d, &coefficient);
  static short offset = -2048;
  static struct descriptor_s offset_d = { 2, DTYPE_W, CLASS_S, (char *)&offset };
  static DESCRIPTOR_FUNCTION_1(value, (unsigned char *)&OpcValue, 0);
  static DESCRIPTOR_FUNCTION_2(add_exp, (unsigned char *)&OpcAdd, &offset_d, &value);
  static DESCRIPTOR_FUNCTION_2(mult_exp, (unsigned char *)&OpcMultiply, &coef_d, &add_exp);
  static DESCRIPTOR(volts_str, "volts");
  static DESCRIPTOR_WITH_UNITS(volts, &mult_exp, &volts_str);
  static DESCRIPTOR_WINDOW(window, &start_d, &end_d, &trigger_d);
  static DESCRIPTOR_DIMENSION(dimension, &window, 0);
  static DESCRIPTOR(time_str, "seconds");
  static DESCRIPTOR_WITH_UNITS(time, &dimension, &time_str);
  static DESCRIPTOR_SIGNAL_1(signal, &volts, &counts, &time);

  short channel_data[32768];
  int status;
  int chan;
  int samples_to_read;
  //int i;
  static struct _f0a0 f0a0 = { 0, 0, 0, 0, 0, 0 };
  short f1a1;
  int min_idx;
  int max_idx;
  ext_clock_nid = setup->head_nid + JOERGER_TR812_N_EXT_CLOCK;
  trigger_nid = setup->head_nid + JOERGER_TR812_N_TRIGGER;
  value.ndesc = 0;
  if (!partial) {
    pio(8, 0, 0);
    if ((CamXandQ(0) & 1) == 0)
      return DEV$_NOT_TRIGGERED;
  }
  pio(0, 0, (short *)&f0a0);
  dimension.axis =
      f0a0.external ? (struct descriptor *)(&ext_clock_d) : (struct descriptor *)(&int_clock_d);
  pio(1, 1, (short *)&f1a1);
  min_idx = ((long)f0a0.pretrigger * (long)-4096) / ((long)f0a0.act_memory + 1);
  max_idx = (long)4096 *((long)f0a0.act_memory + 1) + min_idx - 1;
  for (chan = 0; ((chan < 8) && (status & 1)); chan++) {
    int data_nid =
	setup->head_nid + JOERGER_TR812_N_CHANNEL_1 + (JOERGER_TR812_N_CHANNEL_2 -
						       JOERGER_TR812_N_CHANNEL_1) * chan;
    int start_nid = data_nid + JOERGER_TR812_N_CHANNEL_1_STARTIDX - JOERGER_TR812_N_CHANNEL_1;
    int end_nid = data_nid + JOERGER_TR812_N_CHANNEL_1_ENDIDX - JOERGER_TR812_N_CHANNEL_1;
    if (TreeIsOn(data_nid) & 1) {
      status = DevLong(&start_nid, (int *)&raw.bounds[0].l);
      if (status & 1)
	raw.bounds[0].l = min(max_idx, max(min_idx, raw.bounds[0].l));
      else
	raw.bounds[0].l = min_idx;

      status = DevLong(&end_nid, (int *)&raw.bounds[0].u);
      if (status & 1)
	raw.bounds[0].u = min(max_idx, max(raw.bounds[0].l, raw.bounds[0].u));
      else
	raw.bounds[0].u = max_idx;

      raw.m[0] = raw.bounds[0].u - raw.bounds[0].l + 1;
      if (raw.m[0] > 0) {
	samples_to_read = raw.bounds[0].u - min_idx + 1;
	status = ReadChannel(partial, &f0a0, setup, &chan, &samples_to_read, channel_data);
	if (status & 1) {
	  int gain = 1 << ((f1a1 >> (chan * 2)) & 3);
	  coefficient = 20.0 / 4096 / gain;
	  raw.pointer = (char *)(channel_data + (raw.bounds[0].l - min_idx));
	  raw.a0 = raw.pointer - (raw.bounds[0].l * sizeof(channel_data[0]));
	  raw.arsize = raw.m[0] * 2;
	  status = TreePutRecord(data_nid, (struct descriptor *)&signal, 0);
	}
      }
    }
  }
  return status;
}

static int ReadChannel(int partial, struct _f0a0 *f0a0, InStoreStruct * setup, int *chan_ptr,
		       int *samples_ptr, short *data_ptr)
{
  int status;
  short zero = 0;
  if (partial) {
    pio(25, 1, 0);
    pio(17, 0, &zero);
    f0a0->adc = 1;
    pio(16, 0, (short *)f0a0);
    f0a0->adc = 0;
    pio(16, 0, (short *)f0a0);
  } else {
    pio(17, 0, &zero);
  }
  if (*samples_ptr < 32768) {
    fstop(2, *chan_ptr, *samples_ptr, data_ptr);
  } else {
    stop(2, *chan_ptr, 16384, data_ptr);
    stop(2, *chan_ptr, 16384, data_ptr + 16384);
  }
  return 1;
}
