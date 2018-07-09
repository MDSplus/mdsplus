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
#include "hv1443_gen.h"
#include "devroutines.h"

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

#define HV1443_K_CHANS          16

extern int hv1443___get_settings(struct descriptor *niddsc __attribute__ ((unused)), InGet_settingsStruct * setup);
extern int GenDeviceFree();

EXPORT int hv1443__get_settings(struct descriptor *niddsc_ptr __attribute__ ((unused)),
				struct descriptor *meth __attribute__ ((unused)), int max_chans,
			 int *settings)
{
  int status = 1;
  static InGet_settingsStruct setup;
  if (max_chans != HV1443_K_CHANS)
    return HV1440$_WRONG_POD_TYPE;
  status = hv1443___get_settings(niddsc_ptr, &setup);
  if (status & 1) {
    int i;
    for (i = 0; i < HV1443_K_CHANS; i++) {
      int nid = setup.head_nid + HV1443_N_VOLTAGE_01 + i;
      if (TreeIsOn(nid) & 1) {
	if ((DevLong(&nid, &settings[i]) & 1) == 0)
	  settings[i] = 0;
      } else
	settings[i] = 0;
    }
    GenDeviceFree(&setup);
  }
  return status;
}
