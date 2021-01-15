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
#include "l2256_gen.h"
#include "devroutines.h"

//extern unsigned short OpcAdd;
//extern unsigned short OpcMultiply;
//extern unsigned short OpcValue;



static int one = 1;
#define pio(f,a) {\
 if (!((status = DevCamChk(CamPiow(setup->name,a,f,0,16,0),&one,0)) & 1)) return status;}
#define pioqrep(f,a,d) {\
 if (!((status = DevCamChk(CamPioQrepw(setup->name,a,f,d,16,0),&one,&one)) & 1)) return status;}
#define fstopw(f,a,n,d) {\
 if (!((status = DevCamChk(CamFStopw(setup->name,a,f,n,d,16,0),&one,0)) & 1)) return status;}
#define CHAN_NID(chan, field) c_nids[L2256_N_INPUTS+chan*L2256_K_NODES_PER_INP+field]
#define min(a,b) ((a) <= (b)) ? (a) : (b)
#define max(a,b) ((a) >= (b)) ? (a) : (b)

EXPORT int l2256___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  pio(9, 0)
      pio(26, 0)
      return status;
}

EXPORT int l2256___trigger(struct descriptor *niddsc __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(25, 0)
      return status;
}

EXPORT int l2256___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  //static struct descriptor name = { 0, DTYPE_T, CLASS_D, 0 };
  static DESCRIPTOR_A_BOUNDS(raw, sizeof(short), DTYPE_W, 0, 1, 0);
  static DESCRIPTOR(counts_str, "counts");
  static DESCRIPTOR_WITH_UNITS(counts, &raw, &counts_str);
  static DESCRIPTOR_LONG(start_d, &raw.bounds[0].l);
  static DESCRIPTOR_LONG(end_d, &raw.bounds[0].u);
  static int trigger_nid;
  static DESCRIPTOR_NID(trigger_d, &trigger_nid);
  static float frequency;
  static DESCRIPTOR_FLOAT(frequency_d, &frequency);
  static DESCRIPTOR_RANGE(int_clock_d, 0, 0, &frequency_d);
  static int ext_clock_nid;
  static DESCRIPTOR_NID(ext_clock_d, &ext_clock_nid);
  static float coefficient = .002;
  static DESCRIPTOR_FLOAT(coef_d, &coefficient);
  static float offset = 0;
  static struct descriptor_s offset_d = { 4, DTYPE_NATIVE_FLOAT, CLASS_S, (char *)&offset };
  //static int key;
  //static DESCRIPTOR_LONG(key_d, &key);
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
  static float dts[] = { 0., 5.e-6, 2.e-6, 1.e-6, 500.e-9, 200.e-9, 100.e-9, 50.e-9 };
  static int pre[] = { 0, 128, 256, 348, 512, 640, 768, 896 };
  int channel_nid = setup->head_nid + L2256_N_INPUT;
  short channel_data[1024];
  int status = 1;
  //int maxidx;
  //int minidx;
  //int i;
  int pts;

  //#pragma member_alignment save
  //#pragma nomember_alignment
  struct {
 unsigned __attribute__ ((packed)) frequency:3;
 unsigned __attribute__ ((packed)) pre:3;
 unsigned __attribute__ ((packed)) offset:8;
  } reg;
  //#pragma member_alignment restore

  if (TreeIsOn(channel_nid) & 1) {
    value.ndesc = 0;
    pio(8, 0);
    if ((CamXandQ(0) & 1) == 0)
      return DEV$_NOT_TRIGGERED;
    ext_clock_nid = setup->head_nid + L2256_N_EXT_CLOCK;
    trigger_nid = setup->head_nid + L2256_N_TRIGGER;
    pio(9, 0);
    pio(27, 0);
    pioqrep(1, 0, (short *)&reg);
    offset = reg.offset * -.002;
    frequency = dts[reg.frequency];
    pts = 1024 - pre[reg.pre];
    raw.bounds[0].u = pts;
    raw.bounds[0].l = pts - 1023;
    raw.pointer = (char *)channel_data;
    dimension.axis =
	(reg.frequency ==
	 0) ? (struct descriptor *)&ext_clock_d : (struct descriptor *)&int_clock_d;
    raw.m[0] = 1024;
    raw.arsize = sizeof(channel_data);
    raw.a0 = (char *)(channel_data - raw.bounds[0].l);
    fstopw(2, 0, 1024, channel_data);
    if (status & 1)
      status = TreePutRecord(channel_nid, (struct descriptor *)&signal, 0);
  }
  return status;
}
