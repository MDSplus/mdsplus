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
/*------------------------------------------------------------------------------

		Name:   J1819$METHODS

		Type:   C function

		Author:	Josh Stillerman

		Date:   5-NOV-1991

		Purpose: Methods for J1819 devices.

------------------------------------------------------------------------------

	Call sequence:

int J1819_INIT(struct descriptor *niddsc);

int J1819_STORE(struct descriptor *niddsc);

int J1819_TRIGGER(struct descriptor *niddsc);

------------------------------------------------------------------------------
   Copyright (c) 1991
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

	Description:

------------------------------------------------------------------------------*/
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <strroutines.h>
#include <string.h>
#include <stdlib.h>
#include "j1819_gen.h"
#include "devroutines.h"




//extern unsigned short OpcValue;

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
static int one = 1;
#define pio(f,a,d)  return_on_error(DevCamChk(CamPiow(setup->name, a, f, d, 24, 0), &one, 0),status)
#define stop(f,a,n,d)  return_on_error(DevCamChk(CamStopw(setup->name, a, f, n, d, 24, 0), &one, 0),status)
#define fstop(f,a,n,d)  return_on_error(DevCamChk(CamFStopw(setup->name, a, f, n, d, 24, 0), &one, 0),status)
#define return_on_error(func,statret) status = func; if (!(status & 1)) return statret

EXPORT int j1819___init(struct descriptor *niddsc __attribute__ ((unused)), InInitStruct * setup)
{
  int status;
  int mem_cntrl;
  pio(24, 0, 0);		/* Disable digitization */
  pio(9, 0, 0);			/* Clear memory address */
  mem_cntrl =
      (setup->samples <=
       8182) ? 0 : ((setup->samples <= 16384) ? 1 : ((setup->samples <= 24576) ? 2 : 3));
  pio(17, 0, &mem_cntrl);	/* Set the memory size */
  pio(26, 0, 0);		/* enable digitization */
  pio(9, 1, 0);			/* Clear the flip flop */
  return status;
}

EXPORT int j1819___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  static int start = 1;
  static DESCRIPTOR_LONG(start_dsc, &start);
  static int end;
  static DESCRIPTOR_LONG(end_dsc, &end);
  static int stop_trig_nid;
  static DESCRIPTOR_NID(trigger_dsc, &stop_trig_nid);
  static DESCRIPTOR_WINDOW(window_dsc, &start_dsc, &end_dsc, &trigger_dsc);
  static int ext_clock_in_nid;
  static DESCRIPTOR_NID(clk_dsc, &ext_clock_in_nid);
  static DESCRIPTOR_DIMENSION(dimension_dsc, &window_dsc, &clk_dsc);
  static DESCRIPTOR_FUNCTION_1(value_dsc, (unsigned char *)&OpcValue, 0);
  static DESCRIPTOR_A(raw_dsc, sizeof(int), DTYPE_L, 0, 0);
  static DESCRIPTOR_SIGNAL_1(signal_dsc, &value_dsc, &raw_dsc, &dimension_dsc);
  int num;
  int status;
  int samples_to_read;
  int read_size;
  int *ptr;
  int dummy;
  int fast;
  int nid;
  value_dsc.ndesc = 0;

  stop_trig_nid = setup->head_nid + J1819_N_STOP_TRIG;
  ext_clock_in_nid = setup->head_nid + J1819_N_EXT_CLOCK_IN;
  nid = setup->head_nid + J1819_N_COMMENT;
  fast = TreeIsOn(nid) & 1;
  pio(27, 1, &dummy);
  if ((CamXandQ(0) & 1) == 0)
    return DEV$_NOT_TRIGGERED;
  pio(0, 0, &dummy);		/* skip first sample it is garbage */

  /* set num to the number they asked for but no more than 1 less
     than the total memory size.  We are reading one more sample than
     is 'active' in all cases except the whole memory case */
  num = min(max(1, setup->samples), 32767);
  end = num;
  raw_dsc.arsize = num * sizeof(int);
  raw_dsc.pointer = malloc(raw_dsc.arsize);
  for (ptr = (int *)raw_dsc.pointer, samples_to_read = num;
       samples_to_read; ptr += read_size, samples_to_read -= read_size) {
    read_size = min(samples_to_read, 16383);
    if (fast && num < 16384) {
      fstop(0, 0, read_size, ptr);
    } else {
      stop(0, 0, read_size, ptr);
    }
  }
  nid = setup->head_nid + J1819_N_INPUT;
  return_on_error(TreePutRecord(nid, (struct descriptor *)&signal_dsc, 0), status);
  free(raw_dsc.pointer);
  return 1;
}

EXPORT int j1819___trigger(struct descriptor *niddsc __attribute__ ((unused)), InTriggerStruct * setup)
{
  int status;
  pio(28, 0, 0);
  return status;
}
