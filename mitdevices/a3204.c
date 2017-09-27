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
#include <mds_stdarg.h>
#include "a3204_gen.h"
#include <libroutines.h>
#include <strroutines.h>
#include <treeshr.h>
#include <mdsshr.h>
#include "devroutines.h"
static int one = 1;
#define min(a,b) (((a) <= (b)) ? (a) : (b))
#define max(a,b) (((a) <= (b)) ? (b) : (a))
#define SignalError(call) status = call; if (!(status & 1)) { return status; }
#define pio(f,a,dv) {int d = dv;\
 if (!((status = DevCamChk(CamPiow(setup->name,a,f,&d,16,0),&one,&one)) & 1)) return status;}
extern int DevCamChk();


static int InitChannel(InInitStruct * setup, int chan, int gain, float offset)
{
  int status = 1;
  int input_nid = setup->head_nid + A3204_N_INPUT_1 + chan * (A3204_N_INPUT_2 - A3204_N_INPUT_1);
  int filter_nid = input_nid + A3204_N_INPUT_1_FILTER - A3204_N_INPUT_1;
  if (TreeIsOn(input_nid) & 1) {
    unsigned short ctl = gain | ((TreeIsOn(filter_nid) & 1) ? 0x080 : 0);
    pio(16, chan, ctl)
	ctl = (offset + 10.0) * 2048. / 10. + .4999;
    ctl = min(max(ctl, 0), 4095);
    pio(17, chan, ctl)
  }
  return status;
}

EXPORT int a3204___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  status = InitChannel(setup, 0, setup->input_1_gain_convert, setup->input_1_offset);
  status = InitChannel(setup, 1, setup->input_2_gain_convert, setup->input_2_offset);
  status = InitChannel(setup, 2, setup->input_3_gain_convert, setup->input_3_offset);
  status = InitChannel(setup, 3, setup->input_4_gain_convert, setup->input_4_offset);
  return status;
}

static int StoreChannel(InStoreStruct * setup, int chan)
{
  int status=1;
  int input_nid = setup->head_nid + A3204_N_INPUT_1 + chan * (A3204_N_INPUT_2 - A3204_N_INPUT_1);
  int filter_on_nid = input_nid + A3204_N_INPUT_1_FILTER_ON - A3204_N_INPUT_1;
  int output_nid = input_nid + A3204_N_INPUT_1_OUTPUT - A3204_N_INPUT_1;
  if (TreeIsOn(input_nid) & 1) {
    static float gain;
    static float offset;
    static float gains[] = { .5, 1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0, 512.0 };
    static DESCRIPTOR(expression, "build_with_units(($ - $) * $,\"volts\")");
    static DESCRIPTOR_FLOAT(gain_d, &gain);
    static DESCRIPTOR_FLOAT(offset_d, &offset);
    static EMPTYXD(value);
    static DESCRIPTOR_NID(out_nid_d, 0);
    static DESCRIPTOR(y, "Y");
    static DESCRIPTOR(n, "N");
    unsigned short ctl = 0;
    struct descriptor *filter;
    pio(0, chan, ctl)
	filter = (struct descriptor *)(((ctl & 0x080) == 0x080) ? &y : &n);
    ctl &= 0xf;
    gain = (ctl <= 10) ? 1.0 / gains[ctl] : 2.0;
    pio(1, chan, ctl)
	offset = ctl * 10. / 2048.0 - 10.0;
    out_nid_d.pointer = (char *)&output_nid;
    SignalError(TdiCompile((struct descriptor *)&expression, &out_nid_d, &offset_d, &gain_d, &value MDS_END_ARG));
    SignalError(TreePutRecord(input_nid, (struct descriptor *)&value, 0));
    if (TreeIsOn(filter_on_nid) & 1)
      SignalError(TreePutRecord(filter_on_nid, filter, 0));
  }
  return status;
}

EXPORT int a3204___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  int status;
  int i;
  for (i = 0; i < 4; i++)
    status = StoreChannel(setup, i);
  return status;
}
