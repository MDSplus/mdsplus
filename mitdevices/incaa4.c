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

#include "incaa4_gen.h"
#include <libroutines.h>
#include <strroutines.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <stdlib.h>

typedef struct _CSRegister {
  unsigned clock:4;
  unsigned chans:3;
  unsigned c_off:2;
  unsigned:1;
  unsigned sample_clock:1;
  unsigned master:1;
  unsigned c_gen:1;
  unsigned state:2;
  unsigned recirc:1;
  unsigned:8;
} CSRegister;

static int one = 1;
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define pio(f,a, d) {\
 if (!((status = DevCamChk(CamPiow(setup->name,a,f,d,24,0),&one,&one)) & 1)) return status;}
#define fstop(f,a,num,data) {\
 if (!((status = DevCamChk(CamFStopw(setup->name,a,f,num,data,16,0),&one,&one)) & 1)) return status;}
extern int DevCamChk();



extern int DevLong();

//extern unsigned short OpcMultiply;
//extern unsigned short OpcValue;

EXPORT int incaa4___arm(struct descriptor *niddsc __attribute__ ((unused)), InArmStruct * setup)
{
  int status;
  float freq;
  CSRegister csreg;
  if (setup->int_clk_frq) {
    if ((status = TdiGetFloat(setup->int_clk_frq, &freq)) & 1) {
      int i;
      static float freqs[] = { 500, 250, 125, 50, 10, 5, 1, .5 };
      for (i = 0; i < 8; i++)
	if (freq >= freqs[i])
	  break;
      csreg.clock = i + 1;
    } else
      return status;
  } else
    csreg.clock = 0;
  csreg.chans = setup->active_chans_convert;
  csreg.c_off = 0;
  csreg.master = setup->master == 0;
  if (!csreg.master) {
    csreg.clock = 0;
    csreg.c_gen = 0;
    csreg.recirc = 0;
  } else {
    csreg.c_gen = setup->ext_1mhz != 0;
    csreg.recirc = 1;
  }
  pio(28, 0, 0);
  pio(16, 1, &setup->ptsc);
  pio(16, 2, (int *)&csreg);
  pio(11, 0, 0);
  return status;
}

EXPORT int incaa4___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status = incaa4___arm(niddsc, (InArmStruct *) setup);
  pio(25, 0, 0);
  return status;
}

EXPORT int incaa4___trigger(struct descriptor *niddsc __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(25, 2, 0);
  return status;
}

EXPORT int incaa4___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{

#define min(a,b) ((a) <= (b)) ? (a) : (b)
#define max(a,b) ((a) >= (b)) ? (a) : (b)

  //static struct descriptor_d name = { 0, DTYPE_T, CLASS_D, 0 };
  static DESCRIPTOR_A_BOUNDS(raw, sizeof(short), DTYPE_W, 0, 1, 0);
  static DESCRIPTOR(counts_str, "counts");
  static DESCRIPTOR_WITH_UNITS(counts, &raw, &counts_str);
  static DESCRIPTOR_LONG(start_d, &raw.bounds[0].l);
  static DESCRIPTOR_LONG(end_d, &raw.bounds[0].u);
  static int trig_in_nid;
  static DESCRIPTOR_NID(trigger_d, &trig_in_nid);
  static float frequency;
  static DESCRIPTOR_FLOAT(frequency_d, &frequency);
  static DESCRIPTOR_RANGE(int_clock_d, 0, 0, &frequency_d);
  static int clock_in_nid;
  static DESCRIPTOR_NID(ext_clock_d, &clock_in_nid);
  static float coefficient = 20.0 / 4096;
  static DESCRIPTOR_FLOAT(coef_d, &coefficient);
  //static int key;
  //static DESCRIPTOR_LONG(key_d, &key);
  static DESCRIPTOR_FUNCTION_1(dvalue, (unsigned char *)&OpcValue, 0);
  static DESCRIPTOR_FUNCTION_2(mult_exp, (unsigned char *)&OpcMultiply, &coef_d, &dvalue);
  static DESCRIPTOR(volts_str, "volts");
  static DESCRIPTOR_WITH_UNITS(volts, &mult_exp, &volts_str);
  static DESCRIPTOR_WINDOW(window, &start_d, &end_d, &trigger_d);
  static int clock_out_nid;
  static DESCRIPTOR_NID(clock_out_d, &clock_out_nid);
  static DESCRIPTOR_DIMENSION(dimension, &window, &clock_out_d);
  static DESCRIPTOR(time_str, "seconds");
  static DESCRIPTOR_WITH_UNITS(time, &dimension, &time_str);
  static DESCRIPTOR_SIGNAL_1(signal, &volts, &counts, &time);
  short *data_ptr;
  int status;
  int chan;
  int active_chans;
  static int chans_select[] = { 1, 2, 4 };
  int samples_to_read;
  int mar = 64 * 1024 / 4;
  int samps_per_chan;
  //int i;
  //int polarity;
  CSRegister csreg = { 4, 4, 0, 1, 1, 0, 0, 1 };
  int min_idx;
  int max_idx;
  int start_addr = 0;
  //int tries;
  int memsize;
  trig_in_nid = setup->head_nid + INCAA4_N_TRIG_IN;
  clock_in_nid = setup->head_nid + INCAA4_N_CLOCK_IN;
  clock_out_nid = setup->head_nid + INCAA4_N_CLOCK_OUT;
  dvalue.ndesc = 0;

  pio(0, 2, (int *)&csreg);
  pio(0, 0, &mar);
  memsize = 64 * 1024;
  if (csreg.state != 0)
    return DEV$_NOT_TRIGGERED;
  if (csreg.clock == 0) {
    return_on_error(TreePutRecord(clock_out_nid, (struct descriptor *)&ext_clock_d, 0), status);
  } else {
    static float freqs[] = { 0, 2E-6, 4E-6, 8E-6, 2E-5, 1E-4, 2E-4, 1E-3, 5E-3 };
    frequency = freqs[min(csreg.clock, 9)];
    return_on_error(TreePutRecord(clock_out_nid, (struct descriptor *)&int_clock_d, 0), status);
  }
  active_chans = chans_select[min(csreg.chans, 4)];
  if (csreg.recirc)
    samps_per_chan = memsize / active_chans;
  else
    samps_per_chan = mar / active_chans;
  start_addr = (mar - samps_per_chan * active_chans + memsize) % memsize;
  max_idx = setup->ptsc - 1;
  min_idx = setup->ptsc - samps_per_chan;
  if (raw.pointer)
    free(raw.pointer);
  raw.pointer = malloc(samps_per_chan * 2);
  for (chan = 0; ((chan < active_chans) && (status & 1)); chan++) {
    int input_nid =
	setup->head_nid + INCAA4_N_INPUT_1 + chan * (INCAA4_N_INPUT_2 - INCAA4_N_INPUT_1);
    int startidx_nid = input_nid + 1;
    int endidx_nid = input_nid + 2;
    if (TreeIsOn(input_nid) & 1) {
      status = DevLong(&startidx_nid, &raw.bounds[0].l);
      if (status & 1)
	raw.bounds[0].l = min(max_idx, max(min_idx, raw.bounds[0].l));
      else
	raw.bounds[0].l = min_idx;

      status = DevLong(&endidx_nid, &raw.bounds[0].u);
      if (status & 1)
	raw.bounds[0].u = min(max_idx, max(min_idx, raw.bounds[0].u));
      else
	raw.bounds[0].u = max_idx;

      raw.m[0] = raw.bounds[0].u - raw.bounds[0].l + 1;
      if (raw.m[0] > 0) {
	int addr = (start_addr + chan + (raw.bounds[0].l - min_idx) * active_chans) % memsize;
	int samps;
	pio(16, 0, &addr);
	for (samples_to_read = raw.m[0], data_ptr = (short *)raw.pointer;
	     (samples_to_read > 0) && (status & 1);
	     samples_to_read -= samps, data_ptr += samps, addr += (samps * active_chans)) {
	  pio(16, 0, &addr);
	  samps = min(samples_to_read, 32767);
	  fstop(2, 0, samps, data_ptr);
	}
	if (status & 1) {
	  raw.a0 = raw.pointer - raw.bounds[0].l;
	  raw.arsize = raw.m[0] * 2;
	  status = TreePutRecord(input_nid, (struct descriptor *)&signal, 0);
	}
      }
    }
  }
  pio(16, 0, &mar);
  free(raw.pointer);
  raw.pointer = 0;
  return status;
}
