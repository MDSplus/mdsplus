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
#include "joerger_dac16_gen.h"
#include <libroutines.h>
#include <strroutines.h>
#include <treeshr.h>
#include <mdsshr.h>
#include "devroutines.h"
static int one = 1;
#define return_on_error(f) if (!((status = f) & 1)) return status;
#define pio(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16, 0), &one, 0))
#define pio24(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 24, 0), &one, 0))

extern int DevCamChk();

int joerger_dac16___init(struct descriptor *nid_dsc, InInitStruct * setup)
{
  int i;
  static int c_nids[JOERGER_DAC16_K_CONG_NODES];
  int status;
  unsigned int module_id;
  int range;
  short int bad_chans = 0;
  float *outputs;
  outputs = &setup->output_01;
  return_on_error(DevNids(nid_dsc, sizeof(c_nids), c_nids));
  pio24(1, 15, &module_id);
  range = module_id & 3;
  for (i = 0; i < 16; i++) {
    if (TreeIsOn(c_nids[JOERGER_DAC16_N_OUTPUT_01 + i]) & 1) {
      switch (range) {
      case 0:
	if ((outputs[i] >= 0) && (outputs[i] <= 10)) {
	  short int data = outputs[i] / 10.0 * 4096;
	  pio(16, i, &data);
	} else
	  bad_chans |= 1 << i;
	break;
      case 1:
	if ((outputs[i] >= 0) && (outputs[i] <= 5)) {
	  short int data = outputs[i] / 5.0 * 4096;
	  pio(16, i, &data);
	} else
	  bad_chans |= 1 << i;
	break;
      case 2:
	if ((outputs[i] >= -10) && (outputs[i] <= 10)) {
	  short int data = (outputs[i] + 10) / 20.0 * 4096;
	  pio(16, i, &data);
	} else
	  bad_chans |= 1 << i;
	break;
      case 3:
	if ((outputs[i] >= -5) && (outputs[i] <= 5)) {
	  short int data = (outputs[i] + 5) / 10.0 * 4096;
	  pio(16, i, &data);
	} else
	  bad_chans |= 1 << i;
	break;
      }
    }
  }
  return (bad_chans != 0) ? J_DAC$_OUTRNG : 1;
}
