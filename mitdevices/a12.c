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
#include <strroutines.h>
#include <treeshr.h>
#include <mdsshr.h>
#include "a12_gen.h"
#include "devroutines.h"

#define pio(fv,av) {int a = av; int f = fv; int zero=0; int one=1; \
 if (!((status = DevCamChk(CamPiow(setup->name,a,f,&zero,16,0),&one,&one)) & 1)) return status;}

EXPORT int a12___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  pio(9, 0)
      pio(9, 0)
      pio(17, setup->ext_clock_in != 0)
      pio(26, 0)
      return status;
}

EXPORT int a12___stop(struct descriptor *niddsc __attribute__ ((unused)), InStopStruct * setup)
{
  int status;
  pio(25, 1)
      return status;
}

EXPORT int a12___trigger(struct descriptor *niddsc __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(25, 0)
      return status;
}

static int ReadSetup(char *name, float *freq_ptr, int *polarity);
static int ReadChannel(char *name, int fast, int *max_samps_ptr, int chan, short *data_ptr);

static int sinewave __attribute__ ((unused));

#define A12_N_INPUTS         5
#define A12_K_NODES_PER_INP  3
#define A12_N_INP_HEAD       0
#define A12_N_INP_STARTIDX   1
#define A12_N_INP_ENDIDX     2

//extern unsigned short OpcAdd, OpcMultiply, OpcValue;

EXPORT int a12__store(struct descriptor *niddsc_ptr __attribute__ ((unused)))
{

#define return_on_error(f,retstatus) if (!((status = f) & 1)) {status = retstatus; goto error;}
#define CHAN_NID(chan, field) c_nids[A12_N_INPUTS+chan*A12_K_NODES_PER_INP+field]

#define min(a,b) ((a) <= (b)) ? (a) : (b)
#define max(a,b) ((a) >= (b)) ? (a) : (b)

  static int c_nids[A12_K_CONG_NODES];

  static struct descriptor_d name_d = { 0, DTYPE_T, CLASS_D, 0 };
  static DESCRIPTOR_A_BOUNDS(raw, sizeof(short), DTYPE_W, 0, 1, 0);
  static DESCRIPTOR(counts_str, "counts");
  static DESCRIPTOR_WITH_UNITS(counts, &raw, &counts_str);
  static DESCRIPTOR_LONG(start_d, &raw.bounds[0].l);
  static DESCRIPTOR_LONG(end_d, &raw.bounds[0].u);
  static DESCRIPTOR_NID(trigger_d, &c_nids[A12_N_STOP_TRIG]);
  static float frequency;
  static DESCRIPTOR_FLOAT(frequency_d, &frequency);
  static DESCRIPTOR_RANGE(int_clock_d, 0, 0, &frequency_d);
  static DESCRIPTOR_NID(ext_clock_d, &c_nids[A12_N_EXT_CLOCK_IN]);
  static float coefficient = 10.0 / 4096;
  static DESCRIPTOR_FLOAT(coef_d, &coefficient);
  static short offset = 0;
  static struct descriptor_s offset_d = { 2, DTYPE_W, CLASS_S, (char *)&offset };
  static DESCRIPTOR_FUNCTION_1(dvalue, (unsigned char *)&OpcValue, 0);
  static DESCRIPTOR_FUNCTION_2(add_exp, (unsigned char *)&OpcAdd, &offset_d, &dvalue);
  static DESCRIPTOR_FUNCTION_2(mult_exp, (unsigned char *)&OpcMultiply, &coef_d, &add_exp);
  static DESCRIPTOR(volts_str, "volts");
  static DESCRIPTOR_WITH_UNITS(volts, &mult_exp, &volts_str);
  static DESCRIPTOR_WINDOW(window, &start_d, &end_d, &trigger_d);
  static DESCRIPTOR_DIMENSION(dimension, &window, 0);
  static DESCRIPTOR(time_str, "seconds");
  static DESCRIPTOR_WITH_UNITS(time, &dimension, &time_str);
  static DESCRIPTOR_SIGNAL_1(signal, &volts, &counts, &time);

  short channel_data[32767];
  int status;
  int chan;
  int samples_to_read __attribute__ ((unused));
  int polarity;
  int fast;
  short int module_id;
  int max_samples;
  char *name = 0;
  int one = 1;

  return_on_error(DevNids(niddsc_ptr, sizeof(c_nids), c_nids), status);
  return_on_error(DevText(&c_nids[A12_N_NAME], &name_d), DEV$_BAD_NAME);
  name = MdsDescrToCstring((struct descriptor *)&name_d);
  dvalue.ndesc = 0;

  StrFree1Dx(&name_d);
  return_on_error(DevCamChk(CamPiow(name, 0, 8, 0, 16, 0), &one, 0), status);
  if ((CamXandQ(0) & 1) == 0)
    return DEV$_NOT_TRIGGERED;
  return_on_error(ReadSetup(name, &frequency, &polarity), status);
  dimension.axis =
      (frequency == 0) ? (struct descriptor *)&ext_clock_d : (struct descriptor *)&int_clock_d;
  return_on_error(DevCamChk(CamPiow(name, 0, 6, &module_id, 16, 0), &one, &one), status);
  max_samples = (module_id & 128) ? 32767 : 8192;
  fast = TreeIsOn(c_nids[A12_N_COMMENT]) & 1;
  if ((max_samples == 8192) && ((TreeIsOn(c_nids[A12_N_NAME]) & 1) == 0))
    max_samples = 32767;
  for (chan = 0; ((chan < 6) && (status & 1)); chan++) {
    if (TreeIsOn(CHAN_NID(chan, A12_N_INP_HEAD)) & 1) {
      status = DevLong(&CHAN_NID(chan, A12_N_INP_STARTIDX), (int *)&raw.bounds[0].l);
      if (status & 1)
	raw.bounds[0].l = min(max_samples - 1, max(0, raw.bounds[0].l));
      else
	raw.bounds[0].l = 0;

      status = DevLong(&CHAN_NID(chan, A12_N_INP_ENDIDX), (int *)&raw.bounds[0].u);
      if (status & 1)
	raw.bounds[0].u = min(max_samples - 1, max(0, raw.bounds[0].u));
      else
	raw.bounds[0].u = max_samples - 1;

      raw.m[0] = raw.bounds[0].u - raw.bounds[0].l + 1;
      if (raw.m[0] > 0) {
	samples_to_read = raw.bounds[0].u + 1;
	status = ReadChannel(name, fast, &max_samples, chan, channel_data);
	if (status & 1) {
	  offset = ((1 << chan) & polarity) != 0 ? -2048 : 0;
	  raw.pointer = (char *)(channel_data + (raw.bounds[0].l));
	  raw.a0 = (char *)channel_data;
	  raw.arsize = raw.m[0] * 2;
	  status = TreePutRecord(CHAN_NID(chan, A12_N_INP_HEAD), (struct descriptor *)&signal, 0);
	}
      }
    }
  }
 error:
  MdsFree(name);
  return status;
}

static int ReadSetup(char *name, float *freq_ptr, int *offset)
{
  struct a12_setup {
    unsigned a12_setup_v_offset:6;	/* Bit on indicates bipolar, 0:10V, else unipolar +5:-5V */
    unsigned a12_setup_v_dummy:1;
    unsigned a12_setup_v_period:1;
    unsigned:8;
  } setup;

  static float freq[2] = { 0, 1.E-5 };
  int status;
  int one = 1;
  status = DevCamChk(CamPiow(name, 0, 0, &setup, 16, 0), &one, 0);
  if (status & 1) {
    *offset = setup.a12_setup_v_offset;
    *freq_ptr = freq[setup.a12_setup_v_period];
  }
  return status;
}

static int ReadChannel(char *name, int fast, int *max_samps_ptr, int chan, short *data_ptr)
{
  int serial;
  int chan_sel;
  int status;
  int one = 1;
  int zero = 0;
  return_on_error(DevCamChk(CamPiow(name, 0, 6, &serial, 16, 0), &one, &one), status);
  if (((serial >> 8) & 255) <= 4)	/* If serial number is less equal to 4 then */
    chan_sel = 7 - chan;	/* Channel select backwards from 7 */
  else {			/* Else */
    return_on_error(DevCamChk(CamPiow(name, 1, 24, 0, 16, 0), &one, &one), status);
    chan_sel = chan;		/* Channel select starts at 0 */
  }
  return_on_error(DevCamChk(CamPiow(name, chan_sel, 16, &zero, 16, 0), &one, 0), status);
  if (fast) {
    return_on_error(DevCamChk(CamFStopw(name, 0, 2, *max_samps_ptr, data_ptr, 16, 0), &one, 0),
		    status);
  } else {
    return_on_error(DevCamChk(CamStopw(name, 0, 2, *max_samps_ptr, data_ptr, 16, 0), &one, 0),
		    status);
  }
  return 1;
 error:
  return status;
}
