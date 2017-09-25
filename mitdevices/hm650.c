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
/*****************************************************************************
 	Description:

   Ported from Jeff Casey's fortran support:

Service for Highland M650 eight channel delay/gate generator by MDSplus.  This
service will assume that the hardware switches of the M650 are setup correctly
for the application, and will only load the single threshold value for all
channels, and the eight delay values.  The action takes place on the init
phase, and there is no activity on the store phase.

Assumes that delay information in tree is in nsec.
Assumes that threshold information in tree is in Volts.

Note that the input and output levels (NIM, ECL, TTL); sign (POS, NEG);
logic (trigger on input, trigger by CAMAC, trigger by previous channel
hit, trigger by previous channel done); and gating (autoclr, clear by
following channel, oneshot) are set up by HARDWARE switches inside the 
module, and are not settable or readable by software.

Note also that the front panel VETO and CLR inputs may inhibit action.

------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include "hm650_gen.h"
#include "devroutines.h"


static int one = 1;
#define min(a,b) ((a)<(b) ? (a) : (b))
#define max(a,b) ((a)>(b) ? (a) : (b))
#define pio(f,a,d,mem)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, mem, 0), &one, &one),status)
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

EXPORT int hm650___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status = 1;
  int dly_status = 1;
  int put_status = 1;
  unsigned short int_threshold;
  int i;
  float delays[8];
  int zero = 0;

  pio(21, 0, &zero, 16);

  int_threshold = (setup->threshold + 2.5) / 5. * 4095;
  int_threshold = (min(max(int_threshold, 0), 4095));
  pio(17, 0, &int_threshold, 16);

  delays[0] = setup->delay_0;
  delays[1] = setup->delay_1;
  delays[2] = setup->delay_2;
  delays[3] = setup->delay_3;
  delays[4] = setup->delay_4;
  delays[5] = setup->delay_5;
  delays[6] = setup->delay_6;
  delays[7] = setup->delay_7;
  for (i = 0; i < 8; i++) {
    unsigned short ndelay;
    static float delay;
    static DESCRIPTOR(expr, "BUILD_WITH_UNITS($ + $/1E-9, S)");
    static DESCRIPTOR_FLOAT(delay_dsc, &delay);
    static DESCRIPTOR_NID(trig_dsc, 0);
    delay = delays[i];
    if (delay < 20.) {
      delay = 20;
      dly_status = HM650$_DLYCHNG;
    }
    if (delay <= 102.375)
      ndelay = (int)(delay / 102.4 * 4096);
    else if (delay <= 1023.74)
      ndelay = ((int)(delay / 1024. * 4096)) | (1 << 12);
    else if (delay <= 10237.5)
      ndelay = ((int)(delay / 10240. * 4096)) | (2 << 12);
    else if (delay <= 102375.)
      ndelay = ((int)(delay / 102400. * 4096)) | (3 << 12);
    else {
      dly_status = HM650$_DLYCHNG;
      ndelay = 0xFFFF;
    }
    pio(16, i, &ndelay, 16);
    if (status & 1) {
      static struct descriptor_xd out_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
      int trig_in = setup->head_nid + HM650_N_TRIG_IN_0 + i;
      int trig_out = setup->head_nid + HM650_N_TRIG_OUT_0 + i;
      delay = (ndelay | 0xFFF) / 4096. * 102.4;
      switch (ndelay >> 12) {
      case 1:
	delay *= 10;
	break;
      case 2:
	delay *= 100.;
	break;
      case 3:
	delay *= 1000.;
	break;
      }
      trig_dsc.pointer = (char *)&trig_in;
      put_status = TdiCompile((struct descriptor *)&expr, &trig_dsc, &delay_dsc MDS_END_ARG);
      if (put_status & 2)
	put_status = TreePutRecord(trig_out, (struct descriptor *)&out_xd, 0);
    }
  }

  pio(21, 0, &zero, 16);

  if ((status & 1) == 0)
    return status;
  else if ((dly_status & 1) == 0)
    return dly_status;
  else if ((put_status & 1) == 0)
    return put_status;
  else
    return 1;
}

EXPORT int hm650___trigger(struct descriptor *niddsc __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(25, 0, 0, 16);
  return status;
}
