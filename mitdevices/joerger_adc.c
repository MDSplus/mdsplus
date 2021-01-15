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
#include "joerger_adc_gen.h"
#include "devroutines.h"



#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
static int one = 1;
EXPORT int joerger_adc___store(int *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  int status = 1;
  int data_nid = setup->head_nid + JOERGER_ADC_N_DATA;
  if (TreeIsOn(data_nid) & 1) {
    static short raw[64];
    static ARRAY_BOUNDS(short, 1) raw_d = {
      4, DTYPE_W, CLASS_A, raw, 0, 0, {
	0, 0, 1, 1, 1}, 1, sizeof(raw), raw - 1, {32}, {{1, 32}}};
    static int vstrap_nid;
    static DESCRIPTOR_NID(vstrap_d, &vstrap_nid);
    static EMPTYXD(xd);
    static DESCRIPTOR(expr, "BUILD_WITH_UNITS($ * $/4095,'VOLTS'");
    int bytcnt;
    vstrap_nid = setup->head_nid + JOERGER_ADC_N_VSTRAP;
    return_on_error(DevCamChk(CamQstopw(setup->name, 0, 2, 64, &raw, 16, 0), &one, 0), status);
    bytcnt = CamBytcnt(0);
    raw_d.arsize = bytcnt;
    raw_d.m[0] = raw_d.bounds[0].u = bytcnt / 2;
    TdiCompile((struct descriptor *)&expr, &raw_d, &vstrap_d, &xd MDS_END_ARG);
    status = TreePutRecord(data_nid, (struct descriptor *)&xd, 0);
  }
  return status;
}
