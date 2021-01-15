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
#include "l8201_gen.h"
#include "devroutines.h"

static int one = 1;
static int zero = 0;

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;
#define pio(f,d) return_on_error(DevCamChk(CamPiow(setup->name,0,f,d, 16, 0),&one,0),status)
#define stop(f)  return_on_error(DevCamChk(CamStopw(setup->name,0,f,16384,buffer,16,0),&one,0),status)



static short buffer[16384];
static DESCRIPTOR_A(data, sizeof(short), DTYPE_W, buffer, sizeof(buffer));

EXPORT int l8201___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  return_on_error(TdiData(setup->download, &data MDS_END_ARG), status);
  pio(9, 0);
  stop(16);
  pio(11, &zero);
  pio(26, 0);
  return status;
}

EXPORT int l8201___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{
  int status;
  int upload_nid = setup->head_nid + L8201_N_UPLOAD;
  pio(9, 0);
  pio(2, &buffer[0]);
  stop(2);
  pio(11, &zero);
  pio(26, 0);
  if (TreeIsOn(upload_nid) & 1)
    status = TreePutRecord(upload_nid, (struct descriptor *)&data, 0);
  return status;
}
