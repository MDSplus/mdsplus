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
#include "joerger_adcp_gen.h"
#include "devroutines.h"


extern int DevWait(float);
static int one = 1;
#define return_on_error(f) if (!((status = f) & 1)) return status;
#define pio(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16, 0), &one, 0))

EXPORT int joerger_adcp___init(struct descriptor *nid_dsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  pio(24, 0, 0)
      return status;
}

EXPORT int joerger_adcp___store(struct descriptor *nid_dsc, InStoreStruct * setup)
{
  int status;
  int chan;
  static int c_nids[JOERGER_ADCP_K_CONG_NODES];
  static DESCRIPTOR(output_expr, "$*$");
  static short int buffer[16];
  static DESCRIPTOR_A(buffer_dsc, sizeof(short int), DTYPE_W, &buffer, sizeof(buffer));
  static float coef;
  static DESCRIPTOR_FLOAT(coef_dsc, &coef);
  static struct descriptor_xd output_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  static float coefs[] = { 20.48 / 4096, 10.24 / 4096, 10.24 / 4096, 5.12 / 4096 };
  struct _op_mode {
    unsigned int mode:2;
    unsigned int coef:2;
    unsigned int cont:1;
    unsigned int:11;
  } operating_mode;

  return_on_error(DevNids(nid_dsc, sizeof(c_nids), c_nids));
  if (setup->no_trig) {
    float wait = 10E-6 * 16;
    pio(26, 0, 0);
    return_on_error(DevWait(wait));
  }
  for (chan = 0; chan < 16; chan++)
    pio(0, chan, &buffer[chan]);
  pio(1, 15, (short *)&operating_mode);
  coef = coefs[operating_mode.coef];
  return_on_error(TdiCompile((struct descriptor *)&output_expr, &buffer_dsc, &coef_dsc, &output_xd MDS_END_ARG));
  status = TreePutRecord(c_nids[JOERGER_ADCP_N_INPUTS], (struct descriptor *)&output_xd, 0);
  return status;
}
