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
#include "mit_encoder_gen.h"
#include "devroutines.h"

extern int mit_encoder___set_event();
extern int GenDeviceFree();

static DESCRIPTOR(event_lookup, "EVENT_LOOKUP($)");
static int one = 1;
#define return_on_error(func,statret) status = func; if (!(status & 1)) return statret
#define pio(f,a,d) return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 16, 0), &one, 0), status);

int mit_encoder___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  static short code[] = { 0, 0, 0, 0, 0, 0, 0 };
  short inhibit = 0;
  int chan;
  for (chan = 0; chan < 7; chan++) {
    static int chan_nid;
    chan_nid =
	setup->head_nid + MIT_ENCODER_N_CHANNEL_1_EVENT + (MIT_ENCODER_N_CHANNEL_2 -
							   MIT_ENCODER_N_CHANNEL_1) * chan;
    if (TreeIsOn(chan_nid) & 1) {
      static struct descriptor code_dsc = { 1, DTYPE_BU, CLASS_S, 0 };
      static DESCRIPTOR_NID(cnid_dsc, &chan_nid);
      code_dsc.pointer = (char *)&code[chan];
      return_on_error(TdiExecute((struct descriptor *)&event_lookup, &cnid_dsc, &code_dsc MDS_END_ARG), status);
    } else
      inhibit |= 1 << chan;
  }
  for (chan = 0; chan < 7; chan++)
    if (!(inhibit & (1 << chan))) {
      int a = chan + 1;
      pio(17, a, &code[chan]);
    }
  pio(16, 0, &inhibit);
  return status;
}

int mit_encoder___trigger(struct descriptor *niddsc_ptr __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(25, 0, 0);
  return status;
}

#undef pio
#define pio(f,a,d) return_on_error(DevCamChk(CamPiow(setup.name, a, f, d, 16, 0), &one, 0), status);

EXPORT int mit_encoder__set_event(struct descriptor *niddsc_ptr __attribute__ ((unused)),
				  struct descriptor *method_dsc __attribute__ ((unused)),
				  struct descriptor *event_dsc)
{
  InSet_eventStruct setup;
  int status = mit_encoder___set_event(niddsc_ptr, &setup);
  if (status & 1) {
    static unsigned short event = 0;
    static struct descriptor event_d = { 1, DTYPE_BU, CLASS_S, (char *)&event };
    static int nid;
    static DESCRIPTOR_NID(cnid_dsc, &nid);
    if ((status = TdiExecute((struct descriptor *)&event_lookup, event_dsc, &event_d MDS_END_ARG)) & 1) {
      pio(17, 7, &event);
      pio(25, 0, 0);
      nid = setup.head_nid + MIT_ENCODER_N_SOFT_CHANNEL_EVENT;
      if (TreeIsOn(nid) & 1) {
	struct descriptor_d name = { 0, DTYPE_T, CLASS_D, 0 };
	int lstatus = DevText(&nid, &name);
	if (lstatus && (name.length > 0))
	  if (TdiExecute((struct descriptor *)&event_lookup, &cnid_dsc, &event_d MDS_END_ARG) & 1) {
	    pio(17, 7, &event);
	  }
      }
    }
    GenDeviceFree(&setup);
  }
  return status;
}
