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
#include "joerger_cg_gen.h"
#include "devroutines.h"


static int one = 1;
#define pio(f,a,d,q) {unsigned short dv=d;\
 if (!((status = DevCamChk(CamPiow(setup->name,a,f,&dv,16,0),&one,q)) & 1)) return status;}

static float slopes[] =
    { 1E9, 1, 1E-1, 1E-2, 1E-3, 1E-4, 2E-5, 1E-5, 4E-6, 2E-6, 1E-6, 4E-7, 2E-7, 1E-7, 5E-8,
2.5E-8 };

static int SlopeToFreq(float slope, float *actslope)
{
  int i;
  for (i = 0; i < 16 && slope < (.95 * slopes[i]); i++) ;
  i = i < 16 ? i : 15;
  *actslope = slopes[i];
  return i;
}

static int SetChannel(InInitStruct * setup, struct descriptor *channel_value, int channel,
		      int *polarities)
{
  static EMPTYXD(xd);
  int status;
  int p = *polarities;
  int nid = setup->head_nid + JOERGER_CG_N_CHANNEL_1 + channel * 2 - 2;
  float actslope;
  static float slope;
  static DESCRIPTOR_FLOAT(slope_d, &slope);
  status = TdiSlopeOf(channel_value, &slope_d MDS_END_ARG);
  pio(17, channel - 1, SlopeToFreq(slope, &actslope), &one);
  if (slope != actslope) {
    static DESCRIPTOR(range, "*:*:$");
    slope = actslope;
    TdiCompile((struct descriptor *)&range, &slope_d, &xd MDS_END_ARG);
    TreePutRecord(nid, (struct descriptor *)&xd, 0);
  }
  nid++;
  p |= (TreeIsOn(nid) & 1) << (channel - 1);
  *polarities = p;
  return status;
}

int joerger_cg___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  int polarities = 0;
  int clock_mode = 0;
  pio(9, 1, 0, 0);
  SetChannel(setup, setup->channel_1, 1, &polarities);
  SetChannel(setup, setup->channel_2, 2, &polarities);
  SetChannel(setup, setup->channel_3, 3, &polarities);
  SetChannel(setup, setup->channel_4, 4, &polarities);
  pio(17, 4, clock_mode, &one);
  pio(17, 5, polarities, &one);
  pio(26, 1, 0, 0)
      return status;
}

int joerger_cg___stop(struct descriptor *niddsc __attribute__ ((unused)), InStopStruct * setup)
{
  int status;
  pio(9, 1, 0, 0)
      return status;
}
