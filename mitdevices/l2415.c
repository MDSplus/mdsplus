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
#include "l2415_gen.h"
#include "devroutines.h"


static int one = 1;
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define pio(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16, 0), &one, 0),status)
#define qrep(f,a,count,d)  return_on_error(DevCamChk(CamQrepw(setup->name, a, f, count, d, 16, 0), &one, &one),status)

EXPORT int l2415___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status = 1;
  unsigned short int word_out;
  word_out = setup->voltage * setup->polarity / (setup->range == 3500. ? .25 : .5);
  pio(16, 0, &word_out)
      word_out = setup->current / ((setup->range == 3500.) ? .0025 : .001) * 255;
  pio(16, 1, &word_out)
      return status;
}

EXPORT int l2415___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  static int polarity_nid;
  static int range_nid;
  static int volt_out_nid;
  static int curr_out_nid;
  static DESCRIPTOR_NID(polarity_dsc, &polarity_nid);
  static DESCRIPTOR_NID(range_dsc, &range_nid);
  static struct descriptor_xd out_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  int status = 1;
  static short voltage;
  static struct descriptor voltage_dsc = { sizeof(short), DTYPE_W, CLASS_S, (char *)&voltage };
  static DESCRIPTOR(volt_expr, "BUILD_WITH_UNITS($ * $ * ($ == 3500. ? 1. : 2.), 'Volts')");
  static short current;
  static struct descriptor current_dsc = { sizeof(short), DTYPE_W, CLASS_S, (char *)&current };
  static DESCRIPTOR(current_expr,
		    "BUILD_WITH_UNITS($ * ( $ == 3500. ? .0025/4096. : .001/4096.) , 'Amps')");
  polarity_nid = setup->head_nid + L2415_N_POLARITY;
  range_nid = setup->head_nid + L2415_N_RANGE;
  pio(26, 0, 0);		/* convert output voltage to digital */
  qrep(0, 0, 1, &voltage);	/* read it when we get q back */
  TdiCompile((struct descriptor *)&volt_expr, &voltage_dsc, &polarity_dsc, &range_dsc, &out_xd MDS_END_ARG);
  volt_out_nid = setup->head_nid + L2415_N_VOLT_OUT;
  return_on_error(TreePutRecord(volt_out_nid, (struct descriptor *)&out_xd, 0), status);
  pio(26, 1, 0);		/* convert output current to digital */
  qrep(0, 0, 1, &current);	/* read it when we get q back */
  TdiCompile((struct descriptor *)&current_expr, &current_dsc, &range_dsc, &out_xd MDS_END_ARG);
  curr_out_nid = setup->head_nid + L2415_N_CURR_OUT;
  return_on_error(TreePutRecord(curr_out_nid, (struct descriptor *)&out_xd, 0), status);
  /* turn it off */
/*
  pio(16,0,0)
  pio(16,1,0)
*/
  return 1;
}
