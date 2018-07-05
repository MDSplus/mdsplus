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
#include "l8828_gen.h"
#include "devroutines.h"

#include <camdef.h>
extern int DevCamChk();

static int one = 1;
//static int zero = 0;

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define pio(f,a) {\
 if (!((status = DevCamChk(CamPiow(setup->name,a,f,0,16, 0),&one,0)) & 1)) return status;}
#define piod(f,a, d) {\
 if (!((status = DevCamChk(CamPiow(setup->name,a,f,d,16, 0),&one,0)) & 1)) return status;}
#define fstop(f,a,num,data) {\
 if (!((status = DevCamChk(CamFStopw(setup->name,a,f,num,data,16,0),&one,&one)) & 1)) return status;}
#define qrep(f,a,num,data) {\
 if (!((status = DevCamChk(CamQrepw(setup->name,a,f,num,data,16,0),&one,&one)) & 1)) return status;}

typedef struct {
  unsigned pre_trig:4;
  unsigned clock:3;
  unsigned fill:1;
  unsigned active_mem:7;
  unsigned user:1;
} L8828Control;

EXPORT int l8828___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  L8828Control ctrl;
  //float freq;
  int status;
  //int active_mem_code;
  ctrl.active_mem = (setup->active_mem / 16) - 1;
  ctrl.pre_trig = setup->pre_trig;
  ctrl.clock = setup->clock_convert;

  pio(25, 0)
      piod(16, 0, (short *)&ctrl)
      piod(19, 0, &setup->input_offset)
      pio(9, 0)

      return status;
}

EXPORT int l8828___trigger(struct descriptor *niddsc __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(25, 0)
      return status;
}

EXPORT int l8828___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{

#define min(a,b) ((a) <= (b)) ? (a) : (b)
#define max(a,b) ((a) >= (b)) ? (a) : (b)

  L8828Control ctrl;

  //static struct descriptor_d name = { 0, DTYPE_T, CLASS_D, 0 };
  static DESCRIPTOR_A_BOUNDS(raw, sizeof(unsigned char), DTYPE_BU, 0, 1, 0);
  static DESCRIPTOR(counts_str, "counts");
  static DESCRIPTOR_WITH_UNITS(counts, &raw, &counts_str);
  static DESCRIPTOR_LONG(start_d, &raw.bounds[0].l);
  static DESCRIPTOR_LONG(end_d, &raw.bounds[0].u);
  static int trig_in_nid;
  static DESCRIPTOR_NID(trigger_d, &trig_in_nid);
  static float frequency;
  static DESCRIPTOR_FLOAT(frequency_d, &frequency);
  static DESCRIPTOR_FUNCTION_1(double_dt, (unsigned char *)&OpcDble, &frequency_d);
  static DESCRIPTOR_RANGE(int_clock_d, 0, 0, &double_dt);
  static int clock_in_nid;
  static DESCRIPTOR_NID(ext_clock_d, &clock_in_nid);
  static float coefficient = .510 / 256;
  static DESCRIPTOR_FLOAT(coef_d, &coefficient);
  static short offset = 0;
  static struct descriptor_s offset_d = { 2, DTYPE_W, CLASS_S, (char *)&offset };
  //static int key;
  //static DESCRIPTOR_LONG(key_d, &key);
  static DESCRIPTOR_FUNCTION_1(value, (unsigned char *)&OpcValue, 0);
  static DESCRIPTOR_FUNCTION_2(add_exp, (unsigned char *)&OpcAdd, &offset_d, &value);
  static DESCRIPTOR_FUNCTION_2(mult_exp, (unsigned char *)&OpcMultiply, &coef_d, &add_exp);
  static DESCRIPTOR(volts_str, "volts");
  static DESCRIPTOR_WITH_UNITS(volts, &mult_exp, &volts_str);
  static DESCRIPTOR_WINDOW(window, &start_d, &end_d, &trigger_d);
  static DESCRIPTOR_DIMENSION(dimension, &window, &ext_clock_d);
  static DESCRIPTOR(time_str, "seconds");
  static DESCRIPTOR_WITH_UNITS(time, &dimension, &time_str);
  static DESCRIPTOR_SIGNAL_1(signal, &volts, &counts, &time);

  unsigned char *data_ptr;
  int status;
  int start_addr = 0;
  //int dummy;

  int max_mem;
  int pre_trig;
  int samples_to_read;
  int min_idx;
  int max_idx;

  trig_in_nid = setup->head_nid + L8828_N_TRIGGER;
  clock_in_nid = setup->head_nid + L8828_N_EXT_CLOCK;
  value.ndesc = 0;

  pio(17, 0);
  if (!CamXandQ(0))
    return DEV$_NOT_TRIGGERED;

  piod(2, 0, &start_addr);	/* this is either 0 or 1 */

  piod(0, 0, (short *)&ctrl);

  if (ctrl.clock == 7) {
    dimension.axis = (struct descriptor *)&ext_clock_d;
  } else {
    static float freqs[] = { 4E-9, 8E-9, 16E-9, 32E-9, 64E-9, 128E-9, 256E-9, 0 };
    frequency = freqs[min(ctrl.clock, 7)];
    dimension.axis = (struct descriptor *)&int_clock_d;
  }

/*   max_mem = (ctrl.active_mem+1)*16*1024; */
  max_mem = (ctrl.active_mem + 1) * 16 * 1024;
  pre_trig = ctrl.pre_trig * max_mem / 8;
  max_idx = max_mem - pre_trig - 1 - start_addr;
/*   min_idx =  -pre_trig+16; */
  min_idx = -pre_trig;

  offset = 0;
  piod(1, 0, &offset);
  offset = ((offset >> 8) & 0xFF) - 255;

  data_ptr = malloc(max_mem);
  raw.pointer = (char *)data_ptr + start_addr;
/*  raw.pointer = (char *)data_ptr+start_addr+16; */
  {
    int input_nid = setup->head_nid + L8828_N_INPUT;
    int startidx_nid = setup->head_nid + L8828_N_INPUT_STARTIDX;
    int endidx_nid = setup->head_nid + L8828_N_INPUT_ENDIDX;
    if (TreeIsOn(input_nid) & 1) {
      status = DevLong(&startidx_nid, (int *)&raw.bounds[0].l);
      if (status & 1)
	raw.bounds[0].l = min(max_idx, max(min_idx, raw.bounds[0].l));
      else
	raw.bounds[0].l = min_idx;

      status = DevLong(&endidx_nid, (int *)&raw.bounds[0].u);
      if (status & 1)
	raw.bounds[0].u = min(max_idx, max(min_idx, raw.bounds[0].u));
      else
	raw.bounds[0].u = max_idx;

      status = 1;
      raw.m[0] = raw.bounds[0].u - raw.bounds[0].l + 1;
      if (raw.m[0] > 0) {
	int samps;
/*      for (samples_to_read = raw.m[0]+start_addr+16; */
	for (samples_to_read = raw.m[0];
	     (samples_to_read > 0) && (status & 1); samples_to_read -= samps, data_ptr += samps) {
	  int ios;
	  samps = min(samples_to_read, 65534);
	  ios = (samps + 1) / 2;
	  fstop(2, 0, ios, data_ptr);
	}
	if (status & 1) {
	  raw.a0 = raw.pointer - raw.bounds[0].l;
	  raw.arsize = raw.m[0];
	  status = TreePutRecord(input_nid, (struct descriptor *)&signal, 0);
	}
      }
    }
  }
  free(raw.pointer);
  return status;
}
