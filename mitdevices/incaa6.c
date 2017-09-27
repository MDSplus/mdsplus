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
#include <stdlib.h>
#include "incaa6_gen.h"
#include "devroutines.h"

//extern unsigned short OpcMultiply;
//extern unsigned short OpcValue;



typedef struct _CSRegister {
  unsigned clock:4;
  unsigned chans:3;
  unsigned c_off:2;
  unsigned all_chan:1;
  unsigned sample_clock:1;
  unsigned master:1;
  unsigned c_gen:1;
  unsigned state:2;
  unsigned recirc:1;
  unsigned:8;
} CSRegister;
static int one = 1;
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define pio(f,a,d,mem)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, mem, 0), &one, 0),status)
#define stop(f,a,count,d,mem) return_on_error(DevCamChk(CamStopw(setup->name, a, f, count, d, mem, 0), &one, &one),status)
#define fstop(f,a,count,d,mem) return_on_error(DevCamChk(CamFStopw(setup->name, a, f, count, d, mem, 0), &one, &one),status)
#define min(a,b) ((a) <= (b)) ? (a) : (b)
#define max(a,b) ((a) >= (b)) ? (a) : (b)
#define MAX_CHANS 6

static int arm_init(InInitStruct * setup, int start);

EXPORT int incaa6___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  return arm_init(setup, 1);
}

EXPORT int incaa6___arm(struct descriptor *niddsc_ptr __attribute__ ((unused)), InArmStruct * setup)
{
  return arm_init((InInitStruct *) setup, 0);
}

static int arm_init(InInitStruct * setup, int start)
{
  CSRegister csreg;
  float freq;
  int status;
  int i;
  //int chans;
  //int ival;
  //int ptsc;
  /*********************************************
    Read in the name and mode records.
    If any problem is encountered
    then return the error status.
  **********************************************/

  status = TdiGetFloat(setup->int_clk_frq, &freq);
  csreg.all_chan = 0;
  if (status & 1) {
    static float freqs[] = { 1000, 500, 250, 125, 50, 10, 5 };
    for (i = 0; i < 7; i++)
      if (freq >= freqs[i])
	break;
    csreg.clock = i + 1;
  } else if (status == TreeNODATA)
    csreg.clock = 0;
  else
    return status;
  csreg.c_off = setup->active_chans >> 1;
  csreg.chans = setup->active_chans & 1;
  csreg.master = setup->master;
  csreg.c_gen = setup->ext_1mhz;
  csreg.recirc = 1;
  pio(28, 0, 0, 16);		/* Initialize */
  pio(16, 1, &setup->ptsc, 24);	/* Load PTSC */
  pio(16, 2, (int *)&csreg, 24);	/* Load setup */
  pio(11, 0, 0, 16);		/* Arm */
  if (start)
    pio(25, 0, 0, 16);		/* Start */
  return status;
}

EXPORT int incaa6___trigger(struct descriptor *niddsc_ptr __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(25, 2, 0, 16);		/* Trigger */
  return status;
}

//static int ReadChannel(InStoreStruct * setup, int *chan_ptr, int *samples_ptr, short *data_ptr);
// flag values to put in comment field to specify alternate gains 
#define FIVE_V_KEY "* 5 *"
#define TEN_V_KEY "* 10 *"

EXPORT int incaa6___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{
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
  static float coefficients[] = {20.0 / 4096, 10.0/4096, 5.0/4096};
  static DESCRIPTOR_FLOAT(coef_d, &coefficients[0]);
  //static int key;
  //static DESCRIPTOR_LONG(key_d, &key);
  static DESCRIPTOR_FUNCTION_1(value, (unsigned char *)&OpcValue, 0);
  static DESCRIPTOR_FUNCTION_2(mult_exp, (unsigned char *)&OpcMultiply, &coef_d, &value);
  static DESCRIPTOR(volts_str, "volts");
  static DESCRIPTOR_WITH_UNITS(volts, &mult_exp, &volts_str);
  static DESCRIPTOR_WINDOW(window, &start_d, &end_d, &trigger_d);
  static int clock_out_nid;
  static DESCRIPTOR_NID(clock_out_d, &clock_out_nid);
  static DESCRIPTOR_DIMENSION(dimension, &window, &clock_out_d);
  static DESCRIPTOR(time_str, "seconds");
  static DESCRIPTOR_WITH_UNITS(time, &dimension, &time_str);
  static DESCRIPTOR_SIGNAL_1(signal, &volts, &counts, &time);
  static struct descriptor_d comment_d = { 0, DTYPE_T, CLASS_D, 0 };
  short *data_ptr;
  int status;
  int start;
  int inc;
  int chan;
  int active_chans;
  int samples_to_read;
  int mar = 3 * 1024 * 1024 / 6;
  int samps_per_chan;
  //int i;
  //int polarity;
  CSRegister csreg = { 4, 4, 0, 0, 1, 1, 0, 0, 1 };
  int min_idx;
  int max_idx;
  int start_addr = 0;
  //int tries;
  int memsize;
  int fast;
  int actual_ptsc;
  int comment_nid = setup->head_nid + INCAA6_N_COMMENT;
  trig_in_nid = setup->head_nid + INCAA6_N_TRIG_IN;
  clock_in_nid = setup->head_nid + INCAA6_N_CLOCK_IN;
  clock_out_nid = setup->head_nid + INCAA6_N_CLOCK_OUT;
  value.ndesc = 0;
  // set the channel gain to alternate values if the comment field starts
  // with the flag value
  status = DevText(&comment_nid, &comment_d);
  if (status&1) {
    if( strncmp(comment_d.pointer, TEN_V_KEY, strlen(TEN_V_KEY)) == 0) {
      coef_d.pointer = (void *)&coefficients[1];
    } 
    else if( strncmp(comment_d.pointer, FIVE_V_KEY, strlen(FIVE_V_KEY)) == 0) {
      coef_d.pointer = (void *)&coefficients[2];
    }
    StrFree1Dx(&comment_d);
  }
  fast = TreeIsOn(comment_nid) & 1;
  pio(0, 2, (int *)&csreg, 24);
  pio(25, 1, 0, 16);		/* stop the module */
  pio(0, 1, &actual_ptsc, 24);
  pio(0, 0, &mar, 24);
  memsize = 3 * 1024 * 1024;
  if ((setup->ptsc && (setup->ptsc == actual_ptsc)) || (!setup->ptsc && (csreg.state != 0)))
    return DEV$_NOT_TRIGGERED;
  if (csreg.clock == 0) {
    return_on_error(TreePutRecord(clock_out_nid, (struct descriptor *)&ext_clock_d, 0), status);
  } else {
    static float freqs[] = { 0, 1E-6, 2E-6, 4E-6, 8E-6, 2E-5, 1E-4, 2E-4 };
    frequency = freqs[min(csreg.clock, 7)];
    return_on_error(TreePutRecord(clock_out_nid, (struct descriptor *)&int_clock_d, 0), status);
  }
  active_chans = csreg.chans ? 6 : 3;
  if (csreg.recirc) {
    samps_per_chan = memsize / active_chans;
    start_addr = mar;
  } else {
    samps_per_chan = (csreg.chans) ? (mar / 2) : mar;
    start_addr = 0;
  }
  if (csreg.chans) {
    start = 0;
    inc = 1;
  } else {
    inc = 2;
    start = (csreg.c_off) ? 1 : 0;
  }
  max_idx = setup->ptsc - 1 - actual_ptsc;
  min_idx = setup->ptsc - samps_per_chan - actual_ptsc;
  if (raw.pointer)
    free(raw.pointer);
  raw.pointer = malloc(samps_per_chan * 2);
  for (chan = start; ((chan < MAX_CHANS) && (status & 1)); chan += inc) {
    int data_nid =
	setup->head_nid + INCAA6_N_INPUT_1 + (INCAA6_N_INPUT_2 - INCAA6_N_INPUT_1) * chan;
    int start_nid = data_nid + INCAA6_N_INPUT_1_STARTIDX - INCAA6_N_INPUT_1;
    int end_nid = data_nid + INCAA6_N_INPUT_1_ENDIDX - INCAA6_N_INPUT_1;
    if (TreeIsOn(data_nid) & 1) {
      status = DevLong(&start_nid, (int *)&raw.bounds[0].l);
      if (status & 1)
	raw.bounds[0].l = min(max_idx, max(min_idx, raw.bounds[0].l));
      else
	raw.bounds[0].l = min_idx;

      status = DevLong(&end_nid, (int *)&raw.bounds[0].u);
      if (status & 1)
	raw.bounds[0].u = min(max_idx, max(min_idx, raw.bounds[0].u));
      else
	raw.bounds[0].u = max_idx;

      raw.m[0] = raw.bounds[0].u - raw.bounds[0].l + 1;
      if (raw.m[0] > 0) {
	int addr = (((start_addr + (raw.bounds[0].l - min_idx) * ((inc == 1) ? 2 : 1) +
		      ((inc == 1) ? (chan % 2) : 0))) % (memsize / 3)) | ((chan / 2) << 20);
	int samps;
	pio(16, 0, &addr, 24);
	for (samples_to_read = raw.m[0], data_ptr = (short *)raw.pointer;
	     (samples_to_read > 0) && (status & 1); samples_to_read -= samps, data_ptr += samps) {
	  samps = min(samples_to_read, 32767);
	  if (fast) {
	    fstop(2, 0, samps, data_ptr, 16);
	  } else {
	    stop(2, 0, samps, data_ptr, 16);
	  }
	}
	if (status & 1) {
	  raw.a0 = raw.pointer - raw.bounds[0].l;
	  raw.arsize = raw.m[0] * 2;
	  status = TreePutRecord(data_nid, (struct descriptor *)&signal, 0);
	}
      }
    }
  }
  pio(16, 0, &mar, 24);
  free(raw.pointer);
  raw.pointer = 0;
  return status;
}
