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
#include "b2408_gen.h"
#include <treeshr.h>
#include <libroutines.h>
#include <mitdevices_msg.h>
#include <stdio.h>
#include "devroutines.h"
#include <xmdsshr.h>

static int one = 1;
#define pio(f,a,d,mem)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, mem,0), &one, &one),status)
#define stop(f,a,c,d)  return_on_error(DevCamChk(CamStopw(setup->name, a, f, c, &d, 24, 0), &one, 0),status)
#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

EXPORT int b2408___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status = 1;
  pio(26, 0, 0, 16);
  return status;
}

typedef struct _status_reg {
  unsigned npts:12;
  unsigned dummy:4;
  unsigned ext_clk:1;
  unsigned freq:2;
  unsigned armed:1;
  unsigned counting:1;
  unsigned done:1;
  unsigned trig_lim:1;
  unsigned overflow:1;
  unsigned:8;
} StatusReg;

EXPORT int b2408___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  int j;
  int npts;
  int status = 1;
  static int trig_nid;
  static DESCRIPTOR_NID(trig_dsc, &trig_nid);
  static int ext_clock_nid;
  static DESCRIPTOR_NID(clock_dsc, &ext_clock_nid);
  static int buffer[2048];
  static DESCRIPTOR_A(buffer_dsc, sizeof(int), DTYPE_L, buffer, 0);
  static float clock_mult;
  static DESCRIPTOR_FLOAT(clock_mult_dsc, &clock_mult);
  static struct descriptor_xd xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  static DESCRIPTOR(expr,
		    "BUILD_SIGNAL(BUILD_WITH_UNITS(SLOPE_OF($)*$*$VALUE+$,\"Seconds\"), $, *)");
  static DESCRIPTOR(int_clock_expr,
		    "BUILD_SIGNAL(BUILD_WITH_UNITS(1E-6*$*$VALUE+$,\"Seconds\"), $, *)");
  static float multipliers[] = { 1, 10, 100, 1000 };
  static int zero = 0;
  int output_nid = setup->head_nid + B2408_N_TIMEBASE;
  StatusReg status_reg;

  ext_clock_nid = setup->head_nid + B2408_N_EXT_CLOCK;
  trig_nid = setup->head_nid + B2408_N_TRIGGER;
  if (XmdsIsOn(output_nid)) {
    pio(24, 0, 0, 16);
    pio(1, 0, (int *)&status_reg, 24);
    npts = status_reg.npts;
    clock_mult = multipliers[status_reg.freq];
    if (npts > 0) {
      int ok, tries;
      for (ok = 0, tries = 0; tries < 100 && !ok; tries++) {
	pio(16, 0, &zero, 16);
	stop(2, 0, npts, buffer);
	ok = 1;
	for (j = 1; j < npts; j++) {
	  if (buffer[j - 1] > buffer[j]) {
	    ok = 0;
	    break;
	  }
	}
      }
      if (tries > 1) {
	char comment_c[80];
	struct descriptor comment = { 0, DTYPE_T, CLASS_S, 0 };
	int comment_nid = setup->head_nid + B2408_N_COMMENT;
	sprintf(comment_c, "Increasing times %sobtained after %d attempts", ok ? "" : "not ",
		tries);
	comment.length = strlen(comment_c);
	comment.pointer = comment_c;
	TreePutRecord(comment_nid, (struct descriptor *)&comment, 0);
      }
    } else
      return DEV$_NOT_TRIGGERED;

    buffer_dsc.arsize = npts * sizeof(int);
/*    if (status_reg.ext_clk) { */
    if (1) {
      return_on_error(TdiCompile
		      ((struct descriptor *)&expr, &clock_dsc, &clock_mult_dsc, &trig_dsc, &buffer_dsc,
		       (struct descriptor *)&xd MDS_END_ARG), status);
    } else {
      return_on_error(TdiCompile
		      ((struct descriptor *)&int_clock_expr, &clock_mult_dsc, &trig_dsc, &buffer_dsc,
		       (struct descriptor *)&xd MDS_END_ARG), status);
    }
    return_on_error(TreePutRecord(output_nid, (struct descriptor *)&xd, 0), status);
  }
/*  return (status_reg.overflow) ? B2408$_OVERFLOW : (status_reg.trig_lim) ? B2408$_TRIG_LIM : 1; */
  return (status_reg.overflow) ? B2408$_OVERFLOW : 1;
}
