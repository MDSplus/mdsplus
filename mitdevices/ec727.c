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
#include "ec727_gen.h"
#include "devroutines.h"


static int one = 1;
#define pio(f,a) {if (!((status = DevCamChk(CamPiow(setup->name,a,f,0,16,0),&one,&one)) & 1)) return status;}
#define stop(f,a,n,data) {if (!((status = DevCamChk(CamFStopw(setup->name,a,f,n,data,24,0),&one,0)) & 1)) return status;}

#define min(a,b) ((a) <= (b)) ? (a) : (b)
#define max(a,b) ((a) >= (b)) ? (a) : (b)

EXPORT int ec727___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  pio(24, 0)
      pio(25, 15)
      return status;
}

EXPORT int ec727___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{
  static DESCRIPTOR(sigdef, "BUILD_SIGNAL(BUILD_WITH_UNITS($,'counts'),*,$[ $ : $ ])");
  static DESCRIPTOR_A_BOUNDS(raw, sizeof(int), DTYPE_L, 0, 1, 0);
  static DESCRIPTOR_LONG(start_d, &raw.bounds[0].l);
  static DESCRIPTOR_LONG(end_d, &raw.bounds[0].u);
  static EMPTYXD(signal);
  static int xfer_in;
  static DESCRIPTOR_NID(xfer_in_d, &xfer_in);
  int data[1024][32];
  int cdata[1024];
  int *dptr = (int *)data;
  int status;
  int i;
  int chan;
  xfer_in = setup->head_nid + EC727_N_XFER_IN;
  pio(17, 0);
  for (i = 0; i < 4; i++, dptr += 8192)
    stop(0, 0, 8192, dptr);
  for (chan = 0; ((chan < 32) && (status & 1)); chan++) {
    int c_nid = setup->head_nid + EC727_N_INPUT_01 + chan * (EC727_N_INPUT_02 - EC727_N_INPUT_01);
    int s_nid = c_nid + EC727_N_INPUT_01_STARTIDX - EC727_N_INPUT_01;
    int e_nid = c_nid + EC727_N_INPUT_01_ENDIDX - EC727_N_INPUT_01;
    if (TreeIsOn(c_nid) & 1) {
      status = DevLong(&s_nid, (int *)&raw.bounds[0].l);
      if (status & 1)
	raw.bounds[0].l = min(1023, max(0, raw.bounds[0].l));
      else
	raw.bounds[0].l = 0;

      status = DevLong(&e_nid, (int *)&raw.bounds[0].u);
      if (status & 1)
	raw.bounds[0].u = min(1023, max(0, raw.bounds[0].u));
      else
	raw.bounds[0].u = 1023;

      raw.m[0] = raw.bounds[0].u - raw.bounds[0].l + 1;
      if (raw.m[0] > 0) {
	for (i = 0; i < 1024; i++)
	  cdata[i] = data[i][chan];
	raw.pointer = (char *)(cdata + (raw.bounds[0].l));
	raw.a0 = (char *)cdata;
	raw.arsize = raw.m[0] * 4;
	TdiCompile((struct descriptor *)&sigdef, &raw, &xfer_in_d, &start_d, &end_d, &signal MDS_END_ARG);
	status = TreePutRecord(c_nid, (struct descriptor *)signal.pointer, 0);
      }
    }
  }
  return status;
}
