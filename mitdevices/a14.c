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
#include "a14_gen.h"
#include <strroutines.h>
#include <libroutines.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <stdio.h>

#define pio16(fv,av,d) {int a = av; int f = fv; int one=1;\
   if (!((status = DevCamChk(CamPiow(setup->name,a,f,&d,16,0),&one,&one)) & 1)) return status;}
#define pio(fv,av,d) {int a = av; int f = fv; int one=1;\
   if (!((status = DevCamChk(CamPiow(setup->name,a,f,&d,24,0),&one,&one)) & 1)) return status;}
static int fast = 1;
#define fstop(fv,av,cv,d) {int a = av; int f = fv; int c = cv; int one=1;\
 if (fast) {\
   if (!((status = DevCamChk(CamFStopw(setup->name,a,f,c,d,16,0),&one,0)) & 1)) return status; \
 } else { \
   if (!((status = DevCamChk(CamStopw(setup->name,a,f,c,d,16,0),&one,0)) & 1)) return status;} \
 }

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

extern int DevCamChk();
extern int DevLong();

typedef struct {
  unsigned clock_speed:3;
  unsigned internal_clk:1;
  unsigned channel:3;
  unsigned bits_24:1;
  unsigned mode:3;
  unsigned gated:1;
  unsigned stp_polarity:1;
  unsigned str_polarity:1;
  unsigned clk_polarity:1;
  unsigned ram_size:2;
  unsigned:15;
} StatusRegister;

EXPORT int a14___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  int pts;
  int zero = 0;
  StatusRegister sr;
  sr.clock_speed = setup->clock_divide_convert;
  sr.internal_clk = setup->ext_clock_in == 0;
  sr.mode = setup->mode;
  sr.gated = setup->gated;
  sr.stp_polarity = setup->stp_polarity;
  sr.str_polarity = setup->str_polarity;
  sr.clk_polarity = setup->clk_polarity;
  pio(9, 1, zero)		/* Reset - CMD mode */
      pio(17, 4, sr)		/* Load status register */
      pio(1, 4, sr)		/* Read status register */
      switch (sr.ram_size) {
  case 2:
  case 1:
    pts = (setup->pts + 2047) / 2048;
    break;
  default:
    pts = (setup->pts + 1023) / 1024;
    break;
  }
  pio(17, 3, pts);		/* Load PTS register */
  pio(17, 2, zero);		/* Zero memory pointer */
  pio(26, 0, zero)		/* Enable LAM */
      pio(9, 2, zero)		/* Put in DAQ mode */
      if (sr.mode == 1)
    pio(25, 0, zero);
  return status;
}

EXPORT int a14___stop(struct descriptor *niddsc __attribute__ ((unused)), InStopStruct * setup)
{
  int status;
  int zero = 0;
  pio(25, 1, zero)
      return status;
}

EXPORT int a14___start(struct descriptor *niddsc __attribute__ ((unused)), InStartStruct * setup)
{
  int status;
  int zero = 0;
  pio(25, 0, zero)
      return status;
}

static int ReadChannel(InStoreStruct * setup, int memptr, int first_idx, int samples, int channel,
		       short *buffer, int *samples_read)
{
  int i;
  int address = memptr + first_idx;
  int status = 1;
  typedef union {
    int zero;
    unsigned channel:3;
  } ChannelSelect;
  static ChannelSelect channel_select;
  *samples_read = 0;
  channel_select.channel = channel;
  pio(16, 6, channel_select);
  while (status & 1 && *samples_read < samples) {
    int samples_to_read = min(samples - *samples_read, 32000);
    int smps;
    if (samples_to_read > 1 && (((address + samples_to_read) & 0xff) == 0xff))
      samples_to_read--;
    pio(17, 2, address);
    if (samples_to_read > 1) {
      fstop(2, 0, samples_to_read, &buffer[*samples_read]);
    } else {
      pio16(2, 0, buffer[*samples_read]);
    }
    smps = CamBytcnt(0) / sizeof(short);
    if (!smps)
      break;
    *samples_read += smps;
    address += smps;
  }
  for (i = 0; i < (*samples_read - 1);)
    buffer[i++] &= 4095;
  return status;
}

static int getMemoryPointer(InStoreStruct * setup)
{
  int status;
  int i;
  int ptr;
  int memptr;
  pio(1, 2, memptr);		/* get memory ptr */
  for (i = 0; i < 10; i++) {
    pio(1, 2, ptr);
    if (ptr != memptr) {
      printf("Inconsistent memory pointer, was %d now %d\n", memptr, ptr);
      memptr = ptr;
    }
  }
  return memptr;
}

EXPORT int a14___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  int status = 1;
  static long zero = 0;
  static DESCRIPTOR_LONG(zero_d, &zero);
  static long one = 1;
  static DESCRIPTOR_LONG(one_d, &one);
  static DESCRIPTOR_A_BOUNDS(raw, sizeof(short), DTYPE_W, 0, 1, 0);
  static DESCRIPTOR(counts_str, "counts");
  static DESCRIPTOR_WITH_UNITS(counts, &raw, &counts_str);
  static DESCRIPTOR_LONG(start_d, &raw.bounds[0].l);
  static DESCRIPTOR_LONG(end_d, &raw.bounds[0].u);
  static float coefficient;
  static DESCRIPTOR_FLOAT(coef_d, &coefficient);
  static short offset = 0;
  static struct descriptor_s offset_d = { 2, DTYPE_W, CLASS_S, (char *)&offset };
  static DESCRIPTOR_FUNCTION_1(value, (unsigned char *)&OpcValue, 0);
  static DESCRIPTOR_FUNCTION_2(add_exp, (unsigned char *)&OpcAdd, &offset_d, &value);
  static DESCRIPTOR_FUNCTION_2(mult_exp, (unsigned char *)&OpcMultiply, &coef_d, &add_exp);
  static DESCRIPTOR(volts_str, "volts");
  static DESCRIPTOR_WITH_UNITS(volts, &mult_exp, &volts_str);
  static int trigger_nid;
  static DESCRIPTOR_NID(trigger_nid_dsc, &trigger_nid);
  static int pts;
  static DESCRIPTOR_LONG(pts_dsc, &pts);
  static int clock_divide;
  static DESCRIPTOR_LONG(clock_divide_dsc, &clock_divide);
  static int clock_divides[] = { 1, 2, 4, 10, 20, 40, 100 };
  static float dt;
  static DESCRIPTOR_FLOAT(dt_dsc, &dt);
  static EMPTYXD(dimension);
  static DESCRIPTOR(time_str, "seconds");
  static DESCRIPTOR_WITH_UNITS(time, &dimension, &time_str);
  static DESCRIPTOR_SIGNAL_1(signal, &volts, &counts, &time);
  static DESCRIPTOR(post_ext,
		    "_A14_ADJUST=A14_ADJUST($3,$4),BUILD_DIM(BUILD_WINDOW($5+$1+_A14_ADJUST,$5+$2+_A14_ADJUST,$3),a14_clock_divide($4,$6))");
  static DESCRIPTOR(post_int,
		    "_A14_ADJUST=A14_ADJUST($3,* : * : $4),BUILD_DIM(BUILD_WINDOW($5+$1+_A14_ADJUST,$5+$2+_A14_ADJUST,$3),* : * : $4)");
  static DESCRIPTOR(burst_time_ext,
		    "$1 : ($1 + ($2 - .5) * SLOPE_OF($3) * ($4)) : SLOPE_OF($3) * ($4)");
  static DESCRIPTOR(burst_time_int, "$1 : ($1 + ($2 - .5) * $3) : $3");
  int i;
  int min_idx;
  int max_idx;
  int rtsr;
  int range;
  int memsize_idx;
  int memptr;
  int start_addr;
  int retstatus;
  StatusRegister sr;
  static int max_samples[] = { 128 * 1024, 512 * 1024, 512 * 1024, 512 * 1024 };

  int name_nid = 0;

/*                           0-2.5     0-5      0-5       0-10    +/- 2.5   +/- 5     +/- 5     +/- 10     */
  static float coeffs[] =
      { 2.5 / 4096, 5. / 4096, 5. / 4096, 10. / 4096, 5. / 4096, 10. / 4096, 10. / 4096,
20. / 4096 };
  static short offsets[] = { 0, 0, 0, 0, -2048, -2048, -2048, -2048 };

  name_nid = setup->head_nid + A14_N_NAME;
  fast = TreeIsOn(name_nid) & 1;
  pio(1, 0, rtsr);
  pio(24, 1, zero);		/* select CMD mode */
  memptr = getMemoryPointer(setup);
  pio(1, 3, pts);
  pio(1, 4, sr);
  trigger_nid = setup->head_nid + A14_N_STOP_TRIG;
  clock_divide = clock_divides[sr.clock_speed];
  dt = 1E-6 * clock_divide;
  pio(1, 5, range);
  value.ndesc = 0;
  memsize_idx = max_samples[sr.ram_size] - 1;
  pts *= memsize_idx > 512000 ? 2048 : 1024;
  pts = min(pts, memsize_idx);
  if ((rtsr & 1) && (sr.mode || memptr < pts))
    return DEV$_NOT_TRIGGERED;
  switch (sr.mode) {
  case 0:
  case 4:
    min_idx = 0;
    max_idx = pts ? pts : memsize_idx - 1;
    start_addr = 0;
    break;
  case 1:
    min_idx = pts - memsize_idx;
    max_idx = pts - 1;
    start_addr = memptr - pts;
    break;
  case 2:
  case 3:
    min_idx = 0;
    max_idx = memptr ? memptr - 1 : memsize_idx;
    if (max_idx > memsize_idx)
      max_idx = memsize_idx;
    start_addr = 0;
    break;
  default:
    min_idx = 0;
    max_idx = memsize_idx;
    start_addr = 0;
    break;
  }
  if (raw.pointer)
    free(raw.pointer);
  raw.pointer = malloc((max_idx - min_idx + 1) * sizeof(short));
  for (i = 0; i < 6 && status & 1; i++, range = range >> 3) {
    int chan_nid = setup->head_nid + (A14_N_INPUT_2 - A14_N_INPUT_1) * i + A14_N_INPUT_1;
    if (TreeIsOn(chan_nid) & 1) {
      int start_idx_nid = chan_nid + A14_N_INPUT_1_STARTIDX - A14_N_INPUT_1;
      int end_idx_nid = chan_nid + A14_N_INPUT_1_ENDIDX - A14_N_INPUT_1;
      offset = offsets[range & 7];
      coefficient = coeffs[range & 7];
      status = DevLong(&start_idx_nid, &raw.bounds[0].l);
      if (status & 1)
	raw.bounds[0].l = min(max_idx, max(min_idx, raw.bounds[0].l));
      else
	raw.bounds[0].l = min_idx;
      status = DevLong(&end_idx_nid, &raw.bounds[0].u);
      if (status & 1)
	raw.bounds[0].u = min(max_idx, max(raw.bounds[0].l, raw.bounds[0].u));
      else
	raw.bounds[0].u = max_idx;
      raw.m[0] = raw.bounds[0].u - raw.bounds[0].l + 1;
      if (raw.m[0] > 0) {
	int samples_read;
	status =
	    ReadChannel(setup, start_addr, raw.bounds[0].l, raw.m[0], i, (short *)raw.pointer,
			&samples_read);
	if (status & 1) {
	  raw.a0 = raw.pointer - raw.bounds[0].l * sizeof(short);
	  raw.arsize = samples_read * sizeof(short);
	  raw.bounds[0].u = raw.bounds[0].l + samples_read - 1;
	  raw.m[0] = samples_read;
	  switch (sr.mode) {
	  case 0:
	  case 1:
	    if (setup->dimension)
	      MdsCopyDxXd(setup->dimension, &dimension);
	    else if (setup->ext_clock_in) {
	      static const int clock_divides[] = { 1, 2, 4, 10, 20, 40, 100, 1 };
	      int clock_divide_l = clock_divides[sr.clock_speed];
	      DESCRIPTOR_LONG(clock_divide, &clock_divide_l);
	      TdiCompile((struct descriptor *)&post_ext, &start_d, &end_d, &trigger_nid_dsc, setup->ext_clock_in,
			 (sr.mode == 0) ? &zero_d : &one_d, &clock_divide, &dimension MDS_END_ARG);
	    } else
	      TdiCompile((struct descriptor *)&post_int, &start_d, &end_d, &trigger_nid_dsc, &dt_dsc,
			 (sr.mode == 0) ? &zero_d : &one_d, &dimension MDS_END_ARG);
	    break;
	  case 3:
	    if (setup->dimension)
	      MdsCopyDxXd(setup->dimension, &dimension);
	    else if (setup->ext_clock_in)
	      TdiCompile((struct descriptor *)&burst_time_ext, &trigger_nid_dsc, &pts_dsc, setup->ext_clock_in,
			 &clock_divide_dsc, &dimension MDS_END_ARG);
	    else
	      TdiCompile((struct descriptor *)&burst_time_int, &trigger_nid_dsc, &pts_dsc, &dt_dsc,
			 &dimension MDS_END_ARG);
	    break;
	  default:
	    if (setup->dimension)
	      MdsCopyDxXd(setup->dimension, &dimension);
	    else
	      MdsFree1Dx(&dimension, 0);
	  }
	  status = TreePutRecord(chan_nid, (struct descriptor *)&signal, 0);
	}
      }
    }
  }
  retstatus = status;
  free(raw.pointer);
  raw.pointer = 0;
  pio(17, 2, memptr);
  return retstatus;
}
