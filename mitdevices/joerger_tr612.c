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
#include <strroutines.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <libroutines.h>
#include "joerger_tr612_gen.h"
#include <stdio.h>
#include "devroutines.h"

//extern unsigned short OpcAdd;
//extern unsigned short OpcMultiply;
//extern unsigned short OpcValue;




static int one = 1;
#define pio(f,a,data,q) {\
 if (!((status = DevCamChk(CamPiow(setup->name,a,f,data,16,0),&one,q)) & 1)) return status;}
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define CHAN_NID(chan, field) setup->head_nid + JOERGER_TR612_N_INPUTS+chan*JOERGER_TR612_K_NODES_PER_INP+field

#define min(a,b) ((a) <= (b)) ? (a) : (b)
#define max(a,b) ((a) >= (b)) ? (a) : (b)

struct _pre_mem {
  unsigned pretrig:3;
  unsigned actmem:3;
  unsigned not_used:10;
};

struct _freq_mode {
  unsigned freq:3;
  unsigned mode:1;
  unsigned not_used:2;
  unsigned armed:1;
  unsigned active:1;
  unsigned fill:8;
};

struct _range {
  unsigned range_1:3;
  unsigned range_2:3;
  unsigned range_3:3;
  unsigned range_4:3;
  unsigned unused:4;
};

int joerger_tr612___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  struct _pre_mem reg1;
  struct _freq_mode reg2;
  pio(25, 2, 0, 0);		/* Abort any cycle */
  reg1.pretrig = setup->pretrig;
  reg1.actmem = setup->actmem;
  pio(16, 0, (short *)&reg1, &one);	/* Write Register 1 */
  reg2.freq = setup->frequency;
  reg2.mode = setup->mode;
  pio(16, 1, (short *)&reg2, &one);	/* Write Register 2 */
  pio(9, 0, 0, 0)
      pio(26, 0, 0, 0)
      return status;
}

int joerger_tr612___stop(struct descriptor *niddsc __attribute__ ((unused)), InStopStruct * setup)
{
  int status;
  pio(25, 1, 0, 0)
      return status;
}

int joerger_tr612___trigger(struct descriptor *niddsc __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(25, 0, 0, 0)
      return status;
}

static int ReadChannel(InStoreStruct * setup, int max_samps, int chan, short *data_ptr);

#define JOERGER_TR612_N_INPUTS         9
#define JOERGER_TR612_K_NODES_PER_INP  3
#define JOERGER_TR612_N_INP_HEAD       0
#define JOERGER_TR612_N_INP_STARTIDX   1
#define JOERGER_TR612_N_INP_ENDIDX     2

int joerger_tr612___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  static int max_samples;
  static DESCRIPTOR_LONG(max_samples_d, &max_samples);
  static int one = 1;
  static DESCRIPTOR_LONG(one_d, &one);
  static DESCRIPTOR_A_BOUNDS(raw, sizeof(short), DTYPE_W, 0, 1, 0);
  static DESCRIPTOR(counts_str, "counts");
  static DESCRIPTOR_WITH_UNITS(counts, &raw, &counts_str);
  static DESCRIPTOR_LONG(lower_d, &raw.bounds[0].l);
  static DESCRIPTOR_LONG(upper_d, &raw.bounds[0].u);
  static DESCRIPTOR_FUNCTION_2(start_d, (unsigned char *)&OpcAdd, &lower_d, &one_d);
  static DESCRIPTOR_FUNCTION_2(end_d, (unsigned char *)&OpcAdd, &upper_d, &one_d);
  static int trigger_nid;
  static DESCRIPTOR_NID(trigger_d, &trigger_nid);
  static float frequency;
  static DESCRIPTOR_FLOAT(frequency_d, &frequency);
  static DESCRIPTOR_RANGE(int_clock_d, 0, 0, &frequency_d);
  static int ext_clock_nid;
  static DESCRIPTOR_NID(ext_clock_d, &ext_clock_nid);
  static float coefficient;
  static DESCRIPTOR_FLOAT(coef_d, &coefficient);
  static short offset;
  static struct descriptor_s offset_d = { 2, DTYPE_W, CLASS_S, (char *)&offset };
  static DESCRIPTOR_FUNCTION_0(dvalue, (unsigned char *)&OpcValue);
  static DESCRIPTOR_FUNCTION_2(add_exp, (unsigned char *)&OpcAdd, &offset_d, &dvalue);
  static DESCRIPTOR_FUNCTION_2(mult_exp, (unsigned char *)&OpcMultiply, &coef_d, &add_exp);
  static DESCRIPTOR(volts_str, "volts");
  static DESCRIPTOR_WITH_UNITS(volts, &mult_exp, &volts_str);
  static DESCRIPTOR_WINDOW(window, &start_d, &end_d, &trigger_d);
  static DESCRIPTOR_DIMENSION(dimension, &window, 0);
  static DESCRIPTOR(time_str, "seconds");
  static DESCRIPTOR_WITH_UNITS(time, &dimension, &time_str);
  static DESCRIPTOR_SIGNAL_1(signal, &volts, &counts, &time);
  static EMPTYXD(burst_dimension);
  int status;
  int chan;
  int samples_to_read;
  //int i;
  struct _range range1;
  struct _range range2;
  struct _pre_mem reg1;
  struct _freq_mode reg2;
  static short *channel_data = 0;
  int burst_size;
  int max_idx;
  int min_idx;
  int bursts;
  int max_bursts[] = { 8, 4, 3, 2, 2, 2, 2, 1 };
  float c_coef[6];
  short c_offset[6];
  static float coefs[] =
      { 5. / 4095., 5. / 4095., 10. / 4095., 10. / 4095., 20. / 4095., 20. / 4095., 20. / 4095,
20. / 4095 };
  static float freq[] = { 1. / 3e6, 500e-9, 1E-6, 1E-5, 0, 0, 0, 0 };
  trigger_nid = setup->head_nid + JOERGER_TR612_N_TRIGGER;
  ext_clock_nid = setup->head_nid + JOERGER_TR612_N_EXT_CLOCK;
  pio(0, 1, (short *)&reg2, &one);
  pio(8, 0, 0, 0);
  if (reg2.armed || reg2.active) {
    if (reg2.mode == 0)
      return DEV$_NOT_TRIGGERED;
    pio(25, 2, 0, &one);
  }
  pio(0, 0, (short *)&reg1, &one);
  pio(0, 2, (short *)&range1, &one);
  pio(0, 3, (short *)&range2, &one);
  frequency = freq[reg2.freq];
  dimension.axis =
      (frequency == 0) ? (struct descriptor *)&ext_clock_d : (struct descriptor *)&int_clock_d;
  max_samples = 128 * ((range2.range_4 & 4) ? 512 : 128) * (reg1.actmem + 1);
  if (reg2.mode) {
    int i;
    char burst_dim_s[2048] = "build_dim(*,set_range(0 : $1,[";
    struct descriptor burst_dim = { 0, DTYPE_T, CLASS_S, 0 };
    bursts = max_bursts[reg1.actmem];
    burst_size = max_samples;
    max_samples = 1024 * ((range2.range_4 & 4) ? 512 : 128);
    reg1.pretrig = 0;
    for (i = 0; i < bursts; i++) {
      char dim_part_s[500];
      sprintf(dim_part_s, "BUILD_DIM(BUILD_WINDOW(0,%d,$2[%d]),$3)", burst_size - 1, i);
      strcat(burst_dim_s, dim_part_s);
      strcat(burst_dim_s, (i == (bursts - 1)) ? "]))" : ",");
    }
    max_samples--;
    burst_dim.length = strlen(burst_dim_s);
    burst_dim.pointer = burst_dim_s;
    status =
	TdiCompile(&burst_dim, &max_samples_d, &trigger_d, dimension.axis,
		   &burst_dimension MDS_END_ARG);
    max_samples++;
    time.data = burst_dimension.pointer;
  } else
    time.data = (struct descriptor *)&dimension;
  if (channel_data)
    free(channel_data);
  channel_data = malloc(max_samples * sizeof(short));
  min_idx = -1 * reg1.pretrig * max_samples / 8 + 1;
  max_idx = max_samples + min_idx - 1;
  c_coef[0] = coefs[range1.range_1];
  c_offset[0] = range1.range_1 & 1 ? -2048 : 0;
  c_coef[1] = coefs[range1.range_2];
  c_offset[1] = range1.range_2 & 1 ? -2048 : 0;
  c_coef[2] = coefs[range1.range_3];
  c_offset[2] = range1.range_3 & 1 ? -2048 : 0;
  c_coef[3] = coefs[range1.range_4];
  c_offset[3] = range1.range_4 & 1 ? -2048 : 0;
  c_coef[4] = coefs[range2.range_1];
  c_offset[4] = range2.range_1 & 1 ? -2048 : 0;
  c_coef[5] = coefs[range2.range_2];
  c_offset[5] = range2.range_2 & 1 ? -2048 : 0;
  for (chan = 0; ((chan < 6) && (status & 1)); chan++) {
    int nid = CHAN_NID(chan, JOERGER_TR612_N_INP_HEAD);
    if (TreeIsOn(nid) & 1) {
      nid = CHAN_NID(chan, JOERGER_TR612_N_INP_STARTIDX);
      status = DevLong(&nid, (int *)&raw.bounds[0].l);
      if (status & 1)
	raw.bounds[0].l = min(max_idx, max(min_idx, raw.bounds[0].l));
      else
	raw.bounds[0].l = min_idx;

      nid = CHAN_NID(chan, JOERGER_TR612_N_INP_ENDIDX);
      status = DevLong(&nid, (int *)&raw.bounds[0].u);
      if (status & 1)
	raw.bounds[0].u = min(max_idx, max(min_idx, raw.bounds[0].u));
      else
	raw.bounds[0].u = max_idx;

      raw.m[0] = raw.bounds[0].u - raw.bounds[0].l + 1;
      if (raw.m[0] > 0) {
	samples_to_read = raw.bounds[0].u - min_idx + 1;
	status = ReadChannel(setup, samples_to_read, chan, channel_data);
	if (status & 1) {
	  offset = c_offset[chan];
	  coefficient = c_coef[chan];
	  raw.pointer = (char *)(channel_data + raw.bounds[0].l - min_idx);
	  raw.a0 = (char *)(channel_data - raw.bounds[0].l);
	  raw.arsize = raw.m[0] * sizeof(short);
	  nid = CHAN_NID(chan, JOERGER_TR612_N_INP_HEAD);
	  status = TreePutRecord(nid, (struct descriptor *)&signal, 0);
	}
      }
    }
  }
  if (channel_data) {
    free(channel_data);
    channel_data = 0;
  }
  return status;
}

static int ReadChannel(InStoreStruct * setup, int max_samps, int chan, short *data_ptr)
{
  int status;
  int samples_to_read = max_samps;
  int this_chunk;
  static int zero = 0;
  int bufsize = 32767 * 2;
  pio(17, 0, &zero, &one);
  if (bufsize < (max_samps * 2)) {
    for (this_chunk = min(32767, samples_to_read); samples_to_read > 0;
	 samples_to_read -= this_chunk, data_ptr += this_chunk, this_chunk =
	 min(32767, samples_to_read))
      return_on_error(DevCamChk
		      (CamStopw(setup->name, chan, 2, this_chunk, data_ptr, 16, 0), &one, 0),
		      status);
  } else {
    return_on_error(DevCamChk(CamFStopw(setup->name, chan, 2, max_samps, data_ptr, 16, 0), &one, 0),
		    status);
  }
  return status;
}
