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
#include "h912_gen.h"
#include "devroutines.h"





static int ReadChannel(InStoreStruct * setup, int samps, int chan, short *data_ptr);
static int one = 1;

#define pio(f,a,data,bits) {\
 if (!((status = DevCamChk(CamPiow(setup->name,a,f,data,bits,0),&one,&one)) & 1)) return status;}
#define return_on_error(f) if (!((status = f) & 1)) return status;
#define min(a,b) ((a) <= (b)) ? (a) : (b)
#define max(a,b) ((a) >= (b)) ? (a) : (b)

EXPORT int h912___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  struct _msetup {
    unsigned pretrig:1;
    unsigned clock:4;
    unsigned blocks:4;
    unsigned fill:7;
  } msetup = {
  0, 0, 0, 0};
  int status;
  int nid = setup->head_nid + H912_N_PTS;
  int onstat = TreeIsOn(nid);
  msetup.pretrig = onstat & 1 || onstat == TreePARENT_OFF;
  msetup.clock = setup->int_clock_convert;
  msetup.blocks = setup->blocks_convert;
  pio(16, 0, (short *)&msetup, 16);
  if (msetup.pretrig) {
    int pts;
    status = TdiGetLong(setup->pts, &pts);
    if (status & 1) {
      pio(16, 1, &pts, 16);
    } else
      return H912$_BAD_PTS;
  }
  pio(26, 0, 0, 16)
      return status;
}

EXPORT int h912___stop(struct descriptor *niddsc __attribute__ ((unused)), InStopStruct * setup)
{
  int status;
  pio(25, 0, 0, 16)
      return status;
}

EXPORT int h912___trigger(struct descriptor *niddsc __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(25, 2, 0, 16)
      return status;
}

EXPORT int h912___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  int status;
  //int mstatus;
  struct _status {
    unsigned mode:3;
    unsigned state:2;
    unsigned mem:2;
    unsigned fill:3;
    unsigned blocks:4;
    unsigned clock:4;
    unsigned fill2:6;
  } hstat;
  static float freqs[] =
      { 2E-6, 5E-6, 1E-5, 2E-5, 5E-5, 1E-4, 2E-4, 5E-4, 1E-3, 2E-3, 5E-3, 0., 0., 0., 0., 0. };
  static int memsize[] = { 8192, 32768, 65536, 131072 };
  static int blocktable[] = { 1, 2, 4, 8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 };
  static DESCRIPTOR_FLOAT(frequency, 0);
  static DESCRIPTOR_RANGE(int_clock, 0, 0, &frequency);
  static int ext_clock_nid;
  static DESCRIPTOR_NID(ext_clock, &ext_clock_nid);
  struct descriptor *clock;
  static int blocksize;
  static int blocks;
  static int pts;
  int hstat2;
  static EMPTYXD(timestamps);
  static EMPTYXD(signal);
  static DESCRIPTOR(timestamps_exp, "H912_TIMES($,$,$,$,$)");
  static int trigger_nid;
  static DESCRIPTOR_NID(trigger, &trigger_nid);
  static int timestamps_nid;
  static DESCRIPTOR_NID(timestamps_nid_d, &timestamps_nid);
  static DESCRIPTOR_LONG(blocks_d, &blocks);
  static DESCRIPTOR_LONG(blocksize_d, &blocksize);
  static DESCRIPTOR_LONG(pts_d, &pts);
  static DESCRIPTOR(sig_exp,
		    "BUILD_SIGNAL(BUILD_WITH_UNITS(1.25E-3*$VALUE,'volts'),BUILD_WITH_UNITS($,'counts'),$[ $ : $ ])");
  static DESCRIPTOR_A_BOUNDS(raw, sizeof(short), DTYPE_W, 0, 1, 0);
  static DESCRIPTOR_LONG(start_d, &raw.bounds[0].l);
  static DESCRIPTOR_LONG(end_d, &raw.bounds[0].u);
  static short *buffer;
  int samples;
  int chan;
  trigger_nid = setup->head_nid + H912_N_TRIGGER;
  timestamps_nid = setup->head_nid + H912_N_TIMESTAMPS;
  pio(0, 0, &hstat, 24);
  pio(0, 1, &pts, 24);
  blocks = blocktable[hstat.blocks];
  blocksize = memsize[hstat.mem] / blocks;
  if (hstat.mode == 1)
    pts = blocksize;
  pio(0, 2, &hstat2, 24);
  while (blocks) {
    if (hstat2 & (1 >> (blocks - 1)))
      break;
    else
      blocks--;
  }
  if (blocks == 0)
    return DEV$_NOT_TRIGGERED;
  if (hstat.clock > 10) {
    ext_clock_nid = setup->head_nid + H912_N_EXT_CLOCK;
    clock = (struct descriptor *)&ext_clock;
  } else {
    frequency.pointer = (char *)&freqs[hstat.clock];
    clock = (struct descriptor *)&int_clock;
  }
  return_on_error(TdiCompile
		  ((struct descriptor *)&timestamps_exp, clock, &blocks_d, &blocksize_d, &trigger, &pts_d,
		   &timestamps MDS_END_ARG));
  return_on_error(TreePutRecord(timestamps_nid, (struct descriptor *)&timestamps, 0));
  samples = blocksize * blocks;
  if (buffer) {
    free(buffer);
    buffer = 0;
  }
  buffer = malloc(samples * sizeof(short));
  for (chan = 1; chan < 16; chan++) {
    int sig_nid = setup->head_nid + H912_N_INPUT_1 + (chan - 1) * 3;
    int startidx_nid = sig_nid + 1;
    int endidx_nid = sig_nid + 2;
    int onstat = TreeIsOn(sig_nid);
    if ((onstat & 1) || (onstat == TreePARENT_OFF)) {
      int samples_to_read;
      status = DevLong(&startidx_nid, (int *)&raw.bounds[0].l);
      if (status & 1)
	raw.bounds[0].l = min(samples - 1, max(0, raw.bounds[0].l));
      else
	raw.bounds[0].l = 0;

      status = DevLong(&endidx_nid, (int *)&raw.bounds[0].u);
      if (status & 1)
	raw.bounds[0].u = min(samples - 1, max(0, raw.bounds[0].u));
      else
	raw.bounds[0].u = samples - 1;

      raw.m[0] = raw.bounds[0].u - raw.bounds[0].l + 1;
      if (raw.m[0] > 0) {
	samples_to_read = raw.bounds[0].u + 1;
	status = ReadChannel(setup, samples_to_read, chan, buffer);
	if (status & 1) {
	  raw.pointer = (char *)(buffer + (raw.bounds[0].l));
	  raw.a0 = (char *)buffer;
	  raw.arsize = raw.m[0] * sizeof(short);
	  return_on_error(TdiCompile
			  ((struct descriptor *)&sig_exp, &raw, &timestamps_nid_d, &start_d, &end_d,
			   &signal MDS_END_ARG));
	  return_on_error(TreePutRecord(sig_nid, (struct descriptor *)&signal, 0));
	}
      }
    }
  }
  free(buffer);
  buffer = 0;
  return status;
}

static int ReadChannel(InStoreStruct * setup, int samps, int chan, short *data_ptr)
{
  int chan_select = chan << 17;
  int status;
  short *dptr;
  int remainder;
  int s;
  pio(17, 0, &chan_select, 24);
  if (!(CamXandQ(0) & 1))
    return 0;
  for (dptr = data_ptr, remainder = samps, s = min(remainder, 32767);
       s; dptr += s, remainder -= s, s = min(remainder, 32767)) {
    return_on_error(DevCamChk(CamFStopw(setup->name, 0, 2, s, dptr, 16, 0), &one, 0));
    chan_select += s;
    pio(17, 0, &chan_select, 24);
  }
  return status;
}
